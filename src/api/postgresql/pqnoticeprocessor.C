#include <pqdefinitions.h>

extern "C" {

typedef void (*PQnoticeProcessor) (void *arg, const char *message);

PQnoticeProcessor PQsetNoticeProcessor(PGconn *conn,
					 PQnoticeProcessor proc,
					 void *arg) {
}

}
