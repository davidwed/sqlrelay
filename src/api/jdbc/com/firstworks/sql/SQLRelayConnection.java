package com.firstworks.sql;

import java.sql.*;

import java.util.Properties;
import java.util.Map;
import java.util.concurrent.Executor;

import com.firstworks.sqlrelay.*;

public class SQLRelayConnection extends SQLRelayDebug implements Connection {

	private String		host;
	private short		port;
	private String		socket;
	private String		user;
	private String		password;
	private SQLRConnection	sqlrcon;
	private boolean		readonly;
	private Properties	clientinfo;
	private int		txisolevel;
	private boolean		autocommit;
	private int		networktimeout;

	private Map<String,Class<?>>	typemap;

	public SQLRelayConnection(String host,
					short port,
					String socket,
					String user,
					String password,
					int retrytime,
					int tries) throws SQLException {
		debugFunction();

		this.host=host;
		this.port=port;
		this.socket=socket;
		this.user=user;
		this.password=password;
		sqlrcon=new SQLRConnection(host,port,socket,
						user,password,retrytime,tries);
		readonly=false;
		clientinfo=new Properties();
		// FIXME: defaults to repeatable read on mysql5+
		txisolevel=Connection.TRANSACTION_READ_COMMITTED;
		// FIXME: might not be false, need to get this from server
		autocommit=false;
		// FIXME: the timeout can also be set using an environment
		// variable, so we should get this from the underlying api
		// instead of tracking it here
		networktimeout=0;
		typemap=null;

		if (debug) {
			//sqlrcon.debugOn();
		}
	}

	public String getHost() {
		debugFunction();
		return host;
	}

	public short getPort() {
		debugFunction();
		return port;
	}

	public String getSocket() {
		debugFunction();
		return socket;
	}

	public String getUser() {
		debugFunction();
		return user;
	}

	public String getPassword() {
		debugFunction();
		return password;
	}

	public void	abort(Executor executor) throws SQLException {
		debugFunction();
		close();
	}

	public void	clearWarnings() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
	}

	public void	close() throws SQLException {
		debugFunction();
		sqlrcon.endSession();
		sqlrcon=null;
	}

	public void	commit() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		if (!sqlrcon.commit()) {
			throwErrorMessageException();
		}
	}

	public Array	createArrayOf(String typeName,
					Object[] elements)
					throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Blob	createBlob() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		// FIXME: we might be able to support this...
		return null;
	}

	public Clob	createClob() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		// FIXME: we might be able to support this...
		return null;
	}

	public NClob	createNClob() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		// FIXME: we might be able to support this...
		return null;
	}

	public SQLXML	createSQLXML() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Statement	createStatement() throws SQLException {
		debugFunction();
		return createStatement(ResultSet.TYPE_FORWARD_ONLY,
					ResultSet.CONCUR_READ_ONLY,
					ResultSet.HOLD_CURSORS_OVER_COMMIT);
	}

	public Statement	createStatement(int resultSetType,
						int resultSetConcurrency)
						throws SQLException {
		debugFunction();
		return createStatement(resultSetType,
					resultSetConcurrency,
					ResultSet.HOLD_CURSORS_OVER_COMMIT);
	}

	public Statement	createStatement(int resultSetType,
						int resultSetConcurrency,
						int resultSetHoldability)
						throws SQLException {
		debugFunction();
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
		sqlrcur.getNullsAsNulls();

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
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public boolean	getAutoCommit() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return autocommit;
	}

	public String	getCatalog() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return sqlrcon.getCurrentDatabase();
	}

	public Properties	getClientInfo() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return clientinfo;
	}

	public String	getClientInfo(String name) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		Properties	prop=getClientInfo();
		return prop.getProperty(name);
	}

	public int	getHoldability() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return ResultSet.HOLD_CURSORS_OVER_COMMIT;
	}

	public DatabaseMetaData	getMetaData() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		SQLRelayDatabaseMetaData	metadata=
						new SQLRelayDatabaseMetaData();
		metadata.setConnection(this);
		return metadata;
	}

	public int	getNetworkTimeout() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		// FIXME: the timeout can also be set using an environment
		// variable, so we should get this from the underlying api
		// instead of tracking it here
		return networktimeout;
	}

	public String	getSchema() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return sqlrcon.getCurrentSchema();
	}

	public int	getTransactionIsolation() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return txisolevel;
	}

	public Map<String,Class<?>>	getTypeMap() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return typemap;
	}

	public SQLWarning	getWarnings() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		// sqlrelay doesn't support anything like this
		return null;
	}

	public boolean	isClosed() throws SQLException {
		debugFunction();
		return sqlrcon==null;
	}

	public boolean	isReadOnly() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return readonly;
	}

	public boolean	isValid(int timeout) throws SQLException {
		debugFunction();
		if (isClosed()) {
			return false;
		}
		// FIXME: need to get the current response timeout pre-ping
		// and reset it post-ping, but the java api doesn't currently
		// have getResponseTimeout methods
		sqlrcon.setResponseTimeout(timeout,0);
		return sqlrcon.ping();
	}

	public String	nativeSQL(String sql) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return sql;
	}

	public CallableStatement	prepareCall(String sql)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		SQLRCursor	sqlrcur=new SQLRCursor(sqlrcon);
		sqlrcur.getNullsAsNulls();
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
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public CallableStatement	prepareCall(String sql,
						int resultSetType,
						int resultSetConcurrency,
						int resultSetHoldability)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		SQLRCursor	sqlrcur=new SQLRCursor(sqlrcon);
		sqlrcur.getNullsAsNulls();
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
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql,
						int[] columnIndexes)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql,
						int resultSetType,
						int resultSetConcurrency)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql,
						int resultSetType,
						int resultSetConcurrency,
						int resultSetHoldability)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public PreparedStatement	prepareStatement(String sql,
						String[] columnNames)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public void	releaseSavepoint(Savepoint savepoint)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	rollback() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		if (!sqlrcon.rollback()) {
			throwErrorMessageException();
		}
	}

	public void	rollback(Savepoint savepoint) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	setAutoCommit(boolean autocommit) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		if (!((autocommit)?sqlrcon.autoCommitOn():
					sqlrcon.autoCommitOff())) {
			throwErrorMessageException();
		}
		this.autocommit=autocommit;
	}

	public void	setCatalog(String catalog) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		if (!sqlrcon.selectDatabase(catalog)) {
			throwErrorMessageException();
		}
	}

	public void	setClientInfo(Properties properties)
						throws SQLClientInfoException {
		debugFunction();
		if (sqlrcon==null) {
			throw new SQLClientInfoException();
		}
		clientinfo.clear();
		clientinfo.putAll(properties);
		setClientInfo();
	}

	public void	setClientInfo(String name, String value)
						throws SQLClientInfoException {
		debugFunction();
		if (sqlrcon==null) {
			throw new SQLClientInfoException();
		}
		clientinfo.setProperty(name,value);
		setClientInfo();
	}

	private void	setClientInfo() {
		debugFunction();
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
		debugFunction();
		if (holdability!=ResultSet.HOLD_CURSORS_OVER_COMMIT) {
			throwNotSupportedException();
		}
	}

	public void	setNetworkTimeout(Executor executor,
						int milliseconds)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		// we can ignore executor because we have an internal
		// timeout implementation
		if (milliseconds<0) {
			throwException("timeout < 0");
		}
		if (milliseconds==0) {
			sqlrcon.setConnectTimeout(-1,-1);
		} else {
			sqlrcon.setConnectTimeout(milliseconds/1000,
				((milliseconds-(milliseconds/1000))*1000));
		}
		// FIXME: the timeout can also be set using an environment
		// variable, so we should get this from the underlying api
		// instead of tracking it here
		networktimeout=milliseconds;
	}

	public void	setReadOnly(boolean readonly) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		// FIXME: implement this somehow
		this.readonly=readonly;
	}

	public Savepoint	setSavepoint() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Savepoint	setSavepoint(String name) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public void	setSchema(String schema) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		// FIXME: implement this somehow
	}

	public void	setTransactionIsolation(int level) throws SQLException {
		debugFunction();
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
		debugFunction();
		throwExceptionIfClosed();
		// FIXME: do something with this
		typemap=map;
	}

	public boolean	isWrapperFor(Class<?> iface) throws SQLException {
		debugFunction();
		return (iface==SQLRConnection.class);
	}

	@SuppressWarnings({"unchecked"})
	public <T> T	unwrap(Class<T> iface) throws SQLException {
		debugFunction();
		return (T)((iface==SQLRConnection.class)?sqlrcon:null);
	}

	private void throwExceptionIfClosed() throws SQLException {
		if (sqlrcon==null) {
			throwException("Connection is closed");
		}
	}

	private void throwErrorMessageException() throws SQLException {
		debugFunction();
		throwException(sqlrcon.errorMessage());
	}

	private void throwNotSupportedException() throws SQLException {
		debugFunction();
		throw new SQLFeatureNotSupportedException();
	}

	private void throwException(String reason) throws SQLException {
		debugFunction();
		throw new SQLException(reason);
	}

	public SQLRConnection getSqlrCon() {
		return sqlrcon;
	}
}
