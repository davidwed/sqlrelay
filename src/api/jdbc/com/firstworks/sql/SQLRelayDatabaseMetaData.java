package com.firstworks.sql;
	
import java.sql.*;
	
public class SQLRelayDatabaseMetaData implements DatabaseMetaData {

	private SQLRelayConnection	connection;

	public SQLRelayDatabaseMetaData() {
		connection=null;
		// FIXME: set protected member variables?
	}

	public void setConnection(SQLRelayConnection connection) {
		this.connection=connection;
	}

	public boolean 	allProceduresAreCallable() throws SQLException {
		// Retrieves whether the current user can call all the
		// procedures returned by the method getProcedures.
		// FIXME: this is almost certainly not correct.
		return true;
	}

	public boolean 	allTablesAreSelectable() throws SQLException {
		// Retrieves whether the current user can use all the tables
		// returned by the method getTables in a SELECT statement.
		// FIXME: this is almost certainly not correct.
		return true;
	}

	public boolean 	autoCommitFailureClosesAllResultSets()
							throws SQLException {
		// Retrieves whether a SQLException while autoCommit is true
		// inidcates that all open ResultSets are closed, even ones
		// that are holdable.
		// FIXME: no idea if this is true or not
		return false;
	}

	public boolean 	dataDefinitionCausesTransactionCommit()
							throws SQLException {
		// FIXME: db-specific
		return false;
	}

	public boolean 	dataDefinitionIgnoredInTransactions()
							throws SQLException {
		// FIXME: db-specific
		return false;
	}

	public boolean 	deletesAreDetected(int type) throws SQLException {
		// SQL Relay doesn't currenlty support ResultSet.RowDelete
		return false;
	}

	public boolean 	doesMaxRowSizeIncludeBlobs() throws SQLException {
		return false;
	}

	public boolean 	generatedKeyAlwaysReturned() throws SQLException {
		return true;
	}

	public ResultSet 	getAttributes(String catalog,
						String schemaPattern,
						String typeNamePattern,
						String attributeNamePattern)
						throws SQLException {
		// FIXME: implement this somehow...
		return null;
	}

	public ResultSet 	getBestRowIdentifier(String catalog,
							String schema,
							String table,
							int scope,
							boolean nullable)
							throws SQLException {
		// FIXME: implement this somehow...
		return null;
	}

	public ResultSet 	getCatalogs() throws SQLException {
		// FIXME: implement this by calling sqlrcursor.getDatabases()
		return null;
	}

	public String 	getCatalogSeparator() throws SQLException {
		// FIXME: oracle uses @
		return ".";
	}

	public String 	getCatalogTerm() throws SQLException {
		// FIXME: I think SQL Server uses catalog, maybe sybase
		return "database";
	}

	public ResultSet 	getClientInfoProperties() throws SQLException {
		// FIXME: free form in SQL Relay
		return null;
	}

	public ResultSet 	getColumnPrivileges(
						String catalog,
						String schema,
						String table,
						String columnNamePattern)
						throws SQLException {
		// FIXME: implement this somehow
		return null;
	}

	public ResultSet 	getColumns(String catalog,
						String schemaPattern,
						String tableNamePattern,
						String columnNamePattern)
						throws SQLException {
		// FIXME: implement this by calling sqlrcursor.getColumnList()
		return null;
	}

	public Connection 	getConnection() throws SQLException {
		return connection;
	}

	public ResultSet 	getCrossReference(String parentCatalog,
						String parentSchema,
						String parentTable,
						String foreignCatalog,
						String foreignSchema,
						String foreignTable)
						throws SQLException {
		// FIXME: implement this somehow...
		return null;
	}

	public int 	getDatabaseMajorVersion() throws SQLException {
		// FIXME: SQL Relay or db?
		return 0;
	}

	public int 	getDatabaseMinorVersion() throws SQLException {
		// FIXME: SQL Relay or db?
		return 0;
	}

	public String 	getDatabaseProductName() throws SQLException {
		// FIXME: implement this by calling sqlrcon.identify()
		return null;
	}

	public String 	getDatabaseProductVersion() throws SQLException {
		// FIXME: implement this by calling sqlrcon.dbVersion()
		return null;
	}

	public int 	getDefaultTransactionIsolation() throws SQLException {
		// FIXME: mysql is repeatable read
		return Connection.TRANSACTION_READ_COMMITTED;
	}

	public int 	getDriverMajorVersion() {
		// FIXME: make this come from sqlrclient
		return 1;
	}

	public int 	getDriverMinorVersion() {
		// FIXME: make this come from sqlrclient
		return 2;
	}

	public String 	getDriverName() throws SQLException {
		return "sqlrelay";
	}

	public String 	getDriverVersion() throws SQLException {
		// FIXME: make this come from sqlrclient
		return "1.2.0";
	}

	public ResultSet 	getExportedKeys(String catalog,
						String schema,
						String table)
						throws SQLException {
		// Retrieves a description of the foreign key columns that
		// reference the given table's primary key columns (the foreign
		// keys exported by a table).
		// FIXME: implement this somehow
		return null;
	}

	public String 	getExtraNameCharacters() throws SQLException {
		// FIXME: db-specific
		return null;
	}

	public ResultSet 	getFunctionColumns(
						String catalog,
						String schemaPattern,
						String functionNamePattern,
						String columnNamePattern)
						throws SQLException {
		// FIXME: implement with
		// sqlrcur.getProcedureBindAndColumnList()?
		return null;
	}

	public ResultSet 	getFunctions(String catalog,
						String schemaPattern,
						String functionNamePattern)
						throws SQLException {
		// FIXME: implement this by calling sqlrcur.getProcedures()?
		return null;
	}

	public String 	getIdentifierQuoteString() throws SQLException {
		// FIXME: db-specific
		// * mysql uses back-tick
		// * sqlserver uses braces
		return "\"";
	}

	public ResultSet 	getImportedKeys(String catalog,
						String schema,
						String table)
						throws SQLException {
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
		// FIXME: implement using sqlrcur.getKeyAndIndexList() ?
		return null;
	}

	public int 	getJDBCMajorVersion() throws SQLException {
		// FIXME: make this come from sqlrclient
		return 1;
	}

	public int 	getJDBCMinorVersion() throws SQLException {
		// FIXME: make this come from sqlrclient
		return 2;
	}

	public int 	getMaxBinaryLiteralLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxCatalogNameLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxCharLiteralLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnNameLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInGroupBy() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInIndex() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInOrderBy() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInSelect() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxColumnsInTable() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxConnections() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxCursorNameLength() throws SQLException {
		return 0;
	}

	public int 	getMaxIndexLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxProcedureNameLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxRowSize() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxSchemaNameLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxStatementLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxStatements() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxTableNameLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxTablesInSelect() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public int 	getMaxUserNameLength() throws SQLException {
		// FIXME: db-specific (0 means no limit or unknown)
		return 0;
	}

	public String 	getNumericFunctions() throws SQLException {
		// FIXME: db-specific
		return null;
	}

	public ResultSet 	getPrimaryKeys(String catalog,
						String schema,
						String table)
						throws SQLException {
		// FIXME: implement this by calling sqlrcon.getPrimaryKeysList()
		return null;
	}

	public ResultSet 	getProcedureColumns(
						String catalog,
						String schemaPattern,
						String procedureNamePattern,
						String columnNamePattern)
						throws SQLException {
		// FIXME: implement this by calling
		// sqlrcon.getProcedureBindAndColumnList()
		return null;
	}

	public ResultSet 	getProcedures(String catalog,
						String schemaPattern,
						String procedureNamePattern)
						throws SQLException {
		// FIXME: implement this by calling sqlrcon.getProcedureList()
		return null;
	}

	public String 	getProcedureTerm() throws SQLException {
		return "procedure";
	}

	public ResultSet 	getPseudoColumns(String catalog,
						String schemaPattern,
						String tableNamePattern,
						String columnNamePattern)
						throws SQLException {
		// FIXME: implement this somehow
		return null;
	}

	public int 	getResultSetHoldability() throws SQLException {
		// FIXME: is this correct?
		return ResultSet.CLOSE_CURSORS_AT_COMMIT;
	}

	public RowIdLifetime 	getRowIdLifetime() throws SQLException {
		// FIXME: some dbs do support rowid
		return RowIdLifetime.ROWID_UNSUPPORTED;
	}

	public ResultSet 	getSchemas() throws SQLException {
		// FIXME: implement this by calling sqlrcon.getSchemaList()
		return null;
	}

	public ResultSet 	getSchemas(String catalog,
						String schemaPattern)
						throws SQLException {
		// FIXME: implement this by calling sqlrcon.getSchemaList()
		return null;
	}

	public String 	getSchemaTerm() throws SQLException {
		return "schema";
	}

	public String 	getSearchStringEscape() throws SQLException {
		// FIXME: db-specific
		return "\\";
	}

	public String 	getSQLKeywords() throws SQLException {
		// FIXME: db-specific
		return null;
	}

	public int 	getSQLStateType() throws SQLException {
		// FIXME: no idea
		return sqlStateSQL;
	}

	public String 	getStringFunctions() throws SQLException {
		// FIXME: db-specific
		return null;
	}

	public ResultSet 	getSuperTables(String catalog,
						String schemaPattern,
						String tableNamePattern)
						throws SQLException {
		// FIXME: implement this somehow
		return null;
	}

	public ResultSet 	getSuperTypes(String catalog,
						String schemaPattern,
						String typeNamePattern)
						throws SQLException {
		// FIXME: implement this somehow
		return null;
	}

	public String 	getSystemFunctions() throws SQLException {
		// FIXME: implement this somehow
		return null;
	}

	public ResultSet 	getTablePrivileges(String catalog,
						String schemaPattern,
						String tableNamePattern)
						throws SQLException {
		// FIXME: implement this somehow
		return null;
	}

	public ResultSet 	getTables(String catalog,
						String schemaPattern,
						String tableNamePattern,
						String[] types)
						throws SQLException {
		// FIXME: implement this by calling sqlrcon.getTableList()
		return null;
	}

	public ResultSet 	getTableTypes() throws SQLException {
		// FIXME: implement this somehow
		return null;
	}

	public String 	getTimeDateFunctions() throws SQLException {
		// FIXME: db-specific
		return null;
	}

	public ResultSet 	getTypeInfo() throws SQLException {
		// FIXME: implement this by calling sqlrcon.getTypeInfo()
		return null;
	}

	public ResultSet 	getUDTs(String catalog,
						String schemaPattern,
						String typeNamePattern,
						int[] types)
						throws SQLException {
		// FIXME: implement this somehow
		return null;
	}

	public String 	getURL() throws SQLException {

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
		return connection.getUser();
	}

	public ResultSet 	getVersionColumns(String catalog,
							String schema,
							String table)
							throws SQLException {
		// FIXME: implement this somehow
		return null;
	}

	public boolean 	insertsAreDetected(int type) throws SQLException {
		return false;
	}

	public boolean 	isCatalogAtStart() throws SQLException {
		// FIXME: not in oracle
		return true;
	}

	public boolean 	isReadOnly() throws SQLException {
		// FIXME: implement this somehow
		return false;
	}

	public boolean 	locatorsUpdateCopy() throws SQLException {
		// FIXME: no idea, probably db-specific
		return false;
	}

	public boolean 	nullPlusNonNullIsNull() throws SQLException {
		// FIXME: generally true, but probably db-specific
		return true;
	}

	public boolean 	nullsAreSortedAtEnd() throws SQLException {
		// FIXME: generally true, but probably db-specific
		return true;
	}

	public boolean 	nullsAreSortedAtStart() throws SQLException {
		// FIXME: generally false, but probably db-specific
		return false;
	}

	public boolean 	nullsAreSortedHigh() throws SQLException {
		// FIXME: generally true, but probably db-specific
		return true;
	}

	public boolean 	nullsAreSortedLow() throws SQLException {
		// FIXME: generally false, but probably db-specific
		return false;
	}

	public boolean 	othersDeletesAreVisible(int type) throws SQLException {
		return false;
	}

	public boolean 	othersInsertsAreVisible(int type) throws SQLException {
		return false;
	}

	public boolean 	othersUpdatesAreVisible(int type) throws SQLException {
		return false;
	}

	public boolean 	ownDeletesAreVisible(int type) throws SQLException {
		return false;
	}

	public boolean 	ownInsertsAreVisible(int type) throws SQLException {
		return false;
	}

	public boolean 	ownUpdatesAreVisible(int type) throws SQLException {
		return false;
	}

	public boolean 	storesLowerCaseIdentifiers() throws SQLException {
		// FIXME: db-specific but generally false
		// oracle stores upper case identifiers
		// other db's store mixed case identifiers
		return false;
	}

	public boolean 	storesLowerCaseQuotedIdentifiers() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	storesMixedCaseIdentifiers() throws SQLException {
		// FIXME: generally true, but db-specific, false for oracle
		return true;
	}

	public boolean 	storesMixedCaseQuotedIdentifiers() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	storesUpperCaseIdentifiers() throws SQLException {
		// FIXME: db-specific but generally false
		// oracle stores upper case identifiers
		// other db's store mixed case identifiers
		return false;
	}

	public boolean 	storesUpperCaseQuotedIdentifiers() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsAlterTableWithAddColumn() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsAlterTableWithDropColumn() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsANSI92EntryLevelSQL() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsANSI92FullSQL() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsANSI92IntermediateSQL() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsBatchUpdates() throws SQLException {
		// FIXME: db-specific
		return false;
	}

	public boolean 	supportsCatalogsInDataManipulation()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCatalogsInIndexDefinitions()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCatalogsInPrivilegeDefinitions()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCatalogsInProcedureCalls()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCatalogsInTableDefinitions()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsColumnAliasing() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsConvert() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsConvert(int fromType, int toType)
						throws SQLException {
		// FIXME: db-and-type-specific
		return true;
	}

	public boolean 	supportsCoreSQLGrammar() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsCorrelatedSubqueries() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsDataDefinitionAndDataManipulationTransactions()
							throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsDataManipulationTransactionsOnly()
							throws SQLException {
		// FIXME: db-specific
		return false;
	}

	public boolean 	supportsDifferentTableCorrelationNames()
							throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsExpressionsInOrderBy() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsExtendedSQLGrammar() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsFullOuterJoins() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsGetGeneratedKeys() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsGroupBy() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsGroupByBeyondSelect() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsGroupByUnrelated() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsIntegrityEnhancementFacility()
							throws SQLException {
		// FIXME: db-specific
		return false;
	}

	public boolean 	supportsLikeEscapeClause() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsLimitedOuterJoins() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsMinimumSQLGrammar() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsMixedCaseIdentifiers() throws SQLException {
		// FIXME: db-specific, oracle doesn't
		return true;
	}

	public boolean 	supportsMixedCaseQuotedIdentifiers()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsMultipleOpenResults() throws SQLException {
		return true;
	}

	public boolean 	supportsMultipleResultSets() throws SQLException {
		// FIXME: in progress...
		return false;
	}

	public boolean 	supportsMultipleTransactions() throws SQLException {
		return false;
	}

	public boolean 	supportsNamedParameters() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsNonNullableColumns() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsOpenCursorsAcrossCommit()
						throws SQLException {
		// FIXME: not sure
		return false;
	}

	public boolean 	supportsOpenCursorsAcrossRollback()
						throws SQLException {
		// FIXME: not sure
		return false;
	}

	public boolean 	supportsOpenStatementsAcrossCommit()
						throws SQLException {
		// FIXME: not sure
		return false;
	}

	public boolean 	supportsOpenStatementsAcrossRollback()
						throws SQLException {
		// FIXME: not sure
		return false;
	}

	public boolean 	supportsOrderByUnrelated() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsOuterJoins() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsPositionedDelete() throws SQLException {
		return false;
	}

	public boolean 	supportsPositionedUpdate() throws SQLException {
		return false;
	}

	public boolean 	supportsResultSetConcurrency(int type,
							int concurrency)
							throws SQLException {
		return (type==ResultSet.TYPE_FORWARD_ONLY &&
			concurrency==ResultSet.CONCUR_READ_ONLY);
	}

	public boolean 	supportsResultSetHoldability(int holdability)
							throws SQLException {
		return (holdability==ResultSet.CLOSE_CURSORS_AT_COMMIT);
	}

	public boolean 	supportsResultSetType(int type) throws SQLException {
		return (type==ResultSet.TYPE_FORWARD_ONLY);
	}

	public boolean 	supportsSavepoints() throws SQLException {
		return false;
	}

	public boolean 	supportsSchemasInDataManipulation()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSchemasInIndexDefinitions()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSchemasInPrivilegeDefinitions()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSchemasInProcedureCalls()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSchemasInTableDefinitions()
						throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSelectForUpdate() throws SQLException {
		return false;
	}

	public boolean 	supportsStatementPooling() throws SQLException {
		return false;
	}

	public boolean 	supportsStoredFunctionsUsingCallSyntax()
						throws SQLException {
		// FIXME: db-specific
		return false;
	}

	public boolean 	supportsStoredProcedures() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSubqueriesInComparisons() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSubqueriesInExists() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSubqueriesInIns() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsSubqueriesInQuantifieds() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsTableCorrelationNames() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsTransactionIsolationLevel(int level)
							throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsTransactions() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsUnion() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	supportsUnionAll() throws SQLException {
		// FIXME: db-specific
		return true;
	}

	public boolean 	updatesAreDetected(int type) throws SQLException {
		return false;
	}

	public boolean 	usesLocalFilePerTable() throws SQLException {
		return false;
	}

	public boolean 	usesLocalFiles() throws SQLException {
		return false;
	}

	public boolean	isWrapperFor(Class<?> iface) throws SQLException {
		// FIXME: implement this for SQLRCursor
		return false;
	}

	public <T> T	unwrap(Class<T> iface) throws SQLException {
		// FIXME: implement this for SQLRCursor
		return null;
	}
};
