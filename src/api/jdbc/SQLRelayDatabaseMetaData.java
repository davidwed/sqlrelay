package com.firstworks.sql;
    
import java.sql;
    
public class SQLRelayDatabaseMetaData implements java.sql.DatabaseMetaData {
    
    /** 
     *  PROCEDURE_TYPE - May return a result.
     */
    public static final int procedureResultUnknown
    
    /**
     *  PROCEDURE_TYPE - Does not return a result.
     */
    public static final int procedureNoResult
    
    /**
     *  PROCEDURE_TYPE - Returns a result.
     */
    public static final int procedureReturnsResult
    
    /**
     *  COLUMN_TYPE - nobody knows.
     */
    public static final int procedureColumnUnknown
    
    /**
     *  COLUMN_TYPE - IN parameter.
     */
    public static final int procedureColumnIn
    
    
    /**
     *  COLUMN_TYPE - INOUT parameter.
     */
    public static final int procedureColumnInOut
    
    /**
     *  COLUMN_TYPE - OUT parameter.
     */
    public static final int procedureColumnOut
    
    /**
     *  COLUMN_TYPE - procedure return value.
     */
    public static final int procedureColumnReturn
    
    /**
     *  COLUMN_TYPE - result column in ResultSet.
     */
    public static final int procedureColumnResult
    
    /**
     *  TYPE NULLABLE - does not allow NULL values.
     */
    public static final int procedureNoNulls
    
    /**
     *  TYPE NULLABLE - allows NULL values.
     */
    public static final int procedureNullable
    
    /**
     *  TYPE NULLABLE - nullability unknown.
     */
    public static final int procedureNullableUnknown
    
    /**
     *  COLUMN NULLABLE - might not allow NULL values.
     */
    public static final int columnNoNulls
    
    /**
     *  COLUMN NULLABLE - definitely allows NULL values.
     */
    public static final int columnNullable
    
    /**
     *  COLUMN NULLABLE - nullability unknown.
     */
    public static final int columnNullableUnknown
    
    /**
     *  BEST ROW SCOPE - very temporary, while using row.
     */
    public static final int bestRowTemporary
    
    /**
     *  BEST ROW SCOPE - valid for remainder of current transaction.
     */
    public static final int bestRowTransaction
    
    /**
     *  BEST ROW SCOPE - valid for remainder of current session.
     */
    public static final int bestRowSession
    
    /**
     *  BEST ROW PSEUDO_COLUMN - may or may not be pseudo column.
     */
    public static final int bestRowUnknown
    
    /**
     *  BEST ROW PSEUDO_COLUMN - is NOT a pseudo column.
     */
    public static final int bestRowNotPseudo
    
    /**
     *  BEST ROW PSEUDO_COLUMN - is a pseudo column.
     */
    public static final int bestRowPseudo
    
    /**
     *  VERSION COLUMNS PSEUDO_COLUMN - may or may not be pseudo column.
     */
    public static final int versionColumnUnknown
    
    /**
     *  VERSION COLUMNS PSEUDO_COLUMN - is NOT a pseudo column.
     */
    public static final int versionColumnNotPseudo
    
    /**
     *  VERSION COLUMNS PSEUDO_COLUMN - is a pseudo column.
     */
    public static final int versionColumnPseudo
    
    /**
     *  IMPORT KEY UPDATE_RULE and DELETE_RULE - for update, change imported
     *  key to agree with primary key update; for delete, delete rows that
     *  import a deleted key.
     */
    public static final int importedKeyCascade
    
    /**
     *  IMPORT KEY UPDATE_RULE and DELETE_RULE - do not allow update or delete
     *  of primary key if it has been imported.
     */
    public static final int importedKeyRestrict
    
    /**
     *  IMPORT KEY UPDATE_RULE and DELETE_RULE - change imported key to NULL if 
     *  its primary key has been updated or deleted.
     */
    public static final int importedKeySetNull
    
    /**
     *  IMPORT KEY UPDATE_RULE and DELETE_RULE - do not allow update or delete
     *  of primary key if it has been imported.
     */
    public static final int importedKeyNoAction
    
    /**
     *  IMPORT KEY UPDATE_RULE and DELETE_RULE - change imported key to default 
     *  values if its primary key has been updated or deleted.
     */
    public static final int importedKeySetDefault
    
    /**
     *  IMPORT KEY DEFERRABILITY - see SQL92 for definition
     */
    public static final int importedKeyInitiallyDeferred
    
    /**
     *  IMPORT KEY DEFERRABILITY - see SQL92 for definition
     */
    public static final int importedKeyInitiallyImmediate
    
    /**
     *  IMPORT KEY DEFERRABILITY - see SQL92 for definition
     */
    public static final int importedKeyNotDeferrable
    
    /**
     *  TYPE NULLABLE - does not allow NULL values.
     */
    public static final int typeNoNulls
    
    /**
     *  TYPE NULLABLE - allows NULL values.
     */
    public static final int typeNullable
    
    /**
     *  TYPE NULLABLE - nullability unknown.
     */
    public static final int typeNullableUnknown
    
    /**
     *  TYPE INFO SEARCHABLE - No support.
     */
    public static final int typePredNone
    
    /**
     *  TYPE INFO SEARCHABLE - Only supported with WHERE .. LIKE.
     */
    public static final int typePredChar
    
    /**
     *  TYPE INFO SEARCHABLE - Supported except for WHERE .. LIKE.
     */
    public static final int typePredBasic
    
    /**
     *  TYPE INFO SEARCHABLE - Supported for all WHERE ...
     */
    public static final int typeSearchable
    
    /**
     *  INDEX INFO TYPE - this identifies table statistics that are returned in 
     *  conjuction with a table's index descriptions
     */
    public static final short tableIndexStatistic
    
    /**
     *  INDEX INFO TYPE - this identifies a clustered index
     */
    public static final short tableIndexClustered
    
    /**
     *  INDEX INFO TYPE - this identifies a hashed index
     */
    public static final short tableIndexHashed
    
    /**
     *  INDEX INFO TYPE - this identifies some other form of index
     */
    public static final short tableIndexOther
    
    
    
    
    /**
     *  Can all the procedures returned by getProcedures be called by the
     *  current user?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean allProceduresAreCallable() throws SQLException {
    }
    
    /**
     *  Can all the tables returned by getTable be SELECTed by the current
     *  user?  
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean allTablesAreSelectable() throws SQLException {
    }
    
    /**
     *  What's the url for this database?
     *
     *  Returns:
     *  the url or null if it can't be generated
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getURL() throws SQLException {
    }
    
    /**
     *  What's our user name as known to the database?
     *
     *  Returns:
     *  our database user name
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getUserName() throws SQLException {
    }
    
    /**
     *  Is the database in read-only mode?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean isReadOnly() throws SQLException {
    }
    
    /**
     *  Are NULL values sorted high?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean nullsAreSortedHigh() throws SQLException {
    }
    
    /**
     *  Are NULL values sorted low?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean nullsAreSortedLow() throws SQLException {
    }
    
    /**
     *
     *  Are NULL values sorted at the start regardless of sort order?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean nullsAreSortedAtStart() throws SQLException {
    }
    
    /**
     *  Are NULL values sorted at the end regardless of sort order?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean nullsAreSortedAtEnd() throws SQLException {
    }
    
    /**
     *  What's the name of this database product?
     *
     *  Returns:
     *  database product name
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getDatabaseProductName() throws SQLException {
    }
    
    /**
     *  What's the version of this database product?
     *
     *  Returns:
     *  database version
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getDatabaseProductVersion() throws SQLException {
    }
    
    /**
     *  What's the name of this JDBC driver?
     *
     *  Returns:
     *  JDBC driver name
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getDriverName() throws SQLException {
    }
    
    
    /**
     * What's the version of this JDBC driver?
     *
     * Returns:
     *  JDBC driver version
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getDriverVersion() throws SQLException {
    }
    
    /**
     *  What's this JDBC driver's major version number?
     *
     *  Returns:
     *  JDBC driver major version
     */
    public abstract int getDriverMajorVersion() {
    }
    
    /**
     *  What's this JDBC driver's minor version number?
     *
     *  Returns:
     *  JDBC driver minor version
     */
    public abstract int getDriverMinorVersion() {
    }
    
    /**
     *  Does the database store tables in a local file?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean usesLocalFiles() throws SQLException {
    }
    
    /**
     *  Does the database use a file for each table?
     *
     *  Returns:
     *  true if the database uses a local file for each table
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean usesLocalFilePerTable() throws SQLException {
    }
    
    /**
     *  Does the database treat mixed case unquoted SQL identifiers as case 
     *  sensitive and as a result store them in mixed case? A JDBC-Compliant
     *  driver will always return false.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsMixedCaseIdentifiers() throws SQLException {
    }
    
    /**
     *  Does the database treat mixed case unquoted SQL identifiers as case 
     *  insensitive and store them in upper case?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean storesUpperCaseIdentifiers() throws SQLException {
    }
    
    /**
     *  Does the database treat mixed case unquoted SQL identifiers as case 
     *  insensitive and store them in lower case?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean storesLowerCaseIdentifiers() throws SQLException {
    }
    
    /**
     *  Does the database treat mixed case unquoted SQL identifiers as case 
     *  insensitive and store them in mixed case?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean storesMixedCaseIdentifiers() throws SQLException
    
    
    /**
     *  Does the database treat mixed case quoted SQL identifiers as case
     *  sensitive and as a result store them in mixed case? A JDBC-Compliant
     *  driver will always return true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsMixedCaseQuotedIdentifiers() 
                                                    throws SQLException {
    }
    
    /**
     *  Does the database treat mixed case quoted SQL identifiers as case 
     *  insensitive and store them in upper case?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean storesUpperCaseQuotedIdentifiers()
                                                    throws SQLException {
    }
    
    /**
     *  Does the database treat mixed case quoted SQL identifiers as case 
     *  insensitive and store them in lower case?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean storesLowerCaseQuotedIdentifiers()
                                                    throws SQLException {
    }
    
    /**
     *  Does the database treat mixed case quoted SQL identifiers as case 
     *  insensitive and store them in mixed case?
     *
     *  Returns:
     *   true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean storesMixedCaseQuotedIdentifiers()
                                                    throws SQLException {
    }
    
    /**
     *  What's the string used to quote SQL identifiers? This returns a space
     *  " " if identifier quoting isn't supported. A JDBC-Compliant driver
     *  always uses a double quote character.
     *
     *  Returns:
     *  the quoting string
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getIdentifierQuoteString() throws SQLException {
    }
    
    /**
     *  Get a comma separated list of all a database's SQL keywords that are
     *  NOT also SQL92 keywords.
     *
     *  Returns:
     *  the list
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getSQLKeywords() throws SQLException {
    }
    
    /**
     *  Get a comma separated list of math functions.
     *
     *  Returns:
     *  the list
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getNumericFunctions() throws SQLException {
    }
    
    /**
     *  Get a comma separated list of string functions.
     *
     *  Returns:
     *  the list
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getStringFunctions() throws SQLException {
    }
    
    /**
     *  Get a comma separated list of system functions.
     *
     *  Returns:
     *  the list
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getSystemFunctions() throws SQLException {
    }
    
    /**
     *  Get a comma separated list of time and date functions.
     *
     *  Returns:
     *  the list
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getTimeDateFunctions() throws SQLException {
    }
    
    /**
     *  This is the string that can be used to escape '_' or '%' in the string 
     *  pattern style catalog search parameters.
     *
     *  The '_' character represents any single character.
     *
     *  The '%' character represents any sequence of zero or more characters.
     *
     *  Returns:
     *  the string used to escape wildcard characters
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getSearchStringEscape() throws SQLException {
    }
    
    /**
     *  Get all the "extra" characters that can be used in unquoted identifier 
     *  names (those beyond a-z, A-Z, 0-9 and _).
     *
     *  Returns:
     *  the string containing the extra characters
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getExtraNameCharacters() throws SQLException {
    }
    
    /**
     *  Is "ALTER TABLE" with add column supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsAlterTableWithAddColumn()
                                                    throws SQLException {
    }
    
    /**
     *  Is "ALTER TABLE" with drop column supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsAlterTableWithDropColumn()
                                                    throws SQLException {
    }
    
    /**
     *  Is column aliasing supported?
     *
     *  If so, the SQL AS clause can be used to provide names for computed
     *  columns or to provide alias names for columns as required. A
     *  JDBC-Compliant driver always returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsColumnAliasing() throws SQLException {
    }
    
    /**
     *  Are concatenations between NULL and non-NULL values NULL? A
     *  JDBC-Compliant driver always returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean nullPlusNonNullIsNull() throws SQLException {
    }
    
    /**
     *  Is the CONVERT function between SQL types supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsConvert() throws SQLException {
    }
    
    /**
     *  Is CONVERT between the given SQL types supported?
     *
     *  Parameters:
     *  fromType - the type to convert from
     *  toType - the type to convert to
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     *
     *  See Also: Types
     */
    public abstract boolean supportsConvert(int fromType, int toType)
                                                        throws SQLException {
    }
    
    /**
     *  Are table correlation names supported? A JDBC-Compliant driver always 
     *  returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsTableCorrelationNames() throws SQLException {
    }
    
    /**
     *  If table correlation names are supported, are they restricted to be 
     *  different from the names of the tables?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsDifferentTableCorrelationNames()
                                                        throws SQLException {
    }
    
    /**
     *  Are expressions in "ORDER BY" lists supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsExpressionsInOrderBy() throws SQLException {
    }
    
    /**
     *  Can an "ORDER BY" clause use columns not in the SELECT?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsOrderByUnrelated() throws SQLException {
    }
    
    /**
     *  Is some form of "GROUP BY" clause supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsGroupBy() throws SQLException {
    }
    
    /**
     *  Can a "GROUP BY" clause use columns not in the SELECT?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsGroupByUnrelated() throws SQLException {
    }
    
    /**
     *  Can a "GROUP BY" clause add columns not in the SELECT provided it
     *  specifies all the columns in the SELECT?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsGroupByBeyondSelect() throws SQLException {
    }
    
    /**
     *  Is the escape character in "LIKE" clauses supported? A JDBC-Compliant 
     *  driver always returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsLikeEscapeClause() throws SQLException {
    }
    
    /**
     *  Are multiple ResultSets from a single execute supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsMultipleResultSets() throws SQLException {
    }
    
    /**
     *  Can we have multiple transactions open at once (on different
     *  connections)?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsMultipleTransactions() throws SQLException {
    }
    
    /**
     *  Can columns be defined as non-nullable? A JDBC-Compliant driver always 
     *  returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsNonNullableColumns() throws SQLException {
    }
    
    /**
     *  Is the ODBC Minimum SQL grammar supported? All JDBC-Compliant drivers
     *  must return true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsMinimumSQLGrammar() throws SQLException {
    }
    
    /**
     *  Is the ODBC Core SQL grammar supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsCoreSQLGrammar() throws SQLException {
    }
    
    /**
     *  Is the ODBC Extended SQL grammar supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsExtendedSQLGrammar() throws SQLException {
    }
    
    /**
     *  Is the ANSI92 entry level SQL grammar supported? All JDBC-Compliant
     *  drivers must return true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsANSI92EntryLevelSQL() throws SQLException {
    }
    
    /**
     *  Is the ANSI92 intermediate SQL grammar supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsANSI92IntermediateSQL()
                                                    throws SQLException {
    }
    
    /**
     *  Is the ANSI92 full SQL grammar supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsANSI92FullSQL() throws SQLException {
    }
    
    /**
     *  Is the SQL Integrity Enhancement Facility supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsIntegrityEnhancementFacility() 
                                                    throws SQLException {
    }
    
    /**
     *  Is some form of outer join supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsOuterJoins() throws SQLException {
    }
    
    /**
     *  Are full nested outer joins supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsFullOuterJoins() throws SQLException {
    }
    
    /**
     *  Is there limited support for outer joins? (This will be true if 
     *  supportFullOuterJoins is true.)
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsLimitedOuterJoins() throws SQLException {
    }
    
    /**
     *  What's the database vendor's preferred term for "schema"?
     *
     *  Returns:
     *  the vendor term
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getSchemaTerm() throws SQLException {
    }
    
    /**
     *  What's the database vendor's preferred term for "procedure"?
     *
     *  Returns:
     *  the vendor term
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getProcedureTerm() throws SQLException {
    }
    
    /**
     *  What's the database vendor's preferred term for "catalog"?
     *
     *  Returns:
     *  the vendor term
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getCatalogTerm() throws SQLException {
    }
    
    /**
     *  Does a catalog appear at the start of a qualified table name?
     *  (Otherwise it appears at the end)
     *
     *  Returns:
     *  true if it appears at the start
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean isCatalogAtStart() throws SQLException {
    }
    
    /**
     *  What's the separator between catalog and table name?
     *
     *  Returns:
     *  the separator string
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract String getCatalogSeparator() throws SQLException {
    }
    
    /**
     *  Can a schema name be used in a data manipulation statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSchemasInDataManipulation()
                                                    throws SQLException {
    }
    
    /**
     *  Can a schema name be used in a procedure call statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSchemasInProcedureCalls()
                                                    throws SQLException {
    }
    
    /**
     *  Can a schema name be used in a table definition statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSchemasInTableDefinitions()
                                                    throws SQLException {
    }
    
    /**
     *  Can a schema name be used in an index definition statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSchemasInIndexDefinitions()
                                                    throws SQLException {
    }
    
    /**
     *  Can a schema name be used in a privilege definition statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSchemasInPrivilegeDefinitions()
                                                    throws SQLException {
    }
    
    /**
     *  Can a catalog name be used in a data manipulation statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsCatalogsInDataManipulation()
                                                    throws SQLException {
    }
    
    /**
     *  Can a catalog name be used in a procedure call statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsCatalogsInProcedureCalls()
                                                    throws SQLException {
    }
    
    /**
     *  Can a catalog name be used in a table definition statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsCatalogsInTableDefinitions()
                                                    throws SQLException {
    }
    
    /**
     *  Can a catalog name be used in an index definition statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsCatalogsInIndexDefinitions()
                                                    throws SQLException {
    }
    
    /**
     *  Can a catalog name be used in a privilege definition statement?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsCatalogsInPrivilegeDefinitions()
                                                    throws SQLException {
    }
    
    /**
     *  Is positioned DELETE supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsPositionedDelete() throws SQLException {
    }
    
    /**
     *  Is positioned UPDATE supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsPositionedUpdate() throws SQLException {
    }
    
    /**
     *  Is SELECT for UPDATE supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSelectForUpdate() throws SQLException {
    }
    
    /**
     *  Are stored procedure calls using the stored procedure escape syntax 
     *  supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsStoredProcedures() throws SQLException {
    }
    
    /**
     *  Are subqueries in comparison expressions supported? A JDBC-Compliant
     *  driver always returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSubqueriesInComparisons()
                                                    throws SQLException {
    }
    
    /**
     *  Are subqueries in 'exists' expressions supported? A JDBC-Compliant
     *  driver always returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSubqueriesInExists() throws SQLException {
    }
    
    /**
     *  Are subqueries in 'in' statements supported? A JDBC-Compliant driver
     *  always returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSubqueriesInIns() throws SQLException {
    }
    
    /**
     *  Are subqueries in quantified expressions supported? A JDBC-Compliant
     *  driver always returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsSubqueriesInQuantifieds()
                                                    throws SQLException {
    }
    
    /**
     *  Are correlated subqueries supported? A JDBC-Compliant driver always
     *  returns true.
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsCorrelatedSubqueries() throws SQLException {
    }
    
    /**
     *  Is SQL UNION supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsUnion() throws SQLException {
    }
    
    /**
     *  Is SQL UNION ALL supported?
     *
     *  Returns:
     *  true if so
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsUnionAll() throws SQLException {
    }
    
    /**
     *  Can cursors remain open across commits?
     *
     *  Returns:
     *  true if cursors always remain open;
     *  false if they might not remain open
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsOpenCursorsAcrossCommit()
                                                    throws SQLException {
    }
    
    /**
     *  Can cursors remain open across rollbacks?
     *
     *  Returns:
     *  true if cursors always remain open;
     *  false if they might not remain open
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsOpenCursorsAcrossRollback()
                                                    throws SQLException {
    }
    
    /**
     *  Can statements remain open across commits?
     *
     *  Returns:
     *  true if statements always remain open;
     *  false if they might not remain open
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsOpenStatementsAcrossCommit()
                                                    throws SQLException {
    }
    
    /**
     *  Can statements remain open across rollbacks?
     *
     *  Returns:
     *  true if statements always remain open;
     *  false if they might not remain open
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract boolean supportsOpenStatementsAcrossRollback()
                                                    throws SQLException {
    }
    
    /**
     *  How many hex characters can you have in an inline binary literal?
     *
     *  Returns:
     *  max literal length Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxBinaryLiteralLength() throws SQLException {
    }
    
    /**
     *  What's the max length for a character literal?
     *
     *  Returns:
     *  max literal length Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxCharLiteralLength() throws SQLException {
    }
    
    /**
     *  What's the limit on column name length?
     *
     * Returns:
     * max literal length Throws: SQLException
     * if a database-access error occurs.
     */
    public abstract int getMaxColumnNameLength() throws SQLException {
    }
    
    /**
     *  What's the maximum number of columns in a "GROUP BY" clause?
     *
     *  Returns:
     *  max number of columns Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxColumnsInGroupBy() throws SQLException {
    }
    
    /**
     *  What's the maximum number of columns allowed in an index?
     *
     *  Returns:
     *  max columns Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxColumnsInIndex() throws SQLException {
    }
    
    /**
     *  What's the maximum number of columns in an "ORDER BY" clause?
     *
     *  Returns:
     *  max columns Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxColumnsInOrderBy() throws SQLException {
    }
    
    /**
     *  What's the maximum number of columns in a "SELECT" list?
     *
     *  Returns:
     *  max columns Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxColumnsInSelect() throws SQLException {
    }
    
    /**
     *  What's the maximum number of columns in a table?
     *
     *  Returns:
     *  max columns Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxColumnsInTable() throws SQLException {
    }
    
    /**
     *  How many active connections can we have at a time to this database?
     *
     *  Returns:
     *  max connections Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxConnections() throws SQLException {
    }
    
    /**
     *  What's the maximum cursor name length?
     *
     *  Returns:
     *  max cursor name length in bytes Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxCursorNameLength() throws SQLException {
    }
    
    /**
     *  What's the maximum length of an index (in bytes)?
     *
     *  Returns:
     *  max index length in bytes Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxIndexLength() throws SQLException {
    }
    
    /**
     *  What's the maximum length allowed for a schema name?
     *
     *  Returns:
     *  max name length in bytes Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxSchemaNameLength() throws SQLException {
    }
    
    /**
     *  What's the maximum length of a procedure name?
     *
     *  Returns:
     *  max name length in bytes Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxProcedureNameLength() throws SQLException {
    }
    
    /**
     *  What's the maximum length of a catalog name?
     *
     *  Returns:
     *  max name length in bytes Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxCatalogNameLength() throws SQLException {
    }
    
    /**
     *  What's the maximum length of a single row?
     *
     *  Returns:
     *  max row size in bytes Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxRowSize() throws SQLException {
    }
    
    /**
     *  Did getMaxRowSize() include LONGVARCHAR and LONGVARBINARY blobs?
     *
     *  Returns:
     *  true if so Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract boolean doesMaxRowSizeIncludeBlobs() throws SQLException {
    }
    
    /**
     *  What's the maximum length of a SQL statement?
     *
     *  Returns:
     *  max length in bytes Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxStatementLength() throws SQLException {
    }
    
    /**
     *  How many active statements can we have open at one time to this
     *  database?
     *
     *  Returns:
     *  the maximum Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxStatements() throws SQLException {
    }
    
    /**
     *  What's the maximum length of a table name?
     *
     *  Returns:
     *  max name length in bytes Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxTableNameLength() throws SQLException {
    }
    
    /**
     *  What's the maximum number of tables in a SELECT?
     *
     *  Returns:
     *  the maximum Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxTablesInSelect() throws SQLException {
    }
    
    /**
     *  What's the maximum length of a user name?
     *
     *  Returns:
     *  max name length in bytes Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract int getMaxUserNameLength() throws SQLException {
    }
    
    /**
     *  What's the database's default transaction isolation level? The values
     *  are defined in java.sql.Connection.
     *
     *  Returns:
     *  the default isolation level Throws: SQLException
     *  if a database-access error occurs. See Also: Connection
     */
    public abstract int getDefaultTransactionIsolation() throws SQLException {
    }
    
    /**
     *  Are transactions supported? If not, commit is a noop and the isolation
     *  level is TRANSACTION_NONE.
     *
     *  Returns:
     *  true if transactions are supported Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract boolean supportsTransactions() throws SQLException {
    }
    
    /**
     *  Does the database support the given transaction isolation level?
     *
     *  Parameters:
     *  level - the values are defined in java.sql.Connection Returns:
     *  true if so Throws: SQLException
     *  if a database-access error occurs. See Also:
     *  Connection
     */
    public abstract boolean supportsTransactionIsolationLevel(int level)
                                                        throws SQLException {
    }
    
    /**
     *  Are both data definition and data manipulation statements within a
     *  transaction supported?
     *
     *  Returns:
     *  true if so Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract
        boolean supportsDataDefinitionAndDataManipulationTransactions() 
                                                        throws SQLException {
    }
    
    /**
     *  Are only data manipulation statements within a transaction supported?
     *
     *  Returns:
     *  true if so Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract boolean supportsDataManipulationTransactionsOnly()
                                                        throws SQLException {
    }
    
    /**
     *  Does a data definition statement within a transaction force the
     *  transaction to commit?
     *
     *  Returns:
     *  true if so Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract boolean dataDefinitionCausesTransactionCommit()
                                                        throws SQLException {
    }
    
    /**
     *  Is a data definition statement within a transaction ignored?
     *
     *  Returns:
     *  true if so Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract boolean dataDefinitionIgnoredInTransactions()
                                                        throws SQLException {
    }
    
    /**
     *  Get a description of stored procedures available in a catalog.
     *
     *  Only procedure descriptions matching the schema and procedure name
     *  criteria are returned. They are ordered by PROCEDURE_SCHEM, and
     *  PROCEDURE_NAME.
     *
     *  Each procedure description has the the following columns:
     *
     *     1. PROCEDURE_CAT String => procedure catalog (may be null)
     *     2. PROCEDURE_SCHEM String => procedure schema (may be null)
     *     3. PROCEDURE_NAME String => procedure name
     *     4. reserved for future use
     *     5. reserved for future use
     *     6. reserved for future use
     *     7. REMARKS String => explanatory comment on the procedure
     *     8. PROCEDURE_TYPE short => kind of procedure:
     *            * procedureResultUnknown - May return a result
     *            * procedureNoResult - Does not return a result
     *            * procedureReturnsResult - Returns a result
     *
     *  Parameters:
     *  catalog - a catalog name;
     *      "" retrieves those without a catalog;
     *      null means drop catalog name from the selection criteria
     *  schemaPattern - a schema name pattern;
     *      "" retrieves those without a schema
     *  procedureNamePattern - a procedure name pattern
     *
     *  Returns:
     *  ResultSet - each row is a procedure description
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     *
     *  See Also:
     *  getSearchStringEscape
     */
    public abstract ResultSet getProcedures(String catalog,
                                             String schemaPattern,
                                             String procedureNamePattern)
                                                        throws SQLException {
    }
    
    
    /**
     *  Get a description of a catalog's stored procedure parameters and result
     *  columns.
     *
     *  Only descriptions matching the schema, procedure and parameter name 
     *  criteria are returned. They are ordered by PROCEDURE_SCHEM and 
     *  PROCEDURE_NAME. Within this, the return value, if any, is first. Next 
     *  are the parameter descriptions in call order. The column descriptions 
     *  follow in column number order.
     *
     *  Each row in the ResultSet is a parameter description or column 
     *  description with the following fields:
     *
     *     1. PROCEDURE_CAT String => procedure catalog (may be null)
     *     2. PROCEDURE_SCHEM String => procedure schema (may be null)
     *     3. PROCEDURE_NAME String => procedure name
     *     4. COLUMN_NAME String => column/parameter name
     *     5. COLUMN_TYPE Short => kind of column/parameter:
     *            * procedureColumnUnknown - nobody knows
     *            * procedureColumnIn - IN parameter
     *            * procedureColumnInOut - INOUT parameter
     *            * procedureColumnOut - OUT parameter
     *            * procedureColumnReturn - procedure return value
     *            * procedureColumnResult - result column in ResultSet
     *     6. DATA_TYPE short => SQL type from java.sql.Types
     *     7. TYPE_NAME String => SQL type name
     *     8. PRECISION int => precision
     *     9. LENGTH int => length in bytes of data
     *    10. SCALE short => scale
     *    11. RADIX short => radix
     *    12. NULLABLE short => can it contain NULL?
     *            * procedureNoNulls - does not allow NULL values
     *            * procedureNullable - allows NULL values
     *            * procedureNullableUnknown - nullability unknown
     *    13. REMARKS String => comment describing parameter/column 
     *
     *  Note: Some databases may not return the column descriptions for a 
     *  procedure. Additional columns beyond REMARKS can be defined by the 
     *  database.
     *
     *  Parameters:
     *  catalog - a catalog name;
     *      "" retrieves those without a catalog;
     *      null means drop catalog name from the selection criteria
     *  schemaPattern - a schema name pattern;
     *      "" retrieves those without a schema
     *  procedureNamePattern - a procedure name pattern
     *  columnNamePattern - a column name pattern
     *
     *  Returns:
     *  ResultSet - each row is a stored procedure parameter or column
     *    description
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     *
     *  See Also:
     *  getSearchStringEscape
     */
    public abstract ResultSet getProcedureColumns(String catalog,
                                                   String schemaPattern,
                                                   String procedureNamePattern,
                                                   String columnNamePattern)
                                                        throws SQLException {
    }
    
    /**
     *  Get a description of tables available in a catalog.
     *
     *  Only table descriptions matching the catalog, schema, table name and
     *  type criteria are returned. They are ordered by TABLE_TYPE,
     *  TABLE_SCHEM and TABLE_NAME.
     *
     *  Each table description has the following columns:
     *
     *     1. TABLE_CAT String => table catalog (may be null)
     *     2. TABLE_SCHEM String => table schema (may be null)
     *     3. TABLE_NAME String => table name
     *     4. TABLE_TYPE String => table type. Typical types are "TABLE",
     *        "VIEW", "SYSTEM TABLE", "GLOBAL TEMPORARY", "LOCAL TEMPORARY",
     *        "ALIAS", "SYNONYM".
     *     5. REMARKS String => explanatory comment on the table 
     *
     *  Note: Some databases may not return information for all tables.
     *
     *  Parameters:
     *  catalog - a catalog name;
     *      "" retrieves those without a catalog;
     *      null means drop catalog name from the selection criteria
     *  schemaPattern - a schema name pattern;
     *      "" retrieves those without a schema
     *  tableNamePattern - a table name pattern
     *  types - a list of table types to include;
     *      null returns all types
     *
     *  Returns:
     *  ResultSet - each row is a table description
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     *
     *  See Also:
     *  getSearchStringEscape
     */
    public abstract ResultSet getTables(String catalog,
                                         String schemaPattern,
                                         String tableNamePattern,
                                         String types[]) throws SQLException {
    }
    
    /**
     *  Get the schema names available in this database. The results are
     *  ordered by schema name.
     *
     *  The schema column is:
     *
     *     1. TABLE_SCHEM String => schema name 
     *
     *  Returns:
     *  ResultSet - each row has a single String column that is a schema name
     *
     *  Throws: SQLException if a database-access error occurs.
     */
    public abstract ResultSet getSchemas() throws SQLException
    
    /**
     *  Get the catalog names available in this database. The results are
     *  ordered by catalog name.
     *
     *  The catalog column is:
     *
     *     1. TABLE_CAT String => catalog name 
     *
     *  Returns:
     *  ResultSet - each row has a single String column that is a catalog name
     *
     *  Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract ResultSet getCatalogs() throws SQLException {
    }
    
    /**
     *  Get the table types available in this database. The results are ordered 
     *  by table type.
     *
     *  The table type is:
     *
     *     1. TABLE_TYPE String => table type. Typical types are "TABLE", 
     *          "VIEW", "SYSTEM TABLE", "GLOBAL TEMPORARY", "LOCAL TEMPORARY",
     *          "ALIAS", "SYNONYM". 
     *
     *  Returns:
     *  ResultSet - each row has a single String column that is a table type
     *
     *  Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract ResultSet getTableTypes() throws SQLException {
    }
    
    /**
     *  Get a description of table columns available in a catalog.
     *
     *  Only column descriptions matching the catalog, schema, table and column
     *  name criteria are returned. They are ordered by TABLE_SCHEM, TABLE_NAME 
     *  and ORDINAL_POSITION.
     *
     *  Each column description has the following columns:
     *
     *     1. TABLE_CAT String => table catalog (may be null)
     *     2. TABLE_SCHEM String => table schema (may be null)
     *     3. TABLE_NAME String => table name
     *     4. COLUMN_NAME String => column name
     *     5. DATA_TYPE short => SQL type from java.sql.Types
     *     6. TYPE_NAME String => Data source dependent type name
     *     7. COLUMN_SIZE int => column size. For char or date types this is the
     *        maximum number of characters, for numeric or decimal types this is
     *        precision.
     *     8. BUFFER_LENGTH is not used.
     *     9. DECIMAL_DIGITS int => the number of fractional digits
     *    10. NUM_PREC_RADIX int => Radix (typically either 10 or 2)
     *    11. NULLABLE int => is NULL allowed?
     *            * columnNoNulls - might not allow NULL values
     *            * columnNullable - definitely allows NULL values
     *            * columnNullableUnknown - nullability unknown
     *    12. REMARKS String => comment describing column (may be null)
     *    13. COLUMN_DEF String => default value (may be null)
     *    14. SQL_DATA_TYPE int => unused
     *    15. SQL_DATETIME_SUB int => unused
     *    16. CHAR_OCTET_LENGTH int => for char types the maximum number of
     *        bytes in the column
     *    17. ORDINAL_POSITION int => index of column in table (starting at 1)
     *    18. IS_NULLABLE String => "NO" means column definitely does not allow
     *        NULL values; "YES" means the column might allow NULL values. An
     *        empty string means nobody knows. 
     *
     *  Parameters:
     *  catalog -
     *    a catalog name;
     *    "" retrieves those without a catalog;
     *    null means drop catalog name from the selection criteria
     *  schemaPattern -
     *    a schema name pattern;
     *    "" retrieves those without a schema
     *  tableNamePattern -
     *    a table name pattern
     *  columnNamePattern -
     *    a column name pattern
     *
     *  Returns:
     *  ResultSet - each row is a column description
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     *
     *  See Also:
     *  getSearchStringEscape
     */
    public abstract ResultSet getColumns(String catalog,
                                          String schemaPattern,
                                          String tableNamePattern,
                                          String columnNamePattern)
                                                        throws SQLException {
    }
    
    /**
     *  Get a description of the access rights for a table's columns.
     *
     *  Only privileges matching the column name criteria are returned. They
     *  are ordered by COLUMN_NAME and PRIVILEGE.
     *
     *  Each privilige description has the following columns:
     *
     *     1. TABLE_CAT String => table catalog (may be null)
     *     2. TABLE_SCHEM String => table schema (may be null)
     *     3. TABLE_NAME String => table name
     *     4. COLUMN_NAME String => column name
     *     5. GRANTOR => grantor of access (may be null)
     *     6. GRANTEE String => grantee of access
     *     7. PRIVILEGE String => name of access (SELECT, INSERT, UPDATE,
     *          REFRENCES, ...)
     *     8. IS_GRANTABLE String => "YES" if grantee is permitted to grant to 
     *          others; "NO" if not; null if unknown 
     *
     *  Parameters:
     *  catalog - a catalog name;
     *    "" retrieves those without a catalog;
     *    null means drop catalog name from the selection criteria
     *  schema - a schema name;
     *    "" retrieves those without a schema
     *  table - a table name
     *  columnNamePattern - a column name pattern
     *
     *  Returns:
     *  ResultSet - each row is a column privilege description
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     *
     *  See Also:
     *  getSearchStringEscape
     */
    public abstract ResultSet getColumnPrivileges(String catalog,
                                                   String schema,
                                                   String table,
                                                   String columnNamePattern)
                                                        throws SQLException {
    }
    
    /**
     *  Get a description of the access rights for each table available in a
     *  catalog. Note that a table privilege applies to one or more columns in
     *  the table. It would be wrong to assume that this priviledge applies to
     *  all columns (this may be true for some systems but is not true for all.)
     *
     *  Only privileges matching the schema and table name criteria are
     *  returned. They are ordered by TABLE_SCHEM, TABLE_NAME, and PRIVILEGE.
     *
     *  Each privilige description has the following columns:
     *
     *     1. TABLE_CAT String => table catalog (may be null)
     *     2. TABLE_SCHEM String => table schema (may be null)
     *     3. TABLE_NAME String => table name
     *     4. GRANTOR => grantor of access (may be null)
     *     5. GRANTEE String => grantee of access
     *     6. PRIVILEGE String => name of access (SELECT, INSERT, UPDATE,
     *        REFRENCES, ...)
     *     7. IS_GRANTABLE String => "YES" if grantee is permitted to grant to
     *         others; "NO" if not; null if unknown 
     *
     *  Parameters:
     *  catalog - a catalog name;
     *    "" retrieves those without a catalog;
     *    null means drop catalog name from the selection criteria
     *  schemaPattern - a schema name pattern;
     *    "" retrieves those without a schema
     *  tableNamePattern - a table name pattern
     *
     *  Returns:
     *  ResultSet - each row is a table privilege description
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     *
     *  See Also:
     *  getSearchStringEscape
     */
    public abstract ResultSet getTablePrivileges(String catalog,
                                                  String schemaPattern,
                                                  String tableNamePattern)
                                                        throws SQLException {
    }
    
    /**
     *  Get a description of a table's optimal set of columns that uniquely
     *  identifies a row. They are ordered by SCOPE.
     *
     *  Each column description has the following columns:
     *
     *     1. SCOPE short => actual scope of result
     *            * bestRowTemporary - very temporary, while using row
     *            * bestRowTransaction - valid for remainder of current
     *                                   transaction
     *            * bestRowSession - valid for remainder of current session
     *     2. COLUMN_NAME String => column name
     *     3. DATA_TYPE short => SQL data type from java.sql.Types
     *     4. TYPE_NAME String => Data source dependent type name
     *     5. COLUMN_SIZE int => precision
     *     6. BUFFER_LENGTH int => not used
     *     7. DECIMAL_DIGITS short => scale
     *     8. PSEUDO_COLUMN short => is this a pseudo column like an Oracle
     *                               ROWID
     *            * bestRowUnknown - may or may not be pseudo column
     *            * bestRowNotPseudo - is NOT a pseudo column
     *           * bestRowPseudo - is a pseudo column
     *
     *  Parameters:
     *  catalog - a catalog name;
     *    "" retrieves those without a catalog;
     *    null means drop catalog name from the selection criteria
     *  schema - a schema name;
     *    "" retrieves those without a schema
     *  table - a table name
     *  scope - the scope of interest; use same values as SCOPE
     *  nullable - include columns that are nullable?
     *
     *  Returns:
     *  ResultSet - each row is a column description
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract ResultSet getBestRowIdentifier(String catalog,
                                                    String schema,
                                                    String table,
                                                    int scope,
                                                    boolean nullable)
                                                        throws SQLException {
    }
    
    /**
     *  Get a description of a table's columns that are automatically updated
     *  when any value in a row is updated. They are unordered.
     *
     *  Each column description has the following columns:
     *
     *     1. SCOPE short => is not used
     *     2. COLUMN_NAME String => column name
     *     3. DATA_TYPE short => SQL data type from java.sql.Types
     *     4. TYPE_NAME String => Data source dependent type name
     *     5. COLUMN_SIZE int => precision
     *     6. BUFFER_LENGTH int => length of column value in bytes
     *     7. DECIMAL_DIGITS short => scale
     *     8. PSEUDO_COLUMN short => is this a pseudo column like an Oracle
     *                               ROWID
     *            * versionColumnUnknown - may or may not be pseudo column
     *            * versionColumnNotPseudo - is NOT a pseudo column
     *            * versionColumnPseudo - is a pseudo column
     *
     *  Parameters:
     *  catalog - a catalog name;
     *    "" retrieves those without a catalog;
     *    null means drop catalog name from the selection criteria
     *  schema - a schema name;
     *    "" retrieves those without a schema
     *  table - a table name
     *
     *  Returns:
     *  ResultSet - each row is a column description
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     */
    public abstract ResultSet getVersionColumns(String catalog,
                                                 String schema,
                                                 String table)
                                                    throws SQLException {
    }
    
    /**
     *  Get a description of a table's primary key columns. They are ordered by
     *  COLUMN_NAME.
     *
     *  Each primary key column description has the following columns:
     *
     *     1. TABLE_CAT String => table catalog (may be null)
     *     2. TABLE_SCHEM String => table schema (may be null)
     *     3. TABLE_NAME String => table name
     *     4. COLUMN_NAME String => column name
     *     5. KEY_SEQ short => sequence number within primary key
     *     6. PK_NAME String => primary key name (may be null) 
     *
     *  Parameters:
     *  catalog - a catalog name;
     *    "" retrieves those without a catalog;
     *    null means drop catalog name from the selection criteria
     *  schema - a schema name pattern;
     *    "" retrieves those without a schema
     *  table - a table name
     *
     *  Returns:
     *  ResultSet - each row is a primary key column description
     *
     *  Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract ResultSet getPrimaryKeys(String catalog,
                                              String schema,
                                              String table) throws SQLException {
    }
    
    /**
     *  Get a description of the primary key columns that are referenced by a
     *  table's foreign key columns (the primary keys imported by a table). 
     *  They are ordered by PKTABLE_CAT, PKTABLE_SCHEM, PKTABLE_NAME, and
     *  KEY_SEQ.
     *
     *  Each primary key column description has the following columns:
     *
     *     1. PKTABLE_CAT String => primary key table catalog being imported
     *          (may be null)
     *     2. PKTABLE_SCHEM String => primary key table schema being imported
     *          (may be null)
     *     3. PKTABLE_NAME String => primary key table name being imported
     *     4. PKCOLUMN_NAME String => primary key column name being imported
     *     5. FKTABLE_CAT String => foreign key table catalog (may be null)
     *     6. FKTABLE_SCHEM String => foreign key table schema (may be null)
     *     7. FKTABLE_NAME String => foreign key table name
     *     8. FKCOLUMN_NAME String => foreign key column name
     *     9. KEY_SEQ short => sequence number within foreign key
     *    10. UPDATE_RULE short => What happens to foreign key when primary is
     *          updated:
     *            * importedNoAction - do not allow update of primary key if it 
     *                                 has been imported
     *            * importedKeyCascade - change imported key to agree with 
     *                                   primary key update
     *            * importedKeySetNull - change imported key to NULL if its
     *                                   primary key has been updated
     *            * importedKeySetDefault - change imported key to default
     *                                      values if its primary key has been
     *                                      updated
     *            * importedKeyRestrict - same as importedKeyNoAction (for ODBC 
     *                                    2.x compatibility)
     *    11. DELETE_RULE short => What happens to the foreign key when primary 
     *          is deleted.
     *            * importedKeyNoAction - do not allow delete of primary key if 
     *                                    it has been imported
     *            * importedKeyCascade - delete rows that import a deleted key
     *            * importedKeySetNull - change imported key to NULL if its 
     *                                   primary key has been deleted
     *            * importedKeyRestrict - same as importedKeyNoAction (for ODBC 
     *                                    2.x compatibility)
     *            * importedKeySetDefault - change imported key to default if
     *                                      its primary key has been deleted
     *    12. FK_NAME String => foreign key name (may be null)
     *    13. PK_NAME String => primary key name (may be null)
     *    14. DEFERRABILITY short => can the evaluation of foreign key
     *                                  constraints be deferred until commit
     *            * importedKeyInitiallyDeferred - see SQL92 for definition
     *            * importedKeyInitiallyImmediate - see SQL92 for definition
     *            * importedKeyNotDeferrable - see SQL92 for definition
     *
     *  Parameters:
     *  catalog - a catalog name;
     *    "" retrieves those without a catalog;
     *    null means drop catalog name from the selection criteria
     *  schema - a schema name pattern;
     *    "" retrieves those without a schema
     *  table - a table name
     *
     *  Returns:
     *  ResultSet - each row is a primary key column description
     *
     *  Throws:
     *  SQLException if a database-access error occurs.
     *
     *  See Also:
     *  getExportedKeys
     */
    public abstract ResultSet getImportedKeys(String catalog,
                                               String schema,
                                               String table)
                                                    throws SQLException {
    }
    
    /**
     *  Get a description of the foreign key columns that reference a table's
     *  primary key columns (the foreign keys exported by a table). They are
     *  ordered by FKTABLE_CAT, FKTABLE_SCHEM, FKTABLE_NAME, and KEY_SEQ.
     *
     *  Each foreign key column description has the following columns:
     *
     *     1. PKTABLE_CAT String => primary key table catalog (may be null)
     *     2. PKTABLE_SCHEM String => primary key table schema (may be null)
     *     3. PKTABLE_NAME String => primary key table name
     *     4. PKCOLUMN_NAME String => primary key column name
     *     5. FKTABLE_CAT String => foreign key table catalog (may be null) being exported (may be null)
     *     6. FKTABLE_SCHEM String => foreign key table schema (may be null) being exported (may be null)
     *     7. FKTABLE_NAME String => foreign key table name being exported
     *     8. FKCOLUMN_NAME String => foreign key column name being exported
     *     9. KEY_SEQ short => sequence number within foreign key
     *    10. UPDATE_RULE short => What happens to foreign key when primary is updated:
     *            * importedNoAction - do not allow update of primary key if it has been imported
     *            * importedKeyCascade - change imported key to agree with primary key update
     *            * importedKeySetNull - change imported key to NULL if its primary key has been updated
     *            * importedKeySetDefault - change imported key to default values if its primary key has been updated
     *      * importedKeyRestrict - same as importedKeyNoAction (for ODBC 2.x compatibility)
     *    11. DELETE_RULE short => What happens to the foreign key when primary is deleted.
     *            * importedKeyNoAction - do not allow delete of primary key if it has been imported
     *            * importedKeyCascade - delete rows that import a deleted key
     *            * importedKeySetNull - change imported key to NULL if its primary key has been deleted
     *            * importedKeyRestrict - same as importedKeyNoAction (for ODBC 2.x compatibility)
     *      * importedKeySetDefault - change imported key to default if its primary key has been deleted
     *    12. FK_NAME String => foreign key name (may be null)
     *    13. PK_NAME String => primary key name (may be null)
     *    14. DEFERRABILITY short => can the evaluation of foreign key constraints be deferred until commit
     *            * importedKeyInitiallyDeferred - see SQL92 for definition
     *            * importedKeyInitiallyImmediate - see SQL92 for definition
     *      * importedKeyNotDeferrable - see SQL92 for definition
    
     *  Parameters:
     *  catalog - a catalog name; "" retrieves those without a catalog; null means drop catalog name from the selection criteria schema - a schema name pattern; "" retrieves those without a schema table - a table name Returns:
     *  ResultSet - each row is a foreign key column description Throws: SQLException
     *  if a database-access error occurs. See Also:
     *  getImportedKeys
     */
    public abstract ResultSet getExportedKeys(String catalog,
                                               String schema,
                                               String table) throws SQLException {
    }
    
    /**
     *  Get a description of the foreign key columns in the foreign key table that reference the primary key columns of the primary key table (describe how one table imports another's key.) This should normally return a single foreign key/primary key pair (most tables only import a foreign key from a table once.) They are ordered by FKTABLE_CAT, FKTABLE_SCHEM, FKTABLE_NAME, and KEY_SEQ.
    
     *  Each foreign key column description has the following columns:
    
     *     1. PKTABLE_CAT String => primary key table catalog (may be null)
     *     2. PKTABLE_SCHEM String => primary key table schema (may be null)
     *     3. PKTABLE_NAME String => primary key table name
     *     4. PKCOLUMN_NAME String => primary key column name
     *     5. FKTABLE_CAT String => foreign key table catalog (may be null) being exported (may be null)
     *     6. FKTABLE_SCHEM String => foreign key table schema (may be null) being exported (may be null)
     *     7. FKTABLE_NAME String => foreign key table name being exported
     *     8. FKCOLUMN_NAME String => foreign key column name being exported
     *     9. KEY_SEQ short => sequence number within foreign key
     *    10. UPDATE_RULE short => What happens to foreign key when primary is updated:
     *            * importedNoAction - do not allow update of primary key if it has been imported
     *            * importedKeyCascade - change imported key to agree with primary key update
     *            * importedKeySetNull - change imported key to NULL if its primary key has been updated
     *            * importedKeySetDefault - change imported key to default values if its primary key has been updated
     *      * importedKeyRestrict - same as importedKeyNoAction (for ODBC 2.x compatibility)
     *    11. DELETE_RULE short => What happens to the foreign key when primary is deleted.
     *            * importedKeyNoAction - do not allow delete of primary key if it has been imported
     *            * importedKeyCascade - delete rows that import a deleted key
     *            * importedKeySetNull - change imported key to NULL if its primary key has been deleted
     *            * importedKeyRestrict - same as importedKeyNoAction (for ODBC 2.x compatibility)
     *      * importedKeySetDefault - change imported key to default if its primary key has been deleted
     *    12. FK_NAME String => foreign key name (may be null)
     *    13. PK_NAME String => primary key name (may be null)
     *    14. DEFERRABILITY short => can the evaluation of foreign key constraints be deferred until commit
     *            * importedKeyInitiallyDeferred - see SQL92 for definition
     *            * importedKeyInitiallyImmediate - see SQL92 for definition
     *      * importedKeyNotDeferrable - see SQL92 for definition
    
     *  Parameters:
     *  primaryCatalog - a catalog name; "" retrieves those without a catalog; null means drop catalog name from the selection criteria primarySchema - a schema name pattern; "" retrieves those without a schema primaryTable - the table name that exports the key foreignCatalog - a catalog name; "" retrieves those without a catalog; null means drop catalog name from the selection criteria foreignSchema - a schema name pattern; "" retrieves those without a schema foreignTable - the table name that imports the key Returns:
     *  ResultSet - each row is a foreign key column description Throws: SQLException
     *  if a database-access error occurs. See Also:
     *  getImportedKeys
     */
    public abstract ResultSet getCrossReference(String primaryCatalog,
                                                 String primarySchema,
                                                 String primaryTable,
                                                 String foreignCatalog,
                                                 String foreignSchema,
                                                 String foreignTable)
                                                        throws SQLException {
    }
    
    /**
     *  Get a description of all the standard SQL types supported by this database. They are ordered by DATA_TYPE and then by how closely the data type maps to the corresponding JDBC SQL type.
    
     *  Each type description has the following columns:
    
     *     1. TYPE_NAME String => Type name
     *     2. DATA_TYPE short => SQL data type from java.sql.Types
     *     3. PRECISION int => maximum precision
     *     4. LITERAL_PREFIX String => prefix used to quote a literal (may be null)
     *     5. LITERAL_SUFFIX String => suffix used to quote a literal (may be null)
     *     6. CREATE_PARAMS String => parameters used in creating the type (may be null)
     *     7. NULLABLE short => can you use NULL for this type?
     *            * typeNoNulls - does not allow NULL values
     *            * typeNullable - allows NULL values
     *      * typeNullableUnknown - nullability unknown
     *     8. CASE_SENSITIVE boolean=> is it case sensitive?
     *     9. SEARCHABLE short => can you use "WHERE" based on this type:
     *            * typePredNone - No support
     *            * typePredChar - Only supported with WHERE .. LIKE
     *            * typePredBasic - Supported except for WHERE .. LIKE
     *      * typeSearchable - Supported for all WHERE ..
     *    10. UNSIGNED_ATTRIBUTE boolean => is it unsigned?
     *    11. FIXED_PREC_SCALE boolean => can it be a money value?
     *    12. AUTO_INCREMENT boolean => can it be used for an auto-increment value?
     *    13. LOCAL_TYPE_NAME String => localized version of type name (may be null)
     *    14. MINIMUM_SCALE short => minimum scale supported
     *    15. MAXIMUM_SCALE short => maximum scale supported
     *    16. SQL_DATA_TYPE int => unused
     *    17. SQL_DATETIME_SUB int => unused
     *    18. NUM_PREC_RADIX int => usually 2 or 10 
    
     *  Returns:
     *  ResultSet - each row is a SQL type description Throws: SQLException
     *  if a database-access error occurs.
     */
    public abstract ResultSet getTypeInfo() throws SQLException {
    }
    
    /**
     *  Get a description of a table's indices and statistics. They are ordered by NON_UNIQUE, TYPE, INDEX_NAME, and ORDINAL_POSITION.
    
     *  Each index column description has the following columns:
    
     *     1. TABLE_CAT String => table catalog (may be null)
     *     2. TABLE_SCHEM String => table schema (may be null)
     *     3. TABLE_NAME String => table name
     *     4. NON_UNIQUE boolean => Can index values be non-unique? false when TYPE is tableIndexStatistic
     *     5. INDEX_QUALIFIER String => index catalog (may be null); null when TYPE is tableIndexStatistic
     *     6. INDEX_NAME String => index name; null when TYPE is tableIndexStatistic
     *     7. TYPE short => index type:
     *            * tableIndexStatistic - this identifies table statistics that are returned in conjuction with a table's index descriptions
     *            * tableIndexClustered - this is a clustered index
     *            * tableIndexHashed - this is a hashed index
     *      * tableIndexOther - this is some other style of index
     *     8. ORDINAL_POSITION short => column sequence number within index; zero when TYPE is tableIndexStatistic
     *     9. COLUMN_NAME String => column name; null when TYPE is tableIndexStatistic
     *    10. ASC_OR_DESC String => column sort sequence, "A" => ascending, "D" => descending, may be null if sort sequence is not supported; null when TYPE is tableIndexStatistic
     *    11. CARDINALITY int => When TYPE is tableIndexStatistic, then this is the number of rows in the table; otherwise, it is the number of unique values in the index.
     *    12. PAGES int => When TYPE is tableIndexStatisic then this is the number of pages used for the table, otherwise it is the number of pages used for the current index.
     *    13. FILTER_CONDITION String => Filter condition, if any. (may be null) 
    
     *  Parameters:
     *  catalog - a catalog name; "" retrieves those without a catalog; null means drop catalog name from the selection criteria schema - a schema name pattern; "" retrieves those without a schema table - a table name unique - when true, return only indices for unique values; when false, return indices regardless of whether unique or not approximate - when true, result is allowed to reflect approximate or out of data values; when false, results are requested to be accurate Returns:
     *  ResultSet - each row is an index column description Throws: SQLException
     *  if a database-access error occurs.
     */  
    public abstract ResultSet getIndexInfo(String catalog,
                                            String schema,
                                            String table,
                                            boolean unique,
                                            boolean approximate)
                                                throws SQLException {
    }
    
};
