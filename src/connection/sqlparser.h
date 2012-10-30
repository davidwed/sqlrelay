// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLPARSER
#define SQLPARSER

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class sqlparser {
	public:
			sqlparser();
			~sqlparser();

		bool			parse(const char *query);
		rudiments::xmldom	*getTree();
		rudiments::xmldom	*detachTree();

	private:
		bool	parseInternal(const char *query,
					bool useescapecharacters);

		char	*cleanQuery(const char *query);

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


		rudiments::xmldomnode	*newNode(
					rudiments::xmldomnode *parentnode,
					const char *type);
		rudiments::xmldomnode	*newNode(
					rudiments::xmldomnode *parentnode,
					const char *type,
					const char *value);
		void	setAttribute(rudiments::xmldomnode *node,
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
		bool	endOfQuery(const char *ptr, const char **newptr);


	// Ideally these would be private but other classes (and their children)
	// need to access the static constants.  I could declare them in a
	// separate block but it's way easier if the parse/clause methods and
	// variable are all together.  Maybe I'll move them later.
	public:
		// generic...
		bool	parseTableName(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		static const char	*_table_name_database;
		static const char	*_table_name_schema;
		static const char	*_table_name_table;
		void	splitDatabaseObjectName(
					rudiments::xmldomnode *currentnode,
						const char *name,
						const char *databasetag,
						const char *schematag,
						const char *objecttag);
		bool	parseName(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_name;
		bool	parseType(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_type;
		static const char	*_size;
		bool	parseValues(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_values;
		static const char	*_value;
		bool	parseLength(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_length;
		bool	parseScale(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_scale;
		bool	parseVerbatim(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_verbatim;
		bool	parseRemainderVerbatim(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);



		// create query...
		bool	parseCreate(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	createClause(const char *ptr,
						const char **newptr);
		static const char	*_create;
		bool	parseGlobal(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	globalClause(const char *ptr,
						const char **newptr);
		static const char	*_global;
		bool	parseTemporary(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	temporaryClause(const char *ptr,
						const char **newptr);
		static const char	*_temporary;
		bool	tableClause(const char *ptr,
						const char **newptr);
		bool	parseFulltext(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	fulltext(const char *ptr, const char **newptr);
		static const char	*_fulltext;
		bool	parseSpatial(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	spatial(const char *ptr, const char **newptr);
		static const char	*_spatial;



		// create table...
		bool	parseCreateTable(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_table;
		bool	parseIfNotExists(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	ifNotExistsClause(const char *ptr,
						const char **newptr);
		static const char	*_if_not_exists;
		bool	parseColumnAndConstraintDefinitions(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_columns;
		static const char	*_column;
		bool	parseColumnDefinition(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseConstraints(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_constraints;
		bool	parseUnsigned(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	unsignedClause(const char *ptr,
						const char **newptr);
		static const char	*_unsigned;
		bool	parseZeroFill(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	zeroFillClause(const char *ptr,
						const char **newptr);
		static const char	*_zerofill;
		bool	parseBinary(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	binaryClause(const char *ptr,
						const char **newptr);
		static const char	*_binary;
		bool	parseCharacterSet(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	characterSetClause(const char *ptr,
						const char **newptr);
		static const char	*_character_set;
		bool	parseCollate(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	collateClause(const char *ptr,
						const char **newptr);
		static const char	*_collate;
		bool	parseNull(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	nullClause(const char *ptr,
						const char **newptr);
		static const char	*_null;
		bool	parseNotNull(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	notNullClause(const char *ptr,
						const char **newptr);
		static const char	*_not_null;
		bool	parseDefault(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	defaultClause(const char *ptr,
						const char **newptr);
		static const char	*_default;
		bool	parseAutoIncrement(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	autoIncrementClause(const char *ptr,
						const char **newptr);
		static const char	*_auto_increment;
		bool	parseUniqueKey(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	uniqueKeyClause(const char *ptr,
						const char **newptr);
		static const char	*_unique_key;
		bool	parsePrimaryKey(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	primaryKeyClause(const char *ptr,
						const char **newptr);
		static const char	*_primary_key;
		bool	parseKey(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	keyClause(const char *ptr,
						const char **newptr);
		static const char	*_key;
		bool	parseComment(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	commentClause(const char *ptr,
						const char **newptr);
		static const char	*_comment;
		bool	parseColumnFormat(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	columnFormatClause(const char *ptr,
						const char **newptr);
		static const char	*_column_format;
		bool	parseReferenceDefinition(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	referencesClause(const char *ptr,
						const char **newptr);
		static const char	*_references;
		bool	parseColumnNameList(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseMatch(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	matchClause(const char *ptr,
						const char **newptr);
		static const char	*_match;
		bool	parseOnDelete(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onDeleteClause(const char *ptr,
						const char **newptr);
		static const char	*_on_delete;
		bool	parseOnUpdate(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onUpdateClause(const char *ptr,
						const char **newptr);
		static const char	*_on_update;
		bool	parseReferenceOption(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	referenceOptionClause(const char *ptr,
						const char **newptr);
		bool	parseOnCommit(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onCommitClause(const char *ptr,
						const char **newptr);
		bool	onCommitOptionClause(const char *ptr,
						const char **newptr);
		static const char	*_on_commit;
		bool	parseAs(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	asClause(const char *ptr,
						const char **newptr);
		static const char	*_as;
		bool	parseWithNoLog(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_with_no_log;
		bool	parseConstraint(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseKeyConstraint(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseConstraintClause(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	constraintClause(const char *ptr, const char **newptr);
		static const char	*_constraint;
		bool	parsePrimaryKeyConstraint(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseUniqueConstraint(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseForeignKeyConstraint(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseForeignKey(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	foreignKeyClause(const char *ptr, const char **newptr);
		static const char	*_foreign_key;
		bool	parseIndexOrKeyConstraint(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseFulltextOrSpatialConstraint(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseIndex(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseIndexOption(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseKeyBlockSize(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	keyBlockSize(const char *ptr, const char **newptr);
		static const char	*_key_block_size;
		bool	parseWithParser(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	withParser(const char *ptr, const char **newptr);
		static const char	*_with_parser;
		bool	parseCheckConstraint(
					rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	checkClause(const char *ptr, const char **newptr);
		static const char	*_check;



		// create index
		bool	parseCreateIndex(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	indexClause(const char *ptr, const char **newptr);
		static const char	*_index;
		bool	parseIndexName(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_index_name_database;
		static const char	*_index_name_schema;
		static const char	*_index_name_index;
		bool	parseIndexType(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseBtree(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	btree(const char *ptr, const char **newptr);
		static const char	*_btree;
		bool	parseHash(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	hash(const char *ptr, const char **newptr);
		static const char	*_hash;
		bool	parseOnClause(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);




		// create synonym
		bool	parseCreateSynonym(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	synonymClause(const char *ptr, const char **newptr);
		static const char	*_synonym;
		bool	parseFor(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	forClause(const char *ptr, const char **newptr);
		static const char	*_for;
		bool	parseDatabaseObjectName(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_object_name_database;
		static const char	*_object_name_schema;
		static const char	*_object_name_object;


		// drop query...
		bool	parseDrop(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	dropClause(const char *ptr,
						const char **newptr);
		static const char	*_drop;
		bool	parseDropTemporary(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_drop_temporary;
		bool	parseDropTable(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseIfExists(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	ifExistsClause(const char *ptr,
						const char **newptr);
		static const char	*_if_exists;
		bool	parseTableNameList(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_table_name_list;
		static const char	*_table_name_list_item;
		bool	parseRestrict(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	restrictClause(const char *ptr,
						const char **newptr);
		static const char	*_restrict;
		bool	parseCascade(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	cascadeClause(const char *ptr,
						const char **newptr);
		static const char	*_cascade;
		bool	parseCascadeConstraintsClause(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	cascadeConstraintsClause(const char *ptr,
						const char **newptr);
		static const char	*_cascade_constraints_clause;
		bool	parseDropIndex(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);



		// insert query...
		bool	parseInsert(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertClause(const char *ptr,
						const char **newptr);
		static const char	*_insert;
		bool	parseInsertInto(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertIntoClause(const char *ptr,
						const char **newptr);
		static const char	*_insert_into;
		bool	parseInsertValues(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertValuesClause(const char *ptr,
						const char **newptr);
		static const char	*_insert_values_clause;
		bool	parseInsertValue(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertValueClause(const char *ptr,
						const char **newptr);
		static const char	*_insert_value_clause;
		bool	parseInsertValuesList(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_insert_value;



		// update query...
		bool	parseUpdate(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	updateClause(const char *ptr,
						const char **newptr);
		static const char	*_update;
		bool	parseUpdateSet(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						bool required);
		bool	updateSetClause(const char *ptr,
						const char **newptr);
		static const char	*_update_set;
		static const char	*_assignment;
		static const char	*_equals;



		// delete query...
		bool	parseDelete(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	deleteClause(const char *ptr,
						const char **newptr);
		static const char	*_delete;
		bool	parseDeleteFrom(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	deleteFromClause(const char *ptr,
						const char **newptr);
		static const char	*_delete_from;
		bool	parseUsing(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	usingClause(const char *ptr,
						const char **newptr);
		static const char	*_using;



		// select query...
		bool	parseSelect(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	selectClause(const char *ptr,
						const char **newptr);
		static const char	*_select;
		static const char	*_select_expressions;
		static const char	*_select_expression;
		bool	parseSelectExpressionAlias(
						rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseAlias(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						bool subselect);
		bool	parseSubSelects(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_sub_select;
		bool	parseSubSelectAlias(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_alias;
		bool	parseUnion(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	unionClause(const char *ptr,
						const char **newptr);
		static const char	*_union;
		bool	parseAll(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	allClause(const char *ptr,
						const char **newptr);
		static const char	*_all;
		bool	parseUnique(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	uniqueClause(const char *ptr,
						const char **newptr);
		static const char	*_unique;
		bool	parseNot(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseDistinct(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	distinctClause(const char *ptr,
						const char **newptr);
		static const char	*_distinct;
		bool	parseDistinctRow(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	distinctRowClause(const char *ptr,
						const char **newptr);
		static const char	*_distinct_row;
		bool	parseHighPriority(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	highPriorityClause(const char *ptr,
						const char **newptr);
		static const char	*_high_priority;
		bool	parseStraightJoinSelectOption(
					rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	straightJoinSelectOptionClause(const char *ptr,
						const char **newptr);
		static const char	*_straight_join_select_option;
		bool	parseSqlSmallResult(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlSmallResultClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_small_result;
		bool	parseSqlBigResult(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlBigResultClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_big_result;
		bool	parseSqlBufferResult(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlBufferResultClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_buffer_result;
		bool	parseSqlCache(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlCacheClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_cache;
		bool	parseSqlNoCache(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlNoCacheClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_no_cache;
		bool	parseSqlCalcFoundRows(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlCalcFoundRowsClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_calc_found_rows;
		bool	parseFrom(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	fromClause(const char *ptr,
						const char **newptr);
		static const char	*_from;
		bool	parseTableReferences(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_table_references;
		bool	parseTableReference(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_table_reference;
		bool	parseJoin(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_join_clause;
		bool	innerClause(const char *ptr, const char **newptr);
		static const char	*_inner;
		bool	crossClause(const char *ptr, const char **newptr);
		static const char	*_cross;
		bool	straightJoinClause(const char *ptr,
						const char **newptr);
		static const char	*_straight_join;
		bool	leftClause(const char *ptr, const char **newptr);
		static const char	*_left;
		bool	outerClause(const char *ptr, const char **newptr);
		static const char	*_right;
		bool	rightClause(const char *ptr, const char **newptr);
		static const char	*_outer;
		bool	naturalClause(const char *ptr, const char **newptr);
		static const char	*_natural;
		bool	joinClause(const char *ptr, const char **newptr);
		static const char	*_join;
		bool	parseOn(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	onClause(const char *ptr, const char **newptr);
		static const char	*_on;
		bool	parseJoinUsing(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	joinUsingClause(const char *ptr, const char **newptr);
		static const char	*_join_using;
		bool	parseWhere(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	whereClause(const char *ptr,
						const char **newptr);
		static const char	*_where;
		bool	parseHaving(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	havingClause(const char *ptr,
						const char **newptr);
		static const char	*_having;
		bool	parseWhereClauseTerms(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseAnd(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	andClause(const char *ptr, const char **newptr);
		static const char	*_and;
		bool	parseOr(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	orClause(const char *ptr, const char **newptr);
		static const char 	*_or;
		bool	parseWhereClauseTerm(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_group;
		bool	parseComparison(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						bool checkforgroup);
		static const char	*_comparison;
		bool	notClause(const char *ptr, const char **newptr);
		static const char	*_not;
		bool	parseBetween(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	betweenClause(const char *ptr, const char **newptr);
		static const char	*_between;
		bool	parseIn(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	inClause(const char *ptr, const char **newptr);
		static const char	*_in;
		bool	parseInSet(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_in_set_item;
		bool	parseExists(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	existsClause(const char *ptr, const char **newptr);
		static const char	*_exists;
		bool	parseIs(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	is(const char *ptr, const char **newptr);
		static const char	*_is;
		bool	parseLike(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	like(const char *ptr, const char **newptr);
		static const char	*_like;
		bool	parseMatches(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	matches(const char *ptr, const char **newptr);
		static const char	*_matches;
		bool	parseNullSafeEquals(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	nullSafeEquals(const char *ptr, const char **newptr);
		static const char	*_null_safe_equals;
		bool	parseEquals(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseNotEquals(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_not_equals;
		bool	parseLessThan(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_less_than;
		bool	parseGreaterThan(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_greater_than;
		bool	parseLessThanOrEqualTo(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_less_than_or_equal_to;
		bool	parseGreaterThanOrEqualTo(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_greater_than_or_equal_to;
		bool	parseEscape(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	escape(const char *ptr, const char **newptr);
		static const char	*_escape;
		bool	parseExpression(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseExpression(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						bool ingroup);
		static const char	*_expression;
		bool	parseUnaryOperator(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseCompliment(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_compliment;
		bool	parseInverse(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_inverse;
		bool	parseNegative(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_negative;
		bool	parseBinaryOperator(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseTimes(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_times;
		bool	parseDividedBy(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_divided_by;
		bool	parseModulo(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_modulo;
		bool	parsePlus(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_plus;
		bool	parseMinus(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_minus;
		bool	parseLogicalAnd(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_logical_and;
		bool	parseLogicalOr(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_logical_or;
		bool	parseBitwiseAnd(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_bitwise_and;
		bool	parseBitwiseOr(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_bitwise_or;
		bool	parseBitwiseXor(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_bitwise_xor;
		bool	parseTerm(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_number;
		static const char	*_string_literal;
		static const char	*_bind_variable;
		bool	parseIntervalQualifier(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_interval_qualifier;
		bool	parseTo(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	toClause(const char *ptr, const char **newptr);
		static const char	*_to;
		bool	parseTimeComponent(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						const char *timecomponent,
						const char *precscale);
		static const char	*_precision;
		bool	parseUnquotedLiteral(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseColumnOrFunction(rudiments::xmldomnode *currentnode,
						const char *name,
						const char *ptr,
						const char **newptr);
		static const char	*_column_reference;
		static const char	*_function;
		static const char	*_parameters;
		static const char	*_parameter;
		bool	specialFunctionName(const char *name);
		void	splitColumnName(rudiments::xmldomnode *currentnode,
						const char *name);
		static const char	*_column_name_database;
		static const char	*_column_name_schema;
		static const char	*_column_name_table;
		static const char	*_column_name_column;
		bool	parseOuterJoinOperator(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	outerJoinOperatorClause(const char *ptr,
						const char **newptr);
		static const char	*_outer_join_operator;

		bool	parseGroupBy(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	groupByClause(const char *ptr,
						const char **newptr);
		static const char	*_group_by;
		static const char	*_group_by_item;
		bool	parseWithRollup(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	withRollupClause(const char *ptr,
						const char **newptr);
		static const char	*_with_rollup;
		bool	parseOrderBy(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	orderByClause(const char *ptr,
						const char **newptr);
		static const char	*_order_by;
		static const char	*_order_by_item;
		bool	parseAsc(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	asc(const char *ptr, const char **newptr);
		static const char	*_asc;
		bool	parseDesc(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	desc(const char *ptr, const char **newptr);
		static const char	*_desc;
		bool	parseLimit(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	limitClause(const char *ptr,
						const char **newptr);
		static const char	*_limit;
		bool	parseProcedure(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	procedureClause(const char *ptr,
						const char **newptr);
		static const char	*_procedure;
		bool	parseSelectInto(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	selectIntoClause(const char *ptr,
						const char **newptr);
		static const char	*_select_into;
		bool	parseForUpdate(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	forUpdateClause(const char *ptr,
						const char **newptr);
		static const char	*_for_update;


		// set query...
		bool	parseSet(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool setClause(const char *ptr, const char **newptr);
		static const char	*_set;
		bool	parseSetGlobal(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	setGlobalClause(const char *ptr, const char **newptr);
		static const char	*_set_global;
		bool	parseSetSession(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	setSessionClause(const char *ptr, const char **newptr);
		static const char	*_set_session;
		bool	parseTransaction(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	transactionClause(const char *ptr,
						const char **newptr);
		static const char	*_transaction;
		bool	parseIsolationLevel(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	isolationLevelClause(const char *ptr,
						const char **newptr);
		static const char	*_isolation_level;
		bool	isolationLevelOptionClause(const char *ptr,
						const char **newptr);
		bool	parseTransactionName(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	transactionNameClause(const char *ptr,
						const char **newptr);
		static const char	*_transaction_name;


		// lock query
		bool	parseLock(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	lockClause(const char *ptr, const char **newptr);
		static const char	*_lock;
		static const char	*_in_mode;
		bool	parseLockMode(rudiments::xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	lockModeClause(const char *ptr, const char **newptr);
		static const char	*_lock_mode;
		bool	parseMode(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	modeClause(const char *ptr, const char **newptr);
		static const char	*_mode;
		bool	parseNoWait(rudiments::xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	noWaitClause(const char *ptr, const char **newptr);
		static const char	*_nowait;

		rudiments::xmldom	*tree;
		bool			error;

		bool	useescapecharacters;
};

#endif
