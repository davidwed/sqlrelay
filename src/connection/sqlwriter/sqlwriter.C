// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

sqlwriter::sqlwriter() {
	debugFunction();
}

sqlwriter::~sqlwriter() {
	debugFunction();
}

bool sqlwriter::write(xmldom *tree, stringbuffer *output) {
	debugFunction();
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

	// append a space afterward if it's got siblings or children
	if (!node->getFirstTagChild()->isNullNode() ||
		!node->getNextTagSibling()->isNullNode()) {
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
	return true;
}

const char * const *sqlwriter::baseElements() {
	debugFunction();
	static const char *baseelements[]={
		sqlelement::name,
		sqlelement::type,
		sqlelement::size,
		sqlelement::value,
		sqlelement::options,
		sqlelement::verbatim,

		// create query...
		sqlelement::create_query,

		// table...
		sqlelement::table,
		sqlelement::temporary,
		sqlelement::if_not_exists,

		// column definitions...
		sqlelement::columns,
		sqlelement::column,
		sqlelement::values,
		sqlelement::length,
		sqlelement::scale,

		// constraints...
		sqlelement::constraints,
		sqlelement::unsigned_constraint,
		sqlelement::zerofill,
		sqlelement::binary,
		sqlelement::character_set,
		sqlelement::collate,
		sqlelement::nullable,
		sqlelement::not_nullable,
		sqlelement::default_value,
		sqlelement::auto_increment,
		sqlelement::unique_key,
		sqlelement::primary_key,
		sqlelement::key,
		sqlelement::comment,
		sqlelement::column_format,
		sqlelement::references,
		sqlelement::match,
		sqlelement::on_delete,
		sqlelement::on_update,

		// table options...
		sqlelement::table_options,

		// partition options...
		sqlelement::partition_options,


		// drop...
		sqlelement::drop_query,


		// insert...
		sqlelement::insert_query,
		sqlelement::into,


		// update...
		sqlelement::update_query,


		// delete...
		sqlelement::delete_query,


		// select...
		sqlelement::select_query,
		sqlelement::unique,
		sqlelement::distinct,
		sqlelement::from,
		sqlelement::where,
		sqlelement::order_by,
		sqlelement::group_by,
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
	if (!charstring::compare(nodename,sqlelement::name)) {
		return name(node,output);
	} else if (!charstring::compare(nodename,sqlelement::type)) {
		return type(node,output);
	} else if (!charstring::compare(nodename,sqlelement::size)) {
		return size(node,output);
	} else if (!charstring::compare(nodename,sqlelement::value)) {
		return value(node,output);
	} else if (!charstring::compare(nodename,sqlelement::options)) {
		return options(node,output);
	} else if (!charstring::compare(nodename,sqlelement::verbatim)) {
		return verbatim(node,output);

	// create query...
	} else if (!charstring::compare(nodename,sqlelement::create_query)) {
		return createQuery(node,output);

	// table...
	} else if (!charstring::compare(nodename,sqlelement::table)) {
		return table(node,output);
	} else if (!charstring::compare(nodename,sqlelement::temporary)) {
		return temporary(node,output);
	} else if (!charstring::compare(nodename,sqlelement::if_not_exists)) {
		return ifNotExists(node,output);

	// column definitions...
	} else if (!charstring::compare(nodename,sqlelement::columns)) {
		return columns(node,output);
	} else if (!charstring::compare(nodename,sqlelement::column)) {
		return column(node,output);
	} else if (!charstring::compare(nodename,sqlelement::values)) {
		return values(node,output);
	} else if (!charstring::compare(nodename,sqlelement::length)) {
		return length(node,output);
	} else if (!charstring::compare(nodename,sqlelement::scale)) {
		return scale(node,output);

	// constraints...
	} else if (!charstring::compare(nodename,sqlelement::constraints)) {
		return constraints(node,output);
	} else if (!charstring::compare(nodename,sqlelement::unsigned_constraint)) {
		return unsignedConstraint(node,output);
	} else if (!charstring::compare(nodename,sqlelement::zerofill)) {
		return zerofill(node,output);
	} else if (!charstring::compare(nodename,sqlelement::binary)) {
		return binary(node,output);
	} else if (!charstring::compare(nodename,sqlelement::character_set)) {
		return characterSet(node,output);
	} else if (!charstring::compare(nodename,sqlelement::collate)) {
		return collate(node,output);
	} else if (!charstring::compare(nodename,sqlelement::nullable)) {
		return nullable(node,output);
	} else if (!charstring::compare(nodename,sqlelement::not_nullable)) {
		return notNullable(node,output);
	} else if (!charstring::compare(nodename,sqlelement::default_value)) {
		return defaultValue(node,output);
	} else if (!charstring::compare(nodename,sqlelement::auto_increment)) {
		return autoIncrement(node,output);
	} else if (!charstring::compare(nodename,sqlelement::unique_key)) {
		return uniqueKey(node,output);
	} else if (!charstring::compare(nodename,sqlelement::primary_key)) {
		return primaryKey(node,output);
	} else if (!charstring::compare(nodename,sqlelement::key)) {
		return key(node,output);
	} else if (!charstring::compare(nodename,sqlelement::comment)) {
		return comment(node,output);
	} else if (!charstring::compare(nodename,sqlelement::column_format)) {
		return columnFormat(node,output);
	} else if (!charstring::compare(nodename,sqlelement::references)) {
		return references(node,output);
	} else if (!charstring::compare(nodename,sqlelement::match)) {
		return match(node,output);
	} else if (!charstring::compare(nodename,sqlelement::on_delete)) {
		return onDelete(node,output);
	} else if (!charstring::compare(nodename,sqlelement::on_update)) {
		return onUpdate(node,output);

	// on commit clause...
	} else if (!charstring::compare(nodename,sqlelement::on_commit)) {
		return onCommit(node,output);

	// table options...
	} else if (!charstring::compare(nodename,sqlelement::table_options)) {
		return tableOptions(node,output);

	// partition options...
	} else if (!charstring::compare(nodename,sqlelement::partition_options)) {
		return partitionOptions(node,output);


	// drop...
	} else if (!charstring::compare(nodename,sqlelement::drop_query)) {
		return dropQuery(node,output);


	// insert...
	} else if (!charstring::compare(nodename,sqlelement::insert_query)) {
		return insertQuery(node,output);
	} else if (!charstring::compare(nodename,sqlelement::into)) {
		return into(node,output);


	// update...
	} else if (!charstring::compare(nodename,sqlelement::update_query)) {
		return updateQuery(node,output);


	// delete...
	} else if (!charstring::compare(nodename,sqlelement::delete_query)) {
		return deleteQuery(node,output);


	// select...
	} else if (!charstring::compare(nodename,sqlelement::select_query)) {
		return selectQuery(node,output);
	} else if (!charstring::compare(nodename,sqlelement::unique)) {
		return unique(node,output);
	} else if (!charstring::compare(nodename,sqlelement::distinct)) {
		return distinct(node,output);
	}
	return true;
}

bool sqlwriter::handleEnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,sqlelement::type)) {
		return endType(node,output);
	} else if (!charstring::compare(nodename,sqlelement::size)) {
		return endSize(node,output);
	} else if (!charstring::compare(nodename,sqlelement::options)) {
		return endOptions(node,output);

	// column definitions...
	} else if (!charstring::compare(nodename,sqlelement::columns)) {
		return endColumns(node,output);
	} else if (!charstring::compare(nodename,sqlelement::column)) {
		return endColumn(node,output);
	} else if (!charstring::compare(nodename,sqlelement::values)) {
		return endValues(node,output);
	}
	return true;
}

bool sqlwriter::outputValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(node->getAttributeValue(sqlelement::value));
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
