#include <pqdefinitions.h>
#include <stdlib.h>

extern "C" {

typedef enum {
	CONNECTION_OK,
	CONNECTION_BAD,
	CONNECTION_STARTED,
	CONNECTION_MADE,
	CONNECTION_AWAITING_RESPONSE,
	CONNECTION_AUTH_OK,
	CONNECTION_SETENV
} ConnStatusType;

static void defaultNoticeProcessor(void *arg, const char *message) {
	fprintf(stderr,"%s",message);
}

PGconn *allocatePGconn(const char *conninfo,
				const char *host, const char *port,
				const char *options, const char *tty,
				const char *db, const char *user,
				const char *password) {

	PGconn	*conn=new PGconn;

	conn->sqlrcon=NULL;

	conn->conninfo=(char *)conninfo;
	if (conninfo) {

		conn->connstr=new parameterstring();
		conn->connstr->setDelimiter(' ');
		conn->connstr->parse(conninfo);

		conn->host=conn->connstr->getValue("host");
		conn->host=(conn->host)?conn->host:(char *)"";
		conn->port=conn->connstr->getValue("port");
		// if port is passed in via conninfo, an empty port is NOT
		// translated to 5432
		conn->port=(conn->port)?conn->port:(char *)"";
		conn->options=conn->connstr->getValue("options");
		conn->options=(conn->options)?conn->options:(char *)"";
		conn->tty=conn->connstr->getValue("tty");
		conn->tty=(conn->tty)?conn->tty:(char *)"";
		conn->db=conn->connstr->getValue("dbname");
		conn->db=(conn->db)?conn->db:(char *)"";
		conn->user=conn->connstr->getValue("user");
		conn->user=(conn->user)?conn->user:(char *)"";
		conn->password=conn->connstr->getValue("password");
		conn->password=(conn->password)?conn->password:(char *)"";

	} else {

		conn->connstr=NULL;

		conn->host=strdup((host)?host:"");
		// if port is passed in via parameters, an empty port is
		// translated to 5432
		conn->port=strdup((port)?port:"5432");
		conn->options=strdup((options)?options:"");
		conn->tty=strdup((tty)?tty:"");
		conn->db=strdup((db)?db:"");
		conn->user=strdup((user)?user:"");
		conn->password=strdup((password)?password:"");
	}

	conn->clientencoding=PG_UTF8;

	conn->currentresult=NULL;
	conn->nonblockingmode=0;

	conn->noticeprocessor=defaultNoticeProcessor;
	conn->noticeprocessorarg=(void *)NULL;

	conn->error=NULL;

	conn->sqlrcon=new sqlrconnection(atoi(conn->port)?conn->host:"",
					atoi((conn->port)?conn->port:""),
					(!atoi(conn->port))?conn->port:"",
					conn->user,conn->password,0,1);
	conn->sqlrcon->copyReferences();

	return conn;
}

void freePGconn(PGconn *conn) {

	if (!conn) {
		return;
	}

	delete conn->sqlrcon;
	conn->sqlrcon=NULL;

	if (conn->conninfo) {
		delete conn->connstr;
		conn->connstr=NULL;
		conn->conninfo=NULL;
	} else {
		delete[] conn->host;
		delete[] conn->port;
		delete[] conn->options;
		delete[] conn->tty;
		delete[] conn->db;
		delete[] conn->user;
		delete[] conn->password;
	}

	conn->currentresult=NULL;
	conn->nonblockingmode=0;

	delete[] conn->error;

	delete conn;
	conn=NULL;
}

PGconn *PQsetdbLogin(const char *host, const char *port,
			 const char *options, const char *tty,
			 const char *db,
			 const char *user, const char *password) {
	return allocatePGconn(NULL,host,port,options,tty,db,user,password);
}

PGconn *PQsetdb(const char *host, const char *port,
			 const char *options, const char *tty,
			 const char *db) {
	return PQsetdbLogin(host,port,options,tty,db,NULL,NULL);
}

PGconn *PQconnectdb(const char *conninfo) {
	return allocatePGconn(conninfo,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

void PQfinish(PGconn *conn) {
	freePGconn(conn);
}

void PQreset(PGconn *conn) {
	PQfinish(conn);
	PQsetdbLogin(conn->host,conn->port,conn->options,
			conn->tty,conn->db,conn->user,conn->password);
}

char *PQdb(const PGconn *conn) {
	return conn->db;
}

char *PQuser(const PGconn *conn) {
	return conn->user;
}

char *PQpass(const PGconn *conn) {
	return conn->password;
}

char *PQhost(const PGconn *conn) {
	return conn->host;
}

char *PQport(const PGconn *conn) {
	return conn->port;
}

char *PQtty(const PGconn *conn) {
	return conn->tty;
}

char *PQoptions(const PGconn *conn) {
	return conn->options;
}

ConnStatusType PQstatus(const PGconn *conn) {
	return CONNECTION_OK;
}

char *PQerrorMessage(const PGconn *conn) {
	return (conn->error)?conn->error:(char *)"";
}

int PQsocket(const PGconn *conn) {
	return -1;
}

int PQbackendPID(const PGconn *conn) {
	return -1;
}

unsigned long PQgetssl(PGconn *conn) {
	return 0;
}

int PQclientEncoding(const PGconn *conn) {
	return conn->clientencoding;
}

int PQsetClientEncoding(PGconn *conn, const char *encoding) {
	int	enc=translateEncoding(encoding);
	if (enc>-1) {
		conn->clientencoding=enc;
		return 0;
	}
	return -1;
}

PQnoticeProcessor PQsetNoticeProcessor(PGconn *conn,
					 PQnoticeProcessor proc,
					 void *arg) {

	PQnoticeProcessor	oldprocessor=conn->noticeprocessor;

	conn->noticeprocessor=proc;
	conn->noticeprocessorarg=arg;

	return oldprocessor;
}


}
