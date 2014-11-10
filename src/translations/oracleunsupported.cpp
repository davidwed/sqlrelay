// Copyright (c) 2013  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlreparser.h>
#include <sqlrelay/sqlrserver.h>
#include <debugprint.h>

class oracleunsupported : public sqlrtranslation {
	public:
			oracleunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug);
		bool	usesTree();
		bool	run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree);
};

oracleunsupported::oracleunsupported(sqlrtranslations *sqlts,
						xmldomnode *parameters,
						bool debug) :
				sqlrtranslation(sqlts,parameters,debug) {
}

bool oracleunsupported::usesTree() {
	return true;
}

bool oracleunsupported::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					xmldom *querytree) {
	debugFunction();

	xmldomnode	*root=querytree->getRootNode();

	// unsupported constraints
	xmldomnode	*table=root->getFirstTagChild(sqlreparser::_create)->
					getFirstTagChild(sqlreparser::_table);
	xmldomnode	*columns=table->getFirstTagChild(sqlreparser::_columns);
	for (xmldomnode *col=columns->getFirstTagChild(sqlreparser::_column);
			!col->isNullNode();
			col=col->getNextTagSibling(sqlreparser::_column)) {

		xmldomnode	*constraints=
			col->getFirstTagChild(sqlreparser::_constraints);
		if (constraints->isNullNode()) {
			continue;
		}

		const char	*unsupported[]={
			sqlreparser::_unsigned,
			sqlreparser::_zerofill,
			sqlreparser::_binary,
			sqlreparser::_character_set,
			sqlreparser::_collate,
			sqlreparser::_auto_increment,
			sqlreparser::_key,
			sqlreparser::_comment,
			sqlreparser::_column_format,
			sqlreparser::_match,
			sqlreparser::_on_delete,
			sqlreparser::_on_update,
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
			table->getFirstTagChild(sqlreparser::_with_no_log);
	if (!withnolog->isNullNode()) {
		withnolog->setAttributeValue("supported","false");
	}

	// drop temporary
	xmldomnode	*drop=root->getFirstTagChild(sqlreparser::_drop);
	xmldomnode	*temporary=drop->getFirstTagChild(
						sqlreparser::_drop_temporary);
	if (!temporary->isNullNode()) {
		temporary->setAttributeValue("supported","false");
	}

	// drop restrict
	table=drop->getFirstTagChild(sqlreparser::_table);
	xmldomnode	*restrictclause=table->getFirstTagChild(
						sqlreparser::_restrict_clause);
	if (!restrictclause->isNullNode()) {
		restrictclause->setAttributeValue("supported","false");
	}

	// set global
	xmldomnode	*set=root->getFirstTagChild(sqlreparser::_set);
	xmldomnode	*global=set->getFirstTagChild(sqlreparser::_set_global);
	if (!global->isNullNode()) {
		global->setAttributeValue("supported","false");
	}

	// set session
	xmldomnode	*session=set->getFirstTagChild(sqlreparser::_set_session);
	if (!session->isNullNode()) {
		session->setAttributeValue("supported","false");
	}

	return true;
}

extern "C" {
	sqlrtranslation	*new_oracleunsupported(
					sqlrtranslations *sqlts,
					xmldomnode *parameters,
					bool debug) {
		return new oracleunsupported(sqlts,parameters,debug);
	}
}
