Couchette
=========

Couchette is an experimental implementation of CouchDB in C, funded by [Codethink](http://www.codethink.co.uk). It's using GLib
and in particular GVariant for the on-disk storage, combined with a
write-append btree implementation from [BZero's](http://www.bzero.se/) LDAPd implementation for BSD (http://www.bzero.se/ldapd/).

TODO
----

Currently get crashes appearring from unaligned gvalues. Multiple aspects to this, currenlt working on adding more debugging to gvariant to help figure the issues.

