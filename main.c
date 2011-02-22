#include <glib.h>
#include <libsoup/soup.h>



void server_callback (SoupServer *server,
                      SoupMessage *msg,
                      const char *path,
                      GHashTable *query,
                      SoupClientContext *client,
                      gpointer user_data)
{
	const char *response = "{}";
/*    if (msg->method != SOUP_METHOD_GET) {
        soup_message_set_status (msg, SOUP_STATUS_NOT_IMPLEMENTED);
        return;
    }
*/
    soup_message_set_status (msg, SOUP_STATUS_OK);
    soup_message_set_response (msg, "application/json", SOUP_MEMORY_STATIC,
                   response, strlen(response));

}

int main()
{
	SoupServer *server = soup_server_new (
		SOUP_SERVER_PORT, 3000);
	soup_server_add_handler (server, "/", server_callback,
             NULL,NULL);
}
