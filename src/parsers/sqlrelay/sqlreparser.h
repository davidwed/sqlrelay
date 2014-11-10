// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLREPARSER
#define SQLREPARSER

#include <sqlrelay/private/sqlrserverdll.h>

//#include <sqlrelay/sqlrparser.h>
#include <sqlrelay/sqlrserver.h>

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

class SQLRSERVER_DLLSPEC sqlreparser : public sqlrparser {
	public:
			sqlreparser();
			~sqlreparser();

		bool	parse(const char *query);
		void	useTree(xmldom *tree);
		xmldom	*getTree();
		xmldom	*detachTree();

		bool	write(stringbuffer *output);
		bool	write(xmldomnode *node,
					stringbuffer *output,
					bool omitsiblings);
		bool	write(xmldomnode *node, 
					stringbuffer *output);

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


		xmldomnode	*newNode(xmldomnode *parentnode,
					const char *type);
		xmldomnode	*newNode(xmldomnode *parentnode,
					const char *type,
					const char *value);
		void	setAttribute(xmldomnode *node,
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
		bool	parseTableName(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		static const char	*_table_name_database;
		static const char	*_table_name_schema;
		static const char	*_table_name_table;
		void	splitDatabaseObjectName(xmldomnode *currentnode,
						const char *name,
						const char *databasetag,
						const char *schematag,
						const char *objecttag);
		bool	parseName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_name;
		bool	parseType(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_type;
		static const char	*_size;
		bool	parseValues(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_values;
		static const char	*_value;
		bool	parseLength(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_length;
		bool	parseScale(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_scale;
		bool	parseVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_verbatim;
		bool	parseRemainderVerbatim(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);



		// create query...
		bool	parseCreate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	createClause(const char *ptr,
						const char **newptr);
		static const char	*_create;
		bool	parseGlobal(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	globalClause(const char *ptr,
						const char **newptr);
		static const char	*_global;
		bool	parseTemporary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	temporaryClause(const char *ptr,
						const char **newptr);
		static const char	*_temporary;
		bool	tableClause(const char *ptr,
						const char **newptr);
		bool	parseFulltext(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseFulltext(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode);
		bool	fulltext(const char *ptr, const char **newptr);
		static const char	*_fulltext;
		bool	parseSpatial(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseSpatial(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode);
		bool	spatial(const char *ptr, const char **newptr);
		static const char	*_spatial;



		// create table...
		bool	parseCreateTable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_table;
		bool	parseIfNotExists(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	ifNotExistsClause(const char *ptr,
						const char **newptr);
		static const char	*_if_not_exists;
		bool	parseColumnAndConstraintDefinitions(
					xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_columns;
		static const char	*_column;
		bool	parseColumnDefinition(
					xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseConstraints(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_constraints;
		bool	parseUnsigned(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	unsignedClause(const char *ptr,
						const char **newptr);
		static const char	*_unsigned;
		bool	parseZeroFill(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	zeroFillClause(const char *ptr,
						const char **newptr);
		static const char	*_zerofill;
		bool	parseBinary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	binaryClause(const char *ptr,
						const char **newptr);
		static const char	*_binary;
		bool	parseCharacterSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	characterSetClause(const char *ptr,
						const char **newptr);
		static const char	*_character_set;
		bool	parseCollate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	collateClause(const char *ptr,
						const char **newptr);
		static const char	*_collate;
		bool	parseNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	nullClause(const char *ptr,
						const char **newptr);
		static const char	*_null;
		bool	parseNotNull(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	notNullClause(const char *ptr,
						const char **newptr);
		static const char	*_not_null;
		bool	parseDefault(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	defaultClause(const char *ptr,
						const char **newptr);
		static const char	*_default;
		bool	parseAutoIncrement(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	autoIncrementClause(const char *ptr,
						const char **newptr);
		static const char	*_auto_increment;
		bool	parseIdentity(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	identityClause(const char *ptr,
						const char **newptr);
		static const char	*_identity;
		bool	parseUniqueKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	uniqueKeyClause(const char *ptr,
						const char **newptr);
		static const char	*_unique_key;
		bool	parsePrimaryKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parsePrimaryKey(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode);
		bool	primaryKeyClause(const char *ptr,
						const char **newptr);
		static const char	*_primary_key;
		bool	parseKey(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseKey(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode);
		bool	keyClause(const char *ptr,
						const char **newptr);
		static const char	*_key;
		bool	parseComment(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	commentClause(const char *ptr,
						const char **newptr);
		static const char	*_comment;
		bool	parseColumnFormat(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	columnFormatClause(const char *ptr,
						const char **newptr);
		static const char	*_column_format;
		bool	parseReferenceDefinition(
					xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	referencesClause(const char *ptr,
						const char **newptr);
		static const char	*_references;
		bool	parseColumnNameList(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseMatch(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	matchClause(const char *ptr,
						const char **newptr);
		static const char	*_match;
		bool	parseOnDelete(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onDeleteClause(const char *ptr,
						const char **newptr);
		static const char	*_on_delete;
		bool	parseOnUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	onUpdateClause(const char *ptr,
						const char **newptr);
		static const char	*_on_update;
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
		static const char	*_on_commit;
		bool	parseAs(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	asClause(const char *ptr,
						const char **newptr);
		static const char	*_as;
		bool	parseWithNoLog(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_with_no_log;
		bool	parseConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseKeyConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseConstraintClause(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	constraintClause(const char *ptr, const char **newptr);
		static const char	*_constraint;
		bool	parsePrimaryKeyConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseUniqueConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseForeignKeyConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseForeignKey(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode);
		bool	foreignKeyClause(const char *ptr, const char **newptr);
		static const char	*_foreign_key;
		bool	parseIndexOrKeyConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseFulltextOrSpatialConstraint(
						xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseIndex(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseIndex(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode);
		bool	parseIndexOption(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseKeyBlockSize(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	keyBlockSize(const char *ptr, const char **newptr);
		static const char	*_key_block_size;
		bool	parseWithParser(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	withParser(const char *ptr, const char **newptr);
		static const char	*_with_parser;
		bool	parseCheckConstraint(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	checkClause(const char *ptr, const char **newptr);
		static const char	*_check;



		// create index
		bool	parseCreateIndex(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	indexClause(const char *ptr, const char **newptr);
		static const char	*_index;
		bool	parseIndexName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_index_name_database;
		static const char	*_index_name_schema;
		static const char	*_index_name_index;
		bool	parseIndexType(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseBtree(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	btree(const char *ptr, const char **newptr);
		static const char	*_btree;
		bool	parseHash(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	hash(const char *ptr, const char **newptr);
		static const char	*_hash;
		bool	parseOnClause(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);




		// create synonym
		bool	parseCreateSynonym(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	synonymClause(const char *ptr, const char **newptr);
		static const char	*_synonym;
		bool	parseFor(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	forClause(const char *ptr, const char **newptr);
		static const char	*_for;
		bool	parseDatabaseObjectName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_object_name_database;
		static const char	*_object_name_schema;
		static const char	*_object_name_object;


		// create package, procedure and function
		bool	parseCreatePackage(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	packageClause(const char *ptr, const char **newptr);
		static const char	*_package;
		bool	parseCreateProcedure(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseCreateFunction(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	functionClause(const char *ptr, const char **newptr);


		// drop query...
		bool	parseDrop(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	dropClause(const char *ptr,
						const char **newptr);
		static const char	*_drop;
		bool	parseDropTemporary(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_drop_temporary;
		bool	parseDropTable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseIfExists(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	ifExistsClause(const char *ptr,
						const char **newptr);
		static const char	*_if_exists;
		bool	parseTableNameList(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_table_name_list;
		static const char	*_table_name_list_item;
		bool	parseRestrict(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	restrictClause(const char *ptr,
						const char **newptr);
		static const char	*_restrict_clause;
		bool	parseCascade(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	cascadeClause(const char *ptr,
						const char **newptr);
		static const char	*_cascade;
		bool	parseCascadeConstraintsClause(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	cascadeConstraintsClause(const char *ptr,
						const char **newptr);
		static const char	*_cascade_constraints_clause;
		bool	parseDropIndex(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);



		// insert query...
		bool	parseInsert(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertClause(const char *ptr,
						const char **newptr);
		static const char	*_insert;
		bool	parseInsertInto(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertIntoClause(const char *ptr,
						const char **newptr);
		static const char	*_insert_into;
		bool	parseInsertValues(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertValuesClause(const char *ptr,
						const char **newptr);
		static const char	*_insert_values_clause;
		bool	parseInsertValue(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	insertValueClause(const char *ptr,
						const char **newptr);
		static const char	*_insert_value_clause;
		bool	parseInsertValuesList(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_insert_value;



		// update query...
		bool	parseUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	updateClause(const char *ptr,
						const char **newptr);
		static const char	*_update;
		bool	parseUpdateSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						bool required);
		bool	updateSetClause(const char *ptr,
						const char **newptr);
		static const char	*_update_set;
		static const char	*_assignment;
		static const char	*_equals;



		// delete query...
		bool	parseDelete(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	deleteClause(const char *ptr,
						const char **newptr);
		static const char	*_delete;
		bool	parseDeleteFrom(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	deleteFromClause(const char *ptr,
						const char **newptr);
		static const char	*_delete_from;
		bool	parseUsing(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	usingClause(const char *ptr,
						const char **newptr);
		static const char	*_using;



		// select query...
		bool	parseSelect(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	selectClause(const char *ptr,
						const char **newptr);
		static const char	*_select;
		static const char	*_select_expressions;
		static const char	*_select_expression;
		bool	parseSelectExpressionAlias(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseAlias(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						bool subselect);
		bool	parseSubSelects(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_sub_select;
		bool	parseSubSelectAlias(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_alias;
		bool	parseUnion(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	unionClause(const char *ptr,
						const char **newptr);
		static const char	*_union;
		bool	parseAll(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	allClause(const char *ptr,
						const char **newptr);
		static const char	*_all;
		bool	parseUnique(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseUnique(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr,
					xmldomnode **newnode);
		bool	uniqueClause(const char *ptr,
						const char **newptr);
		static const char	*_unique;
		bool	parseNot(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseDistinct(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	distinctClause(const char *ptr,
						const char **newptr);
		static const char	*_distinct;
		bool	parseDistinctRow(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	distinctRowClause(const char *ptr,
						const char **newptr);
		static const char	*_distinct_row;
		bool	parseHighPriority(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	highPriorityClause(const char *ptr,
						const char **newptr);
		static const char	*_high_priority;
		bool	parseStraightJoinSelectOption(
					xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	straightJoinSelectOptionClause(const char *ptr,
						const char **newptr);
		static const char	*_straight_join_select_option;
		bool	parseSqlSmallResult(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlSmallResultClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_small_result;
		bool	parseSqlBigResult(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlBigResultClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_big_result;
		bool	parseSqlBufferResult(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlBufferResultClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_buffer_result;
		bool	parseSqlCache(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlCacheClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_cache;
		bool	parseSqlNoCache(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlNoCacheClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_no_cache;
		bool	parseSqlCalcFoundRows(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	sqlCalcFoundRowsClause(const char *ptr,
						const char **newptr);
		static const char	*_sql_calc_found_rows;
		bool	parseFrom(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	fromClause(const char *ptr,
						const char **newptr);
		static const char	*_from;
		bool	parseTableReferences(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_table_references;
		bool	parseTableReference(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_table_reference;
		bool	parseJoin(xmldomnode *currentnode,
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
		bool	parseOn(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	onClause(const char *ptr, const char **newptr);
		static const char	*_on;
		bool	parseJoinUsing(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	joinUsingClause(const char *ptr, const char **newptr);
		static const char	*_join_using;
		bool	parseWhere(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	whereClause(const char *ptr,
						const char **newptr);
		static const char	*_where;
		bool	parseHaving(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	havingClause(const char *ptr,
						const char **newptr);
		static const char	*_having;
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
		bool	parseIn(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	inClause(const char *ptr, const char **newptr);
		static const char	*_in;
		bool	parseInSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_in_set_item;
		bool	parseExists(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	existsClause(const char *ptr, const char **newptr);
		static const char	*_exists;
		bool	parseIs(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	is(const char *ptr, const char **newptr);
		static const char	*_is;
		bool	parseLike(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	like(const char *ptr, const char **newptr);
		static const char	*_like;
		bool	parseMatches(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	matches(const char *ptr, const char **newptr);
		static const char	*_matches;
		bool	parseNullSafeEquals(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	nullSafeEquals(const char *ptr, const char **newptr);
		static const char	*_null_safe_equals;
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
		bool	parseEscape(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	escape(const char *ptr, const char **newptr);
		static const char	*_escape;
		bool	parseExpression(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseExpression(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						bool ingroup);
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
		bool	parseIntervalQualifier(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		static const char	*_interval_qualifier;
		bool	parseTo(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	toClause(const char *ptr, const char **newptr);
		static const char	*_to;
		bool	parseTimeComponent(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr,
						const char *timecomponent,
						const char *precision,
						const char *scale);
		static const char	*_from_precision;
		static const char	*_from_scale;
		static const char	*_to_precision;
		static const char	*_to_scale;
		bool	parseUnquotedLiteral(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseColumnOrFunction(xmldomnode *currentnode,
						const char *name,
						const char *ptr,
						const char **newptr);
		static const char	*_column_reference;
		static const char	*_function;
		static const char	*_parameters;
		static const char	*_parameter;
		bool	specialFunctionName(const char *name);
		void	splitColumnName(xmldomnode *currentnode,
						const char *name);
		static const char	*_column_name_database;
		static const char	*_column_name_schema;
		static const char	*_column_name_table;
		static const char	*_column_name_column;
		bool	parseOuterJoinOperator(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	outerJoinOperatorClause(const char *ptr,
						const char **newptr);
		static const char	*_outer_join_operator;

		bool	parseGroupBy(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	groupByClause(const char *ptr,
						const char **newptr);
		static const char	*_group_by;
		static const char	*_group_by_item;
		bool	parseWithRollup(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	withRollupClause(const char *ptr,
						const char **newptr);
		static const char	*_with_rollup;
		bool	parseOrderBy(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	orderByClause(const char *ptr,
						const char **newptr);
		static const char	*_order_by;
		static const char	*_order_by_item;
		bool	parseAsc(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	asc(const char *ptr, const char **newptr);
		static const char	*_asc;
		bool	parseDesc(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	desc(const char *ptr, const char **newptr);
		static const char	*_desc;
		bool	parseLimit(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	limitClause(const char *ptr,
						const char **newptr);
		static const char	*_limit;
		bool	parseProcedure(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	procedureClause(const char *ptr,
						const char **newptr);
		static const char	*_procedure;
		bool	parseSelectIntoTable(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	parseSelectIntoVariables(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	selectIntoClause(const char *ptr,
						const char **newptr);
		static const char	*_select_into;
		bool	parseForUpdate(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	forUpdateClause(const char *ptr,
						const char **newptr);
		static const char	*_for_update;


		// set query...
		bool	parseSet(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool setClause(const char *ptr, const char **newptr);
		static const char	*_set;
		bool	parseSetGlobal(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	setGlobalClause(const char *ptr, const char **newptr);
		static const char	*_set_global;
		bool	parseSetSession(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	setSessionClause(const char *ptr, const char **newptr);
		static const char	*_set_session;
		bool	parseTransaction(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	transactionClause(const char *ptr,
						const char **newptr);
		static const char	*_transaction;
		bool	parseIsolationLevel(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	isolationLevelClause(const char *ptr,
						const char **newptr);
		static const char	*_isolation_level;
		bool	isolationLevelOptionClause(const char *ptr,
						const char **newptr);
		bool	parseTransactionName(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	transactionNameClause(const char *ptr,
						const char **newptr);
		static const char	*_transaction_name;


		// lock query
		bool	parseLock(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	lockClause(const char *ptr, const char **newptr);
		static const char	*_lock;
		static const char	*_in_mode;
		bool	parseLockMode(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	lockModeClause(const char *ptr, const char **newptr);
		static const char	*_lock_mode;
		bool	parseMode(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	modeClause(const char *ptr, const char **newptr);
		static const char	*_mode;
		bool	parseNoWait(xmldomnode *currentnode,
					const char *ptr,
					const char **newptr);
		bool	noWaitClause(const char *ptr, const char **newptr);
		static const char	*_nowait;

		// show query
		bool	parseShow(xmldomnode *currentnode,
						const char *ptr,
						const char **newptr);
		bool	showClause(const char *ptr, const char **newptr);
		static const char	*_show;

		xmldom	*tree;
		bool	foreigntree;
		bool	error;

		bool	useescapecharacters;

		// write methods
		bool	handleStart(xmldomnode *node,
						stringbuffer *output);
		bool	handleEnd(xmldomnode *node,
						stringbuffer *output);

		// generic
		bool	tableNameDatabase(xmldomnode *node,
						stringbuffer *output);
		bool	tableNameSchema(xmldomnode *node,
						stringbuffer *output);
		bool	tableNameTable(xmldomnode *node,
						stringbuffer *output);
		bool	name(xmldomnode *node,
					stringbuffer *output);
		bool	type(xmldomnode *node,
					stringbuffer *output);
		bool	endType(xmldomnode *node,
					stringbuffer *output);
		bool	size(xmldomnode *node,
					stringbuffer *output);
		bool	endSize(xmldomnode *node,
					stringbuffer *output);
		bool	value(xmldomnode *node,
					stringbuffer *output);
		bool	verbatim(xmldomnode *node,
					stringbuffer *output);

		// create query...
		bool	createQuery(xmldomnode *node,
					stringbuffer *output);

		// table...
		bool	table(xmldomnode *node,
					stringbuffer *output);
		bool	global(xmldomnode *node,
					stringbuffer *output);
		bool	temporary(xmldomnode *node,
					stringbuffer *output);
		bool	ifNotExists(xmldomnode *node,
					stringbuffer *output);

		// index...
		bool	fulltext(xmldomnode *node,
					stringbuffer *output);
		bool	spatial(xmldomnode *node,
					stringbuffer *output);
		bool	index(xmldomnode *node,
					stringbuffer *output);
		bool	indexNameDatabase(xmldomnode *node,
					stringbuffer *output);
		bool	indexNameSchema(xmldomnode *node,
					stringbuffer *output);
		bool	indexNameIndex(xmldomnode *node,
					stringbuffer *output);
		bool	btree(xmldomnode *node,
					stringbuffer *output);
		bool	hash(xmldomnode *node,
					stringbuffer *output);
		bool	keyBlockSize(xmldomnode *node,
					stringbuffer *output);
		bool	withParser(xmldomnode *node,
					stringbuffer *output);


		// synonym...
		bool	synonym(xmldomnode *node,
					stringbuffer *output);
		bool	forClause(xmldomnode *node,
					stringbuffer *output);
		bool	objectNameDatabase(xmldomnode *node,
					stringbuffer *output);
		bool	objectNameSchema(xmldomnode *node,
					stringbuffer *output);
		bool	objectNameObject(xmldomnode *node,
					stringbuffer *output);
		

		// column definitions...
		bool	columns(xmldomnode *node,
					stringbuffer *output);
		bool	endColumns(xmldomnode *node,
					stringbuffer *output);
		bool	column(xmldomnode *node,
					stringbuffer *output);
		bool	endColumn(xmldomnode *node,
					stringbuffer *output);
		bool	values(xmldomnode *node,
					stringbuffer *output);
		bool	endValues(xmldomnode *node,
					stringbuffer *output);
		bool	length(xmldomnode *node,
					stringbuffer *output);
		bool	scale(xmldomnode *node,
					stringbuffer *output);

		// constraints...
		bool	constraints(xmldomnode *node,
					stringbuffer *output);
		bool	unsignedConstraint(xmldomnode *node,
					stringbuffer *output);
		bool	zerofill(xmldomnode *node,
					stringbuffer *output);
		bool	binary(xmldomnode *node,
					stringbuffer *output);
		bool	characterSet(xmldomnode *node,
					stringbuffer *output);
		bool	collate(xmldomnode *node,
					stringbuffer *output);
		bool	nullable(xmldomnode *node,
					stringbuffer *output);
		bool	notNull(xmldomnode *node,
					stringbuffer *output);
		bool	defaultValue(xmldomnode *node,
					stringbuffer *output);
		bool	autoIncrement(xmldomnode *node,
					stringbuffer *output);
		bool	identity(xmldomnode *node,
					stringbuffer *output);
		bool	uniqueKey(xmldomnode *node,
					stringbuffer *output);
		bool	primaryKey(xmldomnode *node,
					stringbuffer *output);
		bool	key(xmldomnode *node,
					stringbuffer *output);
		bool	comment(xmldomnode *node,
					stringbuffer *output);
		bool	columnFormat(xmldomnode *node,
					stringbuffer *output);
		bool	references(xmldomnode *node,
					stringbuffer *output);
		bool	endReferences(xmldomnode *node,
					stringbuffer *output);
		bool	match(xmldomnode *node,
					stringbuffer *output);
		bool	onDelete(xmldomnode *node,
					stringbuffer *output);
		bool	onUpdate(xmldomnode *node,
					stringbuffer *output);
		bool	onCommit(xmldomnode *node,
					stringbuffer *output);
		bool	as(xmldomnode *node,
					stringbuffer *output);
		bool	constraint(xmldomnode *node,
					stringbuffer *output);
		bool	foreignKey(xmldomnode *node,
					stringbuffer *output);
		bool	check(xmldomnode *node,
					stringbuffer *output);
		bool	endCheck(xmldomnode *node,
					stringbuffer *output);


		// table creation qualifiers...
		bool	withNoLog(xmldomnode *node,
					stringbuffer *output);


		// drop...
		bool	dropQuery(xmldomnode *node,
					stringbuffer *output);
		bool	ifExists(xmldomnode *node,
					stringbuffer *output);
		bool	endTableNameListItem(xmldomnode *node,
					stringbuffer *output);
		bool	restrictClause(xmldomnode *node,
					stringbuffer *output);
		bool	cascade(xmldomnode *node,
					stringbuffer *output);
		bool	cascadeConstraintsClause(xmldomnode *node,
					stringbuffer *output);


		// insert...
		bool	insertQuery(xmldomnode *node,
					stringbuffer *output);
		bool	insertInto(xmldomnode *node,
					stringbuffer *output);
		bool	insertValuesClause(xmldomnode *node,
					stringbuffer *output);
		bool	endInsertValuesClause(xmldomnode *node,
					stringbuffer *output);
		bool	insertValueClause(xmldomnode *node,
					stringbuffer *output);
		bool	endInsertValueClause(xmldomnode *node,
					stringbuffer *output);
		bool	insertValue(xmldomnode *node,
					stringbuffer *output);
		bool	endInsertValue(xmldomnode *node,
					stringbuffer *output);


		// update...
		bool	updateQuery(xmldomnode *node,
					stringbuffer *output);
		bool	updateSet(xmldomnode *node,
					stringbuffer *output);
		bool	assignment(xmldomnode *node,
					stringbuffer *output);
		bool	endAssignment(xmldomnode *node,
					stringbuffer *output);
		bool	equals(xmldomnode *node,
					stringbuffer *output);


		// delete...
		bool	deleteQuery(xmldomnode *node,
					stringbuffer *output);
		bool	deleteFrom(xmldomnode *node,
					stringbuffer *output);
		bool	usingClause(xmldomnode *node,
					stringbuffer *output);


		// select...
		bool	selectQuery(xmldomnode *node,
					stringbuffer *output);
		bool	selectExpressions(xmldomnode *node,
					stringbuffer *output);
		bool	selectExpression(xmldomnode *node,
					stringbuffer *output);
		bool	endSelectExpression(xmldomnode *node,
					stringbuffer *output);
		bool	subSelect(xmldomnode *node,
					stringbuffer *output);
		bool	endSubSelect(xmldomnode *node,
					stringbuffer *output);
		bool	unionClause(xmldomnode *node,
					stringbuffer *output);
		bool	all(xmldomnode *node,
					stringbuffer *output);
		bool	alias(xmldomnode *node,
					stringbuffer *output);
		bool	unique(xmldomnode *node,
					stringbuffer *output);
		bool	distinct(xmldomnode *node,
					stringbuffer *output);
		bool	from(xmldomnode *node,
					stringbuffer *output);
		bool	tableReferences(xmldomnode *node,
					stringbuffer *output);
		bool	tableReference(xmldomnode *node,
					stringbuffer *output);
		bool	endTableReference(xmldomnode *node,
					stringbuffer *output);
		bool	joinClause(xmldomnode *node,
					stringbuffer *output);
		bool	endJoinClause(xmldomnode *node,
					stringbuffer *output);
		bool	inner(xmldomnode *node,
					stringbuffer *output);
		bool	cross(xmldomnode *node,
					stringbuffer *output);
		bool	straightJoin(xmldomnode *node,
					stringbuffer *output);
		bool	left(xmldomnode *node,
					stringbuffer *output);
		bool	right(xmldomnode *node,
					stringbuffer *output);
		bool	outer(xmldomnode *node,
					stringbuffer *output);
		bool	natural(xmldomnode *node,
					stringbuffer *output);
		bool	join(xmldomnode *node,
					stringbuffer *output);
		bool	on(xmldomnode *node,
					stringbuffer *output);
		bool	joinUsing(xmldomnode *node,
					stringbuffer *output);
		bool	where(xmldomnode *node,
					stringbuffer *output);
		bool	andClause(xmldomnode *node,
					stringbuffer *output);
		bool	orClause(xmldomnode *node,
					stringbuffer *output);
		bool	group(xmldomnode *node,
					stringbuffer *output);
		bool	endGroup(xmldomnode *node,
					stringbuffer *output);
		bool	comparison(xmldomnode *node,
					stringbuffer *output);
		bool	notClause(xmldomnode *node,
					stringbuffer *output);
		bool	between(xmldomnode *node,
					stringbuffer *output);
		bool	in(xmldomnode *node,
					stringbuffer *output);
		bool	endIn(xmldomnode *node,
					stringbuffer *output);
		bool	inSetItem(xmldomnode *node,
					stringbuffer *output);
		bool	endInSetItem(xmldomnode *node,
					stringbuffer *output);
		bool	exists(xmldomnode *node,
					stringbuffer *output);
		bool	endExists(xmldomnode *node,
					stringbuffer *output);
		bool	is(xmldomnode *node,
					stringbuffer *output);
		bool	like(xmldomnode *node,
					stringbuffer *output);
		bool	matches(xmldomnode *node,
					stringbuffer *output);
		bool	nullSafeEquals(xmldomnode *node,
					stringbuffer *output);
		bool	notEquals(xmldomnode *node,
					stringbuffer *output);
		bool	lessThan(xmldomnode *node,
					stringbuffer *output);
		bool	greaterThan(xmldomnode *node,
					stringbuffer *output);
		bool	lessThanOrEqualTo(xmldomnode *node,
					stringbuffer *output);
		bool	greaterThanOrEqualTo(xmldomnode *node,
					stringbuffer *output);
		bool	escape(xmldomnode *node,
					stringbuffer *output);
		bool	expression(xmldomnode *node,
					stringbuffer *output);
		bool	intervalQualifier(xmldomnode *node,
					stringbuffer *output);
		bool	outerJoinOperator(xmldomnode *node,
					stringbuffer *output);
		bool	compliment(xmldomnode *node,
					stringbuffer *output);
		bool	inverse(xmldomnode *node,
					stringbuffer *output);
		bool	negative(xmldomnode *node,
					stringbuffer *output);
		bool	plus(xmldomnode *node,
					stringbuffer *output);
		bool	minus(xmldomnode *node,
					stringbuffer *output);
		bool	times(xmldomnode *node,
					stringbuffer *output);
		bool	dividedBy(xmldomnode *node,
					stringbuffer *output);
		bool	modulo(xmldomnode *node,
					stringbuffer *output);
		bool	bitwiseAnd(xmldomnode *node,
					stringbuffer *output);
		bool	bitwiseOr(xmldomnode *node,
					stringbuffer *output);
		bool	bitwiseXor(xmldomnode *node,
					stringbuffer *output);
		bool	logicalAnd(xmldomnode *node,
					stringbuffer *output);
		bool	logicalOr(xmldomnode *node,
					stringbuffer *output);
		bool	number(xmldomnode *node,
					stringbuffer *output);
		bool	stringLiteral(xmldomnode *node,
					stringbuffer *output);
		bool	bindVariable(xmldomnode *node,
					stringbuffer *output);
		bool	columnReference(xmldomnode *node,
					stringbuffer *output);
		bool	columnNameDatabase(xmldomnode *node,
					stringbuffer *output);
		bool	columnNameSchema(xmldomnode *node,
					stringbuffer *output);
		bool	columnNameTable(xmldomnode *node,
					stringbuffer *output);
		bool	columnNameColumn(xmldomnode *node,
					stringbuffer *output);
		bool	function(xmldomnode *node,
					stringbuffer *output);
		bool	parameters(xmldomnode *node,
					stringbuffer *output);
		bool	endParameters(xmldomnode *node,
					stringbuffer *output);
		bool	parameter(xmldomnode *node,
					stringbuffer *output);
		bool	endParameter(xmldomnode *node,
					stringbuffer *output);
		bool	groupBy(xmldomnode *node,
					stringbuffer *output);
		bool	groupByItem(xmldomnode *node,
					stringbuffer *output);
		bool	endGroupByItem(xmldomnode *node,
					stringbuffer *output);
		bool	withRollup(xmldomnode *node,
					stringbuffer *output);
		bool	having(xmldomnode *node,
					stringbuffer *output);
		bool	orderBy(xmldomnode *node,
					stringbuffer *output);
		bool	orderByItem(xmldomnode *node,
					stringbuffer *output);
		bool	endOrderByItem(xmldomnode *node,
					stringbuffer *output);
		bool	asc(xmldomnode *node,
					stringbuffer *output);
		bool	desc(xmldomnode *node,
					stringbuffer *output);
		bool	limit(xmldomnode *node,
					stringbuffer *output);
		bool	selectInto(xmldomnode *node,
					stringbuffer *output);
		bool	procedure(xmldomnode *node,
					stringbuffer *output);
		bool	forUpdate(xmldomnode *node,
					stringbuffer *output);


		// set...
		bool	setQuery(xmldomnode *node,
					stringbuffer *output);
		bool	setSession(xmldomnode *node,
					stringbuffer *output);
		bool	setGlobal(xmldomnode *node,
					stringbuffer *output);
		bool	transaction(xmldomnode *node,
					stringbuffer *output);
		bool	isolationLevel(xmldomnode *node,
					stringbuffer *output);


		// lock...
		bool	lockQuery(xmldomnode *node,
					stringbuffer *output);
		bool	inMode(xmldomnode *node,
					stringbuffer *output);
		bool	lockMode(xmldomnode *node,
					stringbuffer *output);
		bool	mode(xmldomnode *node,
					stringbuffer *output);
		bool	noWait(xmldomnode *node,
					stringbuffer *output);

		bool	show(xmldomnode *node,
					stringbuffer *output);



		// helper methods
		bool	outputValue(xmldomnode *node,
					stringbuffer *output);
		bool	space(stringbuffer *output);
		bool	comma(stringbuffer *output);
		bool	period(stringbuffer *output);
		bool	leftParen(stringbuffer *output);
		bool	rightParen(stringbuffer *output);
		bool	hasSibling(xmldomnode *node);
		bool	dontAppendSpace(stringbuffer *output);
};

#endif
