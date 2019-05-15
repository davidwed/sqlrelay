// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information
#include <rudiments/charstring.h>
#include <sqlrelay/sqlrclient.h>

#include "sqlrbench.h"

class sqlrelaybench : public sqlrbench {
	public:
		sqlrelaybench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug);
};

class sqlrelaybenchconnection : public sqlrbenchconnection {
	friend class sqlrelaybenchcursor;
	public:
			sqlrelaybenchconnection(const char *connectstring,
						const char *dbtype);
			~sqlrelaybenchconnection();

		bool	connect();
		bool	disconnect();

	private:
		const char	*host;
		uint16_t	port;
		const char	*socket;
		const char	*user;
		const char	*password;
		bool		debug;

		sqlrconnection	*sqlrcon;

		bool		first=true;
};

class sqlrelaybenchcursor : public sqlrbenchcursor {
	public:
			sqlrelaybenchcursor(sqlrbenchconnection *con,
							uint64_t rsbs);
			~sqlrelaybenchcursor();

		bool	query(const char *query, bool getcolumns);

	private:
		sqlrelaybenchconnection	*sqlrbcon;
		sqlrcursor		*sqlrcur;
};

sqlrelaybench::sqlrelaybench(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t samples,
					uint64_t rsbs,
					bool debug) :
					sqlrbench(connectstring,db,
						queries,rows,cols,colsize,
						samples,rsbs,debug) {
	con=new sqlrelaybenchconnection(connectstring,db);
	cur=new sqlrelaybenchcursor(con,rsbs);
	issqlrelay=true;
}


sqlrelaybenchconnection::sqlrelaybenchconnection(
				const char *connectstring,
				const char *db) :
				sqlrbenchconnection(connectstring,db) {
	host=getParam("host");
	port=charstring::toInteger(getParam("port"));
	socket=getParam("socket");
	user=getParam("user");
	password=getParam("password");
	debug=!charstring::compare(getParam("debug"),"yes");
	sqlrcon=new sqlrconnection(host,port,socket,user,password,0,1);
	if (debug) {
		sqlrcon->debugOn();
	}
}

sqlrelaybenchconnection::~sqlrelaybenchconnection() {
	delete sqlrcon;
}

bool sqlrelaybenchconnection::connect() {
	if (first) {
		if (charstring::compare(sqlrcon->identify(),db)) {
			stdoutput.printf("\nSQL Relay is not "
					"connected to %s\n\n",db);
			return false;
		}
		first=false;
	}
	return true;
}

bool sqlrelaybenchconnection::disconnect() {
	sqlrcon->endSession();
	return true;
}

sqlrelaybenchcursor::sqlrelaybenchcursor(sqlrbenchconnection *con,
							uint64_t rsbs) :
							sqlrbenchcursor(con) {
	sqlrbcon=(sqlrelaybenchconnection *)con;
	sqlrcur=new sqlrcursor(sqlrbcon->sqlrcon);
	sqlrcur->setResultSetBufferSize(rsbs);
}

sqlrelaybenchcursor::~sqlrelaybenchcursor() {
	delete sqlrcur;
}

bool sqlrelaybenchcursor::query(const char *query, bool getcolumns) {
	if (getcolumns) {
		sqlrcur->getColumnInfo();
	} else {
		sqlrcur->dontGetColumnInfo();
	}
	if (!sqlrcur->sendQuery(query)) {
		return false;
	}
	uint32_t	colcount=sqlrcur->colCount();
	if (!colcount) {
		return true;
	}
	for (uint64_t row=0; ; row++) {
		for (uint32_t col=0; col<colcount; col++) {
			if (!sqlrcur->getField(row,col)) {
				return true;
			}
		}
	}
}

extern "C" {
	sqlrbench *new_sqlrelaybench(const char *connectstring,
						const char *db,
						uint64_t queries,
						uint64_t rows,
						uint32_t cols,
						uint32_t colsize,
						uint16_t samples,
						uint64_t rsbs,
						bool debug) {
		return new sqlrelaybench(connectstring,db,queries,
						rows,cols,colsize,
						samples,rsbs,debug);
	}
}
