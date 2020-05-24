package com.firstworks.sql;

import java.sql.*;

import java.util.Properties;
import java.util.Map;
import java.util.concurrent.Executor;

import com.firstworks.sqlrelay.*;

public class SQLRelayConnection implements Connection {

	private String		host;
	private short		port;
	private String		socket;
	private String		user;
	private String		password;
	private SQLRConnection	sqlrcon;
	private boolean		readonly;
	private Properties	clientinfo;
	private	int		txisolevel;

	private Map<String,Class<?>>	typemap;

	public SQLRelayConnection(String host,
					short port,
					String socket,
					String user,
					String password,
					int retrytime,
					int tries) throws SQLException {
		this.host=host;
		this.port=port;
		this.socket=socket;
		this.user=user;
		this.password=password;
		sqlrcon=new SQLRConnection(host,port,socket,
						user,password,retrytime,tries);
		// FIXME: defaults to repeatable read on mysql5+
		txisolevel=Connection.TRANSACTION_READ_COMMITTED;
		readonly=false;
		clientinfo=new Properties();
		typemap=null;
//sqlrcon.debugOn();
	}

	public String getHost() {
		return host;
	}

	public short getPort() {
		return port;
	}

	public String getSocket() {
		return socket;
	}

	public String getUser() {
		return user;
	}

	public String getPassword() {
		return password;
	}

	public void	abort(Executor executor) throws SQLException {
		close();
	}

	public void	clearWarnings() throws SQLException {
		throwExceptionIfClosed();
	}

	public void	close() throws SQLException {
		sqlrcon.endSession();
		sqlrcon=null;
	}

	public void	commit() throws SQLException {
		throwExceptionIfClosed();
		if (!sqlrcon.commit()) {
			throwErrorMessageException();
		}
	}

	public Array	createArrayOf(String typeName,
					Object[] elements)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Blob	createBlob() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		// FIXME: we should support this...
		return null;
	}

	public Clob	createClob() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		// FIXME: we should support this...
		return null;
	}

	public NClob	createNClob() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		// FIXME: we should support this...
		return null;
	}

	public SQLXML	createSQLXML() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Statement	createStatement() throws SQLException {
		return createStatement(ResultSet.TYPE_FORWARD_ONLY,
					ResultSet.CONCUR_READ_ONLY,
					ResultSet.HOLD_CURSORS_OVER_COMMIT);
	}

	public Statement	createStatement(int resultSetType,
						int resultSetConcurrency)
						throws SQLException {
		return createStatement(resultSetType,
					resultSetConcurrency,
					ResultSet.HOLD_CURSORS_OVER_COMMIT);
	}

	public Statement	createStatement(int resultSetType,
						int resultSetConcurrency,
						int resultSetHoldability)
						throws SQLException {
		throwExceptionIfClosed();

		// unsupported options
		if (resultSetType==
				ResultSet.TYPE_SCROLL_SENSITIVE ||
			resultSetConcurrency==
				ResultSet.CONCUR_UPDATABLE ||
			resultSetHoldability==
				ResultSet.CLOSE_CURSORS_AT_COMMIT) {
			throwNotSupportedException();
		}

		// create a cursor
		SQLRCursor	sqlrcur=new SQLRCursor(sqlrcon);

		// set result set buffer size as appropriate
		switch (resultSetType) {
			case ResultSet.TYPE_FORWARD_ONLY:
				// FIXME: this can probably be set
				// to something bigger than 1
				sqlrcur.setResultSetBufferSize(1);
				break;
			case ResultSet.TYPE_SCROLL_INSENSITIVE:
				sqlrcur.setResultSetBufferSize(0);
				break;
		}

		// create a statement, attach the cursor to the statement
		SQLRelayStatement	sqlrstmt=new SQLRelayStatement();
		sqlrstmt.setConnection(this);
		sqlrstmt.setSQLRConnection(sqlrcon);
		sqlrstmt.setSQLRCursor(sqlrcur);
		return sqlrstmt;
	}

	public Struct	createStruct(String typeName,
						Object[] attributes)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public boolean	getAutoCommit() throws SQLException {
		throwExceptionIfClosed();
		// FIXME: sqlrclient api doesn't support this
		// but JDBC doesn't allow SQLFeatureNotSupportedException
		return false;
	}

	public String	getCatalog() throws SQLException {
		throwExceptionIfClosed();
		return sqlrcon.getCurrentDatabase();
	}

	public Properties	getClientInfo() throws SQLException {
		throwExceptionIfClosed();
		return clientinfo;
	}

	public String	getClientInfo(String name) throws SQLException {
		throwExceptionIfClosed();
		Properties	prop=getClientInfo();
		return prop.getProperty(name);
	}

	public int	getHoldability() throws SQLException {
		throwExceptionIfClosed();
		return ResultSet.HOLD_CURSORS_OVER_COMMIT;
	}

	public DatabaseMetaData	getMetaData() throws SQLException {
		throwExceptionIfClosed();
		SQLRelayDatabaseMetaData	metadata=
						new SQLRelayDatabaseMetaData();
		metadata.setConnection(this);
		return metadata;
	}

	public int	getNetworkTimeout() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		// FIXME: this can be supported...
		//return sqlrcon.getConnectTimeout();
		return 0;
	}

	public String	getSchema() throws SQLException {
		throwExceptionIfClosed();
		return sqlrcon.getCurrentSchema();
	}

	public int	getTransactionIsolation() throws SQLException {
		throwExceptionIfClosed();
		return txisolevel;
	}

	public Map<String,Class<?>>	getTypeMap() throws SQLException {
		throwExceptionIfClosed();
		return typemap;
	}

	public SQLWarning	getWarnings() throws SQLException {
		throwExceptionIfClosed();
		return null;
	}

	public boolean	isClosed() throws SQLException {
		return sqlrcon==null;
	}

	public boolean	isReadOnly() throws SQLException {
		throwExceptionIfClosed();
		return readonly;
	}

	public boolean	isValid(int timeout) throws SQLException {
		return !isClosed();
	}

	public String	nativeSQL(String sql) throws SQLException {
		throwExceptionIfClosed();
		return sql;
	}

	public CallableStatement	prepareCall(String sql)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRCursor	sqlrcur=new SQLRCursor(sqlrcon);
		sqlrcur.prepareQuery(sql);
		SQLRelayCallableStatement	sqlrstmt=
						new SQLRelayCallableStatement();
		sqlrstmt.setConnection(this);
		sqlrstmt.setSQLRConnection(sqlrcon);
		sqlrstmt.setSQLRCursor(sqlrcur);
		return sqlrstmt;
	}

	public CallableStatement	prepareCall(String sql,
						int resultSetType,
						int resultSetConcurrency)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public CallableStatement	prepareCall(String sql,
						int resultSetType,
						int resultSetConcurrency,
						int resultSetHoldability)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRCursor	sqlrcur=new SQLRCursor(sqlrcon);
		sqlrcur.prepareQuery(sql);
		SQLRelayPreparedStatement	sqlrstmt=
						new SQLRelayPreparedStatement();
		sqlrstmt.setConnection(this);
		sqlrstmt.setSQLRConnection(sqlrcon);
		sqlrstmt.setSQLRCursor(sqlrcur);
		return sqlrstmt;
	}

	public PreparedStatement	prepareStatement(String sql,
						int autoGeneratedKeys)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql,
						int[] columnIndexes)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql,
						int resultSetType,
						int resultSetConcurrency)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql,
						int resultSetType,
						int resultSetConcurrency,
						int resultSetHoldability)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql,
						String[] columnNames)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public void	releaseSavepoint(Savepoint savepoint)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	rollback() throws SQLException {
		throwExceptionIfClosed();
		if (!sqlrcon.rollback()) {
			throwErrorMessageException();
		}
	}

	public void	rollback(Savepoint savepoint) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	setAutoCommit(boolean autocommit) throws SQLException {
		throwExceptionIfClosed();
		if (!((autocommit)?sqlrcon.autoCommitOn():
					sqlrcon.autoCommitOff())) {
			throwErrorMessageException();
		}
	}

	public void	setCatalog(String catalog) throws SQLException {
		throwExceptionIfClosed();
		if (!sqlrcon.selectDatabase(catalog)) {
			throwErrorMessageException();
		}
	}

	public void	setClientInfo(Properties properties)
						throws SQLClientInfoException {
		if (sqlrcon==null) {
			throw new SQLClientInfoException();
		}
		clientinfo.clear();
		clientinfo.putAll(properties);
		setClientInfo();
	}

	public void	setClientInfo(String name, String value)
						throws SQLClientInfoException {
		if (sqlrcon==null) {
			throw new SQLClientInfoException();
		}
		clientinfo.setProperty(name,value);
		setClientInfo();
	}

	private void	setClientInfo() {
		String	info=new String();
		boolean	first=true;
		for (String name: clientinfo.stringPropertyNames()) {
			if (first) {
				first=false;
			} else {
				info+=",";
			}
			info+=name+":"+clientinfo.getProperty(name);
		}
		sqlrcon.setClientInfo(info);
	}

	public void	setHoldability(int holdability) throws SQLException {
		if (holdability!=ResultSet.HOLD_CURSORS_OVER_COMMIT) {
			throwNotSupportedException();
		}
	}

	public void	setNetworkTimeout(Executor executor,
						int milliseconds)
						throws SQLException {
		throwExceptionIfClosed();
		if (executor == null) {
			throw new SQLException("FIXME: executor is null");
		}
		if (milliseconds<0) {
			throw new SQLException("FIXME: timeout < 0");
		}
		sqlrcon.setConnectTimeout(milliseconds/1000,
				((milliseconds-(milliseconds/1000))*1000));
	}

	public void	setReadOnly(boolean readonly) throws SQLException {
		throwExceptionIfClosed();
		// FIXME: do something with this
		this.readonly=readonly;
	}

	public Savepoint	setSavepoint() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Savepoint	setSavepoint(String name) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public void	setSchema(String schema) throws SQLException {
		throwExceptionIfClosed();
		// FIXME: implement this somehow
	}

	public void	setTransactionIsolation(int level) throws SQLException {
		throwExceptionIfClosed();
		switch (level) {
			case Connection.TRANSACTION_READ_UNCOMMITTED:
				// FIXME: implement this somehow
				break;
			case Connection.TRANSACTION_READ_COMMITTED:
				// FIXME: implement this somehow
				break;
			case Connection.TRANSACTION_REPEATABLE_READ:
				// FIXME: implement this somehow
				break;
			case Connection.TRANSACTION_SERIALIZABLE:
				// FIXME: implement this somehow
				break;
			default:
				throwException("Invalid transaction " +
						"isolation level " + level);
		}
		txisolevel=level;
	}

	public void	setTypeMap(Map<String,Class<?>> map)
						throws SQLException {
		throwExceptionIfClosed();
		// FIXME: do something with this
		typemap=map;
	}

	public boolean	isWrapperFor(Class<?> iface) throws SQLException {
		return (iface==SQLRConnection.class);
	}

	@SuppressWarnings({"unchecked"})
	public <T> T	unwrap(Class<T> iface) throws SQLException {
		return (T)((iface==SQLRConnection.class)?sqlrcon:null);
	}

	private void throwExceptionIfClosed() throws SQLException {
		if (sqlrcon==null) {
			throw new SQLException("FIXME: Connection is closed");
		}
	}

	private void throwErrorMessageException() throws SQLException {
		throw new SQLException(sqlrcon.errorMessage());
	}

	private void throwNotSupportedException() throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}

	private void throwException(String reason) throws SQLException {
		throw new SQLException(reason);
	}
}
