// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLWRITER_H
#define SQLWRITER_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqlwriter {
	public:
			sqlwriter();
		virtual	~sqlwriter();

		virtual bool	write(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldom *tree,
				rudiments::stringbuffer *output);
		virtual bool	write(sqlrconnection_svr *sqlrcon,
					sqlrcursor_svr *sqlrcur,
					rudiments::xmldomnode *tree,
					rudiments::stringbuffer *output,
					bool omitsiblings);
	protected:
		virtual bool	write(rudiments::xmldomnode *tree, 
					rudiments::stringbuffer *output);

		virtual const char * const *supportedElements();
		virtual const char * const *unsupportedElements();

		virtual	bool	elementSupported(const char *element);
		virtual bool	handleStart(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	handleEnd(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);

		// generic
		virtual bool	tableNameDatabase(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	tableNameSchema(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	tableNameTable(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	name(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	type(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endType(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	size(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endSize(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	value(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	verbatim(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);

		// create query...
		virtual bool	createQuery(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);

		// table...
		virtual bool	table(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	global(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	temporary(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	ifNotExists(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);

		// index...
		virtual bool	fulltext(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	spatial(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	index(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	indexNameDatabase(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	indexNameSchema(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	indexNameIndex(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	btree(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	hash(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);


		// synonym...
		virtual bool	synonym(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	forClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	objectNameDatabase(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	objectNameSchema(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	objectNameObject(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		

		// column definitions...
		virtual bool	columns(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endColumns(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	column(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endColumn(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	values(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endValues(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	length(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	scale(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);

		// constraints...
		virtual bool	constraints(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	unsignedConstraint(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	zerofill(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	binary(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	characterSet(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	collate(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	nullable(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	notNull(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	defaultValue(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	autoIncrement(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	uniqueKey(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	primaryKey(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	key(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	comment(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	columnFormat(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	references(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endReferences(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	match(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	onDelete(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	onUpdate(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	onCommit(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	as(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	constraint(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	foreignKey(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	check(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);


		// table creation qualifiers...
		virtual bool	withNoLog(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);


		// drop...
		virtual bool	dropQuery(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	ifExists(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endTableNameListItem(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	restrictClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	cascade(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	cascadeConstraintsClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);


		// insert...
		virtual bool	insertQuery(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	insertInto(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	insertValuesClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endInsertValuesClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	insertValueClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endInsertValueClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	insertValue(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endInsertValue(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);


		// update...
		virtual bool	updateQuery(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	updateSet(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	assignment(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endAssignment(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	equals(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);


		// delete...
		virtual bool	deleteQuery(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	deleteFrom(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	usingClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);


		// select...
		virtual bool	selectQuery(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	selectExpressions(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	selectExpression(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endSelectExpression(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	subSelect(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endSubSelect(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	unionClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	all(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	alias(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	unique(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	distinct(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	from(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	tableReferences(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	tableReference(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endTableReference(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	joinClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endJoinClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	inner(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	cross(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	straightJoin(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	left(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	right(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	outer(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	natural(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	join(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	on(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	joinUsing(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	where(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	andClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	orClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	group(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endGroup(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	comparison(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	notClause(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	between(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	in(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endIn(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	inSetItem(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endInSetItem(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	exists(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endExists(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	is(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	like(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	matches(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	nullSafeEquals(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	notEquals(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	lessThan(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	greaterThan(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	lessThanOrEqualTo(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	greaterThanOrEqualTo(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	escape(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	expression(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	intervalQualifier(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	outerJoinOperator(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	compliment(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	inverse(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	negative(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	plus(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	minus(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	times(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	dividedBy(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	modulo(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	bitwiseAnd(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	bitwiseOr(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	bitwiseXor(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	logicalAnd(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	logicalOr(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	number(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	stringLiteral(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	bindVariable(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	columnReference(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	columnNameDatabase(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	columnNameSchema(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	columnNameTable(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	columnNameColumn(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	function(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	parameters(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endParameters(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	parameter(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endParameter(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	groupBy(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	groupByItem(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endGroupByItem(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	withRollup(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	having(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	orderBy(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	orderByItem(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	endOrderByItem(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	asc(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	desc(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	limit(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	selectInto(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	procedure(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	forUpdate(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);


		// set...
		virtual bool	setQuery(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	setSession(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	setGlobal(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	transaction(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	isolationLevel(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);


		// lock...
		virtual bool	lockQuery(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	inMode(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	lockMode(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	mode(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	noWait(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);



		// helper methods
		virtual bool	outputValue(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	space(rudiments::stringbuffer *output);
		virtual bool	comma(rudiments::stringbuffer *output);
		virtual bool	period(rudiments::stringbuffer *output);
		virtual bool	leftParen(rudiments::stringbuffer *output);
		virtual bool	rightParen(rudiments::stringbuffer *output);
		virtual bool	hasSibling(rudiments::xmldomnode *node);
		virtual bool	dontAppendSpace(
					rudiments::stringbuffer *output);

		sqlrconnection_svr *sqlrcon;
		sqlrcursor_svr *sqlrcur;
};

#endif
