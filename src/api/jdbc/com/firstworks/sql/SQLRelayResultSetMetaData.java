package com.firstworks.sql;

import java.sql.*;

import com.firstworks.sqlrelay.*;

public class SQLRelayResultSetMetaData implements ResultSetMetaData {

	private	SQLRCursor	sqlrcur;

	public SQLRelayResultSetMetaData() {
		sqlrcur=null;
	}

	public void	setSQLRCursor(SQLRCursor sqlrcur) {
		this.sqlrcur=sqlrcur;
	}

	public String 	getCatalogName(int column) {
		return null;
	}

	public String 	getColumnClassName(int column) {
		return null;
	}

	public int 	getColumnCount() {
		return 0;
	}

	public int 	getColumnDisplaySize(int column) {
		return 0;
	}

	public String 	getColumnLabel(int column) {
		return null;
	}

	public String 	getColumnName(int column) {
		return null;
	}

	public int 	getColumnType(int column) {
		return 0;
	}

	public String 	getColumnTypeName(int column) {
		return null;
	}

	public int 	getPrecision(int column) {
		return 0;
	}

	public int 	getScale(int column) {
		return 0;
	}

	public String 	getSchemaName(int column) {
		return null;
	}

	public String 	getTableName(int column) {
		return null;
	}

	public boolean 	isAutoIncrement(int column) {
		return false;
	}

	public boolean 	isCaseSensitive(int column) {
		return false;
	}

	public boolean 	isCurrency(int column) {
		return false;
	}

	public boolean 	isDefinitelyWritable(int column) {
		return false;
	}

	public int 	isNullable(int column) {
		return 0;
	}

	public boolean 	isReadOnly(int column) {
		return false;
	}

	public boolean 	isSearchable(int column) {
		return false;
	}

	public boolean 	isSigned(int column) {
		return false;
	}

	public boolean 	isWritable(int column) {
		return false;
	}

	public boolean	isWrapperFor(Class<?> iface) throws SQLException {
		return false;
	}

	public <T> T	unwrap(Class<T> iface) throws SQLException {
		return null;
	}
}
