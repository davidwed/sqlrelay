package com.firstworks.sql;
	
import java.sql.*;
	
public class SQLRelayDatabaseMetaData extends SQLRelayDebug implements DatabaseMetaData {

	private SQLRelayConnection	connection;

	public SQLRelayDatabaseMetaData() {
		debugFunction();
		connection=null;
		// FIXME: set protected member variables?
	}

	public void setConnection(SQLRelayConnection connection) {
		debugFunction();
		this.connection=connection;
	}

	public boolean 	allProceduresAreCallable() throws SQLException {
		debugFunction();
		// Retrieves whether the current user can call all the
		// procedures returned by the method getProcedures.
		// FIXME: this is almost certainly not correct.
		return true;
	}

	public boolean 	allTablesAreSelectable() throws SQLException {
		debugFunction();
		// Retrieves whether the current user can use all the tables
		// returned by the method getTables in a SELECT statement.
		// FIXME: this is almost certainly not correct.
		return true;
	}

	public boolean 	autoCommitFailureClosesAllResultSets()
							throws SQLException {
		debugFunction();
		// Retrieves whether a SQLException while autoCommit is true
		// inidcates that all open ResultSets are closed, even ones
		// that are holdable.
		// FIXME: no idea if this is true or not
		return false;
	}

	public boolean 	dataDefinitionCausesTransactionCommit()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return false;
	}

	public boolean 	dataDefinitionIgnoredInTransactions()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return false;
	}

	public boolean 	deletesAreDetected(int type) throws SQLException {
		debugFunction();
		// SQL Relay doesn't currenlty support ResultSet.RowDelete
		return false;
	}

	public boolean 	doesMaxRowSizeIncludeBlobs() throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	generatedKeyAlwaysReturned() throws SQLException {
		debugFunction();
		return true;
	}

	public ResultSet 	getAttributes(String catalog,
						String schemaPattern,
						String typeNamePattern,
						String attributeNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this somehow...
		return null;
	}

	public ResultSet 	getBestRowIdentifier(String catalog,
							String schema,
							String table,
							int scope,
							boolean nullable)
							throws SQLException {
		debugFunction();
		// FIXME: implement this somehow...
		return null;
	}

	public ResultSet 	getCatalogs() throws SQLException {
		debugFunction();
		// FIXME: implement this by calling sqlrcursor.getDatabases()
		return null;
	}

	public String 	getCatalogSeparator() throws SQLException {
		debugFunction();
		// FIXME: oracle uses @
		return ".";
	}

	public String 	getCatalogTerm() throws SQLException {
		debugFunction();
		// FIXME: I think SQL Server uses catalog, maybe sybase
		return "database";
	}

	public ResultSet 	getClientInfoProperties() throws SQLException {
		debugFunction();
		// FIXME: free form in SQL Relay
		return null;
	}

	public ResultSet 	getColumnPrivileges(
						String catalog,
						String schema,
						String table,
						String columnNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public ResultSet 	getColumns(String catalog,
						String schemaPattern,
						String tableNamePattern,
						String columnNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this by calling sqlrcursor.getColumnList()
		return null;
	}

	public Connection 	getConnection() throws SQLException {
		debugFunction();
		return connection;
	}

	public ResultSet 	getCrossReference(String parentCatalog,
						String parentSchema,
						String parentTable,
						String foreignCatalog,
						String foreignSchema,
						String foreignTable)
						throws SQLException {
		debugFunction();
		// FIXME: implement this somehow...
		return null;
	}

	public int 	getDatabaseMajorVersion() throws SQLException {
		debugFunction();
		// FIXME: SQL Relay or db?
		return 0;
	}

	public int 	getDatabaseMinorVersion() throws SQLException {
		debugFunction();
		// FIXME: SQL Relay or db?
		return 0;
	}

	public String 	getDatabaseProductName() throws SQLException {
		debugFunction();
		// FIXME: cache this...
		return connection.getSqlrCon().identify();
	}

	public String 	getDatabaseProductVersion() throws SQLException {
		debugFunction();
		// FIXME: cache this...
		return connection.getSqlrCon().dbVersion();
	}

	public int 	getDefaultTransactionIsolation() throws SQLException {
		debugFunction();
		return (getDatabaseProductName().equals("mysql"))?
				Connection.TRANSACTION_REPEATABLE_READ:
				Connection.TRANSACTION_READ_COMMITTED;
	}

	public int 	getDriverMajorVersion() {
		debugFunction();
		// FIXME: make this come from sqlrclient
		return 1;
	}

	public int 	getDriverMinorVersion() {
		debugFunction();
		// FIXME: make this come from sqlrclient
		return 2;
	}

	public String 	getDriverName() throws SQLException {
		debugFunction();
		return "sqlrelay";
	}

	public String 	getDriverVersion() throws SQLException {
		debugFunction();
		// FIXME: make this come from sqlrclient
		return "1.2.0";
	}

	public ResultSet 	getExportedKeys(String catalog,
						String schema,
						String table)
						throws SQLException {
		debugFunction();
		// Retrieves a description of the foreign key columns that
		// reference the given table's primary key columns (the foreign
		// keys exported by a table).
		// FIXME: implement this somehow
		return null;
	}

	public String 	getExtraNameCharacters() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return null;
	}

	public ResultSet 	getFunctionColumns(
						String catalog,
						String schemaPattern,
						String functionNamePattern,
						String columnNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement with
		// sqlrcur.getProcedureBindAndColumnList()?
		return null;
	}

	public ResultSet 	getFunctions(String catalog,
						String schemaPattern,
						String functionNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this by calling sqlrcur.getProcedures()?
		return null;
	}

	public String 	getIdentifierQuoteString() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		// * mysql uses back-tick
		// * sqlserver uses braces
		return "\"";
	}

	public ResultSet 	getImportedKeys(String catalog,
						String schema,
						String table)
						throws SQLException {
		debugFunction();
		// Retrieves a description of the primary key columns that are
		// referenced by the given table's foreign key columns (the
		// primary keys imported by a table).
		// FIXME: implement this somehow
		return null;
	}

	public ResultSet 	getIndexInfo(String catalog,
						String schema,
						String table,
						boolean unique,
						boolean approximate)
						throws SQLException {
		debugFunction();
		// FIXME: implement using sqlrcur.getKeyAndIndexList() ?
		return null;
	}

	public int 	getJDBCMajorVersion() throws SQLException {
		debugFunction();
		// FIXME: make this come from sqlrclient
		return 1;
	}

	public int 	getJDBCMinorVersion() throws SQLException {
		debugFunction();
		// FIXME: make this come from sqlrclient
		return 2;
	}

	public int 	getMaxBinaryLiteralLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxCatalogNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxCharLiteralLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInGroupBy() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInIndex() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInOrderBy() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInSelect() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInTable() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxConnections() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxCursorNameLength() throws SQLException {
		debugFunction();
		return 0;
	}

	public int 	getMaxIndexLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxProcedureNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxRowSize() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxSchemaNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxStatementLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxStatements() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxTableNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxTablesInSelect() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxUserNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public String 	getNumericFunctions() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return null;
	}

	public ResultSet 	getPrimaryKeys(String catalog,
						String schema,
						String table)
						throws SQLException {
		debugFunction();
		// FIXME: implement this by calling sqlrcon.getPrimaryKeysList()
		return null;
	}

	public ResultSet 	getProcedureColumns(
						String catalog,
						String schemaPattern,
						String procedureNamePattern,
						String columnNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this by calling
		// sqlrcon.getProcedureBindAndColumnList()
		return null;
	}

	public ResultSet 	getProcedures(String catalog,
						String schemaPattern,
						String procedureNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this by calling sqlrcon.getProcedureList()
		return null;
	}

	public String 	getProcedureTerm() throws SQLException {
		debugFunction();
		return "procedure";
	}

	public ResultSet 	getPseudoColumns(String catalog,
						String schemaPattern,
						String tableNamePattern,
						String columnNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public int 	getResultSetHoldability() throws SQLException {
		debugFunction();
		// FIXME: is this correct?
		return ResultSet.CLOSE_CURSORS_AT_COMMIT;
	}

	public RowIdLifetime 	getRowIdLifetime() throws SQLException {
		debugFunction();
		// FIXME: some dbs do support rowid
		return RowIdLifetime.ROWID_UNSUPPORTED;
	}

	public ResultSet 	getSchemas() throws SQLException {
		debugFunction();
		// FIXME: implement this by calling sqlrcon.getSchemaList()
		return null;
	}

	public ResultSet 	getSchemas(String catalog,
						String schemaPattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this by calling sqlrcon.getSchemaList()
		return null;
	}

	public String 	getSchemaTerm() throws SQLException {
		debugFunction();
		return "schema";
	}

	public String 	getSearchStringEscape() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return "\\";
	}

	public String 	getSQLKeywords() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return null;
	}

	public int 	getSQLStateType() throws SQLException {
		debugFunction();
		// FIXME: no idea
		return sqlStateSQL;
	}

	public String 	getStringFunctions() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return null;
	}

	public ResultSet 	getSuperTables(String catalog,
						String schemaPattern,
						String tableNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public ResultSet 	getSuperTypes(String catalog,
						String schemaPattern,
						String typeNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public String 	getSystemFunctions() throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public ResultSet 	getTablePrivileges(String catalog,
						String schemaPattern,
						String tableNamePattern)
						throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public ResultSet 	getTables(String catalog,
						String schemaPattern,
						String tableNamePattern,
						String[] types)
						throws SQLException {
		debugFunction();
		// FIXME: implement this by calling sqlrcon.getTableList()
		return null;
	}

	public ResultSet 	getTableTypes() throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public String 	getTimeDateFunctions() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return null;
	}

	public ResultSet 	getTypeInfo() throws SQLException {
		debugFunction();
		// FIXME: implement this by calling sqlrcon.getTypeInfo()
		return null;
	}

	public ResultSet 	getUDTs(String catalog,
						String schemaPattern,
						String typeNamePattern,
						int[] types)
						throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public String 	getURL() throws SQLException {
		debugFunction();

		String	host=connection.getHost();
		short	port=connection.getPort();
		String	socket=connection.getSocket();
		String	user=connection.getUser();
		String	password=connection.getPassword();

		String	url="jdbc:sqlrelay://";
		if (user!=null && !user.equals("")) {
			url=url+user;
			if (password!=null && !password.equals("")) {
				url=url+":"+password;
			}
			url=url+"@";
		}
		url=url+host+":"+port;
		if (socket!=null && !socket.equals("")) {
			url=url+":"+socket;
		}

		return url;
	}

	public String 	getUserName() throws SQLException {
		debugFunction();
		return connection.getUser();
	}

	public ResultSet 	getVersionColumns(String catalog,
							String schema,
							String table)
							throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public boolean 	insertsAreDetected(int type) throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	isCatalogAtStart() throws SQLException {
		debugFunction();
		// FIXME: not in oracle
		return true;
	}

	public boolean 	isReadOnly() throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return false;
	}

	public boolean 	locatorsUpdateCopy() throws SQLException {
		debugFunction();
		// FIXME: no idea, probably db-specific
		return false;
	}

	public boolean 	nullPlusNonNullIsNull() throws SQLException {
		debugFunction();
		// FIXME: generally true, but probably db-specific
		return true;
	}

	public boolean 	nullsAreSortedAtEnd() throws SQLException {
		debugFunction();
		// FIXME: generally true, but probably db-specific
		return true;
	}

	public boolean 	nullsAreSortedAtStart() throws SQLException {
		debugFunction();
		// FIXME: generally false, but probably db-specific
		return false;
	}

	public boolean 	nullsAreSortedHigh() throws SQLException {
		debugFunction();
		// FIXME: generally true, but probably db-specific
		return true;
	}

	public boolean 	nullsAreSortedLow() throws SQLException {
		debugFunction();
		// FIXME: generally false, but probably db-specific
		return false;
	}

	public boolean 	othersDeletesAreVisible(int type) throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	othersInsertsAreVisible(int type) throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	othersUpdatesAreVisible(int type) throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	ownDeletesAreVisible(int type) throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	ownInsertsAreVisible(int type) throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	ownUpdatesAreVisible(int type) throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	storesLowerCaseIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific but generally false
		// oracle stores upper case identifiers
		// other db's store mixed case identifiers
		return false;
	}

	public boolean 	storesLowerCaseQuotedIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	storesMixedCaseIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: generally true, but db-specific, false for oracle
		return true;
	}

	public boolean 	storesMixedCaseQuotedIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	storesUpperCaseIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific but generally false
		// oracle stores upper case identifiers
		// other db's store mixed case identifiers
		return false;
	}

	public boolean 	storesUpperCaseQuotedIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsAlterTableWithAddColumn() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsAlterTableWithDropColumn() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsANSI92EntryLevelSQL() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsANSI92FullSQL() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsANSI92IntermediateSQL() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsBatchUpdates() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return false;
	}

	public boolean 	supportsCatalogsInDataManipulation()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCatalogsInIndexDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCatalogsInPrivilegeDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCatalogsInProcedureCalls()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCatalogsInTableDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsColumnAliasing() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsConvert() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsConvert(int fromType, int toType)
						throws SQLException {
		debugFunction();
		// FIXME: db-and-type-specific
		return true;
	}

	public boolean 	supportsCoreSQLGrammar() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCorrelatedSubqueries() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsDataDefinitionAndDataManipulationTransactions()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsDataManipulationTransactionsOnly()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return false;
	}

	public boolean 	supportsDifferentTableCorrelationNames()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsExpressionsInOrderBy() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsExtendedSQLGrammar() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsFullOuterJoins() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsGetGeneratedKeys() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsGroupBy() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsGroupByBeyondSelect() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsGroupByUnrelated() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsIntegrityEnhancementFacility()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return false;
	}

	public boolean 	supportsLikeEscapeClause() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsLimitedOuterJoins() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsMinimumSQLGrammar() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsMixedCaseIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific, oracle doesn't
		return true;
	}

	public boolean 	supportsMixedCaseQuotedIdentifiers()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsMultipleOpenResults() throws SQLException {
		debugFunction();
		return true;
	}

	public boolean 	supportsMultipleResultSets() throws SQLException {
		debugFunction();
		// FIXME: in progress...
		return false;
	}

	public boolean 	supportsMultipleTransactions() throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	supportsNamedParameters() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsNonNullableColumns() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsOpenCursorsAcrossCommit()
						throws SQLException {
		debugFunction();
		// FIXME: not sure
		return false;
	}

	public boolean 	supportsOpenCursorsAcrossRollback()
						throws SQLException {
		debugFunction();
		// FIXME: not sure
		return false;
	}

	public boolean 	supportsOpenStatementsAcrossCommit()
						throws SQLException {
		debugFunction();
		// FIXME: not sure
		return false;
	}

	public boolean 	supportsOpenStatementsAcrossRollback()
						throws SQLException {
		debugFunction();
		// FIXME: not sure
		return false;
	}

	public boolean 	supportsOrderByUnrelated() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsOuterJoins() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsPositionedDelete() throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	supportsPositionedUpdate() throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	supportsResultSetConcurrency(int type,
							int concurrency)
							throws SQLException {
		debugFunction();
		return (type==ResultSet.TYPE_FORWARD_ONLY &&
			concurrency==ResultSet.CONCUR_READ_ONLY);
	}

	public boolean 	supportsResultSetHoldability(int holdability)
							throws SQLException {
		debugFunction();
		return (holdability==ResultSet.CLOSE_CURSORS_AT_COMMIT);
	}

	public boolean 	supportsResultSetType(int type) throws SQLException {
		debugFunction();
		return (type==ResultSet.TYPE_FORWARD_ONLY);
	}

	public boolean 	supportsSavepoints() throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	supportsSchemasInDataManipulation()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSchemasInIndexDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSchemasInPrivilegeDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSchemasInProcedureCalls()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSchemasInTableDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSelectForUpdate() throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	supportsStatementPooling() throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	supportsStoredFunctionsUsingCallSyntax()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return false;
	}

	public boolean 	supportsStoredProcedures() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSubqueriesInComparisons() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSubqueriesInExists() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSubqueriesInIns() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSubqueriesInQuantifieds() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsTableCorrelationNames() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsTransactionIsolationLevel(int level)
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsTransactions() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsUnion() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsUnionAll() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		return true;
	}

	public boolean 	updatesAreDetected(int type) throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	usesLocalFilePerTable() throws SQLException {
		debugFunction();
		return false;
	}

	public boolean 	usesLocalFiles() throws SQLException {
		debugFunction();
		return false;
	}

	public boolean	isWrapperFor(Class<?> iface) throws SQLException {
		debugFunction();
		return false;
	}

	public <T> T	unwrap(Class<T> iface) throws SQLException {
		debugFunction();
		return null;
	}
};
