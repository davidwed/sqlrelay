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
		return sqlrcur.colCount();
	}

	public int 	getColumnDisplaySize(int column) {
		return sqlrcur.getLongest(column-1);
	}

	public String 	getColumnLabel(int column) {
		return sqlrcur.getColumnName(column-1);
	}

	public String 	getColumnName(int column) {
		return sqlrcur.getColumnName(column-1);
	}

	public int 	getColumnType(int column) {
		// FIXME: map these...
		return 0;
	}

	public String 	getColumnTypeName(int column) {
		return sqlrcur.getColumnType(column-1);
	}

	public int 	getPrecision(int column) {
		return (int)sqlrcur.getColumnPrecision(column-1);
	}

	public int 	getScale(int column) {
		return (int)sqlrcur.getColumnScale(column-1);
	}

	public String 	getSchemaName(int column) {
		return null;
	}

	public String 	getTableName(int column) {
		return null;
	}

	public boolean 	isAutoIncrement(int column) {
		return sqlrcur.getColumnIsAutoIncrement(column-1);
	}

	public boolean 	isCaseSensitive(int column) {
		// FIXME: can db type tell us this?
		return false;
	}

	public boolean 	isCurrency(int column) {
		// FIXME: map this
		return false;
	}

	public boolean 	isDefinitelyWritable(int column) {
		return false;
	}

	public int 	isNullable(int column) {
		return (sqlrcur.getColumnIsNullable(column-1))?
						columnNullable:columnNoNulls;
	}

	public boolean 	isReadOnly(int column) {
		return false;
	}

	public boolean 	isSearchable(int column) {
		return false;
	}

	public boolean 	isSigned(int column) {
		return !sqlrcur.getColumnIsUnsigned(column-1);
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
