#include <pqdefinitions.h>

extern "C" {

size_t PQescapeString(char *to, const char *from, size_t length) {
	printf("PQescapeString unimplemented\n");
	return 0;
}

unsigned char *PQescapeBytea(unsigned char *bintext, size_t binlen,
			  				size_t *bytealen) {
	printf("PQescapeBytea unimplemented\n");
	return NULL;
}

unsigned char *PQunescapeBytea(unsigned char *strtext, size_t *retbuflen) {
	printf("PQunescapeBytea unimplemented\n");
	return NULL;
}


}
