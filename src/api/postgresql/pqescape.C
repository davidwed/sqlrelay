extern "C" {

size_t PQescapeString(char *to, const char *from, size_t length) {
}

unsigned char *PQescapeBytea(unsigned char *bintext, size_t binlen,
			  				size_t *bytealen) {
}

unsigned char *PQunescapeBytea(unsigned char *strtext, size_t *retbuflen) {
}


}
