#include <pqdefinitions.h>

extern "C" {

typedef struct {
	int	len;
	int	isint;
	union {
		int	*ptr;
		int	integer;
	} u;
} PQArgBlock;

PGresult *PQfn(PGconn *conn, int fnid, int *result_buf, int *result_len,
	 	int result_is_int, const PQArgBlock *args, int nargs) {
	printf("PQfn unimplemented\n");
	return NULL;
}

}
