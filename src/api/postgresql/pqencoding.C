#include <rudiments/environment.h>
#include <pqdefinitions.h>

extern "C" {

int translateEncoding(const char *encoding) {
	//printf("translateEncoding: %s\n",encoding);
	if (encoding) {
		// FIXME: support other encodings
		if (!strcmp(encoding,"UTF8")) {
			return PG_UTF8;
		}
	}
	return -1;
}

int PQmblen(const unsigned char *s, int encoding) {
	//printf("PQmblem\n");
	// determine length of multibyte encoded char at *s
	// FIXME: support other encodings
	return 1;
}

int PQenv2encoding(void) {
	//printf("PQenv2encoding\n");
	environment	env;
	return translateEncoding(env.getValue("PGCLIENTENCODING"));
}

}
