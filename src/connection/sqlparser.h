// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLPARSER
#define SQLPARSER

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

using namespace rudiments;

class sqlparser {
	public:
			sqlparser();
		virtual	~sqlparser();

		bool	parse(const char *query);
		xmldom	*detachTree();

	private:
		char	*cleanQuery(const char *query);
		bool	inSet(char c, const char *set);

		bool	comparePart(const char *ptr,
					const char **newptr,
					const char *part);
		bool	comparePart(const char *ptr,
					const char **newptr,
					const char * const *parts);

		char	*getWord(const char *ptr,
					const char **newptr);
		char	*getUntil(const char *set,
					const char *ptr,
					const char **newptr);
		char	*getClause(const char *ptr,
					const char *newptr);
		char	*getVerbatim(const char *ptr,
					const char **newptr);


		xmldomnode	*newNode(xmldomnode *parentnode,
						const char *type);
		xmldomnode	*newNode(xmldomnode *parentnode,
						const char *type,
						const char *value);
		void		setAttribute(xmldomnode *node,
						const char *name,
						const char *value);


		bool	whiteSpace(const char *ptr, const char **newptr);
		bool	comma(const char *ptr, const char **newptr);
		bool	equals(const char *ptr, const char **newptr);
		bool	notEquals(const char *ptr, const char **newptr);
		bool	lessThan(const char *ptr, const char **newptr);
		bool	greaterThan(const char *ptr, const char **newptr);
		bool	lessThanOrEqualTo(const char *ptr,
						const char **newptr);
		bool	greaterThanOrEqualTo(const char *ptr,
						const char **newptr);
		bool	leftParen(const char *ptr, const char **newptr);
		bool	rightParen(const char *ptr, const char **newptr);
		bool	compliment(const char *ptr, const char **newptr);
		bool	inverse(const char *ptr, const char **newptr);
		bool	plus(const char *ptr, const char **newptr);
		bool	minus(const char *ptr, const char **newptr);
		bool	times(const char *ptr, const char **newptr);
		bool	dividedBy(const char *ptr, const char **newptr);
		bool	modulo(const char *ptr, const char **newptr);
		bool	bitwiseAnd(const char *ptr, const char **newptr);
		bool	bitwiseOr(const char *ptr, const char **newptr);
		bool	bitwiseXor(const char *ptr, const char **newptr);
		bool	logicalAnd(const char *ptr, const char **newptr);
		bool	logicalOr(const char *ptr, const char **newptr);


	// Ideally these would be private but other classes (and their children)
	// need to access the static constants.  I could declare them in a
	// separate block but it's way easier if the parse/clause methods and
	// variable are all together.  Maybe I'll move them later.
	public:
		// generic...
		bool	parseTableName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_table_name;
		bool	parseName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_name;
		bool	parseType(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_type;
		static const char *_size;
		bool	parseValues(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_values;
		static const char *_value;
		bool	parseLength(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_length;
		bool	parseScale(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_scale;
		bool	parseVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_verbatim;
		bool	parseRemainderVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);



		// create query...
		bool	parseCreate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	createClause(const char *ptr,
						const char **newptr);
		static const char *_create;
		bool	parseCreateTemporary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	temporaryClause(const char *ptr,
						const char **newptr);
		static const char *_create_temporary;
		bool	tableClause(const char *ptr,
						const char **newptr);


		// create table...
		bool	parseCreateTable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_table;
		bool	parseIfNotExists(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	ifNotExistsClause(const char *ptr,
						const char **newptr);
		static const char *_if_not_exists;
		bool	parseColumnAndConstraintDefinitions(
						xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_columns;
		static const char *_column;
		bool	parseColumnDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseConstraints(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_constraints;
		bool	parseUnsigned(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	unsignedClause(const char *ptr,
						const char **newptr);
		static const char *_unsigned;
		bool	parseZeroFill(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	zeroFillClause(const char *ptr,
						const char **newptr);
		static const char *_zerofill;
		bool	parseBinary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	binaryClause(const char *ptr,
						const char **newptr);
		static const char *_binary;
		bool	parseCharacterSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	characterSetClause(const char *ptr,
						const char **newptr);
		static const char *_character_set;
		bool	parseCollate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	collateClause(const char *ptr,
						const char **newptr);
		static const char *_collate;
		bool	parseNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	nullClause(const char *ptr,
						const char **newptr);
		static const char *_null;
		bool	parseNotNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	notNullClause(const char *ptr,
						const char **newptr);
		static const char *_not_null;
		bool	parseDefault(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	defaultClause(const char *ptr,
						const char **newptr);
		static const char *_default;
		bool	parseAutoIncrement(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	autoIncrementClause(const char *ptr,
						const char **newptr);
		static const char *_auto_increment;
		bool	parseUniqueKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	uniqueKeyClause(const char *ptr,
						const char **newptr);
		static const char *_unique_key;
		bool	parsePrimaryKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	primaryKeyClause(const char *ptr,
						const char **newptr);
		static const char *_primary_key;
		bool	parseKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	keyClause(const char *ptr,
						const char **newptr);
		static const char *_key;
		bool	parseComment(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	commentClause(const char *ptr,
						const char **newptr);
		static const char *_comment;
		bool	parseColumnFormat(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	columnFormatClause(const char *ptr,
						const char **newptr);
		static const char *_column_format;
		bool	parseReferenceDefinition(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	referencesClause(const char *ptr,
						const char **newptr);
		static const char *_references;
		bool	parseColumnNameList(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseMatch(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	matchClause(const char *ptr,
						const char **newptr);
		static const char *_match;
		bool	parseOnDelete(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onDeleteClause(const char *ptr,
						const char **newptr);
		static const char *_on_delete;
		bool	parseOnUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onUpdateClause(const char *ptr,
						const char **newptr);
		static const char *_on_update;
		bool	parseReferenceOption(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	referenceOptionClause(const char *ptr,
						const char **newptr);
		bool	parseOnCommit(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onCommitClause(const char *ptr,
						const char **newptr);
		bool	onCommitOptionClause(const char *ptr,
						const char **newptr);
		static const char *_on_commit;
		bool	parseAs(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	asClause(const char *ptr,
						const char **newptr);
		static const char *_as;



		// drop query...
		bool	parseDrop(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	dropClause(const char *ptr,
						const char **newptr);
		static const char *_drop;
		bool	parseDropTemporary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_drop_temporary;
		bool	parseDropTable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseIfExists(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	ifExistsClause(const char *ptr,
						const char **newptr);
		static const char *_if_exists;
		bool	parseTableNameList(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_table_name_list;
		static const char *_table_name_list_item;
		bool	parseRestrict(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	restrictClause(const char *ptr,
						const char **newptr);
		static const char *_restrict;
		bool	parseCascade(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	cascadeClause(const char *ptr,
						const char **newptr);
		static const char *_cascade;



		// insert query...
		bool	parseInsert(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertClause(const char *ptr,
						const char **newptr);
		static const char *_insert;
		bool	parseInsertInto(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertIntoClause(const char *ptr,
						const char **newptr);
		static const char *_insert_into;
		bool	parseInsertValues(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertValuesClause(const char *ptr,
						const char **newptr);
		static const char *_insert_values_clause;
		bool	parseInsertValue(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertValueClause(const char *ptr,
						const char **newptr);
		static const char *_insert_value_clause;
		bool	parseInsertValuesList(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char *_insert_value;



		// update query...
		bool	parseUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	updateClause(const char *ptr,
						const char **newptr);
		static const char *_update;
		bool	parseUpdateSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	updateSetClause(const char *ptr,
						const char **newptr);
		static const char *_update_set;
		static const char *_assignment;
		static const char *_equals;



		// delete query...
		bool	parseDelete(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	deleteClause(const char *ptr,
						const char **newptr);
		static const char *_delete;
		bool	parseDeleteFrom(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	deleteFromClause(const char *ptr,
						const char **newptr);
		static const char *_delete_from;
		bool	parseUsing(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	usingClause(const char *ptr,
						const char **newptr);
		static const char *_using;



		// select query...
		bool	parseSelect(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	selectClause(const char *ptr,
						const char **newptr);
		static const char *_select;
		bool	parseUnique(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	uniqueClause(const char *ptr,
						const char **newptr);
		static const char *_unique;
		bool	parseDistinct(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	distinctClause(const char *ptr,
						const char **newptr);
		static const char *_distinct;
		bool	parseFrom(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	fromClause(const char *ptr,
						const char **newptr);
		static const char *_from;
		bool	parseWhere(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	whereClause(const char *ptr,
						const char **newptr);
		static const char	*_where;
		bool	parseWhereClauseTerms(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseAnd(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	andClause(const char *ptr, const char **newptr);
		static const char	*_and;
		bool	parseOr(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	orClause(const char *ptr, const char **newptr);
		static const char 	*_or;
		bool	parseWhereClauseTerm(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_group;
		bool	parseComparison(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						bool checkforgroup);
		static const char	*_comparison;
		bool	notClause(const char *ptr, const char **newptr);
		static const char	*_not;
		bool	parseBetween(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	betweenClause(const char *ptr, const char **newptr);
		static const char	*_between;
		bool	parseEquals(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseNotEquals(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_not_equals;
		bool	parseLessThan(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_less_than;
		bool	parseGreaterThan(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_greater_than;
		bool	parseLessThanOrEqualTo(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_less_than_or_equal_to;
		bool	parseGreaterThanOrEqualTo(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_greater_than_or_equal_to;
		bool	parseGroupBy(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseExpression(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_expression;
		bool	parseUnaryOperator(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseCompliment(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_compliment;
		bool	parseInverse(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_inverse;
		bool	parseNegative(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_negative;
		bool	parseBinaryOperator(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseTimes(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_times;
		bool	parseDividedBy(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_divided_by;
		bool	parseModulo(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_modulo;
		bool	parsePlus(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_plus;
		bool	parseMinus(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_minus;
		bool	parseLogicalAnd(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_logical_and;
		bool	parseLogicalOr(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_logical_or;
		bool	parseBitwiseAnd(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_bitwise_and;
		bool	parseBitwiseOr(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_bitwise_or;
		bool	parseBitwiseXor(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_bitwise_xor;
		bool	parseTerm(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_number;
		static const char	*_string_literal;
		static const char	*_bind_variable;
		bool	parseColumnOrFunction(xmldomnode *currentnode,
						const char *name,
						const char *ptr,
						const char **newptr);
		static const char	*_function;
		static const char	*_parameters;
		static const char	*_parameter;
		bool	specialFunctionName(const char *name);
		virtual const char * const	 *specialFunctionNames();
		bool	groupByClause(const char *ptr,
						const char **newptr);
		static const char *_group_by;
		bool	parseHaving(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	havingClause(const char *ptr,
						const char **newptr);
		static const char *_having;
		bool	parseOrderBy(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	orderByClause(const char *ptr,
						const char **newptr);
		static const char *_order_by;
		bool	parseLimit(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	limitClause(const char *ptr,
						const char **newptr);
		static const char *_limit;
		bool	parseProcedure(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	procedureClause(const char *ptr,
						const char **newptr);
		static const char *_procedure;
		bool	parseSelectInto(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	selectIntoClause(const char *ptr,
						const char **newptr);
		static const char *_select_into;
		bool	parseForUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	forUpdateClause(const char *ptr,
						const char **newptr);
		static const char *_for_update;


		// set query...
		bool parseSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool setClause(const char *ptr, const char **newptr);
		static const char *_set;
		bool parseSetGlobal(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool setGlobalClause(const char *ptr, const char **newptr);
		static const char *_set_global;
		bool parseSetSession(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool setSessionClause(const char *ptr, const char **newptr);
		static const char *_set_session;
		bool parseTransaction(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool transactionClause(const char *ptr,
						const char **newptr);
		static const char *_transaction;
		bool parseIsolationLevel(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool isolationLevelClause(const char *ptr,
						const char **newptr);
		static const char *_isolation_level;
		bool isolationLevelOptionClause(const char *ptr,
						const char **newptr);
		bool parseTransactionName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool transactionNameClause(const char *ptr,
						const char **newptr);
		static const char *_transaction_name;

		xmldom	*tree;
		bool	error;
};

#endif
