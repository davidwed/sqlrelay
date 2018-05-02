package com.firstworks.sql;

import java.sql.*;

import java.io.InputStream;
import java.io.Reader;
import java.io.ByteArrayInputStream;
import java.io.StringBufferInputStream;
import java.io.StringReader;
import java.io.InputStreamReader;
import java.math.BigDecimal;
import java.util.Calendar;
import java.util.Map;
import java.net.URL;

import com.firstworks.sqlrelay.*;

public class SQLRelayResultSet implements ResultSet {

	private Statement	statement;
	private SQLRCursor	sqlrcur;
	private long		currentrow;
	private	boolean		beforefirst;
	private	boolean		afterlast;
	private	int		fetchdirection;
	private boolean		wasnull;

	public SQLRelayResultSet() {
		reset();
	}

	private void	reset() {
		statement=null;
		sqlrcur=null;
		currentrow=0;
		beforefirst=true;
		afterlast=false;
		fetchdirection=ResultSet.FETCH_FORWARD;
		wasnull=false;
	}

	public void	setStatement(Statement statement) {
		this.statement=statement;
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
			currentrow=0;
			afterlast=false;
		} else if (row>0) {
			beforefirst=false;
			currentrow=row;
			// FIXME: there are some games we can play to decide
			// whether we need to do this getField()
			sqlrcur.getField(currentrow-1,0);
			long	firstrowindex=sqlrcur.firstRowIndex();
			if (sqlrcur.endOfResultSet() &&
				(currentrow-1)>=sqlrcur.rowCount()) {
				afterlast=true;
				return false;
			}
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
		sqlrcur.closeResultSet();
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
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		return null;
	}

	public Array	getArray(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		return null;
	}

	public InputStream	getAsciiStream(int columnindex)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		// FIXME: not sure this is correct, how do we ensure it's ascii?
		return new StringBufferInputStream(field);
	}

	public InputStream	getAsciiStream(String columnlabel)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		// FIXME: not sure this is correct, how do we ensure it's ascii?
		return new StringBufferInputStream(field);
	}

	public BigDecimal	getBigDecimal(int columnindex)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		if (field==null) {
			wasnull=true;
			return null;
		}
		return new BigDecimal(field);
	}

	public BigDecimal	getBigDecimal(int columnindex, int scale)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		// FIXME: do something with scale...
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		if (field==null) {
			wasnull=true;
			return null;
		}
		return new BigDecimal(field);
	}

	public BigDecimal	getBigDecimal(String columnlabel)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		if (field==null) {
			wasnull=true;
			return null;
		}
		return new BigDecimal(field);
	}

	public BigDecimal	getBigDecimal(String columnlabel, int scale)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		// FIXME: do something with scale...
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		if (field==null) {
			wasnull=true;
			return null;
		}
		return new BigDecimal(field);
	}

	public InputStream	getBinaryStream(int columnindex)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		byte[]	field=sqlrcur.getFieldAsByteArray(
					currentrow-1,columnindex-1);
		wasnull=(field==null);
		return new ByteArrayInputStream(field);
	}

	public InputStream	getBinaryStream(String columnlabel)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		byte[]	field=sqlrcur.getFieldAsByteArray(
					currentrow-1,columnlabel);
		wasnull=(field==null);
		return new ByteArrayInputStream(field);
	}

	public Blob	getBlob(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we can support this...
		return null;
	}

	public Blob	getBlob(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we can support this...
		return null;
	}

	public boolean	getBoolean(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		return field.equals("1");
	}

	public boolean	getBoolean(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		return field.equals("1");
	}

	public byte	getByte(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		return (byte)sqlrcur.getFieldAsInteger(
					currentrow-1,columnindex-1);
	}

	public byte	getByte(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		return (byte)sqlrcur.getFieldAsInteger(
					currentrow-1,columnlabel);
	}

	public byte[]	getBytes(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		byte[]	field=sqlrcur.getFieldAsByteArray(
					currentrow-1,columnindex-1);
		wasnull=(field==null);
		return field;
	}

	public byte[]	getBytes(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		byte[]	field=sqlrcur.getFieldAsByteArray(
					currentrow-1,columnlabel);
		wasnull=(field==null);
		return field;
	}

	public Reader	getCharacterStream(int columnindex)
						throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		return (wasnull)?null:(new StringReader(field));
	}

	public Reader	getCharacterStream(String columnlabel)
						throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		return (wasnull)?null:(new StringReader(field));
	}

	public Clob	getClob(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we can support this...
		return null;
	}

	public Clob	getClob(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we can support this...
		return null;
	}

	public int	getConcurrency() throws SQLException {
		throwExceptionIfClosed();
		return ResultSet.CONCUR_READ_ONLY;
	}

	public String	getCursorName() throws SQLException {
		throwExceptionIfClosed();
		return null;
	}

	public Date	getDate(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Date	getDate(int columnindex, Calendar cal)
						throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Date	getDate(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Date	getDate(String columnlabel, Calendar cal)
						throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public double	getDouble(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		return sqlrcur.getFieldAsDouble(currentrow-1,columnindex-1);
	}

	public double	getDouble(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		return sqlrcur.getFieldAsDouble(currentrow-1,columnlabel);
	}

	public int	getFetchDirection() throws SQLException {
		throwExceptionIfClosed();
		return (int)sqlrcur.getResultSetBufferSize();
	}

	public int	getFetchSize() throws SQLException {
		throwExceptionIfClosed();
		return (int)sqlrcur.getResultSetBufferSize();
	}

	public float	getFloat(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		return (float)sqlrcur.getFieldAsDouble(
					currentrow-1,columnindex-1);
	}

	public float	getFloat(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		return (float)sqlrcur.getFieldAsDouble(
					currentrow-1,columnlabel);
	}

	public int	getHoldability() throws SQLException {
		throwExceptionIfClosed();
		// FIXME: is this correct?
		return ResultSet.CLOSE_CURSORS_AT_COMMIT;
	}

	public int	getInt(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		return (int)sqlrcur.getFieldAsInteger(
					currentrow-1,columnindex-1);
	}

	public int	getInt(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		return (int)sqlrcur.getFieldAsInteger(currentrow-1,columnlabel);
	}

	public long	getLong(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		return (long)sqlrcur.getFieldAsInteger(
					currentrow-1,columnindex-1);
	}

	public long	getLong(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		return (long)sqlrcur.getFieldAsInteger(
					currentrow-1,columnlabel);
	}

	public ResultSetMetaData	getMetaData() throws SQLException {
		throwExceptionIfClosed();
		SQLRelayResultSetMetaData	metadata=
						new SQLRelayResultSetMetaData();
		metadata.setSQLRCursor(sqlrcur);
		return metadata;
	}

	public Reader	getNCharacterStream(int columnindex)
						throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		byte[]	bytes=sqlrcur.getFieldAsByteArray(
						currentrow-1,columnindex-1);
		wasnull=(bytes==null);
		return (wasnull)?null:
				(new InputStreamReader(
					new ByteArrayInputStream(bytes)));
	}

	public Reader	getNCharacterStream(String columnlabel)
						throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		byte[]	bytes=sqlrcur.getFieldAsByteArray(
						currentrow-1,columnlabel);
		wasnull=(bytes==null);
		return (wasnull)?null:
				(new InputStreamReader(
					new ByteArrayInputStream(bytes)));
	}

	public NClob	getNClob(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we can support this...
		return null;
	}

	public NClob	getNClob(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we can support this...
		return null;
	}

	public String	getNString(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		byte[]	bytes=sqlrcur.getFieldAsByteArray(
						currentrow-1,columnindex-1);
		wasnull=(bytes==null);
		try {
			return (wasnull)?null:(new String(bytes,"UTF-8"));
		} catch (Exception ex) {
			throw new SQLException(ex.getMessage());
		}
	}

	public String	getNString(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		byte[]	bytes=sqlrcur.getFieldAsByteArray(
						currentrow-1,columnlabel);
		wasnull=(bytes==null);
		try {
			return (wasnull)?null:(new String(bytes,"UTF-8"));
		} catch (Exception ex) {
			throw new SQLException(ex.getMessage());
		}
	}

	public Object	getObject(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public <T> T	getObject(int columnindex, Class<T> type)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Object	getObject(int columnindex, Map<String,Class<?>> map)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Object	getObject(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public <T> T	getObject(String columnlabel, Class<T> type)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Object	getObject(String columnlabel, Map<String,Class<?>> map)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Ref	getRef(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Ref	getRef(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public int	getRow() throws SQLException {
		throwExceptionIfClosed();
		return (int)currentrow;
	}

	public RowId	getRowId(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public RowId	getRowId(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public short	getShort(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		return (short)sqlrcur.getFieldAsInteger(
						currentrow-1,columnindex-1);
	}

	public short	getShort(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		return (short)sqlrcur.getFieldAsInteger(
						currentrow-1,columnlabel);
	}

	public SQLXML	getSQLXML(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public SQLXML	getSQLXML(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Statement	getStatement() throws SQLException {
		return statement;
	}

	public String	getString(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		return field;
	}

	public String	getString(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		return field;
	}

	public Time	getTime(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Time	getTime(int columnindex, Calendar cal)
						throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Time	getTime(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Time	getTime(String columnlabel, Calendar cal)
						throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Timestamp	getTimestamp(int columnindex)
						throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Timestamp	getTimestamp(int columnindex, Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Timestamp	getTimestamp(String columnlabel)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public Timestamp	getTimestamp(String columnlabel, Calendar cal)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we could support this if getFieldAsDate were exposed
		return null;
	}

	public int	getType() throws SQLException {
		throwExceptionIfClosed();
		return ResultSet.TYPE_FORWARD_ONLY;
	}

	public InputStream	getUnicodeStream(int columnindex)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		return new StringBufferInputStream(field);
	}

	public InputStream	getUnicodeStream(String columnlabel)
							throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		return new StringBufferInputStream(field);
	}

	public URL	getURL(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public URL	getURL(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public SQLWarning	getWarnings() throws SQLException {
		throwExceptionIfClosed();
		return null;
	}

	public void	insertRow() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public boolean	isAfterLast() throws SQLException {
		throwExceptionIfClosed();
		return afterlast;
	}

	public boolean	isBeforeFirst() throws SQLException {
		throwExceptionIfClosed();
		return beforefirst;
	}

	public boolean	isClosed() throws SQLException {
		return sqlrcur==null;
	}

	public boolean	isFirst() throws SQLException {
		throwExceptionIfClosed();
		return currentrow==0;
	}

	public boolean	isLast() throws SQLException {
		throwExceptionIfClosed();
		// FIXME: implememt this for real
		return false;
	}

	public boolean	last() throws SQLException {
		return absolute(-1);
	}

	public void	moveToCurrentRow() throws SQLException {
		throwExceptionIfClosed();
		// since we don't support updating result sets, then we can't
		// be on the insert row, and we're always on the current row
	}

	public void	moveToInsertRow() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public boolean	next() throws SQLException {
		return relative(1);
	}

	public boolean	previous() throws SQLException {
		return relative(-1);
	}

	public void	refreshRow() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public boolean	relative(int rows) throws SQLException {
		throwExceptionIfClosed();
		if (rows==0) {
			return true;
		}
		int	newrow=(int)(currentrow+rows);
		if (newrow<1) {
			newrow=1;
		}
		return absolute(newrow);
	}

	public boolean	rowDeleted() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return false;
	}

	public boolean	rowInserted() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return false;
	}

	public boolean	rowUpdated() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
		return false;
	}

	public void	setFetchDirection(int direction) throws SQLException {
		throwExceptionIfClosed();
		fetchdirection=direction;
	}

	public void	setFetchSize(int rows) throws SQLException {
		throwExceptionIfClosed();
		sqlrcur.setResultSetBufferSize(rows);
	}

	public void	updateArray(int columnindex, Array x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateArray(String columnlabel, Array x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(int columnindex,
						InputStream x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(int columnindex,
						InputStream x,
						int length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(int columnindex,
						InputStream x,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(String columnlabel,
						InputStream x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(String columnlabel,
						InputStream x,
						int length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(String columnlabel,
						InputStream x,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBigDecimal(int columnindex,
						BigDecimal x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBigDecimal(String columnlabel,
						BigDecimal x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(int columnindex,
						InputStream x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(int columnindex,
						InputStream x,
						int length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(int columnindex,
						InputStream x,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(String columnlabel,
						InputStream x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(String columnlabel,
						InputStream x,
						int length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(String columnlabel,
						InputStream x,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(int columnindex, Blob x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(int columnindex,
					InputStream inputStream)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(int columnindex,
					InputStream inputStream,
					long length)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(String columnlabel,
					Blob x)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(String columnlabel,
					InputStream inputStream)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(String columnlabel,
					InputStream inputStream,
					long length)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBoolean(int columnindex,
						boolean x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBoolean(String columnlabel,
						boolean x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateByte(int columnindex,
						byte x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateByte(String columnlabel,
						byte x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBytes(int columnindex,
						byte[] x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBytes(String columnlabel,
						byte[] x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(int columnindex,
						Reader x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(int columnindex,
							Reader x,
							int length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(int columnindex,
							Reader x,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(String columnlabel,
							Reader reader)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(String columnlabel,
							Reader reader,
							int length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(String columnlabel,
							Reader reader,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(int columnindex,
					Clob x)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(int columnindex,
					Reader reader)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(int columnindex,
					Reader reader,
					long length)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(String columnlabel,
						Clob x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(String columnlabel,
						Reader reader)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(String columnlabel,
						Reader reader,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateDate(int columnindex, Date x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateDate(String columnlabel, Date x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateDouble(int columnindex, double x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateDouble(String columnlabel, double x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateFloat(int columnindex, float x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateFloat(String columnlabel, float x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateInt(int columnindex, int x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateInt(String columnlabel, int x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateLong(int columnindex, long x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateLong(String columnlabel, long x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNCharacterStream(int columnindex,
							Reader x)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNCharacterStream(int columnindex,
							Reader x,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNCharacterStream(String columnlabel,
							Reader reader)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNCharacterStream(String columnlabel,
							Reader reader,
							long length)
							throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(int columnindex,
						NClob nClob)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(int columnindex,
						Reader reader)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(int columnindex,
						Reader reader,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(String columnlabel,
						NClob nClob)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(String columnlabel,
						Reader reader)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(String columnlabel,
						Reader reader,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNString(int columnindex,
						String nString)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNString(String columnlabel,
						String nString)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNull(int columnindex) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNull(String columnlabel) throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateObject(int columnindex,
						Object x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateObject(int columnindex,
						Object x,
						int scaleOrLength)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateObject(String columnlabel,
						Object x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateObject(String columnlabel,
						Object x,
						int scaleOrLength)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRef(int columnindex, Ref x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRef(String columnlabel, Ref x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRow() throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRowId(int columnindex, RowId x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRowId(String columnlabel, RowId x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateShort(int columnindex, short x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateShort(String columnlabel, short x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateSQLXML(int columnindex,
						SQLXML xmlObject)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateSQLXML(String columnlabel,
						SQLXML xmlObject)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateString(int columnindex,
						String x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateString(String columnlabel,
						String x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateTime(int columnindex,
						Time x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateTime(String columnlabel,
						Time x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateTimestamp(int columnindex,
						Timestamp x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateTimestamp(String columnlabel,
						Timestamp x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public boolean	wasNull() throws SQLException {
		throwExceptionIfClosed();
		return wasnull;
	}

	public boolean	isWrapperFor(Class<?> iface) throws SQLException {
		// FIXME: implement this for SQLRCursor
		return false;
	}

	public <T> T	unwrap(Class<T> iface) throws SQLException {
		// FIXME: implement this for SQLRCursor
		return null;
	}

	private void throwExceptionIfClosed() throws SQLException {
		if (sqlrcur==null) {
			throw new SQLException("FIXME: ResultSet is closed");
		}
	}

	private void throwInvalidColumn(int columnindex) throws SQLException {
		if (columnindex<1 || columnindex>sqlrcur.colCount()) {
			throw new SQLException("FIXME: invalid columnindex");
		}
	}

	private void throwInvalidColumn(String columnlabel)
						throws SQLException {
		String[] cols=sqlrcur.getColumnNames();
		for (int i=0; i<cols.length; i++) {
			if (cols[i].equals(columnlabel)) {
				return;
			}
		}
		throw new SQLException("FIXME: invalid columnindex");
	}

	private void throwErrorMessageException() throws SQLException {
		throw new SQLException(sqlrcur.errorMessage());
	}

	private void throwNotSupportedException() throws SQLException {
		throw new SQLFeatureNotSupportedException();
	}
}
