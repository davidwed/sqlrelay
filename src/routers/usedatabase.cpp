// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <sqlrelay/sqlrclient.h>
#include <rudiments/linkedlist.h>
#include <rudiments/regularexpression.h>

#include <defines.h>

class conndb {
	public:
	conndb(const char *dbname,
			const char *connid,
			sqlrconnection *sqlrcon);
	~conndb();
	char		*dbname;
	const char	*connid;
	sqlrconnection	*sqlrcon;
};

conndb::conndb(const char *dbname,
			const char *connid,
			sqlrconnection *sqlrcon) {
	this->dbname=charstring::duplicate(dbname);
	this->connid=connid;
	this->sqlrcon=sqlrcon;
}

conndb::~conndb() {
	delete[] dbname;
}


class SQLRSERVER_DLLSPEC sqlrrouter_usedatabase : public sqlrrouter {
	public:
		sqlrrouter_usedatabase(sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters);

		const char	*route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn);
	private:
		void		buildDictionary();
		const char	*mapDbName(const char *sqlrconid,
						const char *dbname);

		dictionary<char *,conndb *>	dbs;

		bool	initialized;
};

sqlrrouter_usedatabase::sqlrrouter_usedatabase(sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters) :
					sqlrrouter(cont,rs,parameters) {

	initialized=false;

	dbs.setManageArrayKeys(true);
	dbs.setManageValues(true);
}

const char *sqlrrouter_usedatabase::route(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char **err,
						int64_t *errn) {

	// initialze the return value to the current connection
	const char	*retval=getRouters()->getCurrentConnectionId();

	if (!sqlrcon || !sqlrcur) {
		return NULL;
	}

	// is the query a "use database" query?
	char	*query=sqlrcur->getQueryBuffer();
	if (charstring::compare(query,"use ",4)) {
		return retval;
	}

	// get the db name (alias)
	const char	*dbalias=query+4;

	// initialize the db dictionary, if necessary
	if (!initialized) {
		buildDictionary();
		initialized=true;
	}

	debugStart("route");
	debugWrite("%s",query);

	// get the id of the connection that hosts the db
	conndb		*cdb=NULL;
	if (dbs.getValue((char *)dbalias,&cdb)) {

		stdoutput.printf("%s to %s at %s ",
				dbalias,cdb->dbname,cdb->connid);

		// select the specified db
		if (cdb->sqlrcon->selectDatabase(cdb->dbname)) {
			debugWrite("(success)");
			retval=cdb->connid;
		} else {
			*err=cdb->sqlrcon->errorMessage();
			*errn=cdb->sqlrcon->errorNumber();
			debugWrite("(failed)");
			retval=NULL;
		}
	} else {
		*err=SQLR_ERROR_DBNOTFOUND_STRING;
		*errn=SQLR_ERROR_DBNOTFOUND;
		debugWrite("%s not found",dbalias);
		retval=NULL;
	}

	debugEnd();

	// the original "use database" query shouldn't actually be run now,
	// so disable it by setting the size of the query to 0
	sqlrcur->setQuerySize(0);

	return retval;
}

void sqlrrouter_usedatabase::buildDictionary() {

	debugStart("build dictionary");

	// run through the connections...
	for (uint16_t i=0; i<getRouters()->getConnectionCount(); i++) {

		const char	*sqlrconid=getRouters()->getConnectionIds()[i];
		sqlrconnection	*sqlrcon=getRouters()->getConnections()[i];
		sqlrcursor	sqlrcur(sqlrcon);

		// get the db list
		if (!sqlrcur.getDatabaseList(NULL)) {
			continue;
		}

		// add an entry to the dbs dictionary for each connid/db
		for (uint64_t j=0; j<sqlrcur.rowCount(); j++) {

			const char	*dbname=sqlrcur.getField(j,(uint32_t)0);
			const char	*dbalias=mapDbName(sqlrconid,dbname);

			conndb	*cdb=new conndb(dbname,sqlrconid,sqlrcon);
			dbs.setValue(charstring::duplicate(dbalias),cdb);

			stdoutput.printf("%s -> %s@%s\n",
					dbalias,dbname,sqlrconid);
		}
	}

	debugEnd();
}

const char *sqlrrouter_usedatabase::mapDbName(const char *sqlrconid,
							const char *dbname) {

	// run through the map...
	for (domnode *map=getParameters()->getFirstTagChild("map");
					!map->isNullNode();
					map=map->getNextTagSibling("map")) {

		// if we get a connid/dbname match then return the alias
		if (!charstring::compare(
				map->getAttributeValue("connectionid"),
				sqlrconid) &&
			!charstring::compare(
				map->getAttributeValue("db"),
				dbname)) {
			return map->getAttributeValue("alias");
		}
	}

	// otherwise just return the dbname that was passed in
	return dbname;
}

extern "C" {
	SQLRSERVER_DLLSPEC sqlrrouter *new_sqlrrouter_usedatabase(
						sqlrservercontroller *cont,
						sqlrrouters *rs,
						domnode *parameters) {
		return new sqlrrouter_usedatabase(cont,rs,parameters);
	}
}
