// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLWRITER_H
#define SQLWRITER_H

#include <sqlrelay/sqlrserverdll.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class SQLRSERVER_DLLSPEC sqlwriter {
	public:
			sqlwriter();
		virtual	~sqlwriter();

		virtual bool	write(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldom *tree,
					stringbuffer *output);
		virtual bool	write(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					xmldomnode *tree,
					stringbuffer *output,
					bool omitsiblings);
	protected:
		virtual bool	write(xmldomnode *tree, 
					stringbuffer *output);

		virtual bool	handleStart(xmldomnode *node,
						stringbuffer *output);
		virtual bool	handleEnd(xmldomnode *node,
						stringbuffer *output);

		// generic
		virtual bool	tableNameDatabase(xmldomnode *node,
						stringbuffer *output);
		virtual bool	tableNameSchema(xmldomnode *node,
						stringbuffer *output);
		virtual bool	tableNameTable(xmldomnode *node,
						stringbuffer *output);
		virtual bool	name(xmldomnode *node,
					stringbuffer *output);
		virtual bool	type(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endType(xmldomnode *node,
					stringbuffer *output);
		virtual bool	size(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endSize(xmldomnode *node,
					stringbuffer *output);
		virtual bool	value(xmldomnode *node,
					stringbuffer *output);
		virtual bool	verbatim(xmldomnode *node,
					stringbuffer *output);

		// create query...
		virtual bool	createQuery(xmldomnode *node,
					stringbuffer *output);

		// table...
		virtual bool	table(xmldomnode *node,
					stringbuffer *output);
		virtual bool	global(xmldomnode *node,
					stringbuffer *output);
		virtual bool	temporary(xmldomnode *node,
					stringbuffer *output);
		virtual bool	ifNotExists(xmldomnode *node,
					stringbuffer *output);

		// index...
		virtual bool	fulltext(xmldomnode *node,
					stringbuffer *output);
		virtual bool	spatial(xmldomnode *node,
					stringbuffer *output);
		virtual bool	index(xmldomnode *node,
					stringbuffer *output);
		virtual bool	indexNameDatabase(xmldomnode *node,
					stringbuffer *output);
		virtual bool	indexNameSchema(xmldomnode *node,
					stringbuffer *output);
		virtual bool	indexNameIndex(xmldomnode *node,
					stringbuffer *output);
		virtual bool	btree(xmldomnode *node,
					stringbuffer *output);
		virtual bool	hash(xmldomnode *node,
					stringbuffer *output);
		virtual bool	keyBlockSize(xmldomnode *node,
					stringbuffer *output);
		virtual bool	withParser(xmldomnode *node,
					stringbuffer *output);


		// synonym...
		virtual bool	synonym(xmldomnode *node,
					stringbuffer *output);
		virtual bool	forClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	objectNameDatabase(xmldomnode *node,
					stringbuffer *output);
		virtual bool	objectNameSchema(xmldomnode *node,
					stringbuffer *output);
		virtual bool	objectNameObject(xmldomnode *node,
					stringbuffer *output);
		

		// column definitions...
		virtual bool	columns(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endColumns(xmldomnode *node,
					stringbuffer *output);
		virtual bool	column(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endColumn(xmldomnode *node,
					stringbuffer *output);
		virtual bool	values(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endValues(xmldomnode *node,
					stringbuffer *output);
		virtual bool	length(xmldomnode *node,
					stringbuffer *output);
		virtual bool	scale(xmldomnode *node,
					stringbuffer *output);

		// constraints...
		virtual bool	constraints(xmldomnode *node,
					stringbuffer *output);
		virtual bool	unsignedConstraint(xmldomnode *node,
					stringbuffer *output);
		virtual bool	zerofill(xmldomnode *node,
					stringbuffer *output);
		virtual bool	binary(xmldomnode *node,
					stringbuffer *output);
		virtual bool	characterSet(xmldomnode *node,
					stringbuffer *output);
		virtual bool	collate(xmldomnode *node,
					stringbuffer *output);
		virtual bool	nullable(xmldomnode *node,
					stringbuffer *output);
		virtual bool	notNull(xmldomnode *node,
					stringbuffer *output);
		virtual bool	defaultValue(xmldomnode *node,
					stringbuffer *output);
		virtual bool	autoIncrement(xmldomnode *node,
					stringbuffer *output);
		virtual bool	identity(xmldomnode *node,
					stringbuffer *output);
		virtual bool	uniqueKey(xmldomnode *node,
					stringbuffer *output);
		virtual bool	primaryKey(xmldomnode *node,
					stringbuffer *output);
		virtual bool	key(xmldomnode *node,
					stringbuffer *output);
		virtual bool	comment(xmldomnode *node,
					stringbuffer *output);
		virtual bool	columnFormat(xmldomnode *node,
					stringbuffer *output);
		virtual bool	references(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endReferences(xmldomnode *node,
					stringbuffer *output);
		virtual bool	match(xmldomnode *node,
					stringbuffer *output);
		virtual bool	onDelete(xmldomnode *node,
					stringbuffer *output);
		virtual bool	onUpdate(xmldomnode *node,
					stringbuffer *output);
		virtual bool	onCommit(xmldomnode *node,
					stringbuffer *output);
		virtual bool	as(xmldomnode *node,
					stringbuffer *output);
		virtual bool	constraint(xmldomnode *node,
					stringbuffer *output);
		virtual bool	foreignKey(xmldomnode *node,
					stringbuffer *output);
		virtual bool	check(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endCheck(xmldomnode *node,
					stringbuffer *output);


		// table creation qualifiers...
		virtual bool	withNoLog(xmldomnode *node,
					stringbuffer *output);


		// drop...
		virtual bool	dropQuery(xmldomnode *node,
					stringbuffer *output);
		virtual bool	ifExists(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endTableNameListItem(xmldomnode *node,
					stringbuffer *output);
		virtual bool	restrictClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	cascade(xmldomnode *node,
					stringbuffer *output);
		virtual bool	cascadeConstraintsClause(xmldomnode *node,
					stringbuffer *output);


		// insert...
		virtual bool	insertQuery(xmldomnode *node,
					stringbuffer *output);
		virtual bool	insertInto(xmldomnode *node,
					stringbuffer *output);
		virtual bool	insertValuesClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endInsertValuesClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	insertValueClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endInsertValueClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	insertValue(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endInsertValue(xmldomnode *node,
					stringbuffer *output);


		// update...
		virtual bool	updateQuery(xmldomnode *node,
					stringbuffer *output);
		virtual bool	updateSet(xmldomnode *node,
					stringbuffer *output);
		virtual bool	assignment(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endAssignment(xmldomnode *node,
					stringbuffer *output);
		virtual bool	equals(xmldomnode *node,
					stringbuffer *output);


		// delete...
		virtual bool	deleteQuery(xmldomnode *node,
					stringbuffer *output);
		virtual bool	deleteFrom(xmldomnode *node,
					stringbuffer *output);
		virtual bool	usingClause(xmldomnode *node,
					stringbuffer *output);


		// select...
		virtual bool	selectQuery(xmldomnode *node,
					stringbuffer *output);
		virtual bool	selectExpressions(xmldomnode *node,
					stringbuffer *output);
		virtual bool	selectExpression(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endSelectExpression(xmldomnode *node,
					stringbuffer *output);
		virtual bool	subSelect(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endSubSelect(xmldomnode *node,
					stringbuffer *output);
		virtual bool	unionClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	all(xmldomnode *node,
					stringbuffer *output);
		virtual bool	alias(xmldomnode *node,
					stringbuffer *output);
		virtual bool	unique(xmldomnode *node,
					stringbuffer *output);
		virtual bool	distinct(xmldomnode *node,
					stringbuffer *output);
		virtual bool	from(xmldomnode *node,
					stringbuffer *output);
		virtual bool	tableReferences(xmldomnode *node,
					stringbuffer *output);
		virtual bool	tableReference(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endTableReference(xmldomnode *node,
					stringbuffer *output);
		virtual bool	joinClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endJoinClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	inner(xmldomnode *node,
					stringbuffer *output);
		virtual bool	cross(xmldomnode *node,
					stringbuffer *output);
		virtual bool	straightJoin(xmldomnode *node,
					stringbuffer *output);
		virtual bool	left(xmldomnode *node,
					stringbuffer *output);
		virtual bool	right(xmldomnode *node,
					stringbuffer *output);
		virtual bool	outer(xmldomnode *node,
					stringbuffer *output);
		virtual bool	natural(xmldomnode *node,
					stringbuffer *output);
		virtual bool	join(xmldomnode *node,
					stringbuffer *output);
		virtual bool	on(xmldomnode *node,
					stringbuffer *output);
		virtual bool	joinUsing(xmldomnode *node,
					stringbuffer *output);
		virtual bool	where(xmldomnode *node,
					stringbuffer *output);
		virtual bool	andClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	orClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	group(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endGroup(xmldomnode *node,
					stringbuffer *output);
		virtual bool	comparison(xmldomnode *node,
					stringbuffer *output);
		virtual bool	notClause(xmldomnode *node,
					stringbuffer *output);
		virtual bool	between(xmldomnode *node,
					stringbuffer *output);
		virtual bool	in(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endIn(xmldomnode *node,
					stringbuffer *output);
		virtual bool	inSetItem(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endInSetItem(xmldomnode *node,
					stringbuffer *output);
		virtual bool	exists(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endExists(xmldomnode *node,
					stringbuffer *output);
		virtual bool	is(xmldomnode *node,
					stringbuffer *output);
		virtual bool	like(xmldomnode *node,
					stringbuffer *output);
		virtual bool	matches(xmldomnode *node,
					stringbuffer *output);
		virtual bool	nullSafeEquals(xmldomnode *node,
					stringbuffer *output);
		virtual bool	notEquals(xmldomnode *node,
					stringbuffer *output);
		virtual bool	lessThan(xmldomnode *node,
					stringbuffer *output);
		virtual bool	greaterThan(xmldomnode *node,
					stringbuffer *output);
		virtual bool	lessThanOrEqualTo(xmldomnode *node,
					stringbuffer *output);
		virtual bool	greaterThanOrEqualTo(xmldomnode *node,
					stringbuffer *output);
		virtual bool	escape(xmldomnode *node,
					stringbuffer *output);
		virtual bool	expression(xmldomnode *node,
					stringbuffer *output);
		virtual bool	intervalQualifier(xmldomnode *node,
					stringbuffer *output);
		virtual bool	outerJoinOperator(xmldomnode *node,
					stringbuffer *output);
		virtual bool	compliment(xmldomnode *node,
					stringbuffer *output);
		virtual bool	inverse(xmldomnode *node,
					stringbuffer *output);
		virtual bool	negative(xmldomnode *node,
					stringbuffer *output);
		virtual bool	plus(xmldomnode *node,
					stringbuffer *output);
		virtual bool	minus(xmldomnode *node,
					stringbuffer *output);
		virtual bool	times(xmldomnode *node,
					stringbuffer *output);
		virtual bool	dividedBy(xmldomnode *node,
					stringbuffer *output);
		virtual bool	modulo(xmldomnode *node,
					stringbuffer *output);
		virtual bool	bitwiseAnd(xmldomnode *node,
					stringbuffer *output);
		virtual bool	bitwiseOr(xmldomnode *node,
					stringbuffer *output);
		virtual bool	bitwiseXor(xmldomnode *node,
					stringbuffer *output);
		virtual bool	logicalAnd(xmldomnode *node,
					stringbuffer *output);
		virtual bool	logicalOr(xmldomnode *node,
					stringbuffer *output);
		virtual bool	number(xmldomnode *node,
					stringbuffer *output);
		virtual bool	stringLiteral(xmldomnode *node,
					stringbuffer *output);
		virtual bool	bindVariable(xmldomnode *node,
					stringbuffer *output);
		virtual bool	columnReference(xmldomnode *node,
					stringbuffer *output);
		virtual bool	columnNameDatabase(xmldomnode *node,
					stringbuffer *output);
		virtual bool	columnNameSchema(xmldomnode *node,
					stringbuffer *output);
		virtual bool	columnNameTable(xmldomnode *node,
					stringbuffer *output);
		virtual bool	columnNameColumn(xmldomnode *node,
					stringbuffer *output);
		virtual bool	function(xmldomnode *node,
					stringbuffer *output);
		virtual bool	parameters(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endParameters(xmldomnode *node,
					stringbuffer *output);
		virtual bool	parameter(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endParameter(xmldomnode *node,
					stringbuffer *output);
		virtual bool	groupBy(xmldomnode *node,
					stringbuffer *output);
		virtual bool	groupByItem(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endGroupByItem(xmldomnode *node,
					stringbuffer *output);
		virtual bool	withRollup(xmldomnode *node,
					stringbuffer *output);
		virtual bool	having(xmldomnode *node,
					stringbuffer *output);
		virtual bool	orderBy(xmldomnode *node,
					stringbuffer *output);
		virtual bool	orderByItem(xmldomnode *node,
					stringbuffer *output);
		virtual bool	endOrderByItem(xmldomnode *node,
					stringbuffer *output);
		virtual bool	asc(xmldomnode *node,
					stringbuffer *output);
		virtual bool	desc(xmldomnode *node,
					stringbuffer *output);
		virtual bool	limit(xmldomnode *node,
					stringbuffer *output);
		virtual bool	selectInto(xmldomnode *node,
					stringbuffer *output);
		virtual bool	procedure(xmldomnode *node,
					stringbuffer *output);
		virtual bool	forUpdate(xmldomnode *node,
					stringbuffer *output);


		// set...
		virtual bool	setQuery(xmldomnode *node,
					stringbuffer *output);
		virtual bool	setSession(xmldomnode *node,
					stringbuffer *output);
		virtual bool	setGlobal(xmldomnode *node,
					stringbuffer *output);
		virtual bool	transaction(xmldomnode *node,
					stringbuffer *output);
		virtual bool	isolationLevel(xmldomnode *node,
					stringbuffer *output);


		// lock...
		virtual bool	lockQuery(xmldomnode *node,
					stringbuffer *output);
		virtual bool	inMode(xmldomnode *node,
					stringbuffer *output);
		virtual bool	lockMode(xmldomnode *node,
					stringbuffer *output);
		virtual bool	mode(xmldomnode *node,
					stringbuffer *output);
		virtual bool	noWait(xmldomnode *node,
					stringbuffer *output);

		virtual bool	show(xmldomnode *node,
					stringbuffer *output);



		// helper methods
		virtual bool	outputValue(xmldomnode *node,
					stringbuffer *output);
		virtual bool	space(stringbuffer *output);
		virtual bool	comma(stringbuffer *output);
		virtual bool	period(stringbuffer *output);
		virtual bool	leftParen(stringbuffer *output);
		virtual bool	rightParen(stringbuffer *output);
		virtual bool	hasSibling(xmldomnode *node);
		virtual bool	dontAppendSpace(stringbuffer *output);

		sqlrconnection_svr	*sqlrcon;
		sqlrcursor_svr		*sqlrcur;
};

#endif
