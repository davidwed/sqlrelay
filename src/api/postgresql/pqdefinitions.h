#include <rudiments/connectstring.h>
#include <sqlrelay/sqlrclient.h>

extern "C" {

#define TRUE	1
#define FALSE	0

typedef unsigned int Oid;
typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;
typedef void (*PQnoticeProcessor) (void *arg, const char *message);

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

struct pg_conn;

struct pg_result {
	sqlrcursor	*sqlrcur;

	ExecStatusType	execstatus;

	pg_conn		*parent;

	int		previousnonblockingmode;

	int		queryisnotselect;
};

struct pg_conn {

	sqlrconnection	*sqlrcon;

	connectstring	*connstr;

	char		*conninfo;

	char		*host;
	char		*port;
	char		*options;
	char		*tty;
	char		*db;
	char		*user;
	char		*password;

	int		clientencoding;

	pg_result	*currentresult;
	int		nonblockingmode;

	PQnoticeProcessor	noticeprocessor;
	void			*noticeprocessorarg;

	char		*error;
};

// encodings
#define	PG_UTF8		6

// object id's
#define InvalidOid	0

// functions
PGconn		*allocatePGconn(const char *conninfo,
				const char *host, const char *port,
				const char *options, const char *tty,
				const char *db, const char *user,
				const char *password);
void		freePGconn(PGconn *conn);
int		translateEncoding(const char *encoding);

PGconn		*PQconnectdb(const char *conninfo);
void		PQfinish(PGconn *conn);
PGresult	*PQexec(PGconn *conn, const char *query);

int	PQnfields(const PGresult *res);
int	PQntuples(const PGresult *res);
char	*PQfname(const PGresult *res, int field_num);
int	PQgetlength(const PGresult *res, int tup_num, int field_num);
char	*PQgetvalue(const PGresult *res, int tup_num, int field_num);
int	PQmblen(const unsigned char *s, int encoding);
int	PQclientEncoding(const PGconn *conn);


}
