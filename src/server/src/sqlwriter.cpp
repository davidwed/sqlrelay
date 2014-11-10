// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

bool sqlparser::write(stringbuffer *output) {
	debugFunction();
	return write(tree->getRootNode()->getFirstTagChild(),output,false);
}

bool sqlparser::write(xmldomnode *node,
				stringbuffer *output,
				bool omitsiblings) {
	debugFunction();
	if (omitsiblings) {
		return write(node,output);
	}
	for (xmldomnode *n=node; !n->isNullNode(); n=n->getNextTagSibling()) {
		if (!write(n,output)) {
			return false;
		}
	}
	return true;
}

bool sqlparser::write(xmldomnode *node, stringbuffer *output) {
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

bool sqlparser::handleStart(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	if (!charstring::compare(
		node->getAttributeValue("supported"),"false")) {
		return true;
	}

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,_table_name_database)) {
		return tableNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,_table_name_schema)) {
		return tableNameSchema(node,output);
	}
	if (!charstring::compare(nodename,_table_name_table)) {
		return tableNameTable(node,output);
	}
	if (!charstring::compare(nodename,_name)) {
		return name(node,output);
	}
	if (!charstring::compare(nodename,_type)) {
		return type(node,output);
	}
	if (!charstring::compare(nodename,_size)) {
		return size(node,output);
	}
	if (!charstring::compare(nodename,_value)) {
		return value(node,output);
	}
	if (!charstring::compare(nodename,_verbatim)) {
		return verbatim(node,output);
	}

	// create query...
	if (!charstring::compare(nodename,_create)) {
		return createQuery(node,output);
	}
	if (!charstring::compare(nodename,_global)) {
		return global(node,output);
	}
	if (!charstring::compare(nodename,_temporary)) {
		return temporary(node,output);
	}

	// table...
	if (!charstring::compare(nodename,_table)) {
		return table(node,output);
	}
	if (!charstring::compare(nodename,_if_not_exists)) {
		return ifNotExists(node,output);
	}

	// index...
	if (!charstring::compare(nodename,_fulltext)) {
		return fulltext(node,output);
	}
	if (!charstring::compare(nodename,_spatial)) {
		return spatial(node,output);
	}
	if (!charstring::compare(nodename,_index)) {
		return index(node,output);
	}
	if (!charstring::compare(nodename,_index_name_database)) {
		return indexNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,_index_name_schema)) {
		return indexNameSchema(node,output);
	}
	if (!charstring::compare(nodename,_index_name_index)) {
		return indexNameIndex(node,output);
	}
	if (!charstring::compare(nodename,_btree)) {
		return btree(node,output);
	}
	if (!charstring::compare(nodename,_hash)) {
		return hash(node,output);
	}
	if (!charstring::compare(nodename,_key_block_size)) {
		return keyBlockSize(node,output);
	}
	if (!charstring::compare(nodename,_with_parser)) {
		return withParser(node,output);
	}

	// synonym...
	if (!charstring::compare(nodename,_synonym)) {
		return synonym(node,output);
	}
	if (!charstring::compare(nodename,_for)) {
		return forClause(node,output);
	}
	if (!charstring::compare(nodename,_object_name_database)) {
		return objectNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,_object_name_schema)) {
		return objectNameSchema(node,output);
	}
	if (!charstring::compare(nodename,_object_name_object)) {
		return objectNameObject(node,output);
	}

	// column definitions...
	if (!charstring::compare(nodename,_columns)) {
		return columns(node,output);
	}
	if (!charstring::compare(nodename,_column)) {
		return column(node,output);
	}
	if (!charstring::compare(nodename,_values)) {
		return values(node,output);
	}
	if (!charstring::compare(nodename,_length)) {
		return length(node,output);
	}
	if (!charstring::compare(nodename,_scale)) {
		return scale(node,output);
	}

	// constraints...
	if (!charstring::compare(nodename,_constraints)) {
		return constraints(node,output);
	}
	if (!charstring::compare(nodename,_unsigned)) {
		return unsignedConstraint(node,output);
	}
	if (!charstring::compare(nodename,_zerofill)) {
		return zerofill(node,output);
	}
	if (!charstring::compare(nodename,_binary)) {
		return binary(node,output);
	}
	if (!charstring::compare(nodename,_character_set)) {
		return characterSet(node,output);
	}
	if (!charstring::compare(nodename,_collate)) {
		return collate(node,output);
	}
	if (!charstring::compare(nodename,_null)) {
		return nullable(node,output);
	}
	if (!charstring::compare(nodename,_not_null)) {
		return notNull(node,output);
	}
	if (!charstring::compare(nodename,_default)) {
		return defaultValue(node,output);
	}
	if (!charstring::compare(nodename,_auto_increment)) {
		return autoIncrement(node,output);
	}
	if (!charstring::compare(nodename,_identity)) {
		return identity(node,output);
	}
	if (!charstring::compare(nodename,_unique_key)) {
		return uniqueKey(node,output);
	}
	if (!charstring::compare(nodename,_primary_key)) {
		return primaryKey(node,output);
	}
	if (!charstring::compare(nodename,_key)) {
		return key(node,output);
	}
	if (!charstring::compare(nodename,_comment)) {
		return comment(node,output);
	}
	if (!charstring::compare(nodename,_column_format)) {
		return columnFormat(node,output);
	}
	if (!charstring::compare(nodename,_references)) {
		return references(node,output);
	}
	if (!charstring::compare(nodename,_match)) {
		return match(node,output);
	}
	if (!charstring::compare(nodename,_on_delete)) {
		return onDelete(node,output);
	}
	if (!charstring::compare(nodename,_on_update)) {
		return onUpdate(node,output);
	}
	if (!charstring::compare(nodename,_on_commit)) {
		return onCommit(node,output);
	}
	if (!charstring::compare(nodename,_as)) {
		return as(node,output);
	}
	if (!charstring::compare(nodename,_constraint)) {
		return constraint(node,output);
	}
	if (!charstring::compare(nodename,_foreign_key)) {
		return foreignKey(node,output);
	}
	if (!charstring::compare(nodename,_check)) {
		return check(node,output);
	}


	// drop...
	if (!charstring::compare(nodename,_drop)) {
		return dropQuery(node,output);
	}
	if (!charstring::compare(nodename,_if_exists)) {
		return ifExists(node,output);
	}
	if (!charstring::compare(nodename,_restrict_clause)) {
		return restrictClause(node,output);
	}
	if (!charstring::compare(nodename,_cascade)) {
		return cascade(node,output);
	}
	if (!charstring::compare(nodename,
				_cascade_constraints_clause)) {
		return cascadeConstraintsClause(node,output);
	}


	// insert...
	if (!charstring::compare(nodename,_insert)) {
		return insertQuery(node,output);
	}
	if (!charstring::compare(nodename,_insert_into)) {
		return insertInto(node,output);
	}
	if (!charstring::compare(nodename,_insert_values_clause)) {
		return insertValuesClause(node,output);
	}
	if (!charstring::compare(nodename,_insert_value_clause)) {
		return insertValueClause(node,output);
	}
	if (!charstring::compare(nodename,_insert_value)) {
		return insertValue(node,output);


	// update...
	}
	if (!charstring::compare(nodename,_update)) {
		return updateQuery(node,output);
	}
	if (!charstring::compare(nodename,_update_set)) {
		return updateSet(node,output);
	}
	if (!charstring::compare(nodename,_assignment)) {
		return assignment(node,output);
	}
	if (!charstring::compare(nodename,_equals)) {
		return equals(node,output);
	}


	// delete...
	if (!charstring::compare(nodename,_delete)) {
		return deleteQuery(node,output);
	}
	if (!charstring::compare(nodename,_delete_from)) {
		return deleteFrom(node,output);
	}
	if (!charstring::compare(nodename,_using)) {
		return usingClause(node,output);
	}


	// select...
	if (!charstring::compare(nodename,_select)) {
		return selectQuery(node,output);
	}
	if (!charstring::compare(nodename,_select_expressions)) {
		return selectExpressions(node,output);
	}
	if (!charstring::compare(nodename,_select_expression)) {
		return selectExpression(node,output);
	}
	if (!charstring::compare(nodename,_sub_select)) {
		return subSelect(node,output);
	}
	if (!charstring::compare(nodename,_union)) {
		return unionClause(node,output);
	}
	if (!charstring::compare(nodename,_all)) {
		return all(node,output);
	}
	if (!charstring::compare(nodename,_alias)) {
		return alias(node,output);
	}
	if (!charstring::compare(nodename,_unique)) {
		return unique(node,output);
	}
	if (!charstring::compare(nodename,_distinct)) {
		return distinct(node,output);
	}
	if (!charstring::compare(nodename,_from)) {
		return from(node,output);
	}
	if (!charstring::compare(nodename,_table_references)) {
		return tableReferences(node,output);
	}
	if (!charstring::compare(nodename,_table_reference)) {
		return tableReference(node,output);
	}
	if (!charstring::compare(nodename,_join_clause)) {
		return joinClause(node,output);
	}
	if (!charstring::compare(nodename,_inner)) {
		return inner(node,output);
	}
	if (!charstring::compare(nodename,_cross)) {
		return cross(node,output);
	}
	if (!charstring::compare(nodename,_straight_join)) {
		return straightJoin(node,output);
	}
	if (!charstring::compare(nodename,_left)) {
		return left(node,output);
	}
	if (!charstring::compare(nodename,_right)) {
		return right(node,output);
	}
	if (!charstring::compare(nodename,_outer)) {
		return outer(node,output);
	}
	if (!charstring::compare(nodename,_natural)) {
		return natural(node,output);
	}
	if (!charstring::compare(nodename,_join)) {
		return join(node,output);
	}
	if (!charstring::compare(nodename,_on)) {
		return on(node,output);
	}
	if (!charstring::compare(nodename,_join_using)) {
		return joinUsing(node,output);
	}
	if (!charstring::compare(nodename,_where)) {
		return where(node,output);
	}
	if (!charstring::compare(nodename,_and)) {
		return andClause(node,output);
	}
	if (!charstring::compare(nodename,_or)) {
		return orClause(node,output);
	}
	if (!charstring::compare(nodename,_group)) {
		return group(node,output);
	}
	if (!charstring::compare(nodename,_comparison)) {
		return comparison(node,output);
	}
	if (!charstring::compare(nodename,_not)) {
		return notClause(node,output);
	}
	if (!charstring::compare(nodename,_between)) {
		return between(node,output);
	}
	if (!charstring::compare(nodename,_in)) {
		return in(node,output);
	}
	if (!charstring::compare(nodename,_in_set_item)) {
		return inSetItem(node,output);
	}
	if (!charstring::compare(nodename,_exists)) {
		return exists(node,output);
	}
	if (!charstring::compare(nodename,_is)) {
		return is(node,output);
	}
	if (!charstring::compare(nodename,_like)) {
		return like(node,output);
	}
	if (!charstring::compare(nodename,_matches)) {
		return matches(node,output);
	}
	if (!charstring::compare(nodename,_null_safe_equals)) {
		return nullSafeEquals(node,output);
	}
	if (!charstring::compare(nodename,_not_equals)) {
		return notEquals(node,output);
	}
	if (!charstring::compare(nodename,_less_than)) {
		return lessThan(node,output);
	}
	if (!charstring::compare(nodename,_greater_than)) {
		return greaterThan(node,output);
	}
	if (!charstring::compare(nodename,_less_than_or_equal_to)) {
		return lessThanOrEqualTo(node,output);
	}
	if (!charstring::compare(nodename,
				_greater_than_or_equal_to)) {
		return greaterThanOrEqualTo(node,output);
	}
	if (!charstring::compare(nodename,_escape)) {
		return escape(node,output);
	}
	if (!charstring::compare(nodename,_expression)) {
		return expression(node,output);
	}
	if (!charstring::compare(nodename,_interval_qualifier)) {
		return intervalQualifier(node,output);
	}
	if (!charstring::compare(nodename,_outer_join_operator)) {
		return outerJoinOperator(node,output);
	}
	if (!charstring::compare(nodename,_compliment)) {
		return compliment(node,output);
	}
	if (!charstring::compare(nodename,_inverse)) {
		return inverse(node,output);
	}
	if (!charstring::compare(nodename,_negative)) {
		return negative(node,output);
	}
	if (!charstring::compare(nodename,_plus)) {
		return plus(node,output);
	}
	if (!charstring::compare(nodename,_minus)) {
		return minus(node,output);
	}
	if (!charstring::compare(nodename,_times)) {
		return times(node,output);
	}
	if (!charstring::compare(nodename,_divided_by)) {
		return dividedBy(node,output);
	}
	if (!charstring::compare(nodename,_modulo)) {
		return modulo(node,output);
	}
	if (!charstring::compare(nodename,_logical_and)) {
		return logicalAnd(node,output);
	}
	if (!charstring::compare(nodename,_logical_or)) {
		return logicalOr(node,output);
	}
	if (!charstring::compare(nodename,_bitwise_and)) {
		return bitwiseAnd(node,output);
	}
	if (!charstring::compare(nodename,_bitwise_or)) {
		return bitwiseOr(node,output);
	}
	if (!charstring::compare(nodename,_bitwise_xor)) {
		return bitwiseXor(node,output);
	}
	if (!charstring::compare(nodename,_number)) {
		return number(node,output);
	}
	if (!charstring::compare(nodename,_string_literal)) {
		return stringLiteral(node,output);
	}
	if (!charstring::compare(nodename,_bind_variable)) {
		return bindVariable(node,output);
	}
	if (!charstring::compare(nodename,_column_reference)) {
		return columnReference(node,output);
	}
	if (!charstring::compare(nodename,_column_name_database)) {
		return columnNameDatabase(node,output);
	}
	if (!charstring::compare(nodename,_column_name_schema)) {
		return columnNameSchema(node,output);
	}
	if (!charstring::compare(nodename,_column_name_table)) {
		return columnNameTable(node,output);
	}
	if (!charstring::compare(nodename,_column_name_column)) {
		return columnNameColumn(node,output);
	}
	if (!charstring::compare(nodename,_function)) {
		return function(node,output);
	}
	if (!charstring::compare(nodename,_parameters)) {
		return parameters(node,output);
	}
	if (!charstring::compare(nodename,_parameter)) {
		return parameter(node,output);
	}
	if (!charstring::compare(nodename,_group_by)) {
		return groupBy(node,output);
	}
	if (!charstring::compare(nodename,_group_by_item)) {
		return groupByItem(node,output);
	}
	if (!charstring::compare(nodename,_with_rollup)) {
		return withRollup(node,output);
	}
	if (!charstring::compare(nodename,_having)) {
		return having(node,output);
	}
	if (!charstring::compare(nodename,_order_by)) {
		return orderBy(node,output);
	}
	if (!charstring::compare(nodename,_order_by_item)) {
		return orderByItem(node,output);
	}
	if (!charstring::compare(nodename,_asc)) {
		return asc(node,output);
	}
	if (!charstring::compare(nodename,_desc)) {
		return desc(node,output);
	}
	if (!charstring::compare(nodename,_limit)) {
		return limit(node,output);
	}
	if (!charstring::compare(nodename,_select_into)) {
		return selectInto(node,output);
	}
	if (!charstring::compare(nodename,_procedure)) {
		return procedure(node,output);
	}
	if (!charstring::compare(nodename,_for_update)) {
		return forUpdate(node,output);
	}
	if (!charstring::compare(nodename,_set)) {
		return setQuery(node,output);
	}
	if (!charstring::compare(nodename,_set_global)) {
		return setGlobal(node,output);
	}
	if (!charstring::compare(nodename,_set_session)) {
		return setSession(node,output);
	}
	if (!charstring::compare(nodename,_transaction)) {
		return transaction(node,output);
	}
	if (!charstring::compare(nodename,_limit)) {
		return limit(node,output);
	}
	if (!charstring::compare(nodename,_select_into)) {
		return selectInto(node,output);
	}
	if (!charstring::compare(nodename,_procedure)) {
		return procedure(node,output);
	}
	if (!charstring::compare(nodename,_for_update)) {
		return forUpdate(node,output);
	}
	if (!charstring::compare(nodename,_set)) {
		return setQuery(node,output);
	}
	if (!charstring::compare(nodename,_set_global)) {
		return setGlobal(node,output);
	}
	if (!charstring::compare(nodename,_set_session)) {
		return setSession(node,output);
	}
	if (!charstring::compare(nodename,_transaction)) {
		return transaction(node,output);
	}
	if (!charstring::compare(nodename,_isolation_level)) {
		return isolationLevel(node,output);
	}
	if (!charstring::compare(nodename,_lock)) {
		return lockQuery(node,output);
	}
	if (!charstring::compare(nodename,_in_mode)) {
		return inMode(node,output);
	}
	if (!charstring::compare(nodename,_lock_mode)) {
		return lockMode(node,output);
	}
	if (!charstring::compare(nodename,_mode)) {
		return mode(node,output);
	}
	if (!charstring::compare(nodename,_nowait)) {
		return noWait(node,output);
	}
	if (!charstring::compare(nodename,_show)) {
		return show(node,output);
	}
	return true;
}

bool sqlparser::handleEnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();

	if (!charstring::compare(
		node->getAttributeValue("supported"),"false")) {
		return true;
	}

	const char	*nodename=node->getName();

	// generic...
	if (!charstring::compare(nodename,_type)) {
		return endType(node,output);
	}
	if (!charstring::compare(nodename,_size)) {
		return endSize(node,output);
	}

	// column definitions...
	if (!charstring::compare(nodename,_columns)) {
		return endColumns(node,output);
	}
	if (!charstring::compare(nodename,_column)) {
		return endColumn(node,output);
	}
	if (!charstring::compare(nodename,_values)) {
		return endValues(node,output);
	}
	if (!charstring::compare(nodename,_check)) {
		return endCheck(node,output);
	}

	// drop...
	if (!charstring::compare(nodename,
					_table_name_list_item)) {
		return endTableNameListItem(node,output);
	}

	// insert...
	if (!charstring::compare(nodename,
					_insert_values_clause)) {
		return endInsertValuesClause(node,output);
	}
	if (!charstring::compare(nodename,
					_insert_value_clause)) {
		return endInsertValueClause(node,output);
	}
	if (!charstring::compare(nodename,_insert_value)) {
		return endInsertValue(node,output);
	}

	// update...
	if (!charstring::compare(nodename,_assignment)) {
		return endAssignment(node,output);
	}

	// select...
	if (!charstring::compare(nodename,
					_select_expression)) {
		return endSelectExpression(node,output);
	}
	if (!charstring::compare(nodename,_sub_select)) {
		return endSubSelect(node,output);
	}
	if (!charstring::compare(nodename,_group)) {
		return endGroup(node,output);
	}
	if (!charstring::compare(nodename,_parameters)) {
		return endParameters(node,output);
	}
	if (!charstring::compare(nodename,_parameter)) {
		return endParameter(node,output);
	}
	if (!charstring::compare(nodename,_table_reference)) {
		return endTableReference(node,output);
	}
	if (!charstring::compare(nodename,_join_clause)) {
		return endJoinClause(node,output);
	}
	if (!charstring::compare(nodename,_order_by_item)) {
		return endOrderByItem(node,output);
	}
	if (!charstring::compare(nodename,_group_by_item)) {
		return endGroupByItem(node,output);
	}
	if (!charstring::compare(nodename,_in)) {
		return endIn(node,output);
	}
	if (!charstring::compare(nodename,_in_set_item)) {
		return endInSetItem(node,output);
	}
	if (!charstring::compare(nodename,_exists)) {
		return endExists(node,output);
	}
	return true;
}

bool sqlparser::outputValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(node->getAttributeValue(_value));
	return true;
}

bool sqlparser::space(stringbuffer *output) {
	debugFunction();
	output->append(" ");
	return true;
}

bool sqlparser::comma(stringbuffer *output) {
	debugFunction();
	output->append(",");
	return true;
}

bool sqlparser::period(stringbuffer *output) {
	debugFunction();
	output->append(".");
	return true;
}

bool sqlparser::leftParen(stringbuffer *output) {
	debugFunction();
	output->append("(");
	return true;
}

bool sqlparser::rightParen(stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlparser::hasSibling(xmldomnode *node) {
	debugFunction();
	return !node->getNextTagSibling()->isNullNode();
}

bool sqlparser::dontAppendSpace(stringbuffer *output) {
	debugFunction();
	size_t	length=output->getStringLength();
	return (length &&
		character::inSet(output->getString()[length-1]," .(,"));
}

bool sqlparser::createQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("create");
	return true;
}

bool sqlparser::table(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("table");
	return true;
}

bool sqlparser::global(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("global");
	return true;
}

bool sqlparser::temporary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("temporary");
	return true;
}

bool sqlparser::ifNotExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("if not exists");
	return true;
}

bool sqlparser::columns(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlparser::endColumns(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlparser::column(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endColumn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlparser::values(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlparser::endValues(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlparser::length(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::scale(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	comma(output);
	outputValue(node,output);
	return true;
}

bool sqlparser::constraints(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::unsignedConstraint(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unsigned");
	return true;
}

bool sqlparser::zerofill(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("zerofill");
	return true;
}

bool sqlparser::binary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("binary");
	return true;
}

bool sqlparser::characterSet(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("character set ");
	outputValue(node,output);
	return true;
}

bool sqlparser::collate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("collate ");
	outputValue(node,output);
	return true;
}

bool sqlparser::nullable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("null");
	return true;
}

bool sqlparser::notNull(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("not null");
	return true;
}

bool sqlparser::defaultValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("default ");
	outputValue(node,output);
	return true;
}

bool sqlparser::autoIncrement(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("auto_increment");
	return true;
}

bool sqlparser::identity(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("identity");
	return true;
}

bool sqlparser::uniqueKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique key");
	return true;
}

bool sqlparser::primaryKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("primary key");
	return true;
}

bool sqlparser::key(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("key");
	return true;
}

bool sqlparser::comment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("comment ");
	outputValue(node,output);
	return true;
}

bool sqlparser::columnFormat(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("column_format ");
	outputValue(node,output);
	return true;
}

bool sqlparser::references(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("references ");
	return true;
}

bool sqlparser::endReferences(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::match(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("match ");
	outputValue(node,output);
	return true;
}

bool sqlparser::onDelete(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on delete ");
	outputValue(node,output);
	return true;
}

bool sqlparser::onUpdate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on update ");
	outputValue(node,output);
	return true;
}

bool sqlparser::onCommit(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on commit ");
	outputValue(node,output);
	return true;
}

bool sqlparser::as(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("as ");
	outputValue(node,output);
	return true;
}

bool sqlparser::constraint(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	const char	*name=node->getAttributeValue("value");
	if (name) {
		output->append("constraint ")->append(name);
	}
	return true;
}

bool sqlparser::foreignKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("foreign key ");
	outputValue(node,output);
	return true;
}

bool sqlparser::check(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("check (");
	return true;
}

bool sqlparser::endCheck(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlparser::withNoLog(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with no log ");
	outputValue(node,output);
	return true;
}

bool sqlparser::fulltext(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("fulltext ");
	outputValue(node,output);
	return true;
}

bool sqlparser::spatial(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("spatial ");
	outputValue(node,output);
	return true;
}

bool sqlparser::index(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("index ");
	outputValue(node,output);
	return true;
}

bool sqlparser::indexNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlparser::indexNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlparser::indexNameIndex(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::btree(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("btree ");
	outputValue(node,output);
	return true;
}

bool sqlparser::hash(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("hash ");
	outputValue(node,output);
	return true;
}

bool sqlparser::keyBlockSize(xmldomnode *node, stringbuffer *output) {
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

bool sqlparser::withParser(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with parser ");
	outputValue(node,output);
	return true;
}

bool sqlparser::synonym(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("synonym ");
	return true;
}

bool sqlparser::forClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("for ");
	return true;
}

bool sqlparser::objectNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlparser::objectNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlparser::objectNameObject(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::deleteQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("delete");
	return true;
}

bool sqlparser::deleteFrom(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("from");
	return true;
}

bool sqlparser::usingClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("using");
	return true;
}

bool sqlparser::dropQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("drop");
	return true;
}

bool sqlparser::ifExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("if exists");
	return true;
}

bool sqlparser::endTableNameListItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (!node->getNextSibling()->isNullNode()) {
		comma(output);
	}
	return true;
}

bool sqlparser::restrictClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("restrict");
	return true;
}

bool sqlparser::cascade(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cascade");
	return true;
}

bool sqlparser::cascadeConstraintsClause(xmldomnode *node,
						stringbuffer *output) {
	debugFunction();
	output->append("constraints");
	return true;
}

bool sqlparser::tableNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlparser::tableNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlparser::tableNameTable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::name(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::type(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::endType(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::size(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlparser::endSize(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlparser::value(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlparser::verbatim(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::insertQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("insert");
	return true;
}

bool sqlparser::insertInto(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("into");
	return true;
}

bool sqlparser::insertValuesClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("values (");
	return true;
}

bool sqlparser::insertValueClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("value (");
	return true;
}

bool sqlparser::insertValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endInsertValuesClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlparser::endInsertValueClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlparser::endInsertValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlparser::selectQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("select");
	return true;
}

bool sqlparser::selectExpressions(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::selectExpression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endSelectExpression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlparser::subSelect(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlparser::endSubSelect(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlparser::unionClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("union");
	return true;
}

bool sqlparser::all(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("all");
	return true;
}

bool sqlparser::alias(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::unique(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique");
	return true;
}

bool sqlparser::distinct(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("distinct");
	return true;
}

bool sqlparser::from(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("from");
	return true;
}

bool sqlparser::tableReferences(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::tableReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endTableReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node isn't a join clause
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		charstring::compare(next->getName(),_join_clause)) {
		comma(output);
	}
	return true;
}

bool sqlparser::joinClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endJoinClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node isn't a join clause
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		charstring::compare(next->getName(),_join_clause)) {
		comma(output);
	}
	return true;
}

bool sqlparser::inner(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("inner");
	return true;
}

bool sqlparser::cross(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("cross");
	return true;
}

bool sqlparser::straightJoin(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("straight_join");
	return true;
}

bool sqlparser::left(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("left");
	return true;
}

bool sqlparser::right(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("right");
	return true;
}

bool sqlparser::outer(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("outer");
	return true;
}

bool sqlparser::natural(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("natural");
	return true;
}

bool sqlparser::join(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("join");
	return true;
}

bool sqlparser::on(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on");
	return true;
}

bool sqlparser::joinUsing(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("using");
	return true;
}

bool sqlparser::groupBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("group by");
	return true;
}

bool sqlparser::groupByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endGroupByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	// write a comma if the next node is another group-by-item
	xmldomnode	*next=node->getNextTagSibling();
	if (!next->isNullNode() &&
		!charstring::compare(next->getName(),_group_by_item)) {
		comma(output);
	}
	return true;
}

bool sqlparser::withRollup(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("with rollup");
	return true;
}

bool sqlparser::having(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("having");
	return true;
}

bool sqlparser::orderBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("order by");
	return true;
}

bool sqlparser::orderByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endOrderByItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlparser::asc(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("asc");
	return true;
}

bool sqlparser::desc(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("desc");
	return true;
}

bool sqlparser::limit(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("limit");
	return true;
}

bool sqlparser::selectInto(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("into");
	return true;
}

bool sqlparser::procedure(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("procedure");
	return true;
}

bool sqlparser::forUpdate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("for update");
	return true;
}

bool sqlparser::where(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("where");
	return true;
}

bool sqlparser::andClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("and");
	return true;
}

bool sqlparser::orClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("or");
	return true;
}

bool sqlparser::comparison(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::notClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("not");
	return true;
}

bool sqlparser::group(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("(");
	return true;
}

bool sqlparser::endGroup(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(")");
	return true;
}

bool sqlparser::between(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("between");
	return true;
}

bool sqlparser::in(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("in ");
	leftParen(output);
	return true;
}

bool sqlparser::endIn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlparser::inSetItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endInSetItem(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlparser::exists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("exists ");
	leftParen(output);
	return true;
}

bool sqlparser::endExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlparser::is(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("is");
	return true;
}

bool sqlparser::like(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("like");
	return true;
}

bool sqlparser::matches(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("matches");
	return true;
}

bool sqlparser::nullSafeEquals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<=>");
	return true;
}

bool sqlparser::notEquals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("!=");
	return true;
}

bool sqlparser::lessThan(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<");
	return true;
}

bool sqlparser::greaterThan(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(">");
	return true;
}

bool sqlparser::lessThanOrEqualTo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("<=");
	return true;
}

bool sqlparser::greaterThanOrEqualTo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(">=");
	return true;
}

bool sqlparser::escape(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("escape");
	return true;
}

bool sqlparser::expression(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::intervalQualifier(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append(node->getAttributeValue(_from));
	const char	*precision=
			node->getAttributeValue(_from_precision);
	const char	*scale=
			node->getAttributeValue(_from_scale);
	if (precision) {
		leftParen(output);
		output->append(precision);
		if (scale) {
			comma(output);
			output->append(scale);
		}
		rightParen(output);
	}
	if (charstring::length(node->getAttributeValue(_to))) {
		output->append(" to ");
		output->append(node->getAttributeValue(_to));
		precision=node->getAttributeValue(_to_precision);
		scale=node->getAttributeValue(_to_scale);
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

bool sqlparser::outerJoinOperator(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("(+)");
	return true;
}

bool sqlparser::compliment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("~");
	return true;
}

bool sqlparser::inverse(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("!");
	return true;
}

bool sqlparser::negative(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("-");
	return true;
}

bool sqlparser::plus(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("+");
	return true;
}

bool sqlparser::minus(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("-");
	return true;
}

bool sqlparser::times(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("*");
	return true;
}

bool sqlparser::dividedBy(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("/");
	return true;
}

bool sqlparser::modulo(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("%");
	return true;
}

bool sqlparser::bitwiseAnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("&");
	return true;
}

bool sqlparser::bitwiseOr(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("|");
	return true;
}

bool sqlparser::bitwiseXor(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("^");
	return true;
}

bool sqlparser::logicalAnd(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("&&");
	return true;
}

bool sqlparser::logicalOr(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("||");
	return true;
}

bool sqlparser::number(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::stringLiteral(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::bindVariable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::columnReference(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::columnNameDatabase(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlparser::columnNameSchema(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlparser::columnNameTable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	if (hasSibling(node)) {
		period(output);
	}
	return true;
}

bool sqlparser::columnNameColumn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::function(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::parameters(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlparser::endParameters(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlparser::parameter(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endParameter(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlparser::updateQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("update");
	return true;
}

bool sqlparser::updateSet(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("set");
	return true;
}

bool sqlparser::assignment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlparser::endAssignment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		output->append(",");
	}
	return true;
}

bool sqlparser::equals(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("=");
	return true;
}

bool sqlparser::setQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("set");
	return true;
}

bool sqlparser::setSession(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("session");
	return true;
}

bool sqlparser::setGlobal(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("global");
	return true;
}

bool sqlparser::transaction(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("transaction ");
	return true;
}

bool sqlparser::isolationLevel(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("isolation level ");
	outputValue(node,output);
	return true;
}

bool sqlparser::lockQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("lock");
	return true;
}

bool sqlparser::inMode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("in");
	return true;
}

bool sqlparser::lockMode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlparser::mode(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("mode");
	return true;
}

bool sqlparser::noWait(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("nowait");
	return true;
}

bool sqlparser::show(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("show ");
	outputValue(node,output);
	return true;
}
