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
		//printf("allocatePGconn w/conninfo\n");

		conn->connstr=new connectstring();
		conn->connstr->parse(conninfo);

		conn->host=conn->connstr->getValue("host");
		conn->host=(conn->host)?conn->host:(char *)"";
		conn->port=conn->connstr->getValue("port");
		conn->port=(conn->port)?conn->port:(char *)"";
		conn->options=conn->connstr->getValue("options");
		conn->options=(conn->options)?conn->options:(char *)"";
		conn->tty=conn->connstr->getValue("tty");
		conn->tty=(conn->tty)?conn->tty:(char *)"";
		conn->db=conn->connstr->getValue("db");
		conn->db=(conn->db)?conn->db:(char *)"";
		conn->user=conn->connstr->getValue("user");
		conn->user=(conn->user)?conn->user:(char *)"";
		conn->password=conn->connstr->getValue("password");
		conn->password=(conn->password)?conn->password:(char *)"";

	} else {
		//printf("allocatePGconn w/out conninfo\n");

		conn->connstr=NULL;

		conn->host=strdup((host)?host:"");
		conn->port=strdup((port)?port:"");
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

	conn->sqlrcon=new sqlrconnection(host,atoi((port)?port:""),
						"",user,password,0,1);
	//conn->sqlrcon->debugOn();
	conn->sqlrcon->copyReferences();

	return conn;
}

void freePGconn(PGconn *conn) {
	//printf("freePGconn\n");

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

// connect to the database using paramaters
PGconn *PQsetdbLogin(const char *host, const char *port,
			 const char *options, const char *tty,
			 const char *db,
			 const char *user, const char *password) {
	//printf("PQsetdbLogin\n");
	return allocatePGconn(NULL,host,port,options,tty,db,user,password);
}

// older, simpler connection function that didn't have user, password
PGconn *PQsetdb(const char *host, const char *port,
			 const char *options, const char *tty,
			 const char *db) {
	//printf("PQsetdb\n");
	return PQsetdbLogin(host,port,options,tty,db,NULL,NULL);
}

// connect to the database using a conninfo string
PGconn *PQconnectdb(const char *conninfo) {
	//printf("PQconnectdb\n");
	return allocatePGconn(conninfo,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

// close the current connection
void PQfinish(PGconn *conn) {
	//printf("PQfinish\n");
	freePGconn(conn);
}

// close and re-open the connection
void PQreset(PGconn *conn) {
	//printf("PQreset\n");
	PQfinish(conn);
	PQsetdbLogin(conn->host,conn->port,conn->options,
			conn->tty,conn->db,conn->user,conn->password);
}

// accessor functions for PGconn objects
char *PQdb(const PGconn *conn) {
	//printf("PQdb: %s\n",conn->db);
	return conn->db;
}

char *PQuser(const PGconn *conn) {
	//printf("PQuser: %s\n",conn->user);
	return conn->user;
}

char *PQpass(const PGconn *conn) {
	//printf("PQpass: %s\n",conn->password);
	return conn->password;
}

char *PQhost(const PGconn *conn) {
	//printf("PQhost: %s\n",conn->host);
	return conn->host;
}

char *PQport(const PGconn *conn) {
	//printf("PQport: %s\n",conn->port);
	return conn->port;
}

char *PQtty(const PGconn *conn) {
	//printf("PQtty: %s\n",conn->tty);
	return conn->tty;
}

char *PQoptions(const PGconn *conn) {
	//printf("PQoptions: %s\n",conn->options);
	return conn->options;
}

ConnStatusType PQstatus(const PGconn *conn) {
	//printf("PQstatus\n");
	return CONNECTION_OK;
}

char *PQerrorMessage(const PGconn *conn) {
	return (conn->error)?conn->error:(char *)"";
}

int PQsocket(const PGconn *conn) {
	//printf("PQsocket\n");
	return -1;
}

int PQbackendPID(const PGconn *conn) {
	//printf("PQbackendPID\n");
	return -1;
}

unsigned long PQgetssl(PGconn *conn) {
	//printf("PQgetSSL\n");
	return 0;
}

int PQclientEncoding(const PGconn *conn) {
	//printf("PQclientEncoding\n");
	return conn->clientencoding;
}

int PQsetClientEncoding(PGconn *conn, const char *encoding) {
	//printf("PQsetClientEncoding\n");
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

	//printf("PQsetNoticeProcessor\n");
	PQnoticeProcessor	oldprocessor=conn->noticeprocessor;

	conn->noticeprocessor=proc;
	conn->noticeprocessorarg=arg;

	return oldprocessor;
}


}
