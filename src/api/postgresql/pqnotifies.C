#include <pqdefinitions.h>

extern "C" {

typedef struct pgNotify {
	char	*relname;
	int	be_pid;
} PGnotify;

PGnotify *PQnotifies(PGconn *conn) {
	return NULL;
}

void PQfreeNotify(PGnotify *notify) {
}


}
