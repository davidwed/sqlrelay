// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <sqltranslation.h>
#include <debugprint.h>

class oracleunsupported : public sqltranslation {
	public:
			oracleunsupported(sqltranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
	private:
		void	replaceIsolationLevel(xmldomnode *node);
};

oracleunsupported::oracleunsupported(sqltranslations *sqlts,
						xmldomnode *parameters) :
					sqltranslation(sqlts,parameters) {
}

bool oracleunsupported::run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	xmldomnode	*root=querytree->getRootNode();

	// unsupported constraints
	xmldomnode	*table=root->getFirstTagChild(sqlparser::_create)->
					getFirstTagChild(sqlparser::_table);
	xmldomnode	*columns=table->getFirstTagChild(sqlparser::_columns);
	for (xmldomnode *col=columns->getFirstTagChild(sqlparser::_column);
			!col->isNullNode();
			col=col->getNextTagSibling(sqlparser::_column)) {

		xmldomnode	*constraints=
			col->getFirstTagChild(sqlparser::_constraints);
		if (constraints->isNullNode()) {
			continue;
		}

		const char	*unsupported[]={
			sqlparser::_unsigned,
			sqlparser::_zerofill,
			sqlparser::_binary,
			sqlparser::_character_set,
			sqlparser::_collate,
			sqlparser::_auto_increment,
			sqlparser::_key,
			sqlparser::_comment,
			sqlparser::_column_format,
			sqlparser::_match,
			sqlparser::_on_delete,
			sqlparser::_on_update,
			NULL
		};
		for (const char * const *cstr=unsupported; *cstr; cstr++) {
			xmldomnode	*constraint=
				constraints->getFirstTagChild(*cstr);
			if (!constraint->isNullNode()) {
				constraints->deleteChild(constraint);
			}
		}
	}

	// create with no log
	xmldomnode	*withnolog=
			table->getFirstTagChild(sqlparser::_with_no_log);
	if (!withnolog->isNullNode()) {
		table->deleteChild(withnolog);
	}

	// drop temporary
	xmldomnode	*drop=root->getFirstTagChild(sqlparser::_drop);
	xmldomnode	*temporary=drop->getFirstTagChild(
						sqlparser::_drop_temporary);
	if (!temporary->isNullNode()) {
		drop->deleteChild(temporary);
	}

	// drop restrict
	table=drop->getFirstTagChild(sqlparser::_table);
	xmldomnode	*restrictclause=table->getFirstTagChild(
						sqlparser::_restrict_clause);
	if (!restrictclause->isNullNode()) {
		table->deleteChild(restrictclause);
	}

	// set global
	xmldomnode	*set=root->getFirstTagChild(sqlparser::_set);
	xmldomnode	*global=set->getFirstTagChild(sqlparser::_set_global);
	if (!global->isNullNode()) {
		set->deleteChild(global);
	}

	// set session
	xmldomnode	*session=set->getFirstTagChild(sqlparser::_set_session);
	if (!session->isNullNode()) {
		set->deleteChild(session);
	}

	return true;
}

extern "C" {
	sqltranslation	*new_oracleunsupported(sqltranslations *sqlts,
					xmldomnode *parameters) {
		return new oracleunsupported(sqlts,parameters);
	}
}
