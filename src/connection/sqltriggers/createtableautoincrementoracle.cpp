// Copyright (c) 1999-2012  David Muse
// See the file COPYING for more information

#include <sqltriggers/createtableautoincrementoracle.h>
#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <debugprint.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

extern "C" {
	sqltrigger	*new_createtableautoincrementoracle(
					xmldomnode *parameters) {
		return new createtableautoincrementoracle(parameters);
	}
}

createtableautoincrementoracle::createtableautoincrementoracle(
			xmldomnode *parameters) : sqltrigger(parameters) {
}

bool createtableautoincrementoracle::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree,
					bool before,
					bool success) {
	debugFunction();

	// create...
	xmldomnode	*node=querytree->getRootNode()->
					getFirstTagChild(sqlparser::_create);
	if (node->isNullNode()) {
		return false;
	}

	// table...
	xmldomnode	*tablenode=node->getFirstTagChild(sqlparser::_table);
	if (node->isNullNode()) {
		return false;
	}

	// table name database and schema...
	node=tablenode->getFirstTagChild(sqlparser::_table_name_database);
	const char	*database=node->getAttributeValue(sqlparser::_value);
	node=tablenode->getFirstTagChild(sqlparser::_table_name_schema);
	const char	*schema=node->getAttributeValue(sqlparser::_value);

	// table name...
	node=tablenode->getFirstTagChild(sqlparser::_table_name_table);
	if (node->isNullNode()) {
		return false;
	}
	const char	*table=node->getAttributeValue(sqlparser::_value);

	// columns
	node=tablenode->getFirstTagChild(sqlparser::_columns);
	if (node->isNullNode()) {
		return false;
	}

	// for each column...
	for (xmldomnode *columnnode=node->getFirstTagChild(sqlparser::_column);
		!columnnode->isNullNode();
		columnnode=columnnode->getNextTagSibling(sqlparser::_column)) {

		// constraints...
		xmldomnode	*constraintsnode=
			columnnode->getFirstTagChild(
				sqlparser::_constraints);
		if (constraintsnode->isNullNode()) {
			continue;
		}

		// auto_increment...
		xmldomnode	*autoincnode=
			constraintsnode->getFirstTagChild(
				sqlparser::_auto_increment);
		if (autoincnode->isNullNode()) {
			continue;
		}

		// column name
		xmldomnode	*namenode=
			columnnode->getFirstTagChild(
				sqlparser::_name);
		if (namenode->isNullNode()) {
			continue;
		}
		const char	*columnname=
				namenode->getAttributeValue(sqlparser::_value);

		createSequenceAndTrigger(sqlrcon,sqlrcur,
					database,schema,table,columnname);
	}

	return true;
}

bool createtableautoincrementoracle::createSequenceAndTrigger(
						sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *database,
						const char *schema,
						const char *table,
						const char *columnname) {
	debugFunction();

	// query to create the table-sequence map table
	stringbuffer	query;
	query.append("create table "
			"	autoincrement_sequences "
			"	(tablename varchar(1024), "
			"	sequencename varchar(1024))");

	// create the table-sequence map table
	runQuery(sqlrcon,query.getString(),query.getStringLength());

	// sequence name
	stringbuffer	seqname;
	if (database) {
		seqname.append(database)->append('.');
	}
	if (schema) {
		seqname.append(schema)->append('.');
	}
	seqname.append("seq_")->append(table);
	seqname.append('_')->append(columnname);

	// query to add the sequence to the map table
	query.clear();
	query.append("insert into autoincrement_sequences values ('");
	if (database) {
		query.append(database)->append('.');
	}
	if (schema) {
		query.append(schema)->append('.');
	}
	query.append(table);
	query.append("','");
	query.append(seqname.getString());
	query.append("')");

	// add the sequence to the map table
	runQuery(sqlrcon,query.getString(),query.getStringLength());
	
	// query to create the sequence
	query.clear();
	query.append("create sequence ");
	query.append(seqname.getString());
	query.append(" minvalue 1");
	query.append(" maxvalue 999999999999999999999999999");
	query.append(" start with 1");
	query.append(" increment by 1 ");
	query.append(" cache 20");

	// create the sequence
	runQuery(sqlrcon,query.getString(),query.getStringLength());

	// trigger name
	stringbuffer	trgname;
	if (database) {
		trgname.append(database)->append('.');
	}
	if (schema) {
		trgname.append(schema)->append('.');
	}
	trgname.append("trg_")->append(table);
	trgname.append('_')->append(columnname);

	// query to create the trigger
	query.clear();
	query.append("create or replace trigger ");
	query.append(trgname.getString());
	query.append(" before insert on ");
	query.append(table);
	query.append(" for each row ");
	query.append("	declare ");
	query.append("		v_newval number(12) := 0; ");
	query.append("		v_incval number(12) := 0; ");
	query.append("begin ");
	query.append("	if (:new.")->append(columnname)->append("=0) or ");
	query.append("		(:new.")->append(columnname)->append(" is null) then ");
	query.append("		select ");
	query.append("			")->append(seqname.getString())->append(".nextval ");
	query.append("		into ");
	query.append("			v_newval ");
	query.append("		from ");
	query.append("			dual; ");
	query.append("	else ");
	query.append("		v_newval := :new.")->append(columnname)->append("; ");
	query.append("		v_incval := 1; ");
	query.append("		loop ");
	query.append("			exit when v_incval>=v_newval; ");
	query.append("			select ");
	query.append("			")->append(seqname.getString())->append(".nextval ");
	query.append("			into ");
	query.append("				v_incval ");
	query.append("			from ");
	query.append("				dual; ");
	query.append("		end loop; ");
	query.append("	end if; ");
	query.append("	serialpkg.set_last_serial(v_newval); ");
	query.append("	:new.")->append(columnname)->append(" := v_newval; ");
	query.append("end;");

	// create the trigger
	runQuery(sqlrcon,query.getString(),query.getStringLength());

	return true;
}

bool createtableautoincrementoracle::runQuery(sqlrconnection_svr *sqlrcon,
							const char *query,
							uint32_t length) {
	debugFunction();

	if (sqlrcon->debugtriggers) {
		printf("running trigger:\n%s\n",query);
	}

	bool	retval=false;

	sqlrcursor_svr	*cur=sqlrcon->initCursorUpdateStats();
	if (cur->openCursorInternal(sqlrcon->cursorcount+1) &&
		cur->prepareQuery(query,length) &&
		sqlrcon->executeQueryUpdateStats(cur,query,length,true)) {
		// success...
		retval=true;
		if (sqlrcon->debugtriggers) {
			printf("success\n");
		}
	} else {
		// error...
		if (sqlrcon->debugtriggers) {
			const char	*err;
			int64_t		errno;
			bool		liveconn;
			cur->errorMessage(&err,&errno,&liveconn);
			printf("error:\n%s\n",err);
		}
	}
	if (sqlrcon->debugtriggers) {
		printf("\n");
	}
	cur->cleanUpData(true,true);
	cur->closeCursor();
	sqlrcon->deleteCursorUpdateStats(cur);
	return retval;
}
