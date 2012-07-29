// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltriggers/droptableautoincrementoracle.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <debugprint.h>

extern "C" {
	sqltrigger	*new_droptableautoincrementoracle(
					xmldomnode *parameters) {
		return new droptableautoincrementoracle(parameters);
	}
}

droptableautoincrementoracle::droptableautoincrementoracle(
			xmldomnode *parameters) : sqltrigger(parameters) {
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

		// table name...
		node=listitemnode->getFirstTagChild(sqlparser::_table_name);
		if (node->isNullNode()) {
			continue;
		}
		const char	*tablename=
				node->getAttributeValue(sqlparser::_value);
		if (!tablename) {
			continue;
		}

		// drop the sequences
		dropSequences(sqlrcon,sqlrcur,tablename);
	}
	return true;
}

bool droptableautoincrementoracle::dropSequences(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *tablename) {
	debugFunction();

	// query to get the sequences from the table-sequence map table
	stringbuffer	query;
	query.append("select * from autoincrement_sequences ");
	query.append("where tablename='")->append(tablename)->append("'");

	// get the sequences from the table-sequence map table
	if (sqlrcon->debugtriggers) {
		printf("running trigger:\n%s\n",query.getString());
	}
	sqlrcursor_svr	*cur=sqlrcon->initCursorUpdateStats();
	if (cur->openCursorInternal(sqlrcon->cursorcount+1) &&
		cur->prepareQuery(query.getString(),query.getStringLength()) &&
		sqlrcon->executeQueryUpdateStats(cur,query.getString(),
						query.getStringLength(),true)) {

		// success...
		if (sqlrcon->debugtriggers) {
			printf("success\n\n");
		}

		// get the rows...
		while (cur->fetchRow()) {

			// get the second field
			const char	*field=NULL;
			uint64_t	fieldlength=0;
			bool		blob=false;
			bool		null=false;
			cur->getField(1,&field,&fieldlength,&blob,&null);

			// drop the sequence
			dropSequence(sqlrcon,sqlrcur,field);

			cur->nextRow();
		}
	} else {
		// error...
		if (sqlrcon->debugtriggers) {
			const char	*err;
			int64_t		errno;
			bool		liveconn;
			cur->errorMessage(&err,&errno,&liveconn);
			printf("error:\n%s\n\n",err);
		}
	}
	cur->cleanUpData(true,true);
	cur->closeCursor();
	sqlrcon->deleteCursorUpdateStats(cur);

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
	if (sqlrcon->debugtriggers) {
		printf("running trigger:\n%s\n",query.getString());
	}
	sqlrcursor_svr	*cur=sqlrcon->initCursorUpdateStats();
	if (cur->openCursorInternal(sqlrcon->cursorcount+1) &&
		cur->prepareQuery(query.getString(),query.getStringLength()) &&
		sqlrcon->executeQueryUpdateStats(cur,query.getString(),
						query.getStringLength(),true)) {
		// success...
		if (sqlrcon->debugtriggers) {
			printf("success\n\n");
		}

		// delete the sequence from the map
		deleteSequence(sqlrcon,sqlrcur,sequencename);
	} else {
		// error...
		if (sqlrcon->debugtriggers) {
			const char	*err;
			int64_t		errno;
			bool		liveconn;
			cur->errorMessage(&err,&errno,&liveconn);
			printf("error:\n%s\n\n",err);
		}
	}
	cur->cleanUpData(true,true);
	cur->closeCursor();
	sqlrcon->deleteCursorUpdateStats(cur);

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
	if (sqlrcon->debugtriggers) {
		printf("running trigger:\n%s\n",query.getString());
	}
	sqlrcursor_svr	*cur=sqlrcon->initCursorUpdateStats();
	if (cur->openCursorInternal(sqlrcon->cursorcount+1) &&
		cur->prepareQuery(query.getString(),query.getStringLength()) &&
		sqlrcon->executeQueryUpdateStats(cur,query.getString(),
						query.getStringLength(),true)) {
		// success...
		if (sqlrcon->debugtriggers) {
			printf("success\n\n");
		}
	} else {
		// error...
		if (sqlrcon->debugtriggers) {
			const char	*err;
			int64_t		errno;
			bool		liveconn;
			cur->errorMessage(&err,&errno,&liveconn);
			printf("error:\n%s\n\n",err);
		}
	}
	cur->cleanUpData(true,true);
	cur->closeCursor();
	sqlrcon->deleteCursorUpdateStats(cur);

	return true;
}
