#include <rudiments/environment.h>
#include <pqdefinitions.h>

// FIXME: support encodings other than UTF8

extern "C" {

int translateEncoding(const char *encoding) {
	// translate "encoding" into an encoding id
	if (encoding) {
		if (!strcmp(encoding,"UTF8")) {
			return PG_UTF8;
		}
	}
	return -1;
}

int PQmblen(const unsigned char *s, int encoding) {
	// determine length of multibyte encoded char at *s
	return 1;
}

int PQenv2encoding(void) {
	// get value of PGCLIENTENCODING environment variable
	// and translate it to an encoding ID
	environment	env;
	return translateEncoding(env.getValue("PGCLIENTENCODING"));
}

}
