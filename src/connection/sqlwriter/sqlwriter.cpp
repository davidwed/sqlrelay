// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <sqlparser.h>
#include <debugprint.h>
#include <rudiments/character.h>

sqlwriter::sqlwriter() {
	debugFunction();
	sqlrcon=NULL;
	sqlrcur=NULL;
}

sqlwriter::~sqlwriter() {
	debugFunction();
}

bool sqlwriter::write(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *tree,
					stringbuffer *output) {
	debugFunction();
	return write(sqlrcon,sqlrcur,
			tree->getRootNode()->getFirstTagChild(),
			output,false);
}

bool sqlwriter::write(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *tree,
					stringbuffer *output,
					bool omitsiblings) {
	debugFunction();
	this->sqlrcon=sqlrcon;
	this->sqlrcur=sqlrcur;
	if (omitsiblings) {
		return write(tree,output);
	}
	for (xmldomnode *node=tree;
		!node->isNullNode(); node=node->getNextTagSibling()) {
		if (!write(node,output)) {
			return false;
		}
	}
	return true;
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
	if (!dontAppendSpace(output) &&
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
	if (!dontAppendSpace(output) &&
		!node->getNextTagSibling()->isNullNode()) {
		space(output);
	}
	return true;
}

const char * const *sqlwriter::baseElements() {
	debugFunction();
	static const char *baseelements[]={
		sqlparser::_table_name,
		sqlparser::_name,
		sqlparser::_type,
		sqlparser::_size,
		sqlparser::_values,
		sqlparser::_value,
		sqlparser::_length,
		sqlparser::_scale,
		sqlparser::_verbatim,

		// create query...
		sqlparser::_create,
		sqlparser::_global,
		sqlparser::_temporary,

		// table...
		sqlparser::_table,
		sqlparser::_if_not_exists,

		// index...
		sqlparser::_fulltext,
		sqlparser::_spatial,
		sqlparser::_index,
		sqlparser::_index_name,
		sqlparser::_btree,
		sqlparser::_hash,

		// column definitions...
		sqlparser::_columns,
		sqlparser::_column,

		// constraints...
		sqlparser::_constraints,
		sqlparser::_unsigned,
		sqlparser::_zerofill,
		sqlparser::_binary,
		sqlparser::_character_set,
		sqlparser::_collate,
		sqlparser::_null,
		sqlparser::_not_null,
		sqlparser::_default,
		sqlparser::_auto_increment,
		sqlparser::_unique_key,
		sqlparser::_primary_key,
		sqlparser::_key,
		sqlparser::_comment,
		sqlparser::_column_format,
		sqlparser::_references,
		sqlparser::_match,
		sqlparser::_on_delete,
		sqlparser::_on_update,


		// drop...
		sqlparser::_drop,
		sqlparser::_if_exists,
		sqlparser::_table_name_list,
		sqlparser::_table_name_list_item,
		sqlparser::_restrict,
		sqlparser::_cascade,
		sqlparser::_cascade_constraints_clause,


		// insert...
		sqlparser::_insert,
		sqlparser::_insert_into,
		sqlparser::_insert_values_clause,
		sqlparser::_insert_value_clause,
		sqlparser::_insert_value,


		// update...
		sqlparser::_update,
		sqlparser::_update_set,
		sqlparser::_assignment,
		sqlparser::_equals,


		// delete...
		sqlparser::_delete,
		sqlparser::_delete_from,
		sqlparser::_using,


		// select...
		sqlparser::_select,
		sqlparser::_select_expression,
		sqlparser::_select_expressions,
		sqlparser::_sub_select,
		sqlparser::_union,
		sqlparser::_all,
		sqlparser::_alias,
		sqlparser::_unique,
		sqlparser::_distinct,
		sqlparser::_from,
		sqlparser::_table_references,
		sqlparser::_table_reference,
		sqlparser::_join_clause,
		sqlparser::_inner,
		sqlparser::_cross,
		sqlparser::_straight_join,
		sqlparser::_left,
		sqlparser::_right,
		sqlparser::_outer,
		sqlparser::_natural,
		sqlparser::_join,
		sqlparser::_on,
		sqlparser::_join_using,
		sqlparser::_where,
		sqlparser::_and,
		sqlparser::_or,
		sqlparser::_group,
		sqlparser::_comparison,
		sqlparser::_not,
		sqlparser::_between,
		sqlparser::_in,
		sqlparser::_in_set_item,
		sqlparser::_exists,
		sqlparser::_is,
		sqlparser::_like,
		sqlparser::_null_safe_equals,
		sqlparser::_not_equals,
		sqlparser::_less_than,
		sqlparser::_greater_than,
		sqlparser::_less_than_or_equal_to,
		sqlparser::_greater_than_or_equal_to,
		sqlparser::_expression,
		sqlparser::_interval_qualifier,
		sqlparser::_compliment,
		sqlparser::_inverse,
		sqlparser::_negative,
		sqlparser::_plus,
		sqlparser::_minus,
		sqlparser::_times,
		sqlparser::_divided_by,
		sqlparser::_modulo,
		sqlparser::_logical_and,
		sqlparser::_logical_or,
		sqlparser::_bitwise_and,
		sqlparser::_bitwise_or,
		sqlparser::_bitwise_xor,
		sqlparser::_number,
		sqlparser::_string_literal,
		sqlparser::_bind_variable,
		sqlparser::_column_reference,
		sqlparser::_column_name_database,
		sqlparser::_column_name_schema,
		sqlparser::_column_name_table,
		sqlparser::_column_name_column,
		sqlparser::_function,
		sqlparser::_parameters,
		sqlparser::_parameter,
		sqlparser::_group_by,
		sqlparser::_group_by_item,
		sqlparser::_with_rollup,
		sqlparser::_having,
		sqlparser::_order_by,
		sqlparser::_order_by_item,
		sqlparser::_asc,
		sqlparser::_desc,
		sqlparser::_limit,
		sqlparser::_select_into,
		sqlparser::_for_update,


		// set...
		sqlparser::_set,
		sqlparser::_set_global,
		sqlparser::_set_session,
		sqlparser::_transaction,
		sqlparser::_isolation_level,


		// lock...
		sqlparser::_lock,
		sqlparser::_in_mode,
		sqlparser::_lock_mode,
		sqlparser::_mode,
		sqlparser::_nowait,

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
	if (!charstring::compare(nodename,sqlparser::_table_name)) {
		return tableName(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_name)) {
		return name(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_type)) {
		return type(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_size)) {
		return size(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_value)) {
		return value(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_verbatim)) {
		return verbatim(node,output);

	// create query...
	} else if (!charstring::compare(nodename,sqlparser::_create)) {
		return createQuery(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_global)) {
		return global(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_temporary)) {
		return temporary(node,output);

	// table...
	} else if (!charstring::compare(nodename,sqlparser::_table)) {
		return table(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_if_not_exists)) {
		return ifNotExists(node,output);

	// index...
	} else if (!charstring::compare(nodename,sqlparser::_fulltext)) {
		return fulltext(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_spatial)) {
		return spatial(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_index)) {
		return index(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_index_name)) {
		return indexName(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_btree)) {
		return btree(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_hash)) {
		return hash(node,output);

	// column definitions...
	} else if (!charstring::compare(nodename,sqlparser::_columns)) {
		return columns(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_column)) {
		return column(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_values)) {
		return values(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_length)) {
		return length(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_scale)) {
		return scale(node,output);

	// constraints...
	} else if (!charstring::compare(nodename,sqlparser::_constraints)) {
		return constraints(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_unsigned)) {
		return unsignedConstraint(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_zerofill)) {
		return zerofill(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_binary)) {
		return binary(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_character_set)) {
		return characterSet(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_collate)) {
		return collate(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_null)) {
		return nullable(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_not_null)) {
		return notNull(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_default)) {
		return defaultValue(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_auto_increment)) {
		return autoIncrement(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_unique_key)) {
		return uniqueKey(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_primary_key)) {
		return primaryKey(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_key)) {
		return key(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_comment)) {
		return comment(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_column_format)) {
		return columnFormat(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_references)) {
		return references(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_match)) {
		return match(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_on_delete)) {
		return onDelete(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_on_update)) {
		return onUpdate(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_on_commit)) {
		return onCommit(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_as)) {
		return as(node,output);


	// drop...
	} else if (!charstring::compare(nodename,sqlparser::_drop)) {
		return dropQuery(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_if_exists)) {
		return ifExists(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_restrict)) {
		return restrictClause(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_cascade)) {
		return cascade(node,output);
	} else if (!charstring::compare(nodename,
				sqlparser::_cascade_constraints_clause)) {
		return cascadeConstraintsClause(node,output);

	// insert...
	} else if (!charstring::compare(nodename,sqlparser::_insert)) {
		return insertQuery(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_insert_into)) {
		return insertInto(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_insert_values_clause)) {
		return insertValuesClause(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_insert_value_clause)) {
		return insertValueClause(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_insert_value)) {
		return insertValue(node,output);


	// update...
	} else if (!charstring::compare(nodename,sqlparser::_update)) {
		return updateQuery(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_update_set)) {
		return updateSet(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_assignment)) {
		return assignment(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_equals)) {
		return equals(node,output);


	// delete...
	} else if (!charstring::compare(nodename,sqlparser::_delete)) {
		return deleteQuery(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_delete_from)) {
		return deleteFrom(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_using)) {
		return usingClause(node,output);


	// select...
	} else if (!charstring::compare(nodename,sqlparser::_select)) {
		return selectQuery(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_select_expressions)) {
		return selectExpressions(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_select_expression)) {
		return selectExpression(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_sub_select)) {
		return subSelect(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_union)) {
		return unionClause(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_all)) {
		return all(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_alias)) {
		return alias(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_unique)) {
		return unique(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_distinct)) {
		return distinct(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_from)) {
		return from(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_table_references)) {
		return tableReferences(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_table_reference)) {
		return tableReference(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_join_clause)) {
		return joinClause(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_inner)) {
		return inner(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_cross)) {
		return cross(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_straight_join)) {
		return straightJoin(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_left)) {
		return left(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_right)) {
		return right(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_outer)) {
		return outer(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_natural)) {
		return natural(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_join)) {
		return join(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_on)) {
		return on(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_join_using)) {
		return joinUsing(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_where)) {
		return where(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_and)) {
		return andClause(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_or)) {
		return orClause(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_group)) {
		return group(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_comparison)) {
		return comparison(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_not)) {
		return notClause(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_between)) {
		return between(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_in)) {
		return in(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_in_set_item)) {
		return inSetItem(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_exists)) {
		return exists(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_is)) {
		return is(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_like)) {
		return like(node,output);
	} else if (!charstring::compare(nodename,
				sqlparser::_null_safe_equals)) {
		return nullSafeEquals(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_not_equals)) {
		return notEquals(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_less_than)) {
		return lessThan(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_greater_than)) {
		return greaterThan(node,output);
	} else if (!charstring::compare(nodename,
				sqlparser::_less_than_or_equal_to)) {
		return lessThanOrEqualTo(node,output);
	} else if (!charstring::compare(nodename,
				sqlparser::_greater_than_or_equal_to)) {
		return greaterThanOrEqualTo(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_expression)) {
		return expression(node,output);
	} else if (!charstring::compare(nodename,
				sqlparser::_interval_qualifier)) {
		return intervalQualifier(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_compliment)) {
		return compliment(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_inverse)) {
		return inverse(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_negative)) {
		return negative(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_plus)) {
		return plus(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_minus)) {
		return minus(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_times)) {
		return times(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_divided_by)) {
		return dividedBy(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_modulo)) {
		return modulo(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_logical_and)) {
		return logicalAnd(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_logical_or)) {
		return logicalOr(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_bitwise_and)) {
		return bitwiseAnd(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_bitwise_or)) {
		return bitwiseOr(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_bitwise_xor)) {
		return bitwiseXor(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_number)) {
		return number(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_string_literal)) {
		return stringLiteral(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_bind_variable)) {
		return bindVariable(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_column_reference)) {
		return columnReference(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_column_name_database)) {
		return columnNameDatabase(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_column_name_schema)) {
		return columnNameSchema(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_column_name_table)) {
		return columnNameTable(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_column_name_column)) {
		return columnNameColumn(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_function)) {
		return function(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_parameters)) {
		return parameters(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_parameter)) {
		return parameter(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_group_by)) {
		return groupBy(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_group_by_item)) {
		return groupByItem(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_with_rollup)) {
		return withRollup(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_having)) {
		return having(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_order_by)) {
		return orderBy(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_order_by_item)) {
		return orderByItem(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_asc)) {
		return asc(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_desc)) {
		return desc(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_limit)) {
		return limit(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_select_into)) {
		return selectInto(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_procedure)) {
		return procedure(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_for_update)) {
		return forUpdate(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_set)) {
		return setQuery(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_set_global)) {
		return setGlobal(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_set_session)) {
		return setSession(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_transaction)) {
		return transaction(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_limit)) {
		return limit(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_select_into)) {
		return selectInto(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_procedure)) {
		return procedure(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_for_update)) {
		return forUpdate(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_set)) {
		return setQuery(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_set_global)) {
		return setGlobal(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_set_session)) {
		return setSession(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_transaction)) {
		return transaction(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_isolation_level)) {
		return isolationLevel(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_lock)) {
		return lockQuery(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_in_mode)) {
		return inMode(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_lock_mode)) {
		return lockMode(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_mode)) {
		return mode(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_nowait)) {
		return noWait(node,output);
	}
	return true;
}

bool sqlwriter::handleEnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,sqlparser::_type)) {
		return endType(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_size)) {
		return endSize(node,output);

	// column definitions...
	} else if (!charstring::compare(nodename,sqlparser::_columns)) {
		return endColumns(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_column)) {
		return endColumn(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_values)) {
		return endValues(node,output);

	// drop...
	} else if (!charstring::compare(nodename,
					sqlparser::_table_name_list_item)) {
		return endTableNameListItem(node,output);

	// insert...
	} else if (!charstring::compare(nodename,
					sqlparser::_insert_values_clause)) {
		return endInsertValuesClause(node,output);
	} else if (!charstring::compare(nodename,
					sqlparser::_insert_value_clause)) {
		return endInsertValueClause(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_insert_value)) {
		return endInsertValue(node,output);

	// update...
	} else if (!charstring::compare(nodename,sqlparser::_assignment)) {
		return endAssignment(node,output);

	// select...
	} else if (!charstring::compare(nodename,
					sqlparser::_select_expression)) {
		return endSelectExpression(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_sub_select)) {
		return endSubSelect(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_group)) {
		return endGroup(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_parameters)) {
		return endParameters(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_parameter)) {
		return endParameter(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_table_reference)) {
		return endTableReference(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_join_clause)) {
		return endJoinClause(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_order_by_item)) {
		return endOrderByItem(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_group_by_item)) {
		return endGroupByItem(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_in)) {
		return endIn(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_in_set_item)) {
		return endInSetItem(node,output);
	} else if (!charstring::compare(nodename,sqlparser::_exists)) {
		return endExists(node,output);
	}
	return true;
}

bool sqlwriter::outputValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(node->getAttributeValue(sqlparser::_value));
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

bool sqlwriter::period(stringbuffer *output) {
	debugFunction();
	output->append(".");
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

bool sqlwriter::dontAppendSpace(stringbuffer *output) {
	debugFunction();
	size_t	length=output->getStringLength();
	return (length &&
		character::inSet(output->getString()[length-1]," .(,"));
}
