// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrservercontroller.h>
#include <sqlrelay/sqlrserverconnection.h>
#include <sqlrelay/sqlrservercursor.h>
#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrtrigger.h>
#include <debugprint.h>

class droptableautoincrementoracle : public sqlrtrigger {
	public:
			droptableautoincrementoracle(
					xmldomnode *parameters, bool debug);
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success);
	private:
		bool	dropSequences(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *database,
						const char *schema,
						const char *tablename);
		bool	dropSequence(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *sequencename);
		bool	deleteSequence(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *sequencename);
};

droptableautoincrementoracle::droptableautoincrementoracle(
					xmldomnode *parameters, bool debug) :
					sqlrtrigger(parameters,debug) {
}

bool droptableautoincrementoracle::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success) {
	debugFunction();

	// drop...
	xmldomnode	*node=querytree->getRootNode()->
					getFirstTagChild(sqlreparser::_drop);
	if (node->isNullNode()) {
		return false;
	}

	// table...
	node=node->getFirstTagChild(sqlreparser::_table);
	if (node->isNullNode()) {
		return false;
	}

	// table name list...
	node=node->getFirstTagChild(sqlreparser::_table_name_list);
	if (node->isNullNode()) {
		return false;
	}

	// for each table name...
	for (xmldomnode *listitemnode=
			node->getFirstTagChild(
				sqlreparser::_table_name_list_item);
		!listitemnode->isNullNode();
			listitemnode=listitemnode->
				getNextTagSibling(
					sqlreparser::_table_name_list_item)) {

		// table name database and schema...
		node=listitemnode->getFirstTagChild(
					sqlreparser::_table_name_database);
		const char	*database=
				node->getAttributeValue(sqlreparser::_value);
		node=listitemnode->getFirstTagChild(
					sqlreparser::_table_name_schema);
		const char	*schema=
				node->getAttributeValue(sqlreparser::_value);

		// table name...
		node=listitemnode->getFirstTagChild(
					sqlreparser::_table_name_table);
		if (node->isNullNode()) {
			continue;
		}
		const char	*table=
				node->getAttributeValue(sqlreparser::_value);
		if (!table) {
			continue;
		}

		// drop the sequences
		dropSequences(sqlrcon,sqlrcur,database,schema,table);
	}

	return true;
}

bool droptableautoincrementoracle::dropSequences(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
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
	sqlrservercursor	*cur=sqlrcon->cont->newCursor();
	if (sqlrcon->cont->open(cur) &&
		sqlrcon->cont->prepareQuery(
			cur,query.getString(),query.getStringLength()) &&
		sqlrcon->cont->executeQuery(cur)) {

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

bool droptableautoincrementoracle::dropSequence(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
						const char *sequencename) {
	debugFunction();

	// query to drop the sequence
	stringbuffer	query;
	query.append("drop sequence ")->append(sequencename);

	// drop the sequence
	if (debug) {
		stdoutput.printf("running trigger:\n%s\n",query.getString());
	}
	sqlrservercursor	*cur=sqlrcon->cont->newCursor();
	if (sqlrcon->cont->open(cur) &&
		sqlrcon->cont->prepareQuery(
			cur,query.getString(),query.getStringLength()) &&
		sqlrcon->cont->executeQuery(cur)) {

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

bool droptableautoincrementoracle::deleteSequence(sqlrserverconnection *sqlrcon,
						sqlrservercursor *sqlrcur,
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
	sqlrservercursor	*cur=sqlrcon->cont->newCursor();
	if (sqlrcon->cont->open(cur) &&
		sqlrcon->cont->prepareQuery(
			cur,query.getString(),query.getStringLength()) &&
		sqlrcon->cont->executeQuery(cur)) {

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
