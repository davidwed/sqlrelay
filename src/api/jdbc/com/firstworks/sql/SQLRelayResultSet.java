package com.firstworks.sql;

import java.sql.*;

import java.io.InputStream;
import java.io.Reader;
import java.io.StringBufferInputStream;
import java.math.BigDecimal;
import java.util.Calendar;
import java.util.Map;
import java.net.URL;

import com.firstworks.sqlrelay.*;

public class SQLRelayResultSet implements ResultSet {

	private SQLRCursor	sqlrcur;
	private long		currentrow;
	private	boolean		beforefirst;
	private	boolean		afterlast;

	public SQLRelayResultSet() {
		reset();
	}

	private void	reset() {
		sqlrcur=null;
		currentrow=-1;
		beforefirst=true;
		afterlast=false;
	}

	public void	setSQLRCursor(SQLRCursor sqlrcur) {
		this.sqlrcur=sqlrcur;
	}

	public boolean	absolute(int row) throws SQLException {
		throwExceptionIfClosed();
		if (row<currentrow) {
			throw new SQLException(
					"FIXME: ResultSet "+
					"type is Forward-Only");
		} else if (row==0) {
			beforefirst=true;
			currentrow=-1;
			afterlast=false;
		} else if (row>0) {
			beforefirst=false;
			currentrow=row-1;
			// FIXME: set afterlast...
		} else if (row<0) {
			// FIXME: implement this...
			return false;
		}
		return true;
	}

	public void	afterLast() throws SQLException {
		throwExceptionIfClosed();
		afterlast=true;
	}

	public void	beforeFirst() throws SQLException {
		throwExceptionIfClosed();
		beforefirst=true;
	}

	public void	cancelRowUpdates() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	clearWarnings() throws SQLException {
		throwExceptionIfClosed();
	}

	public void	close() throws SQLException {
		reset();
	}

	public void	deleteRow() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public int	findColumn(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		for (int i=0; i<sqlrcur.colCount(); i++) {
			if (sqlrcur.getColumnName(i).equals(columnlabel)) {
				return i+1;
			}
		}
		throw new SQLException("FIXME: Column not found");
	}

	public boolean	first() throws SQLException {
		return absolute(1);
	}

	public Array	getArray(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public Array	getArray(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return null;
	}

	public InputStream	getAsciiStream(int columnindex)
							throws SQLException {
		throwExceptionIfClosed();
		// FIXME: not sure this is correct, how do we ensure it's ascii?
		return new StringBufferInputStream(
				sqlrcur.getField(currentrow,columnindex-1));
	}

	public InputStream	getAsciiStream(String columnlabel)
							throws SQLException {
		throwExceptionIfClosed();
		// FIXME: not sure this is correct, how do we ensure it's ascii?
		return new StringBufferInputStream(
				sqlrcur.getField(currentrow,columnlabel));
	}

	public BigDecimal	getBigDecimal(int columnindex)
							throws SQLException {
		throwExceptionIfClosed();
		String	field=sqlrcur.getField(currentrow,columnindex-1);
		if (field==null) {
			return null;
		}
		return new BigDecimal(field);
	}

	public BigDecimal	getBigDecimal(int columnindex, int scale)
							throws SQLException {
		throwExceptionIfClosed();
		// FIXME: do something with scale...
		String	field=sqlrcur.getField(currentrow,columnindex-1);
		if (field==null) {
			return null;
		}
		return new BigDecimal(field);
	}

	public BigDecimal	getBigDecimal(String columnlabel)
							throws SQLException {
		throwExceptionIfClosed();
		String	field=sqlrcur.getField(currentrow,columnlabel);
		if (field==null) {
			return null;
		}
		return new BigDecimal(field);
	}

	public BigDecimal	getBigDecimal(String columnlabel, int scale)
							throws SQLException {
		throwExceptionIfClosed();
		// FIXME: do something with scale...
		String	field=sqlrcur.getField(currentrow,columnlabel);
		if (field==null) {
			return null;
		}
		return new BigDecimal(field);
	}

	public InputStream	getBinaryStream(int columnindex) throws SQLException {
		return null;
	}

	public InputStream	getBinaryStream(String columnlabel) throws SQLException {
		return null;
	}

	public Blob	getBlob(int columnindex) throws SQLException {
		return null;
	}

	public Blob	getBlob(String columnlabel) throws SQLException {
		return null;
	}

	public boolean	getBoolean(int columnindex) throws SQLException {
		return false;
	}

	public boolean	getBoolean(String columnlabel) throws SQLException {
		return false;
	}

	public byte	getByte(int columnindex) throws SQLException {
		return 0;
	}

	public byte	getByte(String columnlabel) throws SQLException {
		return 0;
	}

	public byte[]	getBytes(int columnindex) throws SQLException {
		return null;
	}

	public byte[]	getBytes(String columnlabel) throws SQLException {
		return null;
	}

	public Reader	getCharacterStream(int columnindex) throws SQLException {
		return null;
	}

	public Reader	getCharacterStream(String columnlabel) throws SQLException {
		return null;
	}

	public Clob	getClob(int columnindex) throws SQLException {
		return null;
	}

	public Clob	getClob(String columnlabel) throws SQLException {
		return null;
	}

	public int	getConcurrency() throws SQLException {
		return 0;
	}

	public String	getCursorName() throws SQLException {
		return null;
	}

	public Date	getDate(int columnindex) throws SQLException {
		return null;
	}

	public Date	getDate(int columnindex, Calendar cal) throws SQLException {
		return null;
	}

	public Date	getDate(String columnlabel) throws SQLException {
		return null;
	}

	public Date	getDate(String columnlabel, Calendar cal) throws SQLException {
		return null;
	}

	public double	getDouble(int columnindex) throws SQLException {
		return 0.0;
	}

	public double	getDouble(String columnlabel) throws SQLException {
		return 0.0;
	}

	public int	getFetchDirection() throws SQLException {
		return 0;
	}

	public int	getFetchSize() throws SQLException {
		return 0;
	}

	public float	getFloat(int columnindex) throws SQLException {
		return 0.0f;
	}

	public float	getFloat(String columnlabel) throws SQLException {
		return 0.0f;
	}

	public int	getHoldability() throws SQLException {
		return 0;
	}

	public int	getInt(int columnindex) throws SQLException {
		return (int)sqlrcur.getFieldAsInteger(currentrow,columnindex-1);
	}

	public int	getInt(String columnlabel) throws SQLException {
		return (int)sqlrcur.getFieldAsInteger(currentrow,columnlabel);
	}

	public long	getLong(int columnindex) throws SQLException {
		return (long)sqlrcur.getFieldAsInteger(currentrow,columnindex-1);
	}

	public long	getLong(String columnlabel) throws SQLException {
		return (long)sqlrcur.getFieldAsInteger(currentrow,columnlabel);
	}

	public ResultSetMetaData	getMetaData() throws SQLException {
		return null;
	}

	public Reader	getNCharacterStream(int columnindex) throws SQLException {
		return null;
	}

	public Reader	getNCharacterStream(String columnlabel) throws SQLException {
		return null;
	}

	public NClob	getNClob(int columnindex) throws SQLException {
		return null;
	}

	public NClob	getNClob(String columnlabel) throws SQLException {
		return null;
	}

	public String	getNString(int columnindex) throws SQLException {
		return null;
	}

	public String	getNString(String columnlabel) throws SQLException {
		return null;
	}

	public Object	getObject(int columnindex) throws SQLException {
		return null;
	}

	public <T> T	getObject(int columnindex, Class<T> type) throws SQLException {
		return null;
	}

	public Object	getObject(int columnindex, Map<String,Class<?>> map) throws SQLException {
		return null;
	}

	public Object	getObject(String columnlabel) throws SQLException {
		return null;
	}

	public <T> T	getObject(String columnlabel, Class<T> type) throws SQLException {
		return null;
	}

	public Object	getObject(String columnlabel, Map<String,Class<?>> map) throws SQLException {
		return null;
	}

	public Ref	getRef(int columnindex) throws SQLException {
		return null;
	}

	public Ref	getRef(String columnlabel) throws SQLException {
		return null;
	}

	public int	getRow() throws SQLException {
		return 0;
	}

	public RowId	getRowId(int columnindex) throws SQLException {
		return null;
	}

	public RowId	getRowId(String columnlabel) throws SQLException {
		return null;
	}

	public short	getShort(int columnindex) throws SQLException {
		return 0;
	}

	public short	getShort(String columnlabel) throws SQLException {
		return 0;
	}

	public SQLXML	getSQLXML(int columnindex) throws SQLException {
		return null;
	}

	public SQLXML	getSQLXML(String columnlabel) throws SQLException {
		return null;
	}

	public Statement	getStatement() throws SQLException {
		return null;
	}

	public String	getString(int columnindex) throws SQLException {
		return null;
	}

	public String	getString(String columnlabel) throws SQLException {
		return null;
	}

	public Time	getTime(int columnindex) throws SQLException {
		return null;
	}

	public Time	getTime(int columnindex, Calendar cal) throws SQLException {
		return null;
	}

	public Time	getTime(String columnlabel) throws SQLException {
		return null;
	}

	public Time	getTime(String columnlabel, Calendar cal) throws SQLException {
		return null;
	}

	public Timestamp	getTimestamp(int columnindex) throws SQLException {
		return null;
	}

	public Timestamp	getTimestamp(int columnindex, Calendar cal) throws SQLException {
		return null;
	}

	public Timestamp	getTimestamp(String columnlabel) throws SQLException {
		return null;
	}

	public Timestamp	getTimestamp(String columnlabel, Calendar cal) throws SQLException {
		return null;
	}

	public int	getType() throws SQLException {
		return 0;
	}

	public InputStream	getUnicodeStream(int columnindex) throws SQLException {
		return null;
	}

	public InputStream	getUnicodeStream(String columnlabel) throws SQLException {
		return null;
	}

	public URL	getURL(int columnindex) throws SQLException {
		return null;
	}

	public URL	getURL(String columnlabel) throws SQLException {
		return null;
	}

	public SQLWarning	getWarnings() throws SQLException {
		return null;
	}

	public void	insertRow() throws SQLException {
	}

	public boolean	isAfterLast() throws SQLException {
		return false;
	}

	public boolean	isBeforeFirst() throws SQLException {
		return false;
	}

	public boolean	isClosed() throws SQLException {
		return false;
	}

	public boolean	isFirst() throws SQLException {
		return false;
	}

	public boolean	isLast() throws SQLException {
		return false;
	}

	public boolean	last() throws SQLException {
		return absolute(-1);
	}

	public void	moveToCurrentRow() throws SQLException {
	}

	public void	moveToInsertRow() throws SQLException {
	}

	public boolean	next() throws SQLException {
		currentrow++;
		return true;
	}

	public boolean	previous() throws SQLException {
		return false;
	}

	public void	refreshRow() throws SQLException {
	}

	public boolean	relative(int rows) throws SQLException {
		return false;
	}

	public boolean	rowDeleted() throws SQLException {
		return false;
	}

	public boolean	rowInserted() throws SQLException {
		return false;
	}

	public boolean	rowUpdated() throws SQLException {
		return false;
	}

	public void	setFetchDirection(int direction) throws SQLException {
	}

	public void	setFetchSize(int rows) throws SQLException {
	}

	public void	updateArray(int columnindex, Array x) throws SQLException {
	}

	public void	updateArray(String columnlabel, Array x) throws SQLException {
	}

	public void	updateAsciiStream(int columnindex, InputStream x) throws SQLException {
	}

	public void	updateAsciiStream(int columnindex, InputStream x, int length) throws SQLException {
	}

	public void	updateAsciiStream(int columnindex, InputStream x, long length) throws SQLException {
	}

	public void	updateAsciiStream(String columnlabel, InputStream x) throws SQLException {
	}

	public void	updateAsciiStream(String columnlabel, InputStream x, int length) throws SQLException {
	}

	public void	updateAsciiStream(String columnlabel, InputStream x, long length) throws SQLException {
	}

	public void	updateBigDecimal(int columnindex, BigDecimal x) throws SQLException {
	}

	public void	updateBigDecimal(String columnlabel, BigDecimal x) throws SQLException {
	}

	public void	updateBinaryStream(int columnindex, InputStream x) throws SQLException {
	}

	public void	updateBinaryStream(int columnindex, InputStream x, int length) throws SQLException {
	}

	public void	updateBinaryStream(int columnindex, InputStream x, long length) throws SQLException {
	}

	public void	updateBinaryStream(String columnlabel, InputStream x) throws SQLException {
	}

	public void	updateBinaryStream(String columnlabel, InputStream x, int length) throws SQLException {
	}

	public void	updateBinaryStream(String columnlabel, InputStream x, long length) throws SQLException {
	}

	public void	updateBlob(int columnindex, Blob x) throws SQLException {
	}

	public void	updateBlob(int columnindex, InputStream inputStream) throws SQLException {
	}

	public void	updateBlob(int columnindex, InputStream inputStream, long length) throws SQLException {
	}

	public void	updateBlob(String columnlabel, Blob x) throws SQLException {
	}

	public void	updateBlob(String columnlabel, InputStream inputStream) throws SQLException {
	}

	public void	updateBlob(String columnlabel, InputStream inputStream, long length) throws SQLException {
	}

	public void	updateBoolean(int columnindex, boolean x) throws SQLException {
	}

	public void	updateBoolean(String columnlabel, boolean x) throws SQLException {
	}

	public void	updateByte(int columnindex, byte x) throws SQLException {
	}

	public void	updateByte(String columnlabel, byte x) throws SQLException {
	}

	public void	updateBytes(int columnindex, byte[] x) throws SQLException {
	}

	public void	updateBytes(String columnlabel, byte[] x) throws SQLException {
	}

	public void	updateCharacterStream(int columnindex, Reader x) throws SQLException {
	}

	public void	updateCharacterStream(int columnindex, Reader x, int length) throws SQLException {
	}

	public void	updateCharacterStream(int columnindex, Reader x, long length) throws SQLException {
	}

	public void	updateCharacterStream(String columnlabel, Reader reader) throws SQLException {
	}

	public void	updateCharacterStream(String columnlabel, Reader reader, int length) throws SQLException {
	}

	public void	updateCharacterStream(String columnlabel, Reader reader, long length) throws SQLException {
	}

	public void	updateClob(int columnindex, Clob x) throws SQLException {
	}

	public void	updateClob(int columnindex, Reader reader) throws SQLException {
	}

	public void	updateClob(int columnindex, Reader reader, long length) throws SQLException {
	}

	public void	updateClob(String columnlabel, Clob x) throws SQLException {
	}

	public void	updateClob(String columnlabel, Reader reader) throws SQLException {
	}

	public void	updateClob(String columnlabel, Reader reader, long length) throws SQLException {
	}

	public void	updateDate(int columnindex, Date x) throws SQLException {
	}

	public void	updateDate(String columnlabel, Date x) throws SQLException {
	}

	public void	updateDouble(int columnindex, double x) throws SQLException {
	}

	public void	updateDouble(String columnlabel, double x) throws SQLException {
	}

	public void	updateFloat(int columnindex, float x) throws SQLException {
	}

	public void	updateFloat(String columnlabel, float x) throws SQLException {
	}

	public void	updateInt(int columnindex, int x) throws SQLException {
	}

	public void	updateInt(String columnlabel, int x) throws SQLException {
	}

	public void	updateLong(int columnindex, long x) throws SQLException {
	}

	public void	updateLong(String columnlabel, long x) throws SQLException {
	}

	public void	updateNCharacterStream(int columnindex, Reader x) throws SQLException {
	}

	public void	updateNCharacterStream(int columnindex, Reader x, long length) throws SQLException {
	}

	public void	updateNCharacterStream(String columnlabel, Reader reader) throws SQLException {
	}

	public void	updateNCharacterStream(String columnlabel, Reader reader, long length) throws SQLException {
	}

	public void	updateNClob(int columnindex, NClob nClob) throws SQLException {
	}

	public void	updateNClob(int columnindex, Reader reader) throws SQLException {
	}

	public void	updateNClob(int columnindex, Reader reader, long length) throws SQLException {
	}

	public void	updateNClob(String columnlabel, NClob nClob) throws SQLException {
	}

	public void	updateNClob(String columnlabel, Reader reader) throws SQLException {
	}

	public void	updateNClob(String columnlabel, Reader reader, long length) throws SQLException {
	}

	public void	updateNString(int columnindex, String nString) throws SQLException {
	}

	public void	updateNString(String columnlabel, String nString) throws SQLException {
	}

	public void	updateNull(int columnindex) throws SQLException {
	}

	public void	updateNull(String columnlabel) throws SQLException {
	}

	public void	updateObject(int columnindex, Object x) throws SQLException {
	}

	public void	updateObject(int columnindex, Object x, int scaleOrLength) throws SQLException {
	}

	public void	updateObject(String columnlabel, Object x) throws SQLException {
	}

	public void	updateObject(String columnlabel, Object x, int scaleOrLength) throws SQLException {
	}

	public void	updateRef(int columnindex, Ref x) throws SQLException {
	}

	public void	updateRef(String columnlabel, Ref x) throws SQLException {
	}

	public void	updateRow() throws SQLException {
	}

	public void	updateRowId(int columnindex, RowId x) throws SQLException {
	}

	public void	updateRowId(String columnlabel, RowId x) throws SQLException {
	}

	public void	updateShort(int columnindex, short x) throws SQLException {
	}

	public void	updateShort(String columnlabel, short x) throws SQLException {
	}

	public void	updateSQLXML(int columnindex, SQLXML xmlObject) throws SQLException {
	}

	public void	updateSQLXML(String columnlabel, SQLXML xmlObject) throws SQLException {
	}

	public void	updateString(int columnindex, String x) throws SQLException {
	}

	public void	updateString(String columnlabel, String x) throws SQLException {
	}

	public void	updateTime(int columnindex, Time x) throws SQLException {
	}

	public void	updateTime(String columnlabel, Time x) throws SQLException {
	}

	public void	updateTimestamp(int columnindex, Timestamp x) throws SQLException {
	}

	public void	updateTimestamp(String columnlabel, Timestamp x) throws SQLException {
	}

	public boolean	wasNull() throws SQLException {
		return false;
	}

	public boolean	isWrapperFor(Class<?> iface) throws SQLException {
		return false;
	}

	public <T> T	unwrap(Class<T> iface) throws SQLException {
		return null;
	}

	private void throwExceptionIfClosed() throws SQLException {
		if (sqlrcur==null) {
			throw new SQLException("FIXME: ResultSet is closed");
		}
	}

	private void throwErrorMessageException() throws SQLException {
		throw new SQLException(sqlrcur.errorMessage());
	}

	private void throwNotSupportedException() throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}
}
