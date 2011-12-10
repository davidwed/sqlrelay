// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

sqlwriter::sqlwriter() {
	debugFunction();
	sqlrcon=NULL;
	sqlrcur=NULL;
}

sqlwriter::~sqlwriter() {
	debugFunction();
}

bool sqlwriter::write(sqlrconnection_svr *sqlrcon, sqlrcursor_svr *sqlrcur,
					xmldom *tree, stringbuffer *output) {
	debugFunction();
	this->sqlrcon=sqlrcon;
	this->sqlrcur=sqlrcur;
	return write(tree->getRootNode()->getFirstTagChild(),output);
}

bool sqlwriter::write(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	// ignore unsupported elements outright
	if (!elementSupported(node->getName())) {
		return true;
	}

	// handle the element
	if (!handleStart(node,output)) {
		return false;
	}

	// append a space afterward if it's got children
	if (!lastWasSpace(output) &&
		!node->getFirstTagChild()->isNullNode()) {
		space(output);
	}

	// handle the children
	for (xmldomnode *child=node->getFirstTagChild();
		!child->isNullNode(); child=child->getNextTagSibling()) {
		if (!write(child,output)) {
			return false;
		}
	}

	// some elements need to do things on the back end, so handle that too
	if (!handleEnd(node,output)) {
		return false;
	}

	// append a space afterward if it's got siblings
	if (!lastWasSpace(output) &&
		!node->getNextTagSibling()->isNullNode()) {
		space(output);
	}
	return true;
}

const char * const *sqlwriter::baseElements() {
	debugFunction();
	static const char *baseelements[]={
		sqlelement::_name,
		sqlelement::_type,
		sqlelement::_size,
		sqlelement::_value,
		sqlelement::_options,
		sqlelement::_verbatim,

		// create query...
		sqlelement::_create,
		sqlelement::_create_temporary,

		// table...
		sqlelement::_table,
		sqlelement::_if_not_exists,

		// column definitions...
		sqlelement::_columns,
		sqlelement::_column,
		sqlelement::_values,
		sqlelement::_length,
		sqlelement::_scale,

		// constraints...
		sqlelement::_constraints,
		sqlelement::_unsigned,
		sqlelement::_zerofill,
		sqlelement::_binary,
		sqlelement::_character_set,
		sqlelement::_collate,
		sqlelement::_null,
		sqlelement::_not_null,
		sqlelement::_default,
		sqlelement::_auto_increment,
		sqlelement::_unique_key,
		sqlelement::_primary_key,
		sqlelement::_key,
		sqlelement::_comment,
		sqlelement::_column_format,
		sqlelement::_references,
		sqlelement::_match,
		sqlelement::_on_delete,
		sqlelement::_on_update,
		sqlelement::_as,


		// drop...
		sqlelement::_drop,
		sqlelement::_drop_temporary,
		sqlelement::_if_exists,
		sqlelement::_table_name_list,
		sqlelement::_table_name_list_item,
		sqlelement::_restrict,
		sqlelement::_cascade,


		// insert...
		sqlelement::_insert,
		sqlelement::_into,


		// update...
		sqlelement::_update,


		// delete...
		sqlelement::_delete,


		// select...
		sqlelement::_select,
		sqlelement::_unique,
		sqlelement::_distinct,
		sqlelement::_from,
		sqlelement::_where,
		sqlelement::_order_by,
		sqlelement::_group_by,
		NULL
	};
	return baseelements;
}

const char * const *sqlwriter::additionalElements() {
	debugFunction();
	static const char *additionalelements[]={
		NULL
	};
	return additionalelements;
}

const char * const *sqlwriter::unsupportedElements() {
	debugFunction();
	static const char *unsupportedelements[]={
		NULL
	};
	return unsupportedelements;
}

bool sqlwriter::elementSupported(const char *element) {
	debugFunction();

	// is it specifically unsupported
	const char * const *elements=unsupportedElements();
	for (uint64_t i=0; elements[i]; i++) {
		if (!charstring::compare(element,elements[i])) {
			return false;
		}
	}

	// is it among the base elements
	elements=baseElements();
	for (uint64_t i=0; elements[i]; i++) {
		if (!charstring::compare(element,elements[i])) {
			return true;
		}
	}

	// is it an additional element supported by the subclass
	elements=additionalElements();
	for (uint64_t i=0; elements[i]; i++) {
		if (!charstring::compare(element,elements[i])) {
			return true;
		}
	}
	return false;
}

bool sqlwriter::handleStart(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,sqlelement::_name)) {
		return name(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_type)) {
		return type(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_size)) {
		return size(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_value)) {
		return value(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_options)) {
		return options(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_verbatim)) {
		return verbatim(node,output);

	// create query...
	} else if (!charstring::compare(nodename,sqlelement::_create)) {
		return createQuery(node,output);
	} else if (!charstring::compare(nodename,
					sqlelement::_create_temporary)) {
		return temporary(node,output);

	// table...
	} else if (!charstring::compare(nodename,sqlelement::_table)) {
		return table(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_if_not_exists)) {
		return ifNotExists(node,output);

	// column definitions...
	} else if (!charstring::compare(nodename,sqlelement::_columns)) {
		return columns(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_column)) {
		return column(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_values)) {
		return values(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_length)) {
		return length(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_scale)) {
		return scale(node,output);

	// constraints...
	} else if (!charstring::compare(nodename,sqlelement::_constraints)) {
		return constraints(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_unsigned)) {
		return unsignedConstraint(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_zerofill)) {
		return zerofill(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_binary)) {
		return binary(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_character_set)) {
		return characterSet(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_collate)) {
		return collate(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_null)) {
		return nullable(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_not_null)) {
		return notNull(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_default)) {
		return defaultValue(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_auto_increment)) {
		return autoIncrement(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_unique_key)) {
		return uniqueKey(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_primary_key)) {
		return primaryKey(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_key)) {
		return key(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_comment)) {
		return comment(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_column_format)) {
		return columnFormat(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_references)) {
		return references(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_match)) {
		return match(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_on_delete)) {
		return onDelete(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_on_update)) {
		return onUpdate(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_on_commit)) {
		return onCommit(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_as)) {
		return as(node,output);


	// drop...
	} else if (!charstring::compare(nodename,sqlelement::_drop)) {
		return dropQuery(node,output);
	} else if (!charstring::compare(nodename,
					sqlelement::_drop_temporary)) {
		return temporary(node,output);
	} else if (!charstring::compare(nodename,
					sqlelement::_if_exists)) {
		return ifExists(node,output);
	} else if (!charstring::compare(nodename,
					sqlelement::_restrict)) {
		return restrictClause(node,output);
	} else if (!charstring::compare(nodename,
					sqlelement::_cascade)) {
		return cascade(node,output);

	// insert...
	} else if (!charstring::compare(nodename,sqlelement::_insert)) {
		return insertQuery(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_into)) {
		return into(node,output);


	// update...
	} else if (!charstring::compare(nodename,sqlelement::_update)) {
		return updateQuery(node,output);


	// delete...
	} else if (!charstring::compare(nodename,sqlelement::_delete)) {
		return deleteQuery(node,output);


	// select...
	} else if (!charstring::compare(nodename,sqlelement::_select)) {
		return selectQuery(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_unique)) {
		return unique(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_distinct)) {
		return distinct(node,output);
	}
	return true;
}

bool sqlwriter::handleEnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,sqlelement::_type)) {
		return endType(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_size)) {
		return endSize(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_options)) {
		return endOptions(node,output);

	// column definitions...
	} else if (!charstring::compare(nodename,sqlelement::_columns)) {
		return endColumns(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_column)) {
		return endColumn(node,output);
	} else if (!charstring::compare(nodename,sqlelement::_values)) {
		return endValues(node,output);

	// drop...
	} else if (!charstring::compare(nodename,
					sqlelement::_table_name_list_item)) {
		return endTableNameListItem(node,output);
	}
	return true;
}

bool sqlwriter::outputValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(node->getAttributeValue(sqlelement::_value));
	return true;
}

bool sqlwriter::space(stringbuffer *output) {
	debugFunction();
	output->append(" ");
	return true;
}

bool sqlwriter::comma(stringbuffer *output) {
	debugFunction();
	output->append(",");
	return true;
}

bool sqlwriter::leftParen(stringbuffer *output) {
	debugFunction();
	output->append("(");
	return true;
}

bool sqlwriter::rightParen(stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlwriter::hasSibling(xmldomnode *node) {
	debugFunction();
	return !node->getNextTagSibling()->isNullNode();
}

bool sqlwriter::lastWasSpace(stringbuffer *output) {
	debugFunction();
	size_t	length=output->getStringLength();
	return (length && output->getString()[length-1]==' ');
}
