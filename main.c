#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <glib.h>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>
#include "btree.h"

#define ALIGN_64(addr) (char*)((((ssize_t)addr)+(7))&(~(7)))

void show_dbstats(const char *prefix, const struct btree_stat *st);

static struct btree *btree = NULL;
static int fd = -1;

void get(SoupServer *server,
         SoupMessage *msg,
         const char *path)
{

	struct btval key, val;
	key.data = (void*)path+1;
	key.size = strlen(path)-1;
	key.free_data = FALSE;
	key.mp = NULL;
	g_debug ("GET\n Path=%s\n Fetching key %s", path, (char*)key.data);
	const struct btree_stat *stat = btree_stat(btree);
	show_dbstats("",stat);
	if (0 == btree_get(btree,&key,&val)) {
		g_debug ("data checksum is %s", g_compute_checksum_for_data(G_CHECKSUM_MD5, val.data, val.size)); 
		/* we need to make the data 64 bits aligned for gvariant.
		   Best plan is to make all data aligned in the store, but 
		   for now, lets just copy it to somewhere aligned. 
		   TODO: think about fragmentation. it may just be ok, as so far
		   we delete everything we malloc in this function, despite the
		   lifetimes being interleaved.
		*/
		char *buf = g_slice_alloc(val.size + 8);
		char *data = ALIGN_64(buf);
		memcpy (data, val.data, val.size);
		g_debug ("aligned data checksum is %s", g_compute_checksum_for_data(G_CHECKSUM_MD5, data, val.size)); 
		GVariant *gv = g_variant_new_from_data(G_VARIANT_TYPE_VARIANT, data, val.size, TRUE, NULL, NULL);
	
		char *debug = g_variant_print (gv, TRUE);
		g_debug ("converted to %s", debug);
		g_free (debug);

		int length;			
		char* ret = json_gvariant_serialize_data(gv, &length);
		g_variant_unref(gv);
		g_slice_free1 (val.size + 8, buf);
		soup_message_set_status (msg, SOUP_STATUS_OK);
		/*TODO: does soup do anything sensible with it's memory management of responses to reduce the risk of fragmentation?  probably not..*/
		soup_message_set_response (msg, "application/json", SOUP_MEMORY_TAKE, ret, length);
	} else {
		soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
	}
}

void post(SoupServer *server,
         SoupMessage *msg,
         const char *path)
{
	JsonParser *parser = json_parser_new ();
	GError *err = NULL;
	g_debug("POST\n path= %s\nbody = '%s'", path, msg->request_body->data);
	if (json_parser_load_from_data (parser, msg->request_body->data, -1, &err)) {
		g_debug("parsed sucessfully");
		GVariant *gv = json_gvariant_deserialize(json_parser_get_root(parser), NULL, NULL);
		if (gv) {
			char *debug = g_variant_print (gv, TRUE);
			g_debug ("converted to %s", debug);
			g_free (debug);
			//We need to store the signature, so package in a v
			GVariant *packaged  = g_variant_new_variant(gv);

			struct btval key;
			key.data = (void*)path+1;
			key.size = strlen(path)-1;
			key.free_data = FALSE;
			key.mp = NULL;
			struct btval val;
			val.data =(void*) g_variant_get_data(packaged);
			val.size = g_variant_get_size(packaged);
			val.free_data = FALSE;
			val.mp = NULL;
			g_debug ("inserting with key %s", (char*)key.data);
			g_debug ("data checksum is %s", g_compute_checksum_for_data(G_CHECKSUM_MD5, val.data, val.size)); 
			int success = btree_put(btree, &key, &val,0);
			if (0!=success) {
				g_debug("put failed: %s)", strerror(errno));
			}
			g_variant_unref (packaged);
			g_variant_unref (gv);	
		}
	} else {
		g_debug("failed to parse json: %s", err->message);
		g_error_free(err);
	}
	soup_message_set_status (msg, SOUP_STATUS_OK);
}


void print_headers (const char *name,
                    const char *value,
                    gpointer user_data)
{
	g_debug("%s %s", name, value);
}

void server_callback (SoupServer *server,
                      SoupMessage *msg,
                      const char *path,
                      gpointer data,
                      SoupClientContext *client,
                      gpointer user_data)
{
	g_debug("got server callback");
        printf ("%s %s HTTP/1.%d\n", msg->method, path,
                soup_message_get_http_version (msg));
	soup_message_headers_foreach (msg->request_headers,
                                      print_headers, NULL);

	if (msg->method == SOUP_METHOD_GET)
		get(server,msg,path);
	else if (msg->method == SOUP_METHOD_POST)
		post(server,msg,path);
	else {
		soup_message_set_status (msg, SOUP_STATUS_NOT_IMPLEMENTED);
		return;
	}

}

int main()
{
	g_type_init();

	SoupServer *server = soup_server_new (
		SOUP_SERVER_PORT, 3000,
		NULL);
	soup_server_add_handler (server, NULL, (SoupServerCallback) server_callback,
             NULL,NULL);

	btree = btree_open("/tmp/badger", 0, 0666);
	soup_server_run(server);
}

#define ZDIV(t,n)       ((n) == 0 ? 0 : (float)(t) / (n))

void show_dbstats(const char *prefix, const struct btree_stat *st)
{
        printf("%s timestamp: %s", prefix, ctime(&st->created_at));
        printf("%s page size: %u\n", prefix, st->psize);
        printf("%s depth: %u\n", prefix, st->depth);
        printf("%s revisions: %u\n", prefix, st->revisions);
        printf("%s entries: %llu\n", prefix, st->entries);
        printf("%s branch/leaf/overflow pages: %u/%u/%u\n",
            prefix, st->branch_pages, st->leaf_pages, st->overflow_pages);

        printf("%s cache size: %u of %u (%.1f%% full)\n", prefix,
            st->cache_size, st->max_cache,
            100 * ZDIV(st->cache_size, st->max_cache));
        printf("%s page reads: %llu\n", prefix, st->reads);
        printf("%s cache hits: %llu (%.1f%%)\n", prefix, st->hits,
            100 * ZDIV(st->hits, (st->hits + st->reads)));
}


