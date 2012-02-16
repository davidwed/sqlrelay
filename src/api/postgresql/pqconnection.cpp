/*
Portions Copyright (c) 1996-2002, The PostgreSQL Global Development Group
 
Portions Copyright (c) 1994, The Regents of the University of California

Portions Copyright (c) 2003-2004, David Muse

Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without a written agreement is hereby granted, provided that the above copyright notice and this paragraph and the following two paragraphs appear in all copies.
 
IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <pqdefinitions.h>
#include <stdlib.h>
#include <rudiments/charstring.h>

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
	debugFunction();
	fprintf(stderr,"%s",message);
}

PGconn *allocatePGconn(const char *conninfo,
				const char *host, const char *port,
				const char *options, const char *tty,
				const char *db, const char *user,
				const char *password) {
	debugFunction();

	PGconn	*conn=new PGconn;

	conn->sqlrcon=NULL;

	conn->conninfo=(char *)conninfo;
	if (conninfo) {

		conn->connstr=new parameterstring();
		conn->connstr->setDelimiter(' ');
		conn->connstr->parse(conninfo);

		const char	*tmphost=conn->connstr->getValue("host");
		conn->host=charstring::duplicate((tmphost)?tmphost:"");
		const char	*tmpport=conn->connstr->getValue("port");
		// if port is passed in via conninfo, an empty port is NOT
		// translated to 5432
		conn->port=charstring::duplicate((tmpport)?tmpport:"");
		const char	*tmpoptions=conn->connstr->getValue("options");
		conn->options=charstring::duplicate((tmpoptions)?tmpoptions:"");
		const char	*tmptty=conn->connstr->getValue("tty");
		conn->tty=charstring::duplicate((tmptty)?tmptty:"");
		const char	*tmpdbname=conn->connstr->getValue("dbname");
		conn->db=charstring::duplicate((tmpdbname)?tmpdbname:"");
		const char	*tmpuser=conn->connstr->getValue("user");
		conn->user=charstring::duplicate((tmpuser)?tmpuser:"");
		const char	*tmppassword=conn->connstr->
						getValue("password");
		conn->password=charstring::duplicate((tmppassword)?
							tmppassword:"");

	} else {

		conn->connstr=NULL;

		conn->host=charstring::duplicate((host)?host:"");
		// if port is passed in via parameters, an empty port is
		// translated to 5432
		conn->port=charstring::duplicate((port)?port:"5432");
		conn->options=charstring::duplicate((options)?options:"");
		conn->tty=charstring::duplicate((tty)?tty:"");
		conn->db=charstring::duplicate((db)?db:"");
		conn->user=charstring::duplicate((user)?user:"");
		conn->password=charstring::duplicate((password)?password:"");
	}

	conn->clientencoding=PG_UTF8;

	conn->currentresult=NULL;
	conn->nonblockingmode=0;

	conn->noticeprocessor=defaultNoticeProcessor;
	conn->noticeprocessorarg=(void *)NULL;

	conn->error=NULL;

	int	portnumber=charstring::toInteger(conn->port);

	conn->sqlrcon=new sqlrconnection((portnumber)?conn->host:"",
					portnumber,
					(!portnumber)?conn->port:"",
					conn->user,conn->password,0,1,true);

	conn->removetrailingsemicolons=-1;

	conn->errorverbosity=PQERRORS_DEFAULT;

	conn->sqlrcon->selectDatabase(conn->db);

	return conn;
}

void freePGconn(PGconn *conn) {
	debugFunction();

	if (!conn) {
		return;
	}

	delete conn->sqlrcon;
	conn->sqlrcon=NULL;

	if (conn->conninfo) {
		delete conn->connstr;
		conn->connstr=NULL;
		conn->conninfo=NULL;
	}
	delete[] conn->host;
	delete[] conn->port;
	delete[] conn->options;
	delete[] conn->tty;
	delete[] conn->db;
	delete[] conn->user;
	delete[] conn->password;

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
	debugFunction();
	return allocatePGconn(NULL,host,port,options,tty,db,user,password);
}

PGconn *PQsetdb(const char *host, const char *port,
			 const char *options, const char *tty,
			 const char *db) {
	debugFunction();
	return PQsetdbLogin(host,port,options,tty,db,NULL,NULL);
}

PGconn *PQconnectdb(const char *conninfo) {
	debugFunction();
	return allocatePGconn(conninfo,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

PGconn *PQconnectdbParams(const char **keywords,
				const char **values,
				int expanddbname) {
	debugFunction();

	const char *host=NULL;
	const char *port=NULL;
	const char *options=NULL;
	const char *tty=NULL;
	const char *db=NULL;
	const char *user=NULL;
	const char *password=NULL;

	for (uint16_t i=0; keywords[i]; i++) {
		printf("%s=%s\n",keywords[i],values[i]);
		if (!charstring::compare(keywords[i],"host")) {
			host=values[i];
		} else if (!charstring::compare(keywords[i],"hostaddr")) {
			host=values[i];
		} else if (!charstring::compare(keywords[i],"port")) {
			port=values[i];
		} else if (!charstring::compare(keywords[i],"options")) {
			options=values[i];
		} else if (!charstring::compare(keywords[i],"tty")) {
			tty=values[i];
		} else if (!charstring::compare(keywords[i],"dbname")) {
			db=values[i];
		} else if (!charstring::compare(keywords[i],"user")) {
			user=values[i];
		} else if (!charstring::compare(keywords[i],"password")) {
			password=values[i];
		}
	}

	return allocatePGconn(NULL,host,port,options,tty,db,user,password);
}

void PQfinish(PGconn *conn) {
	debugFunction();
	freePGconn(conn);
}

void PQreset(PGconn *conn) {
	debugFunction();
	PQfinish(conn);
	PQsetdbLogin(conn->host,conn->port,conn->options,
			conn->tty,conn->db,conn->user,conn->password);
}

char *PQdb(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->db);
	return conn->db;
}

char *PQuser(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->user);
	return conn->user;
}

char *PQpass(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->password);
	return conn->password;
}

char *PQhost(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->host);
	return conn->host;
}

char *PQport(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->port);
	return conn->port;
}

char *PQtty(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->tty);
	return conn->tty;
}

char *PQoptions(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->options);
	return conn->options;
}

ConnStatusType PQstatus(const PGconn *conn) {
	debugFunction();
	return CONNECTION_OK;
}

PGVerbosity PQsetErrorVerbosity(PGconn *conn, PGVerbosity verbosity) {
	PGVerbosity	retval=conn->errorverbosity;
	conn->errorverbosity=verbosity;
	return retval;
}

char *PQerrorMessage(const PGconn *conn) {
	debugFunction();
	debugPrintf("%s\n",conn->error);
	return (conn->error)?conn->error:(char *)"";
}

int PQsocket(const PGconn *conn) {
	debugFunction();
	return -1;
}

int PQbackendPID(const PGconn *conn) {
	debugFunction();
	return -1;
}

unsigned long PQgetssl(PGconn *conn) {
	debugFunction();
	return 0;
}

int PQclientEncoding(const PGconn *conn) {
	debugFunction();
	return conn->clientencoding;
}

int PQsetClientEncoding(PGconn *conn, const char *encoding) {
	debugFunction();
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
	debugFunction();

	PQnoticeProcessor	oldprocessor=conn->noticeprocessor;

	conn->noticeprocessor=proc;
	conn->noticeprocessorarg=arg;

	return oldprocessor;
}

const char *PQparameterStatus(const PGconn *conn, const char *paramName) {
	debugFunction();
	// FIXME: Return sensible values for this func.
	return NULL;
}

int PQserverVersion() {
	return 80100;
}

}
