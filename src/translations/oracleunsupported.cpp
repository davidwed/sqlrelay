// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrconnection.h>
#include <sqlrelay/sqlrcursor.h>
#include <sqlrelay/sqlparser.h>
#include <sqlrelay/sqlrtranslation.h>
#include <debugprint.h>

class oracleunsupported : public sqlrtranslation {
	public:
			oracleunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters);
		bool	run(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *querytree);
};

oracleunsupported::oracleunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters) :
					sqlrtranslation(sqlts,parameters) {
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

		for (xmldomnode *con=constraints->getFirstTagChild();
					!con->isNullNode();
					con=con->getNextTagSibling()) {
			for (const char * const *cstr=unsupported;
							*cstr; cstr++) {
				if (!charstring::compare(
						con->getName(),*cstr)) {
					con->setAttributeValue(
						"supported","false");
				}
			}
		}
	}

	// create with no log
	xmldomnode	*withnolog=
			table->getFirstTagChild(sqlparser::_with_no_log);
	if (!withnolog->isNullNode()) {
		withnolog->setAttributeValue("supported","false");
	}

	// drop temporary
	xmldomnode	*drop=root->getFirstTagChild(sqlparser::_drop);
	xmldomnode	*temporary=drop->getFirstTagChild(
						sqlparser::_drop_temporary);
	if (!temporary->isNullNode()) {
		temporary->setAttributeValue("supported","false");
	}

	// drop restrict
	table=drop->getFirstTagChild(sqlparser::_table);
	xmldomnode	*restrictclause=table->getFirstTagChild(
						sqlparser::_restrict_clause);
	if (!restrictclause->isNullNode()) {
		restrictclause->setAttributeValue("supported","false");
	}

	// set global
	xmldomnode	*set=root->getFirstTagChild(sqlparser::_set);
	xmldomnode	*global=set->getFirstTagChild(sqlparser::_set_global);
	if (!global->isNullNode()) {
		global->setAttributeValue("supported","false");
	}

	// set session
	xmldomnode	*session=set->getFirstTagChild(sqlparser::_set_session);
	if (!session->isNullNode()) {
		session->setAttributeValue("supported","false");
	}

	return true;
}

extern "C" {
	sqlrtranslation	*new_oracleunsupported(
					sqlrtranslations *sqlts,
					xmldomnode *parameters) {
		return new oracleunsupported(sqlts,parameters);
	}
}
