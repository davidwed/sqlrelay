#include <sqlrelay/sqlrclient.h>

typedef unsigned int Oid;
typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;

typedef enum {
	PGRES_EMPTY_QUERY = 0,
	PGRES_COMMAND_OK,
	PGRES_TUPLES_OK,
	PGRES_COPY_OUT,
	PGRES_COPY_IN,
	PGRES_BAD_RESPONSE,
	PGRES_NONFATAL_ERROR,
	PGRES_FATAL_ERROR
} ExecStatusType;

struct pg_conn {

	sqlrconnection	*sqlrcon;

	char	*host;
	char	*port;
	char	*options;
	char	*tty;
	char	*db;
	char	*user;
	char	*password;

	int	socket;
};

struct pg_result {
	sqlrcursor	*sqlrcur;

	ExecStatusType	execstatus;
};
