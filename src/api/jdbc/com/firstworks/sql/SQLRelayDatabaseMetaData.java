package com.firstworks.sql;
	
import java.sql.*;
import java.util.regex.*;

import com.firstworks.sqlrelay.*;
	
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
		boolean	result=false;
		debugPrintln("  result: "+result);
		return result;
	}

	public boolean 	allTablesAreSelectable() throws SQLException {
		debugFunction();
		// Retrieves whether the current user can use all the tables
		// returned by the method getTables in a SELECT statement.
		boolean	result=false;
		debugPrintln("  result: "+result);
		return result;
	}

	public boolean 	autoCommitFailureClosesAllResultSets()
							throws SQLException {
		debugFunction();
		// Retrieves whether a SQLException while autoCommit is true
		// inidcates that all open ResultSets are closed, even ones
		// that are holdable.
		// FIXME: no idea if this is true or not
		boolean	result=false;
		debugPrintln("  result: "+result);
		return result;
	}

	public boolean 	dataDefinitionCausesTransactionCommit()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	result=false;
		debugPrintln("  result: "+result);
		return result;
	}

	public boolean 	dataDefinitionIgnoredInTransactions()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	result=false;
		debugPrintln("  result: "+result);
		return result;
	}

	public boolean 	deletesAreDetected(int type) throws SQLException {
		debugFunction();
		// SQL Relay doesn't currenlty support ResultSet.RowDelete
		boolean	result=false;
		debugPrintln("  result: "+result);
		return result;
	}

	public boolean 	doesMaxRowSizeIncludeBlobs() throws SQLException {
		debugFunction();
		boolean	result=false;
		debugPrintln("  result: "+result);
		return result;
	}

	public boolean 	generatedKeyAlwaysReturned() throws SQLException {
		debugFunction();
		boolean	result=true;
		debugPrintln("  result: "+result);
		return result;
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

		SQLRelayResultSet	resultset=null;
		SQLRelayStatement	stmt=(SQLRelayStatement)
						connection.createStatement();
		SQLRCursor		sqlrcur=stmt.getSQLRCursor();

		boolean	result=sqlrcur.getDatabaseListWithFormat(null,3);

		debugPrintln("  result: "+result);

		if (result) {

			debugPrintln("  colcount: "+sqlrcur.colCount());

			if (sqlrcur.colCount()>0) {
				resultset=new SQLRelayResultSet();
				resultset.setStatement(stmt);
				resultset.setSQLRCursor(sqlrcur);
			}
		} else {
			throwErrorMessageException(sqlrcur);
		}
		
		return resultset;
	}

	public String 	getCatalogSeparator() throws SQLException {
		debugFunction();
		// FIXME: oracle uses @
		String	separator=".";
		debugPrintln("  separator: "+separator);
		return separator;
	}

	public String 	getCatalogTerm() throws SQLException {
		debugFunction();
		// FIXME: I think SQL Server uses catalog, maybe sybase
		String	term="database";
		debugPrintln("  term: "+term);
		return term;
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

		debugPrintln("  catalog: "+catalog);
		debugPrintln("  schema pattern: "+schemaPattern);
		debugPrintln("  table name pattern: "+tableNamePattern);
		debugPrintln("  column name pattern: "+columnNamePattern);

		SQLRelayResultSet	resultset=null;
		SQLRelayStatement	stmt=(SQLRelayStatement)
						connection.createStatement();
		SQLRCursor		sqlrcur=stmt.getSQLRCursor();

		if (schemaPattern!=null && !schemaPattern.equals("")) {
			// FIXME: not all db's use . and not all db's
			// use schema.table...
			tableNamePattern=schemaPattern+"."+tableNamePattern;
		}

		boolean	result=sqlrcur.getColumnListWithFormat(
						tableNamePattern,
						columnNamePattern,3);

		debugPrintln("  result: "+result);

		if (result) {

			debugPrintln("  colcount: "+sqlrcur.colCount());

			if (sqlrcur.colCount()>0) {
				resultset=new SQLRelayResultSet();
				resultset.setStatement(stmt);
				resultset.setSQLRCursor(sqlrcur);
			}
		} else {
			throwErrorMessageException(sqlrcur);
		}
		
		return resultset;
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
		int	majorversion=getDatabaseVersion(true);
		debugPrintln("  major version: "+majorversion);
		return majorversion;
	}

	public int 	getDatabaseMinorVersion() throws SQLException {
		debugFunction();
		int	minorversion=getDatabaseVersion(false);
		debugPrintln("  minor version: "+minorversion);
		return minorversion;
	}

	private int	getDatabaseVersion(boolean major) {
		// FIXME: cache/fetch dbVersion
		Matcher	matcher=Pattern.compile("[0-9]*\\.[0-9]*").
			matcher(connection.getSQLRConnection().dbVersion());
		if (matcher.find()) {
			String[]	parts=matcher.group().split("\\.");
			if (parts!=null && parts.length>((major)?0:1)) {
				return Integer.parseInt(parts[(major)?0:1]);
			}
		}
		return -1;
	}

	public String 	getDatabaseProductName() throws SQLException {
		debugFunction();
		// FIXME: cache/fetch identify
		String	id=connection.getSQLRConnection().identify();
		debugPrintln("  id: "+id);
		return id;
	}

	public String 	getDatabaseProductVersion() throws SQLException {
		debugFunction();
		// FIXME: cache/fetch dbVersion
		String	productversion=
				connection.getSQLRConnection().dbVersion();
		debugPrintln("  product version: "+productversion);
		return productversion;
	}

	public int 	getDefaultTransactionIsolation() throws SQLException {
		debugFunction();
		int	isolation=(getDatabaseProductName().equals("mysql"))?
					Connection.TRANSACTION_REPEATABLE_READ:
					Connection.TRANSACTION_READ_COMMITTED;
		debugPrintln("  isolation: "+isolation);
		return isolation;
	}

	public int 	getDriverMajorVersion() {
		debugFunction();
		int		majorversion=-1;
		String[]	parts=connection.
					getSQLRConnection().
					clientVersion().split(".");
		if (parts!=null && parts.length>0) {
			majorversion=Integer.parseInt(parts[0]);
		}
		debugPrintln("  major version: "+majorversion);
		return majorversion;
	}

	public int 	getDriverMinorVersion() {
		debugFunction();
		int		minorversion=-1;
		String[]	parts=connection.
					getSQLRConnection().
					clientVersion().split(".");
		if (parts!=null && parts.length>1) {
			minorversion=Integer.parseInt(parts[1]);
		}
		debugPrintln("  minor version: "+minorversion);
		return minorversion;
	}

	public String 	getDriverName() throws SQLException {
		debugFunction();
		String	drivername="sqlrelay";
		debugPrintln("  driver name: "+drivername);
		return drivername;
	}

	public String 	getDriverVersion() throws SQLException {
		debugFunction();
		String	driverversion=connection.
					getSQLRConnection().
					clientVersion();
		debugPrintln("  driver version: "+driverversion);
		return driverversion;
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
		String	extranamechars="#@";
		debugPrintln("  extra name characters: "+extranamechars);
		return extranamechars;
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
		// * sqlserver uses braces
		String	identifierquotestring=
			(getDatabaseProductName().equals("mysql"))?"`":"\"";
		debugPrintln("  identifier quote string: "+
					identifierquotestring);
		return identifierquotestring;
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
		// FIXME: get this from ???
		int	jdbcmajorversion=4;
		debugPrintln("  jdbc major version: "+jdbcmajorversion);
		return jdbcmajorversion;
	}

	public int 	getJDBCMinorVersion() throws SQLException {
		debugFunction();
		// FIXME: get this from ???
		int	jdbcminorversion=3;
		debugPrintln("  jdbc minor version: "+jdbcminorversion);
		return jdbcminorversion;
	}

	public int 	getMaxBinaryLiteralLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxbinaryliterallength=0;
		debugPrintln("  max binary literal length: "+
						maxbinaryliterallength);
		return maxbinaryliterallength;
	}

	public int 	getMaxCatalogNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxcatalognamelength=0;
		debugPrintln("  max catalog name length: "+
						maxcatalognamelength);
		return maxcatalognamelength;
	}

	public int 	getMaxCharLiteralLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxcharliterallength=0;
		debugPrintln("  max char literal length: "+
						maxcharliterallength);
		return maxcharliterallength;
	}

	public int 	getMaxColumnNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxcolumnnamelength=0;
		debugPrintln("  max column name length: "+
						maxcolumnnamelength);
		return maxcolumnnamelength;
	}

	public int 	getMaxColumnsInGroupBy() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxcolumnsingroup=0;
		debugPrintln("  max columns in group: "+maxcolumnsingroup);
		return maxcolumnsingroup;
	}

	public int 	getMaxColumnsInIndex() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxcolumnsinindex=0;
		debugPrintln("  max columns in index: "+maxcolumnsinindex);
		return maxcolumnsinindex;
	}

	public int 	getMaxColumnsInOrderBy() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxcolumnsinorderby=0;
		debugPrintln("  max columns in order by: "+maxcolumnsinorderby);
		return maxcolumnsinorderby;
	}

	public int 	getMaxColumnsInSelect() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxcolumnsinselect=0;
		debugPrintln("  max columns in select: "+maxcolumnsinselect);
		return maxcolumnsinselect;
	}

	public int 	getMaxColumnsInTable() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxcolumnsintable=0;
		debugPrintln("  max columns in table: "+maxcolumnsintable);
		return maxcolumnsintable;
	}

	public int 	getMaxConnections() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxconnections=0;
		debugPrintln("  max connections: "+maxconnections);
		return maxconnections;
	}

	public int 	getMaxCursorNameLength() throws SQLException {
		debugFunction();
		int	maxcursornamelength=0;
		debugPrintln("  max cursor name length: "+
						maxcursornamelength);
		return maxcursornamelength;
	}

	public int 	getMaxIndexLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxindexlength=0;
		debugPrintln("  max index length: "+maxindexlength);
		return maxindexlength;
	}

	public int 	getMaxProcedureNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxprocedurenamelength=0;
		debugPrintln("  max procedure name length: "+
						maxprocedurenamelength);
		return maxprocedurenamelength;
	}

	public int 	getMaxRowSize() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxrowsize=0;
		debugPrintln("  max row size: "+maxrowsize);
		return maxrowsize;
	}

	public int 	getMaxSchemaNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxschemanamelength=0;
		debugPrintln("  max schema name length: "+maxschemanamelength);
		return maxschemanamelength;
	}

	public int 	getMaxStatementLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxstatementlength=0;
		debugPrintln("  max statement length: "+maxstatementlength);
		return maxstatementlength;
	}

	public int 	getMaxStatements() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxstatements=0;
		debugPrintln("  max statements: "+maxstatements);
		return maxstatements;
	}

	public int 	getMaxTableNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxtablenamelength=0;
		debugPrintln("  max table name length: "+maxtablenamelength);
		return maxtablenamelength;
	}

	public int 	getMaxTablesInSelect() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxtablesinselect=0;
		debugPrintln("  max tables in select: "+maxtablesinselect);
		return maxtablesinselect;
	}

	public int 	getMaxUserNameLength() throws SQLException {
		debugFunction();
		// FIXME: db-specific (0 means no limit or unknown)
		int	maxusernamelength=0;
		debugPrintln("  max user name length: "+maxusernamelength);
		return maxusernamelength;
	}

	public String 	getNumericFunctions() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		String	numericfunctions=null;
		debugPrintln("  numeric functions: "+numericfunctions);
		return numericfunctions;
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
		String	procedureterm="procedure";
		debugPrintln("  procedure term: "+procedureterm);
		return procedureterm;
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
		int	resultsetholdability=ResultSet.CLOSE_CURSORS_AT_COMMIT;
		debugPrintln("  result set holdability: "+resultsetholdability);
		return resultsetholdability;
	}

	public RowIdLifetime 	getRowIdLifetime() throws SQLException {
		debugFunction();
		// FIXME: some dbs do support rowid
		RowIdLifetime	rowidlifetime=RowIdLifetime.ROWID_UNSUPPORTED;
		debugPrint("  rowid lifetime: ");
		switch (rowidlifetime) {
			case ROWID_UNSUPPORTED:
				debugPrint("ROWID_UNSUPPORTED");
				break;
			case ROWID_VALID_OTHER:
				debugPrint("ROWID_VALID_OTHER");
				break;
			case ROWID_VALID_TRANSACTION:
				debugPrint("ROWID_VALID_TRANSACTION");
				break;
			case ROWID_VALID_SESSION:
				debugPrint("ROWID_VALID_SESSION");
				break;
			case ROWID_VALID_FOREVER:
				debugPrint("ROWID_VALID_FOREVER");
				break;
		}
		return rowidlifetime;
	}

	public ResultSet 	getSchemas() throws SQLException {
		debugFunction();
		return getSchemas(null,null);
	}

	public ResultSet 	getSchemas(String catalog,
						String schemaPattern)
						throws SQLException {
		debugFunction();

		debugPrintln("  catalog: "+catalog);
		debugPrintln("  schema pattern: "+schemaPattern);

		SQLRelayResultSet	resultset=null;
		SQLRelayStatement	stmt=(SQLRelayStatement)
						connection.createStatement();
		SQLRCursor		sqlrcur=stmt.getSQLRCursor();

		boolean	result=sqlrcur.getSchemaListWithFormat(schemaPattern,3);

		debugPrintln("  result: "+result);

		if (result) {

			debugPrintln("  colcount: "+sqlrcur.colCount());

			if (sqlrcur.colCount()>0) {
				resultset=new SQLRelayResultSet();
				resultset.setStatement(stmt);
				resultset.setSQLRCursor(sqlrcur);
			}
		} else {
			throwErrorMessageException(sqlrcur);
		}
		
		return resultset;
	}

	public String 	getSchemaTerm() throws SQLException {
		debugFunction();
		String	schematerm="schema";
		debugPrintln("  schema term: "+schematerm);
		return schematerm;
	}

	public String 	getSearchStringEscape() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		String	searchstringescape="\\";
		debugPrintln("  search string escape: "+searchstringescape);
		return searchstringescape;
	}

	public String 	getSQLKeywords() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		String	sqlkeywords=null;
		debugPrintln("  sql keywords: "+sqlkeywords);
		return sqlkeywords;
	}

	public int 	getSQLStateType() throws SQLException {
		debugFunction();
		// FIXME: no idea
		int	sqlstatetype=sqlStateSQL;
		debugPrintln("  sql state type: "+sqlstatetype);
		return sqlstatetype;
	}

	public String 	getStringFunctions() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		String	stringfunctions=null;
		debugPrintln("  string functions: "+stringfunctions);
		return stringfunctions;
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
		String	systemfunctions=null;
		debugPrintln("  system functions: "+systemfunctions);
		return systemfunctions;
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

		String	wild=null;

		// If tblname was empty or %, then leave "wild" NULL.
		// Otherwise concatenate catalog/schema's until it's in one
		// of the following formats:
		// * table
		// * schema.table
		// * catalog.schema.table
		// If tblname already contains a . then just use it as-is.
		if (tableNamePattern.contains(".")) {

			StringBuilder	wildstr;
			if (catalog==null || catalog.equals("")) {
				wildstr.append(catalog).append('.');
			}
			if (schemaPattern==null || schemaPattern.equals("")) {
				wildstr.append(schemaPattern).append('.');
			} else if (wildstr.length()) {
				wildstr.append("%.");
			}
			if (!charstring::isNullOrEmpty(schname)) {
				wildstr.append(tblname);
			} else {
				wildstr.append('%');
			}
			delete[] tblname;
			tblname=wildstr.detachString();
		}
		wild=tblname;

		SQLRelayResultSet	resultset=null;
		SQLRelayStatement	stmt=(SQLRelayStatement)
						connection.createStatement();
		SQLRCursor		sqlrcur=stmt.getSQLRCursor();

		debugPrint("  types: ");
		int	objecttypes=0;
		if (types==null) {
			debugPrintln("null");
			objecttypes=1|2|3|4;
		} else {
			for (String type: types) {
				debugPrint(type+",");
				if (type.equals("TABLE") ||
					type.equals("SYSTEM TABLE") ||
					type.equals("GLOBAL TEMPORARY") ||
					type.equals("LOCAL TEMPORARY")) {
					objecttypes|=1;
				} else if (type.equals("VIEW")) {
					objecttypes|=2;
				} else if (type.equals("ALIAS")) {
					objecttypes|=3;
				} else if (type.equals("SYNONYM")) {
					objecttypes|=4;
				}
			}
			debugPrintln("");
		}

		boolean	result=sqlrcur.getTableListWithFormat(
						tableNamePattern,3,objecttypes);

		debugPrintln("  result: "+result);

		if (result) {

			debugPrintln("  colcount: "+sqlrcur.colCount());

			if (sqlrcur.colCount()>0) {
				resultset=new SQLRelayResultSet();
				resultset.setStatement(stmt);
				resultset.setSQLRCursor(sqlrcur);
			}
		} else {
			throwErrorMessageException(sqlrcur);
		}
		
		return resultset;
	}

	public ResultSet 	getTableTypes() throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		return null;
	}

	public String 	getTimeDateFunctions() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		String	timedatefunctions=null;
		debugPrintln("  timedate functions: "+timedatefunctions);
		return timedatefunctions;
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

		debugPrintln("  url: "+url);

		return url;
	}

	public String 	getUserName() throws SQLException {
		debugFunction();
		String	username=connection.getUser();
		debugPrintln("  user name: "+username);
		return username;
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
		boolean	insertsaredetected=false;
		debugPrintln("  type: "+type);
		debugPrintln("  inserts are detected: "+insertsaredetected);
		return insertsaredetected;
	}

	public boolean 	isCatalogAtStart() throws SQLException {
		debugFunction();
		// FIXME: not in oracle
		boolean	iscatalogatstart=true;
		debugPrintln("  is catalog at start: "+iscatalogatstart);
		return iscatalogatstart;
	}

	public boolean 	isReadOnly() throws SQLException {
		debugFunction();
		// FIXME: implement this somehow
		boolean	isreadonly=false;
		debugPrintln("  is read only: "+isreadonly);
		return isreadonly;
	}

	public boolean 	locatorsUpdateCopy() throws SQLException {
		debugFunction();
		// FIXME: no idea, probably db-specific
		boolean	locatorsupdatecopy=false;
		debugPrintln("  locators update copy: "+locatorsupdatecopy);
		return locatorsupdatecopy;
	}

	public boolean 	nullPlusNonNullIsNull() throws SQLException {
		debugFunction();
		// FIXME: generally true, but probably db-specific
		boolean	nullplusnonnullisnull=true;
		debugPrintln("  null plus non null is null: "+
						nullplusnonnullisnull);
		return nullplusnonnullisnull;
	}

	public boolean 	nullsAreSortedAtEnd() throws SQLException {
		debugFunction();
		// FIXME: generally true, but probably db-specific
		boolean	nullsaresortedatend=true;
		debugPrintln("  nulls are sorted at end: "+
						nullsaresortedatend);
		return nullsaresortedatend;
	}

	public boolean 	nullsAreSortedAtStart() throws SQLException {
		debugFunction();
		// FIXME: generally false, but probably db-specific
		boolean	nullsaresortedatstart=false;
		debugPrintln("  nulls are sorted at start: "+
						nullsaresortedatstart);
		return nullsaresortedatstart;
	}

	public boolean 	nullsAreSortedHigh() throws SQLException {
		debugFunction();
		// FIXME: generally true, but probably db-specific
		boolean	nullsaresortedhigh=true;
		debugPrintln("  nulls are sorted high: "+
						nullsaresortedhigh);
		return nullsaresortedhigh;
	}

	public boolean 	nullsAreSortedLow() throws SQLException {
		debugFunction();
		// FIXME: generally false, but probably db-specific
		boolean	nullsaresortedlow=false;
		debugPrintln("  nulls are sorted low: "+
						nullsaresortedlow);
		return nullsaresortedlow;
	}

	public boolean 	othersDeletesAreVisible(int type) throws SQLException {
		debugFunction();
		boolean	othersdeletesarevisible=false;
		debugPrintln("  others deletes are visible: "+
						othersdeletesarevisible);
		return othersdeletesarevisible;
	}

	public boolean 	othersInsertsAreVisible(int type) throws SQLException {
		debugFunction();
		boolean	othersinsertssarevisible=false;
		debugPrintln("  others inserts are visible: "+
						othersinsertssarevisible);
		return othersinsertssarevisible;
	}

	public boolean 	othersUpdatesAreVisible(int type) throws SQLException {
		debugFunction();
		boolean	othersupdatessarevisible=false;
		debugPrintln("  others updates are visible: "+
						othersupdatessarevisible);
		return othersupdatessarevisible;
	}

	public boolean 	ownDeletesAreVisible(int type) throws SQLException {
		debugFunction();
		boolean	owndeletesarevisible=false;
		debugPrintln("  own deletes are visible: "+
						owndeletesarevisible);
		return owndeletesarevisible;
	}

	public boolean 	ownInsertsAreVisible(int type) throws SQLException {
		debugFunction();
		boolean	owninsertsarevisible=false;
		debugPrintln("  own inserts are visible: "+
						owninsertsarevisible);
		return owninsertsarevisible;
	}

	public boolean 	ownUpdatesAreVisible(int type) throws SQLException {
		debugFunction();
		boolean	ownupdatesarevisible=false;
		debugPrintln("  own updates are visible: "+
						ownupdatesarevisible);
		return ownupdatesarevisible;
	}

	public boolean 	storesLowerCaseIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific but generally false
		// oracle stores upper case identifiers
		// other db's store mixed case identifiers
		boolean	storeslowercaseidentifiers=false;
		debugPrintln("  stores lower case identifiers: "+
						storeslowercaseidentifiers);
		return storeslowercaseidentifiers;
	}

	public boolean 	storesLowerCaseQuotedIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	storeslowercasequotedidentifiers=false;
		debugPrintln("  stores lower case quoted identifiers: "+
					storeslowercasequotedidentifiers);
		return storeslowercasequotedidentifiers;
	}

	public boolean 	storesMixedCaseIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: generally true, but db-specific, false for oracle
		boolean	storesmixedcaseidentifiers=true;
		debugPrintln("  stores mixed case identifiers: "+
						storesmixedcaseidentifiers);
		return storesmixedcaseidentifiers;
	}

	public boolean 	storesMixedCaseQuotedIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	storesmixedcasequotedidentifiers=true;
		debugPrintln("  stores mixed case quoted identifiers: "+
					storesmixedcasequotedidentifiers);
		return storesmixedcasequotedidentifiers;
	}

	public boolean 	storesUpperCaseIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific but generally false
		// oracle stores upper case identifiers
		// other db's store mixed case identifiers
		boolean	storesuppercaseidentifiers=false;
		debugPrintln("  stores upper case identifiers: "+
						storesuppercaseidentifiers);
		return storesuppercaseidentifiers;
	}

	public boolean 	storesUpperCaseQuotedIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	storesuppercasequotedidentifiers=true;
		debugPrintln("  stores upper case quoted identifiers: "+
					storesuppercasequotedidentifiers);
		return storesuppercasequotedidentifiers;
	}

	public boolean 	supportsAlterTableWithAddColumn() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsaltertablewithaddcommand=true;
		debugPrintln("  supports alter table with add command: "+
					supportsaltertablewithaddcommand);
		return supportsaltertablewithaddcommand;
	}

	public boolean 	supportsAlterTableWithDropColumn() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsaltertablewithdropcommand=true;
		debugPrintln("  supports alter table with drop command: "+
					supportsaltertablewithdropcommand);
		return supportsaltertablewithdropcommand;
	}

	public boolean 	supportsANSI92EntryLevelSQL() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsansi92entrylevelsql=true;
		debugPrintln("  supports ansi92 entry level sql: "+
						supportsansi92entrylevelsql);
		return supportsansi92entrylevelsql;
	}

	public boolean 	supportsANSI92FullSQL() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsansi92fullsql=true;
		debugPrintln("  supports ansi92 full sql: "+
						supportsansi92fullsql);
		return supportsansi92fullsql;
	}

	public boolean 	supportsANSI92IntermediateSQL() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsansi92intermediatesql=true;
		debugPrintln("  supports ansi92 intermediate sql: "+
						supportsansi92intermediatesql);
		return supportsansi92intermediatesql;
	}

	public boolean 	supportsBatchUpdates() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsbatchupdates=false;
		debugPrintln("  supports batch updates: "+supportsbatchupdates);
		return supportsbatchupdates;
	}

	public boolean 	supportsCatalogsInDataManipulation()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportscatalogsindatamanipulation=true;
		debugPrintln("  supports catalogs in data manipulations: "+
					supportscatalogsindatamanipulation);
		return supportscatalogsindatamanipulation;
	}

	public boolean 	supportsCatalogsInIndexDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportscatalogsinindexdefinitions=true;
		debugPrintln("  supports catalogs in index definitions: "+
					supportscatalogsinindexdefinitions);
		return supportscatalogsinindexdefinitions;
	}

	public boolean 	supportsCatalogsInPrivilegeDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportscatalogsinprivilegedefinitions=true;
		debugPrintln("  supports catalogs in privilege definitions: "+
					supportscatalogsinprivilegedefinitions);
		return supportscatalogsinprivilegedefinitions;
	}

	public boolean 	supportsCatalogsInProcedureCalls()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportscatalogsinprocedurecalls=true;
		debugPrintln("  supports catalogs in procedure calls: "+
					supportscatalogsinprocedurecalls);
		return supportscatalogsinprocedurecalls;
	}

	public boolean 	supportsCatalogsInTableDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportscatalogsintabledefinitions=true;
		debugPrintln("  supports catalogs in table definitions: "+
					supportscatalogsintabledefinitions);
		return supportscatalogsintabledefinitions;
	}

	public boolean 	supportsColumnAliasing() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportscolumnaliasing=true;
		debugPrintln("  supports column aliasing: "+
					supportscolumnaliasing);
		return supportscolumnaliasing;
	}

	public boolean 	supportsConvert() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsconvert=true;
		debugPrintln("  supports convert: "+supportsconvert);
		return supportsconvert;
	}

	public boolean 	supportsConvert(int fromType, int toType)
						throws SQLException {
		debugFunction();
		// FIXME: db-and-type-specific
		boolean	supportsconvert=true;
		debugPrintln("  from type: "+fromType);
		debugPrintln("  to type: "+toType);
		debugPrintln("  supports convert: "+supportsconvert);
		return supportsconvert;
	}

	public boolean 	supportsCoreSQLGrammar() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportscoresqlgrammar=true;
		debugPrintln("  supports core sql grammar: "+
						supportscoresqlgrammar);
		return supportscoresqlgrammar;
	}

	public boolean 	supportsCorrelatedSubqueries() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportscorrelatedsubqueries=true;
		debugPrintln("  supports correlated subqueries: "+
						supportscorrelatedsubqueries);
		return supportscorrelatedsubqueries;
	}

	public boolean 	supportsDataDefinitionAndDataManipulationTransactions()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	sddadmt=true;
		debugPrintln("  supports data definition "+
			"and data manipulation transactions: "+sddadmt);
		return sddadmt;
	}

	public boolean 	supportsDataManipulationTransactionsOnly()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	sdmto=false;
		debugPrintln("  supports data manipulation "+
				"transactions only: "+sdmto);
		return sdmto;
	}

	public boolean 	supportsDifferentTableCorrelationNames()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	sdtcn=true;
		debugPrintln("  supports different table "+
				"correlation names: "+sdtcn);
		return sdtcn;
	}

	public boolean 	supportsExpressionsInOrderBy() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsexpressionsinorderby=true;
		debugPrintln("  supports expressions in order by: "+
						supportsexpressionsinorderby);
		return supportsexpressionsinorderby;
	}

	public boolean 	supportsExtendedSQLGrammar() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsextendedsqlgrammar=true;
		debugPrintln("  supports extended sql grammar: "+
						supportsextendedsqlgrammar);
		return supportsextendedsqlgrammar;
	}

	public boolean 	supportsFullOuterJoins() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsfullouterjoins=true;
		debugPrintln("  supports full outer joins: "+
						supportsfullouterjoins);
		return supportsfullouterjoins;
	}

	public boolean 	supportsGetGeneratedKeys() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsgetgeneratedkeys=true;
		debugPrintln("  supports get generated keys: "+
						supportsgetgeneratedkeys);
		return supportsgetgeneratedkeys;
	}

	public boolean 	supportsGroupBy() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsgroupby=true;
		debugPrintln("  supports group by: "+supportsgroupby);
		return supportsgroupby;
	}

	public boolean 	supportsGroupByBeyondSelect() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsgroupbybeyondselect=true;
		debugPrintln("  supports group by beyond select: "+
						supportsgroupbybeyondselect);
		return supportsgroupbybeyondselect;
	}

	public boolean 	supportsGroupByUnrelated() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsgroupbyunrelated=true;
		debugPrintln("  supports group by unrelated: "+
						supportsgroupbyunrelated);
		return supportsgroupbyunrelated;
	}

	public boolean 	supportsIntegrityEnhancementFacility()
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsintegrityenhancementfacility=false;
		debugPrintln("  supports integrity enhancement facility: "+
					supportsintegrityenhancementfacility);
		return supportsintegrityenhancementfacility;
	}

	public boolean 	supportsLikeEscapeClause() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportslikeescapeclause=true;
		debugPrintln("  supports like escape clause: "+
						supportslikeescapeclause);
		return supportslikeescapeclause;
	}

	public boolean 	supportsLimitedOuterJoins() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportslimitedouterjoins=true;
		debugPrintln("  supports limited outer joins: "+
						supportslimitedouterjoins);
		return supportslimitedouterjoins;
	}

	public boolean 	supportsMinimumSQLGrammar() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsminimumsqlgrammar=true;
		debugPrintln("  supports minimum sql grammar: "+
						supportsminimumsqlgrammar);
		return supportsminimumsqlgrammar;
	}

	public boolean 	supportsMixedCaseIdentifiers() throws SQLException {
		debugFunction();
		// FIXME: db-specific, oracle doesn't
		boolean	supportsmixedcaseidentifiers=true;
		debugPrintln("  supports mixed case identifiers: "+
						supportsmixedcaseidentifiers);
		return supportsmixedcaseidentifiers;
	}

	public boolean 	supportsMixedCaseQuotedIdentifiers()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsmixedcasequotedidentifiers=true;
		debugPrintln("  supports mixed case quoted identifiers: "+
					supportsmixedcasequotedidentifiers);
		return supportsmixedcasequotedidentifiers;
	}

	public boolean 	supportsMultipleOpenResults() throws SQLException {
		debugFunction();
		boolean	supportsmultipleopenresults=true;
		debugPrintln("  supports multiple open results: "+
						supportsmultipleopenresults);
		return supportsmultipleopenresults;
	}

	public boolean 	supportsMultipleResultSets() throws SQLException {
		debugFunction();
		// FIXME: in progress...
		boolean	supportsmultipleresultsets=false;
		debugPrintln("  supports multiple result sets: "+
						supportsmultipleresultsets);
		return supportsmultipleresultsets;
	}

	public boolean 	supportsMultipleTransactions() throws SQLException {
		debugFunction();
		boolean	supportsmultipletransactions=false;
		debugPrintln("  supports multiple transactions: "+
						supportsmultipletransactions);
		return supportsmultipletransactions;
	}

	public boolean 	supportsNamedParameters() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsnamedparameters=true;
		debugPrintln("  supports named parameters: "+
						supportsnamedparameters);
		return supportsnamedparameters;
	}

	public boolean 	supportsNonNullableColumns() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsnonnullablecolumns=true;
		debugPrintln("  supports non-nullable columns: "+
						supportsnonnullablecolumns);
		return supportsnonnullablecolumns;
	}

	public boolean 	supportsOpenCursorsAcrossCommit()
						throws SQLException {
		debugFunction();
		// FIXME: not sure
		boolean	supportsopencursorsacrosscommit=false;
		debugPrintln("  supports open cursors across commit: "+
					supportsopencursorsacrosscommit);
		return supportsopencursorsacrosscommit;
	}

	public boolean 	supportsOpenCursorsAcrossRollback()
						throws SQLException {
		debugFunction();
		// FIXME: not sure
		boolean	supportsopencursorsacrossrollback=false;
		debugPrintln("  supports open cursors across rollback: "+
					supportsopencursorsacrossrollback);
		return supportsopencursorsacrossrollback;
	}

	public boolean 	supportsOpenStatementsAcrossCommit()
						throws SQLException {
		debugFunction();
		// FIXME: not sure
		boolean	supportsopenstatementsacrosscommit=false;
		debugPrintln("  supports open statements across commit: "+
					supportsopenstatementsacrosscommit);
		return supportsopenstatementsacrosscommit;
	}

	public boolean 	supportsOpenStatementsAcrossRollback()
						throws SQLException {
		debugFunction();
		// FIXME: not sure
		boolean	supportsopenstatementsacrossrollback=false;
		debugPrintln("  supports open statements across rollback: "+
					supportsopenstatementsacrossrollback);
		return supportsopenstatementsacrossrollback;
	}

	public boolean 	supportsOrderByUnrelated() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsorderbyunrelated=true;
		debugPrintln("  supports order by unrelated: "+
					supportsorderbyunrelated);
		return supportsorderbyunrelated;
	}

	public boolean 	supportsOuterJoins() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsouterjoins=true;
		debugPrintln("  supports outer joins: "+supportsouterjoins);
		return supportsouterjoins;
	}

	public boolean 	supportsPositionedDelete() throws SQLException {
		debugFunction();
		boolean	supportspositioneddelete=false;
		debugPrintln("  supports positioned delete: "+
						supportspositioneddelete);
		return supportspositioneddelete;
	}

	public boolean 	supportsPositionedUpdate() throws SQLException {
		debugFunction();
		boolean	supportspositionedupdate=false;
		debugPrintln("  supports positioned update: "+
						supportspositionedupdate);
		return supportspositionedupdate;
	}

	public boolean 	supportsResultSetConcurrency(int type,
							int concurrency)
							throws SQLException {
		debugFunction();
		boolean	supportsresultsetconcurrency=
				(type==ResultSet.TYPE_FORWARD_ONLY &&
				concurrency==ResultSet.CONCUR_READ_ONLY);
		debugPrintln("  supports result set concurrency: "+
						supportsresultsetconcurrency);
		return supportsresultsetconcurrency;
	}

	public boolean 	supportsResultSetHoldability(int holdability)
							throws SQLException {
		debugFunction();
		boolean	supportsresultsetholdability=
			(holdability==ResultSet.CLOSE_CURSORS_AT_COMMIT);
		debugPrintln("  supports result set holdability: "+
						supportsresultsetholdability);
		return supportsresultsetholdability;
	}

	public boolean 	supportsResultSetType(int type) throws SQLException {
		debugFunction();
		boolean	supportsresultsettype=
			(type==ResultSet.TYPE_FORWARD_ONLY);
		debugPrintln("  supports result set type: "+
						supportsresultsettype);
		return supportsresultsettype;
	}

	public boolean 	supportsSavepoints() throws SQLException {
		debugFunction();
		boolean	supportssavepoints=false;
		debugPrintln("  supports savepoints: "+supportssavepoints);
		return supportssavepoints;
	}

	public boolean 	supportsSchemasInDataManipulation()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsschemasindatamanipulation=true;
		debugPrintln("  supports schemas in data manipulation: "+
					supportsschemasindatamanipulation);
		return supportsschemasindatamanipulation;
	}

	public boolean 	supportsSchemasInIndexDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsschemasinindexdefinitions=true;
		debugPrintln("  supports schemas in index definitions: "+
					supportsschemasinindexdefinitions);
		return supportsschemasinindexdefinitions;
	}

	public boolean 	supportsSchemasInPrivilegeDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsschemasinprivilegedefinitions=true;
		debugPrintln("  supports schemas in privilege definitions: "+
					supportsschemasinprivilegedefinitions);
		return supportsschemasinprivilegedefinitions;
	}

	public boolean 	supportsSchemasInProcedureCalls()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsschemasinprocedurecalls=true;
		debugPrintln("  supports schemas in procedure calls: "+
					supportsschemasinprocedurecalls);
		return supportsschemasinprocedurecalls;
	}

	public boolean 	supportsSchemasInTableDefinitions()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsschemasintabledefinitions=true;
		debugPrintln("  supports schemas in table definitions: "+
					supportsschemasintabledefinitions);
		return supportsschemasintabledefinitions;
	}

	public boolean 	supportsSelectForUpdate() throws SQLException {
		debugFunction();
		boolean	supportsselectforupdate=false;
		debugPrintln("  supports select for update: "+
						supportsselectforupdate);
		return supportsselectforupdate;
	}

	public boolean 	supportsStatementPooling() throws SQLException {
		debugFunction();
		boolean	supportsstatementpooling=false;
		debugPrintln("  supports statement pooling: "+
						supportsstatementpooling);
		return supportsstatementpooling;
	}

	public boolean 	supportsStoredFunctionsUsingCallSyntax()
						throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	ssfucs=false;
		debugPrintln("  supports stored functions "+
					"using call syntax: "+ssfucs);
		return ssfucs;
	}

	public boolean 	supportsStoredProcedures() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsstoredprocedures=true;
		debugPrintln("  supports stored procedures: "+
						supportsstoredprocedures);
		return supportsstoredprocedures;
	}

	public boolean 	supportsSubqueriesInComparisons() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportssubqueriesincomparisons=true;
		debugPrintln("  supports subqueries in comparisons: "+
					supportssubqueriesincomparisons);
		return supportssubqueriesincomparisons;
	}

	public boolean 	supportsSubqueriesInExists() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportssubqueriesinexists=true;
		debugPrintln("  supports subqueries in exists: "+
						supportssubqueriesinexists);
		return supportssubqueriesinexists;
	}

	public boolean 	supportsSubqueriesInIns() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportssubqueriesinins=true;
		debugPrintln("  supports subqueries in ins: "+
						supportssubqueriesinins);
		return supportssubqueriesinins;
	}

	public boolean 	supportsSubqueriesInQuantifieds() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportssubqueriesinquantifieds=true;
		debugPrintln("  supports subqueries in quantifieds: "+
					supportssubqueriesinquantifieds);
		return supportssubqueriesinquantifieds;
	}

	public boolean 	supportsTableCorrelationNames() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportssubqueriesincorrelationnames=true;
		debugPrintln("  supports subqueries in correlation names: "+
					supportssubqueriesincorrelationnames);
		return supportssubqueriesincorrelationnames;
	}

	public boolean 	supportsTransactionIsolationLevel(int level)
							throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportstransactionisolationlevel=true;
		debugPrintln("  supports transaction isolation level: "+
					supportstransactionisolationlevel);
		return supportstransactionisolationlevel;
	}

	public boolean 	supportsTransactions() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportstransactions=true;
		debugPrintln("  supports transactions: "+supportstransactions);
		return supportstransactions;
	}

	public boolean 	supportsUnion() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsunion=true;
		debugPrintln("  supports union: "+supportsunion);
		return supportsunion;
	}

	public boolean 	supportsUnionAll() throws SQLException {
		debugFunction();
		// FIXME: db-specific
		boolean	supportsunionall=true;
		debugPrintln("  supports union all: "+supportsunionall);
		return supportsunionall;
	}

	public boolean 	updatesAreDetected(int type) throws SQLException {
		debugFunction();
		boolean	updatesaredetected=false;
		debugPrintln("  updates are detected: "+updatesaredetected);
		return updatesaredetected;
	}

	public boolean 	usesLocalFilePerTable() throws SQLException {
		debugFunction();
		boolean	useslocalfilepertable=false;
		debugPrintln("  uses local file per table: "+
						useslocalfilepertable);
		return useslocalfilepertable;
	}

	public boolean 	usesLocalFiles() throws SQLException {
		debugFunction();
		boolean	useslocalfiles=false;
		debugPrintln("  uses local files: "+useslocalfiles);
		return useslocalfiles;
	}

	protected void throwErrorMessageException(SQLRCursor sqlrcur)
							throws SQLException {
		debugFunction();
		throw new SQLException(sqlrcur.errorMessage());
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
