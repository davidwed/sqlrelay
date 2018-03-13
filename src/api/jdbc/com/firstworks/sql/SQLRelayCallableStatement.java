package com.firstworks.sql;

import java.sql.*;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.util.Calendar;
import java.util.Map;
import java.net.URL;

import com.firstworks.sqlrelay.*;

public class SQLRelayCallableStatement
		extends SQLRelayPreparedStatement
		implements CallableStatement {

	public Array 	getArray(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Array 	getArray(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public BigDecimal 	getBigDecimal(int parameterIndex)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public BigDecimal 	getBigDecimal(int parameterIndex, int scale)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public BigDecimal 	getBigDecimal(String parameterName)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Blob 	getBlob(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Blob 	getBlob(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public boolean 	getBoolean(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return false;
	}

	public boolean 	getBoolean(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return false;
	}

	public byte 	getByte(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0;
	}

	public byte 	getByte(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0;
	}

	public byte[] 	getBytes(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public byte[] 	getBytes(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Reader 	getCharacterStream(int parameterIndex)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Reader 	getCharacterStream(String parameterName)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Clob 	getClob(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Clob 	getClob(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Date 	getDate(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Date 	getDate(int parameterIndex, Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Date 	getDate(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Date 	getDate(String parameterName, Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public double 	getDouble(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0.0;
	}

	public double 	getDouble(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0.0;
	}

	public float 	getFloat(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0.0f;
	}

	public float 	getFloat(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0.0f;
	}

	public int 	getInt(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0;
	}

	public int 	getInt(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0;
	}

	public long 	getLong(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0;
	}

	public long 	getLong(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0;
	}

	public Reader 	getNCharacterStream(int parameterIndex)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Reader 	getNCharacterStream(String parameterName)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public NClob 	getNClob(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public NClob 	getNClob(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public String 	getNString(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public String 	getNString(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Object 	getObject(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public <T> T 	getObject(int parameterIndex, Class<T> type)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Object 	getObject(int parameterIndex, Map<String,Class<?>> map)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Object 	getObject(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public <T> T 	getObject(String parameterName, Class<T> type)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Object 	getObject(String parameterName,
						Map<String,Class<?>> map)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Ref 	getRef(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Ref 	getRef(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public RowId 	getRowId(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public RowId 	getRowId(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public short 	getShort(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0;
	}

	public short 	getShort(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return 0;
	}

	public SQLXML 	getSQLXML(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public SQLXML 	getSQLXML(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public String 	getString(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public String 	getString(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Time 	getTime(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Time 	getTime(int parameterIndex, Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Time 	getTime(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Time 	getTime(String parameterName, Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Timestamp 	getTimestamp(int parameterIndex)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Timestamp 	getTimestamp(int parameterIndex,
							Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Timestamp 	getTimestamp(String parameterName)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Timestamp 	getTimestamp(String parameterName,
							Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public URL 	getURL(int parameterIndex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public URL 	getURL(String parameterName) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public void 	registerOutParameter(int parameterIndex,
							int sqlType)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	registerOutParameter(int parameterIndex,
							int sqlType,
							int scale)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	registerOutParameter(int parameterIndex,
							int sqlType,
							String typeName)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	registerOutParameter(String parameterName,
							int sqlType)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	registerOutParameter(String parameterName,
							int sqlType,
							int scale)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	registerOutParameter(String parameterName,
							int sqlType,
							String typeName)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setAsciiStream(String parameterName,
							InputStream x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setAsciiStream(String parameterName,
							InputStream x,
							int length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setAsciiStream(String parameterName,
							InputStream x,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setBigDecimal(String parameterName, BigDecimal x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setBinaryStream(String parameterName, InputStream x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setBinaryStream(String parameterName,
							InputStream x,
							int length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setBinaryStream(String parameterName,
							InputStream x,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setBlob(String parameterName, Blob x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setBlob(String parameterName, InputStream inputStream)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setBlob(String parameterName,
						InputStream inputStream,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setBoolean(String parameterName, boolean x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setByte(String parameterName, byte x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setBytes(String parameterName, byte[] x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setCharacterStream(String parameterName,
							Reader reader)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setCharacterStream(String parameterName,
							Reader reader,
							int length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setCharacterStream(String parameterName,
							Reader reader,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setClob(String parameterName, Clob x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setClob(String parameterName, Reader reader)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setClob(String parameterName, Reader reader,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setDate(String parameterName, Date x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setDate(String parameterName, Date x, Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setDouble(String parameterName, double x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setFloat(String parameterName, float x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setInt(String parameterName, int x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setLong(String parameterName, long x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setNCharacterStream(String parameterName,
							Reader value)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setNCharacterStream(String parameterName,
							Reader value,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setNClob(String parameterName, NClob value)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setNClob(String parameterName, Reader reader)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setNClob(String parameterName, Reader reader,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setNString(String parameterName, String value)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setNull(String parameterName, int sqlType)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setNull(String parameterName, int sqlType,
							String typeName)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setObject(String parameterName, Object x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setObject(String parameterName, Object x,
							int targetSqlType)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setObject(String parameterName, Object x,
							int targetSqlType,
							int scale)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setRowId(String parameterName, RowId x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setShort(String parameterName, short x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setSQLXML(String parameterName, SQLXML xmlObject)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setString(String parameterName, String x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setTime(String parameterName, Time x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setTime(String parameterName, Time x,
							Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setTimestamp(String parameterName, Timestamp x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setTimestamp(String parameterName,
						Timestamp x,
						Calendar cal)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setURL(String parameterName, URL val)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public boolean 	wasNull() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return false;
	}
}
