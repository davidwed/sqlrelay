package com.firstworks.sql;

import java.sql.*;

import com.firstworks.sqlrelay.*;

public class SQLRelayResultSetMetaData extends SQLRelayDebug implements ResultSetMetaData {

	private	SQLRCursor	sqlrcur;

	public SQLRelayResultSetMetaData() {
		debugFunction();
		sqlrcur=null;
	}

	public void	setSQLRCursor(SQLRCursor sqlrcur) {
		debugFunction();
		this.sqlrcur=sqlrcur;
	}

	public String 	getCatalogName(int column) {
		debugFunction();
		return null;
	}

	public String 	getColumnClassName(int column) {
		debugFunction();
		return null;
	}

	public int 	getColumnCount() {
		debugFunction();
		return sqlrcur.colCount();
	}

	public int 	getColumnDisplaySize(int column) {
		debugFunction();
		return sqlrcur.getLongest(column-1);
	}

	public String 	getColumnLabel(int column) {
		debugFunction();
		return sqlrcur.getColumnName(column-1);
	}

	public String 	getColumnName(int column) {
		debugFunction();
		return sqlrcur.getColumnName(column-1);
	}

	public int 	getColumnType(int column) {
		debugFunction();
		// FIXME: map these...
		return 0;
	}

	public String 	getColumnTypeName(int column) {
		debugFunction();
		return sqlrcur.getColumnType(column-1);
	}

	public int 	getPrecision(int column) {
		debugFunction();
		return (int)sqlrcur.getColumnPrecision(column-1);
	}

	public int 	getScale(int column) {
		debugFunction();
		return (int)sqlrcur.getColumnScale(column-1);
	}

	public String 	getSchemaName(int column) {
		debugFunction();
		return null;
	}

	public String 	getTableName(int column) {
		debugFunction();
		return null;
	}

	public boolean 	isAutoIncrement(int column) {
		debugFunction();
		return sqlrcur.getColumnIsAutoIncrement(column-1);
	}

	public boolean 	isCaseSensitive(int column) {
		debugFunction();
		// FIXME: can db type tell us this?
		return false;
	}

	public boolean 	isCurrency(int column) {
		debugFunction();
		// FIXME: map this
		return false;
	}

	public boolean 	isDefinitelyWritable(int column) {
		debugFunction();
		return false;
	}

	public int 	isNullable(int column) {
		debugFunction();
		return (sqlrcur.getColumnIsNullable(column-1))?
						columnNullable:columnNoNulls;
	}

	public boolean 	isReadOnly(int column) {
		debugFunction();
		return false;
	}

	public boolean 	isSearchable(int column) {
		debugFunction();
		return false;
	}

	public boolean 	isSigned(int column) {
		debugFunction();
		return !sqlrcur.getColumnIsUnsigned(column-1);
	}

	public boolean 	isWritable(int column) {
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
}
