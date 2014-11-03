// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrcontroller.h>
#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtrigger.h>
#include <debugprint.h>

class droptableautoincrementoracle : public sqlrtrigger {
	public:
			droptableautoincrementoracle(
					xmldomnode *parameters, bool debug);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success);
	private:
		bool	dropSequences(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *database,
						const char *schema,
						const char *tablename);
		bool	dropSequence(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *sequencename);
		bool	deleteSequence(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *sequencename);
};

droptableautoincrementoracle::droptableautoincrementoracle(
					xmldomnode *parameters, bool debug) :
					sqlrtrigger(parameters,debug) {
}

bool droptableautoincrementoracle::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success) {
	debugFunction();

	// drop...
	xmldomnode	*node=querytree->getRootNode()->
					getFirstTagChild(sqlparser::_drop);
	if (node->isNullNode()) {
		return false;
	}

	// table...
	node=node->getFirstTagChild(sqlparser::_table);
	if (node->isNullNode()) {
		return false;
	}

	// table name list...
	node=node->getFirstTagChild(sqlparser::_table_name_list);
	if (node->isNullNode()) {
		return false;
	}

	// for each table name...
	for (xmldomnode *listitemnode=
			node->getFirstTagChild(
				sqlparser::_table_name_list_item);
		!listitemnode->isNullNode();
			listitemnode=listitemnode->
				getNextTagSibling(
					sqlparser::_table_name_list_item)) {

		// table name database and schema...
		node=listitemnode->getFirstTagChild(
					sqlparser::_table_name_database);
		const char	*database=
				node->getAttributeValue(sqlparser::_value);
		node=listitemnode->getFirstTagChild(
					sqlparser::_table_name_schema);
		const char	*schema=
				node->getAttributeValue(sqlparser::_value);

		// table name...
		node=listitemnode->getFirstTagChild(
					sqlparser::_table_name_table);
		if (node->isNullNode()) {
			continue;
		}
		const char	*table=
				node->getAttributeValue(sqlparser::_value);
		if (!table) {
			continue;
		}

		// drop the sequences
		dropSequences(sqlrcon,sqlrcur,database,schema,table);
	}

	return true;
}

bool droptableautoincrementoracle::dropSequences(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *database,
						const char *schema,
						const char *table) {
	debugFunction();

	// query to get the sequences from the table-sequence map table
	stringbuffer	query;
	query.append("select * from autoincrement_sequences ");
	query.append("where tablename='");
	if (database) {
		query.append(database)->append('.');
	}
	if (schema) {
		query.append(schema)->append('.');
	}
	query.append(table)->append("'");

	// get the sequences from the table-sequence map table
	if (debug) {
		stdoutput.printf("running trigger:\n%s\n",query.getString());
	}
	sqlrcursor_svr	*cur=sqlrcon->cont->newCursor();
	if (sqlrcon->cont->open(cur) &&
		sqlrcon->cont->prepareQuery(
			cur,query.getString(),query.getStringLength()) &&
		sqlrcon->cont->executeQuery(
			cur,query.getString(),query.getStringLength())) {

		// success...
		if (debug) {
			stdoutput.printf("success\n\n");
		}

		// get the rows...
		while (sqlrcon->cont->fetchRow(cur)) {

			// get the second field
			const char	*field=NULL;
			uint64_t	fieldlength=0;
			bool		blob=false;
			bool		null=false;
			sqlrcon->cont->getField(
				cur,1,&field,&fieldlength,&blob,&null);

			// drop the sequence
			dropSequence(sqlrcon,sqlrcur,field);

			sqlrcon->cont->nextRow(cur);
		}

	} else {

		// FIXME: If this failed, it might be because the table was
		// created by explicitly specifying the current database and
		// schema but dropped by just specifying the table or
		// schema.table.  We need to try again, specifying the current
		// schema and database explicitly.

		// error...
		if (debug) {
			uint32_t	errorlength;
			int64_t		errnum;
			bool		liveconnection;
			sqlrcon->cont->errorMessage(cur,
					sqlrcon->cont->getErrorBuffer(cur),
					sqlrcon->cont->cfgfl->
						getMaxErrorLength(),
					&errorlength,
					&errnum,
					&liveconnection);
			sqlrcon->cont->setErrorLength(cur,errorlength);
			sqlrcon->cont->setErrorNumber(cur,errnum);
			sqlrcon->cont->setLiveConnection(cur,liveconnection);
			stdoutput.printf("error:\n%s\n",
					sqlrcon->cont->getErrorBuffer());
		}
	}
	sqlrcon->cont->closeResultSet(cur);
	sqlrcon->cont->close(cur);
	sqlrcon->cont->deleteCursor(cur);

	return true;
}

bool droptableautoincrementoracle::dropSequence(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *sequencename) {
	debugFunction();

	// query to drop the sequence
	stringbuffer	query;
	query.append("drop sequence ")->append(sequencename);

	// drop the sequence
	if (debug) {
		stdoutput.printf("running trigger:\n%s\n",query.getString());
	}
	sqlrcursor_svr	*cur=sqlrcon->cont->newCursor();
	if (sqlrcon->cont->open(cur) &&
		sqlrcon->cont->prepareQuery(
			cur,query.getString(),query.getStringLength()) &&
		sqlrcon->cont->executeQuery(
			cur,query.getString(),query.getStringLength())) {

		// success...
		if (debug) {
			stdoutput.printf("success\n\n");
		}

		// delete the sequence from the map
		deleteSequence(sqlrcon,sqlrcur,sequencename);
	} else {
		// error...
		if (debug) {
			uint32_t	errorlength;
			int64_t		errnum;
			bool		liveconnection;
			sqlrcon->cont->errorMessage(cur,
					sqlrcon->cont->getErrorBuffer(cur),
					sqlrcon->cont->cfgfl->
						getMaxErrorLength(),
					&errorlength,
					&errnum,
					&liveconnection);
			sqlrcon->cont->setErrorLength(cur,errorlength);
			sqlrcon->cont->setErrorNumber(cur,errnum);
			sqlrcon->cont->setLiveConnection(cur,liveconnection);
			stdoutput.printf("error:\n%s\n",
					sqlrcon->cont->getErrorBuffer(cur));
		}
	}
	sqlrcon->cont->closeResultSet(cur);
	sqlrcon->cont->close(cur);
	sqlrcon->cont->deleteCursor(cur);

	return true;
}

bool droptableautoincrementoracle::deleteSequence(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *sequencename) {
	debugFunction();

	// query to delete the sequence
	stringbuffer	query;
	query.append("delete from autoincrement_sequences ");
	query.append("where sequencename='");
	query.append(sequencename)->append("'");

	// delete the sequence
	if (debug) {
		stdoutput.printf("running trigger:\n%s\n",query.getString());
	}
	sqlrcursor_svr	*cur=sqlrcon->cont->newCursor();
	if (sqlrcon->cont->open(cur) &&
		sqlrcon->cont->prepareQuery(
			cur,query.getString(),query.getStringLength()) &&
		sqlrcon->cont->executeQuery(
			cur,query.getString(),query.getStringLength())) {
		// success...
		if (debug) {
			stdoutput.printf("success\n\n");
		}
	} else {
		// error...
		if (debug) {
			uint32_t	errorlength;
			int64_t		errnum;
			bool		liveconnection;
			sqlrcon->cont->errorMessage(cur,
					sqlrcon->cont->getErrorBuffer(cur),
					sqlrcon->cont->cfgfl->
							getMaxErrorLength(),
					&errorlength,
					&errnum,
					&liveconnection);
			sqlrcon->cont->setErrorLength(cur,errorlength);
			sqlrcon->cont->setErrorNumber(cur,errnum);
			sqlrcon->cont->setLiveConnection(cur,liveconnection);
			stdoutput.printf("error:\n%s\n",
					sqlrcon->cont->getErrorBuffer(cur));
		}
	}
	sqlrcon->cont->closeResultSet(cur);
	sqlrcon->cont->close(cur);
	sqlrcon->cont->deleteCursor(cur);

	return true;
}

extern "C" {
	sqlrtrigger	*new_droptableautoincrementoracle(
				xmldomnode *parameters, bool debug) {
		return new droptableautoincrementoracle(parameters,debug);
	}
}
