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

bool sqlwriter::handleStart(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,sqlparser::_table_name_database)) {
		return tableNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_table_name_schema)) {
		return tableNameSchema(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_table_name_table)) {
		return tableNameTable(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_name)) {
		return name(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_type)) {
		return type(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_size)) {
		return size(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_value)) {
		return value(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_verbatim)) {
		return verbatim(node,output);
	}

	// create query...
	if (!charstring::compare(nodename,sqlparser::_create)) {
		return createQuery(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_global)) {
		return global(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_temporary)) {
		return temporary(node,output);
	}

	// table...
	if (!charstring::compare(nodename,sqlparser::_table)) {
		return table(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_if_not_exists)) {
		return ifNotExists(node,output);
	}

	// index...
	if (!charstring::compare(nodename,sqlparser::_fulltext)) {
		return fulltext(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_spatial)) {
		return spatial(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_index)) {
		return index(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_index_name_database)) {
		return indexNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_index_name_schema)) {
		return indexNameSchema(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_index_name_index)) {
		return indexNameIndex(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_btree)) {
		return btree(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_hash)) {
		return hash(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_key_block_size)) {
		return keyBlockSize(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_with_parser)) {
		return withParser(node,output);
	}

	// synonym...
	if (!charstring::compare(nodename,sqlparser::_synonym)) {
		return synonym(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_for)) {
		return forClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_object_name_database)) {
		return objectNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_object_name_schema)) {
		return objectNameSchema(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_object_name_object)) {
		return objectNameObject(node,output);
	}

	// column definitions...
	if (!charstring::compare(nodename,sqlparser::_columns)) {
		return columns(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_column)) {
		return column(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_values)) {
		return values(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_length)) {
		return length(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_scale)) {
		return scale(node,output);
	}

	// constraints...
	if (!charstring::compare(nodename,sqlparser::_constraints)) {
		return constraints(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_unsigned)) {
		return unsignedConstraint(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_zerofill)) {
		return zerofill(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_binary)) {
		return binary(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_character_set)) {
		return characterSet(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_collate)) {
		return collate(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_null)) {
		return nullable(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_not_null)) {
		return notNull(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_default)) {
		return defaultValue(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_auto_increment)) {
		return autoIncrement(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_identity)) {
		return identity(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_unique_key)) {
		return uniqueKey(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_primary_key)) {
		return primaryKey(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_key)) {
		return key(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_comment)) {
		return comment(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_column_format)) {
		return columnFormat(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_references)) {
		return references(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_match)) {
		return match(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_on_delete)) {
		return onDelete(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_on_update)) {
		return onUpdate(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_on_commit)) {
		return onCommit(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_as)) {
		return as(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_constraint)) {
		return constraint(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_foreign_key)) {
		return foreignKey(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_check)) {
		return check(node,output);
	}


	// drop...
	if (!charstring::compare(nodename,sqlparser::_drop)) {
		return dropQuery(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_if_exists)) {
		return ifExists(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_restrict_clause)) {
		return restrictClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_cascade)) {
		return cascade(node,output);
	}
	if (!charstring::compare(nodename,
				sqlparser::_cascade_constraints_clause)) {
		return cascadeConstraintsClause(node,output);
	}


	// insert...
	if (!charstring::compare(nodename,sqlparser::_insert)) {
		return insertQuery(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_insert_into)) {
		return insertInto(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_insert_values_clause)) {
		return insertValuesClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_insert_value_clause)) {
		return insertValueClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_insert_value)) {
		return insertValue(node,output);


	// update...
	}
	if (!charstring::compare(nodename,sqlparser::_update)) {
		return updateQuery(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_update_set)) {
		return updateSet(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_assignment)) {
		return assignment(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_equals)) {
		return equals(node,output);
	}


	// delete...
	if (!charstring::compare(nodename,sqlparser::_delete)) {
		return deleteQuery(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_delete_from)) {
		return deleteFrom(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_using)) {
		return usingClause(node,output);
	}


	// select...
	if (!charstring::compare(nodename,sqlparser::_select)) {
		return selectQuery(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_select_expressions)) {
		return selectExpressions(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_select_expression)) {
		return selectExpression(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_sub_select)) {
		return subSelect(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_union)) {
		return unionClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_all)) {
		return all(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_alias)) {
		return alias(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_unique)) {
		return unique(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_distinct)) {
		return distinct(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_from)) {
		return from(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_table_references)) {
		return tableReferences(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_table_reference)) {
		return tableReference(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_join_clause)) {
		return joinClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_inner)) {
		return inner(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_cross)) {
		return cross(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_straight_join)) {
		return straightJoin(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_left)) {
		return left(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_right)) {
		return right(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_outer)) {
		return outer(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_natural)) {
		return natural(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_join)) {
		return join(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_on)) {
		return on(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_join_using)) {
		return joinUsing(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_where)) {
		return where(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_and)) {
		return andClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_or)) {
		return orClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_group)) {
		return group(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_comparison)) {
		return comparison(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_not)) {
		return notClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_between)) {
		return between(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_in)) {
		return in(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_in_set_item)) {
		return inSetItem(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_exists)) {
		return exists(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_is)) {
		return is(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_like)) {
		return like(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_matches)) {
		return matches(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_null_safe_equals)) {
		return nullSafeEquals(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_not_equals)) {
		return notEquals(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_less_than)) {
		return lessThan(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_greater_than)) {
		return greaterThan(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_less_than_or_equal_to)) {
		return lessThanOrEqualTo(node,output);
	}
	if (!charstring::compare(nodename,
				sqlparser::_greater_than_or_equal_to)) {
		return greaterThanOrEqualTo(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_escape)) {
		return escape(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_expression)) {
		return expression(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_interval_qualifier)) {
		return intervalQualifier(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_outer_join_operator)) {
		return outerJoinOperator(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_compliment)) {
		return compliment(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_inverse)) {
		return inverse(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_negative)) {
		return negative(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_plus)) {
		return plus(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_minus)) {
		return minus(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_times)) {
		return times(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_divided_by)) {
		return dividedBy(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_modulo)) {
		return modulo(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_logical_and)) {
		return logicalAnd(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_logical_or)) {
		return logicalOr(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_bitwise_and)) {
		return bitwiseAnd(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_bitwise_or)) {
		return bitwiseOr(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_bitwise_xor)) {
		return bitwiseXor(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_number)) {
		return number(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_string_literal)) {
		return stringLiteral(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_bind_variable)) {
		return bindVariable(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_column_reference)) {
		return columnReference(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_column_name_database)) {
		return columnNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_column_name_schema)) {
		return columnNameSchema(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_column_name_table)) {
		return columnNameTable(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_column_name_column)) {
		return columnNameColumn(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_function)) {
		return function(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_parameters)) {
		return parameters(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_parameter)) {
		return parameter(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_group_by)) {
		return groupBy(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_group_by_item)) {
		return groupByItem(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_with_rollup)) {
		return withRollup(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_having)) {
		return having(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_order_by)) {
		return orderBy(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_order_by_item)) {
		return orderByItem(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_asc)) {
		return asc(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_desc)) {
		return desc(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_limit)) {
		return limit(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_select_into)) {
		return selectInto(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_procedure)) {
		return procedure(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_for_update)) {
		return forUpdate(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_set)) {
		return setQuery(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_set_global)) {
		return setGlobal(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_set_session)) {
		return setSession(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_transaction)) {
		return transaction(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_limit)) {
		return limit(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_select_into)) {
		return selectInto(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_procedure)) {
		return procedure(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_for_update)) {
		return forUpdate(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_set)) {
		return setQuery(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_set_global)) {
		return setGlobal(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_set_session)) {
		return setSession(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_transaction)) {
		return transaction(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_isolation_level)) {
		return isolationLevel(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_lock)) {
		return lockQuery(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_in_mode)) {
		return inMode(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_lock_mode)) {
		return lockMode(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_mode)) {
		return mode(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_nowait)) {
		return noWait(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_show)) {
		return show(node,output);
	}
	return true;
}

bool sqlwriter::handleEnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,sqlparser::_type)) {
		return endType(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_size)) {
		return endSize(node,output);
	}

	// column definitions...
	if (!charstring::compare(nodename,sqlparser::_columns)) {
		return endColumns(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_column)) {
		return endColumn(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_values)) {
		return endValues(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_check)) {
		return endCheck(node,output);
	}

	// drop...
	if (!charstring::compare(nodename,
					sqlparser::_table_name_list_item)) {
		return endTableNameListItem(node,output);
	}

	// insert...
	if (!charstring::compare(nodename,
					sqlparser::_insert_values_clause)) {
		return endInsertValuesClause(node,output);
	}
	if (!charstring::compare(nodename,
					sqlparser::_insert_value_clause)) {
		return endInsertValueClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_insert_value)) {
		return endInsertValue(node,output);
	}

	// update...
	if (!charstring::compare(nodename,sqlparser::_assignment)) {
		return endAssignment(node,output);
	}

	// select...
	if (!charstring::compare(nodename,
					sqlparser::_select_expression)) {
		return endSelectExpression(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_sub_select)) {
		return endSubSelect(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_group)) {
		return endGroup(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_parameters)) {
		return endParameters(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_parameter)) {
		return endParameter(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_table_reference)) {
		return endTableReference(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_join_clause)) {
		return endJoinClause(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_order_by_item)) {
		return endOrderByItem(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_group_by_item)) {
		return endGroupByItem(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_in)) {
		return endIn(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_in_set_item)) {
		return endInSetItem(node,output);
	}
	if (!charstring::compare(nodename,sqlparser::_exists)) {
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

bool sqlwriter::createQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("create");
	return true;
}

bool sqlwriter::table(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("table");
	return true;
}

bool sqlwriter::global(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("global");
	return true;
}

bool sqlwriter::temporary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("temporary");
	return true;
}

bool sqlwriter::ifNotExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("if not exists");
	return true;
}

bool sqlwriter::columns(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endColumns(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::column(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endColumn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::values(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endValues(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::length(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::scale(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	comma(output);
	outputValue(node,output);
	return true;
}

bool sqlwriter::constraints(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::unsignedConstraint(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unsigned");
	return true;
}

bool sqlwriter::zerofill(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("zerofill");
	return true;
}

bool sqlwriter::binary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("binary");
	return true;
}

bool sqlwriter::characterSet(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("character set ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::collate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("collate ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::nullable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("null");
	return true;
}

bool sqlwriter::notNull(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("not null");
	return true;
}

bool sqlwriter::defaultValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("default ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::autoIncrement(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("auto_increment");
	return true;
}

bool sqlwriter::identity(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("identity");
	return true;
}

bool sqlwriter::uniqueKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique key");
	return true;
}

bool sqlwriter::primaryKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("primary key");
	return true;
}

bool sqlwriter::key(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("key");
	return true;
}

bool sqlwriter::comment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("comment ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::columnFormat(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("column_format ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::references(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("references ");
	return true;
}

bool sqlwriter::endReferences(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::match(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("match ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::onDelete(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on delete ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::onUpdate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on update ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::onCommit(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on commit ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::as(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("as ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::constraint(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	const char	*name=node->getAttributeValue("value");
	if (name) {
		output->append("constraint ")->append(name);
	}
	return true;
}

bool sqlwriter::foreignKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("foreign key ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::check(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("check (");
	return true;
}

bool sqlwriter::endCheck(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlwriter::withNoLog(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with no log ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::fulltext(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("fulltext ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::spatial(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("spatial ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::index(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("index ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::indexNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::indexNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::indexNameIndex(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::btree(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("btree ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::hash(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("hash ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::keyBlockSize(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("key_block_size");
	if (!charstring::compare(node->getAttributeValue("equals"),"true")) {
		output->append("=");
	} else {
		output->append(" ");
	}
	outputValue(node,output);
	return true;
}

bool sqlwriter::withParser(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with parser ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::synonym(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("synonym ");
	return true;
}

bool sqlwriter::forClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("for ");
	return true;
}

bool sqlwriter::objectNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::objectNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::objectNameObject(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::deleteQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("delete");
	return true;
}

bool sqlwriter::deleteFrom(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("from");
	return true;
}

bool sqlwriter::usingClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("using");
	return true;
}

bool sqlwriter::dropQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("drop");
	return true;
}

bool sqlwriter::ifExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("if exists");
	return true;
}

bool sqlwriter::endTableNameListItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (!node->getNextSibling()->isNullNode()) {
		comma(output);
	}
	return true;
}

bool sqlwriter::restrictClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("restrict");
	return true;
}

bool sqlwriter::cascade(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cascade");
	return true;
}

bool sqlwriter::cascadeConstraintsClause(xmldomnode *node,
						stringbuffer *output) {
	debugFunction();
	output->append("constraints");
	return true;
}

bool sqlwriter::tableNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::tableNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::tableNameTable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::name(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::type(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::endType(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::size(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endSize(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::value(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::verbatim(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::insertQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("insert");
	return true;
}

bool sqlwriter::insertInto(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("into");
	return true;
}

bool sqlwriter::insertValuesClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("values (");
	return true;
}

bool sqlwriter::insertValueClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("value (");
	return true;
}

bool sqlwriter::insertValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endInsertValuesClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlwriter::endInsertValueClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlwriter::endInsertValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::selectQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("select");
	return true;
}

bool sqlwriter::selectExpressions(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::selectExpression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endSelectExpression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::subSelect(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endSubSelect(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::unionClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("union");
	return true;
}

bool sqlwriter::all(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("all");
	return true;
}

bool sqlwriter::alias(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::unique(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique");
	return true;
}

bool sqlwriter::distinct(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("distinct");
	return true;
}

bool sqlwriter::from(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("from");
	return true;
}

bool sqlwriter::tableReferences(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::tableReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endTableReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node isn't a join clause
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		charstring::compare(next->getName(),sqlparser::_join_clause)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::joinClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endJoinClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node isn't a join clause
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		charstring::compare(next->getName(),sqlparser::_join_clause)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::inner(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("inner");
	return true;
}

bool sqlwriter::cross(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cross");
	return true;
}

bool sqlwriter::straightJoin(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("straight_join");
	return true;
}

bool sqlwriter::left(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("left");
	return true;
}

bool sqlwriter::right(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("right");
	return true;
}

bool sqlwriter::outer(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("outer");
	return true;
}

bool sqlwriter::natural(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("natural");
	return true;
}

bool sqlwriter::join(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("join");
	return true;
}

bool sqlwriter::on(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on");
	return true;
}

bool sqlwriter::joinUsing(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("using");
	return true;
}

bool sqlwriter::groupBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("group by");
	return true;
}

bool sqlwriter::groupByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endGroupByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node is another group-by-item
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		!charstring::compare(next->getName(),
					sqlparser::_group_by_item)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::withRollup(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with rollup");
	return true;
}

bool sqlwriter::having(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("having");
	return true;
}

bool sqlwriter::orderBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("order by");
	return true;
}

bool sqlwriter::orderByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endOrderByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::asc(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("asc");
	return true;
}

bool sqlwriter::desc(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("desc");
	return true;
}

bool sqlwriter::limit(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("limit");
	return true;
}

bool sqlwriter::selectInto(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("into");
	return true;
}

bool sqlwriter::procedure(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("procedure");
	return true;
}

bool sqlwriter::forUpdate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("for update");
	return true;
}

bool sqlwriter::where(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("where");
	return true;
}

bool sqlwriter::andClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("and");
	return true;
}

bool sqlwriter::orClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("or");
	return true;
}

bool sqlwriter::comparison(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::notClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("not");
	return true;
}

bool sqlwriter::group(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("(");
	return true;
}

bool sqlwriter::endGroup(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlwriter::between(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("between");
	return true;
}

bool sqlwriter::in(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("in ");
	leftParen(output);
	return true;
}

bool sqlwriter::endIn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::inSetItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endInSetItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::exists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("exists ");
	leftParen(output);
	return true;
}

bool sqlwriter::endExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::is(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("is");
	return true;
}

bool sqlwriter::like(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("like");
	return true;
}

bool sqlwriter::matches(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("matches");
	return true;
}

bool sqlwriter::nullSafeEquals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<=>");
	return true;
}

bool sqlwriter::notEquals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("!=");
	return true;
}

bool sqlwriter::lessThan(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<");
	return true;
}

bool sqlwriter::greaterThan(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(">");
	return true;
}

bool sqlwriter::lessThanOrEqualTo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<=");
	return true;
}

bool sqlwriter::greaterThanOrEqualTo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(">=");
	return true;
}

bool sqlwriter::escape(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("escape");
	return true;
}

bool sqlwriter::expression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::intervalQualifier(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(node->getAttributeValue(sqlparser::_from));
	const char	*precision=
			node->getAttributeValue(sqlparser::_from_precision);
	const char	*scale=
			node->getAttributeValue(sqlparser::_from_scale);
	if (precision) {
		leftParen(output);
		output->append(precision);
		if (scale) {
			comma(output);
			output->append(scale);
		}
		rightParen(output);
	}
	if (charstring::length(node->getAttributeValue(sqlparser::_to))) {
		output->append(" to ");
		output->append(node->getAttributeValue(sqlparser::_to));
		precision=node->getAttributeValue(sqlparser::_to_precision);
		scale=node->getAttributeValue(sqlparser::_to_scale);
		if (precision) {
			leftParen(output);
			output->append(precision);
			if (scale) {
				comma(output);
				output->append(scale);
			}
			rightParen(output);
		}
	}
	return true;
}

bool sqlwriter::outerJoinOperator(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("(+)");
	return true;
}

bool sqlwriter::compliment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("~");
	return true;
}

bool sqlwriter::inverse(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("!");
	return true;
}

bool sqlwriter::negative(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("-");
	return true;
}

bool sqlwriter::plus(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("+");
	return true;
}

bool sqlwriter::minus(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("-");
	return true;
}

bool sqlwriter::times(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("*");
	return true;
}

bool sqlwriter::dividedBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("/");
	return true;
}

bool sqlwriter::modulo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("%");
	return true;
}

bool sqlwriter::bitwiseAnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("&");
	return true;
}

bool sqlwriter::bitwiseOr(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("|");
	return true;
}

bool sqlwriter::bitwiseXor(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("^");
	return true;
}

bool sqlwriter::logicalAnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("&&");
	return true;
}

bool sqlwriter::logicalOr(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("||");
	return true;
}

bool sqlwriter::number(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::stringLiteral(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::bindVariable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::columnReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::columnNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::columnNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::columnNameTable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlwriter::columnNameColumn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::function(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::parameters(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endParameters(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::parameter(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endParameter(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::updateQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("update");
	return true;
}

bool sqlwriter::updateSet(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("set");
	return true;
}

bool sqlwriter::assignment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endAssignment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		output->append(",");
	}
	return true;
}

bool sqlwriter::equals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("=");
	return true;
}

bool sqlwriter::setQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("set");
	return true;
}

bool sqlwriter::setSession(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("session");
	return true;
}

bool sqlwriter::setGlobal(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("global");
	return true;
}

bool sqlwriter::transaction(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("transaction ");
	return true;
}

bool sqlwriter::isolationLevel(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("isolation level ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::lockQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("lock");
	return true;
}

bool sqlwriter::inMode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("in");
	return true;
}

bool sqlwriter::lockMode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::mode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("mode");
	return true;
}

bool sqlwriter::noWait(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("nowait");
	return true;
}

bool sqlwriter::show(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("show ");
	outputValue(node,output);
	return true;
}
