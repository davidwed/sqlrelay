typedef enum {
	CONNECTION_OK,
	CONNECTION_BAD,
	CONNECTION_STARTED,
	CONNECTION_MADE,
	CONNECTION_AWAITING_RESPONSE,
	CONNECTION_AUTH_OK,
	CONNECTION_SETENV
} ConnStatusType;

typedef enum {
	PGRES_POLLING_FAILED = 0,
	PGRES_POLLING_READING,
	PGRES_POLLING_WRITING,
	PGRES_POLLING_OK,
	PGRES_POLLING_ACTIVE
} PostgresPollingStatusType;

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

typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;

typedef struct pgNotify {
	char	*relname;
	int	be_pid;
} PGnotify;

typedef void (*PQnoticeProcessor) (void *arg, const char *message);

typedef char pqbool;

typedef struct _PQprintOpt {
	pqbool		header;	
	pqbool		align;
	pqbool		standard;
	pqbool		html3;
	pqbool		expanded;
	pqbool		pager;
	char		*fieldSep;
	char		*tableOpt;
	char		*caption;
	char		**fieldName;
} PQprintOpt;

typedef struct _PQconninfoOption {
	char	*keyword;
	char	*envvar;
	char	*compiled;
	char	*val;
	char	*label;
	char	*dispchar;
	int	dispsize;
} PQconninfoOption;


typedef struct {
	int	len;
	int	isint;
	union {
		int	*ptr;
		int	integer;
	} u;
} PQArgBlock;

/* Asynchronous (non-blocking) */
PGconn *PQconnectStart(const char *conninfo);
PostgresPollingStatusType PQconnectPoll(PGconn *conn);

/* Synchronous (blocking) */
PGconn *PQconnectdb(const char *conninfo) {

	char	*host="";
	char	*port="";
	char	*user="";
	char	*password="";

	// extract host, hostaddr, port, user, password, use them below...

	sqlrconnection	*sqlrconn=new sqlrconnection(host,atoi(port),"",
							user,password,0,1);
}

PGconn *PQsetdbLogin(const char *pghost, const char *pgport,
			 const char *pgoptions, const char *pgtty,
			 const char *dbName,
			 const char *login, const char *pwd) {

	sqlrconnection	*sqlrconn=new sqlrconnection(pghost,pgport,"",
							login,pwd,0,1);
}

#define PQsetdb(M_PGHOST,M_PGPORT,M_PGOPT,M_PGTTY,M_DBNAME)  \
	PQsetdbLogin(M_PGHOST, M_PGPORT, M_PGOPT, M_PGTTY, M_DBNAME, NULL, NULL)

/* close the current connection and free the PGconn data structure */
void PQfinish(PGconn *conn) {
	delete sqlrconn;
}

/* get info about connection options known to PQconnectdb */
PQconninfoOption *PQconndefaults(void);

/* free the data structure returned by PQconndefaults() */
void PQconninfoFree(PQconninfoOption *connOptions);

/* Asynchronous (non-blocking) */
int PQresetStart(PGconn *conn);
PostgresPollingStatusType PQresetPoll(PGconn *conn);

/* Synchronous (blocking) */
void PQreset(PGconn *conn);

/* issue a cancel request */
int PQrequestCancel(PGconn *conn);

/* Accessor functions for PGconn objects */
char *PQdb(const PGconn *conn);
char *PQuser(const PGconn *conn);
char *PQpass(const PGconn *conn);
char *PQhost(const PGconn *conn);
char *PQport(const PGconn *conn);
char *PQtty(const PGconn *conn);
char *PQoptions(const PGconn *conn);
ConnStatusType PQstatus(const PGconn *conn);
char *PQerrorMessage(const PGconn *conn);
int PQsocket(const PGconn *conn);
int PQbackendPID(const PGconn *conn);
int PQclientEncoding(const PGconn *conn);
int PQsetClientEncoding(PGconn *conn, const char *encoding);

#ifdef USE_SSL
/* Get the SSL structure associated with a connection */
SSL *PQgetssl(PGconn *conn);
#endif


/* Enable/disable tracing */
void PQtrace(PGconn *conn, FILE *debug_port);
void PQuntrace(PGconn *conn);

/* Override default notice processor */
PQnoticeProcessor PQsetNoticeProcessor(PGconn *conn,
					 PQnoticeProcessor proc,
					 void *arg);

/* Quoting strings before inclusion in queries. */
size_t PQescapeString(char *to, const char *from, size_t length);
unsigned char *PQescapeBytea(unsigned char *bintext, size_t binlen,
			  size_t *bytealen);
unsigned char *PQunescapeBytea(unsigned char *strtext,
				size_t *retbuflen);

/* Simple synchronous query */
PGresult *PQexec(PGconn *conn, const char *query) {
	sqlrcursor	*sqlrcur=new sqlrcursor(sqlrconn);
	sqlrcur->sendQuery(query);
}

PGnotify *PQnotifies(PGconn *conn);
void PQfreeNotify(PGnotify *notify);

/* Interface for multiple-result or asynchronous queries */
int PQsendQuery(PGconn *conn, const char *query);
PGresult *PQgetResult(PGconn *conn);

/* Routines for managing an asychronous query */
int PQisBusy(PGconn *conn);
int PQconsumeInput(PGconn *conn);

/* Routines for copy in/out */
int PQgetline(PGconn *conn, char *string, int length);
int PQputline(PGconn *conn, const char *string);
int PQgetlineAsync(PGconn *conn, char *buffer, int bufsize);
int PQputnbytes(PGconn *conn, const char *buffer, int nbytes);
int PQendcopy(PGconn *conn);

/* Set blocking/nonblocking connection to the backend */
int PQsetnonblocking(PGconn *conn, int arg);
int PQisnonblocking(const PGconn *conn);

/* Force the write buffer to be written (or at least try) */
int PQflush(PGconn *conn);
int PQsendSome(PGconn *conn);

/*
 * "Fast path" interface --- not really recommended for application
 * use
 */
PGresult *PQfn(PGconn *conn, int fnid, int *result_buf, int *result_len,
	 	int result_is_int, const PQArgBlock *args, int nargs);

/* Accessor functions for PGresult objects */
ExecStatusType PQresultStatus(const PGresult *res);
char *PQresStatus(ExecStatusType status);
char *PQresultErrorMessage(const PGresult *res);
int PQntuples(const PGresult *res);
int PQnfields(const PGresult *res);
int PQbinaryTuples(const PGresult *res);
char *PQfname(const PGresult *res, int field_num);
int PQfnumber(const PGresult *res, const char *field_name);
Oid PQftype(const PGresult *res, int field_num);
int PQfsize(const PGresult *res, int field_num);
int PQfmod(const PGresult *res, int field_num);
char *PQcmdStatus(PGresult *res);
char *PQoidStatus(const PGresult *res);
Oid PQoidValue(const PGresult *res);
char *PQcmdTuples(PGresult *res);
char *PQgetvalue(const PGresult *res, int tup_num, int field_num);
int PQgetlength(const PGresult *res, int tup_num, int field_num);
int PQgetisnull(const PGresult *res, int tup_num, int field_num);

/* Delete a PGresult */
void PQclear(PGresult *res);

/*
 * Make an empty PGresult with given status (some apps find this
 * useful). If conn is not NULL and status indicates an error, the
 * conn's errorMessage is copied.
 */
PGresult *PQmakeEmptyPGresult(PGconn *conn, ExecStatusType status);


void PQprint(FILE *fout, const PGresult *res, const PQprintOpt *ps);

/*
 * really old printing routines
 */
void PQdisplayTuples(const PGresult *res, FILE *fp, int fillAlign,
				const char *fieldSep, int printHeader,
				int quiet);

void PQprintTuples(const PGresult *res, FILE *fout, int printAttName,
			  int terseOutput, int width);

/* Large-object access routines */
int lo_open(PGconn *conn, Oid lobjId, int mode);
int lo_close(PGconn *conn, int fd);
int lo_read(PGconn *conn, int fd, char *buf, size_t len);
int lo_write(PGconn *conn, int fd, char *buf, size_t len);
int lo_lseek(PGconn *conn, int fd, int offset, int whence);
Oid lo_creat(PGconn *conn, int mode);
int lo_tell(PGconn *conn, int fd);
int lo_unlink(PGconn *conn, Oid lobjId);
Oid lo_import(PGconn *conn, const char *filename);
int lo_export(PGconn *conn, Oid lobjId, const char *filename);

/* Determine length of multibyte encoded char at *s */
int PQmblen(const unsigned char *s, int encoding);

/* Get encoding id from environment variable PGCLIENTENCODING */
int PQenv2encoding(void);
