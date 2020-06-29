package com.firstworks.sql;

import java.sql.*;

import java.io.InputStream;
import java.io.Reader;
import java.io.ByteArrayInputStream;
import java.io.StringBufferInputStream;
import java.io.StringReader;
import java.io.InputStreamReader;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Calendar;
import java.util.Map;
import java.net.URL;
import java.net.MalformedURLException;

import com.firstworks.sqlrelay.*;

public class SQLRelayResultSet extends SQLRelayDebug implements ResultSet {

	private Statement	statement;
	private SQLRCursor	sqlrcur;
	private long		currentrow;
	private	boolean		beforefirst;
	private	boolean		islast;
	private	boolean		afterlast;
	private	int		fetchdirection;
	private boolean		wasnull;

	public SQLRelayResultSet() {
		debugFunction();
		reset();
	}

	private void	reset() {
		debugFunction();
		statement=null;
		sqlrcur=null;
		currentrow=0;
		beforefirst=true;
		islast=false;
		afterlast=false;
		fetchdirection=ResultSet.FETCH_FORWARD;
		wasnull=false;
	}

	public void	setStatement(Statement statement) {
		debugFunction();
		this.statement=statement;
	}

	public void	setSQLRCursor(SQLRCursor sqlrcur) {
		debugFunction();
		this.sqlrcur=sqlrcur;
	}

	public boolean	absolute(int row) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		debugPrintln("  row: "+row);
		if (row<currentrow) {
			String	ex="FIXME: ResultSet "+
					"type is Forward-Only";
			debugPrintln("  exception: "+ex);
			throw new SQLException(ex);
		} else if (row==0) {
			beforefirst=true;
			currentrow=0;
			islast=false;
			afterlast=false;
		} else if (row>0) {
			beforefirst=false;
			currentrow=row;
			// FIXME: we can evaulate the result set buffer size
			// to decide whether or not we need to call getField()
			sqlrcur.getField(currentrow-1,0);
			long	rowcount=sqlrcur.rowCount();
			if (sqlrcur.endOfResultSet()) {
				if (currentrow-1==rowcount-1) {
					islast=true;
					afterlast=false;
				} else if (currentrow-1>=rowcount) {
					islast=false;
					afterlast=true;
					debugPrintln("  after last");
					return false;
				}
			}
		} else if (row<0) {
			// FIXME: implement this...
			// position relative to end of result set
			String	ex="FIXME: negative row not supported";
			debugPrintln("  exception: "+ex);
			throw new SQLException(ex);
		}
		debugPrintln("  success");
		return true;
	}

	public void	afterLast() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		afterlast=true;
	}

	public void	beforeFirst() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		beforefirst=true;
	}

	public void	cancelRowUpdates() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	clearWarnings() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
	}

	public void	close() throws SQLException {
		debugFunction();
		if (sqlrcur!=null) {
			sqlrcur.closeResultSet();
		}
		reset();
	}

	public void	deleteRow() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public int	findColumn(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		for (int i=0; i<sqlrcur.colCount(); i++) {
			if (sqlrcur.getColumnName(i).equals(columnlabel)) {
				debugPrintln("  column: "+(i+1));
				return i+1;
			}
		}
		String	ex=("Column not found");
		debugPrintln("  "+ex);
		throw new SQLException(ex);
	}

	public boolean	first() throws SQLException {
		debugFunction();
		return absolute(1);
	}

	public Array	getArray(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		return null;
	}

	public Array	getArray(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		return null;
	}

	public InputStream	getAsciiStream(int columnindex)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		// FIXME: not sure this is correct, how do we ensure it's ascii?
		return new StringBufferInputStream(field);
	}

	public InputStream	getAsciiStream(String columnlabel)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		// FIXME: not sure this is correct, how do we ensure it's ascii?
		return new StringBufferInputStream(field);
	}

	public BigDecimal	getBigDecimal(int columnindex)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		if (wasnull) {
			return null;
		}
		return new BigDecimal(field);
	}

	public BigDecimal	getBigDecimal(int columnindex, int scale)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		if (wasnull) {
			return null;
		}
		return new BigDecimal(
				new BigInteger(field.replace("\\.","")),
				scale);
	}

	public BigDecimal	getBigDecimal(String columnlabel)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		if (wasnull) {
			return null;
		}
		return new BigDecimal(field);
	}

	public BigDecimal	getBigDecimal(String columnlabel, int scale)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		if (wasnull) {
			return null;
		}
		return new BigDecimal(
				new BigInteger(field.replace("\\.","")),
				scale);
	}

	public InputStream	getBinaryStream(int columnindex)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		byte[]	field=sqlrcur.getFieldAsByteArray(
					currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrint("  field: ");
		debugPrint(field);
		debugPrint("\n");
		debugPrintln("  was null: "+wasnull);
		return new ByteArrayInputStream(field);
	}

	public InputStream	getBinaryStream(String columnlabel)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		byte[]	field=sqlrcur.getFieldAsByteArray(
					currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrint("  field: ");
		debugPrint(field);
		debugPrint("\n");
		debugPrintln("  was null: "+wasnull);
		return new ByteArrayInputStream(field);
	}

	public Blob	getBlob(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we could theoretically support this, but currently
		// SQLRelayResultSetMetaData.getColumnType/getColumnClassName
		// don't return any lob types, so it's not currently necessary
		return null;
	}

	public Blob	getBlob(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we could theoretically support this, but currently
		// SQLRelayResultSetMetaData.getColumnType/getColumnClassName
		// don't return any lob types, so it's not currently necessary
		return null;
	}

	public boolean	getBoolean(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field.equals("1");
	}

	public boolean	getBoolean(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field.equals("1");
	}

	public byte	getByte(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		long	field=sqlrcur.getFieldAsInteger(
					currentrow-1,columnindex-1);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return (byte)field;
	}

	public byte	getByte(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		long	field=sqlrcur.getFieldAsInteger(
					currentrow-1,columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return (byte)field;
	}

	public byte[]	getBytes(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		byte[]	field=sqlrcur.getFieldAsByteArray(
					currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrint("  field: ");
		debugPrint(field);
		debugPrint("\n");
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public byte[]	getBytes(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		byte[]	field=sqlrcur.getFieldAsByteArray(
					currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrint("  field: ");
		debugPrint(field);
		debugPrint("\n");
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public Reader	getCharacterStream(int columnindex)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return (wasnull)?null:(new StringReader(field));
	}

	public Reader	getCharacterStream(String columnlabel)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return (wasnull)?null:(new StringReader(field));
	}

	public Clob	getClob(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we could theoretically support this, but currently
		// SQLRelayResultSetMetaData.getColumnType/getColumnClassName
		// don't return any lob types, so it's not currently necessary
		return null;
	}

	public Clob	getClob(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we could theoretically support this, but currently
		// SQLRelayResultSetMetaData.getColumnType/getColumnClassName
		// don't return any lob types, so it's not currently necessary
		return null;
	}

	public int	getConcurrency() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		int	concurrency=ResultSet.CONCUR_READ_ONLY;
		debugPrintln("  concurrency: "+concurrency);
		return concurrency;
	}

	public String	getCursorName() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		String	cursorname=null;
		debugPrintln("  cursor name: "+cursorname);
		return cursorname;
	}

	public Date	getDate(int columnindex) throws SQLException {
		debugFunction();
		// FIXME: pass in some default calendar
		return getDate(columnindex,null);
	}

	public Date	getDate(int columnindex, Calendar cal)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		// FIXME: use cal
		// FIXME: field isn't guaranteed to be in iso format
		return (wasnull)?null:Date.valueOf(field);
	}

	public Date	getDate(String columnlabel) throws SQLException {
		debugFunction();
		// FIXME: pass in some default calendar
		return getDate(columnlabel,null);
	}

	public Date	getDate(String columnlabel, Calendar cal)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		// FIXME: use cal
		// FIXME: field isn't guaranteed to be in iso format
		return (wasnull)?null:Date.valueOf(field);
	}

	public double	getDouble(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		double	field=sqlrcur.getFieldAsDouble(
					currentrow-1,columnindex-1);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public double	getDouble(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		double	field=sqlrcur.getFieldAsDouble(
					currentrow-1,columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public int	getFetchDirection() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		int	direction=ResultSet.FETCH_FORWARD;
		debugPrintln("  direction: "+direction);
		return direction;
	}

	public int	getFetchSize() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		int	size=(int)sqlrcur.getResultSetBufferSize();
		debugPrintln("  size: "+size);
		return size;
	}

	public float	getFloat(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		float	field=(float)sqlrcur.getFieldAsDouble(
					currentrow-1,columnindex-1);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public float	getFloat(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		float	field=(float)sqlrcur.getFieldAsDouble(
					currentrow-1,columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public int	getHoldability() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		// FIXME: is this correct?
		int	holdability=ResultSet.CLOSE_CURSORS_AT_COMMIT;
		debugPrintln("  holdability: "+holdability);
		return holdability;
	}

	public int	getInt(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		int	field=(int)sqlrcur.getFieldAsInteger(
					currentrow-1,columnindex-1);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public int	getInt(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		int	field=(int)sqlrcur.getFieldAsInteger(
					currentrow-1,columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public long	getLong(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		long	field=(long)sqlrcur.getFieldAsInteger(
					currentrow-1,columnindex-1);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public long	getLong(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		long	field=(long)sqlrcur.getFieldAsInteger(
					currentrow-1,columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  was null: "+wasnull);
		return field;
	}

	public ResultSetMetaData	getMetaData() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		SQLRelayResultSetMetaData	metadata=
						new SQLRelayResultSetMetaData();
		metadata.setSQLRCursor(sqlrcur);
		return metadata;
	}

	public Reader	getNCharacterStream(int columnindex)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		byte[]	field=sqlrcur.getFieldAsByteArray(
						currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrint("  field: ");
		debugPrint(field);
		debugPrint("\n");
		debugPrintln("  was null: "+wasnull);
		return (wasnull)?null:
				(new InputStreamReader(
					new ByteArrayInputStream(field)));
	}

	public Reader	getNCharacterStream(String columnlabel)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		byte[]	field=sqlrcur.getFieldAsByteArray(
						currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrint("  field: ");
		debugPrint(field);
		debugPrint("\n");
		debugPrintln("  was null: "+wasnull);
		return (wasnull)?null:
				(new InputStreamReader(
					new ByteArrayInputStream(field)));
	}

	public NClob	getNClob(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we could theoretically support this, but currently
		// SQLRelayResultSetMetaData.getColumnType/getColumnClassName
		// don't return any lob types, so it's not currently necessary
		return null;
	}

	public NClob	getNClob(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we could theoretically support this, but currently
		// SQLRelayResultSetMetaData.getColumnType/getColumnClassName
		// don't return any lob types, so it's not currently necessary
		return null;
	}

	public String	getNString(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		byte[]	field=sqlrcur.getFieldAsByteArray(
						currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrint("  field: ");
		debugPrint(field);
		debugPrint("\n");
		debugPrintln("  was null: "+wasnull);
		try {
			return (wasnull)?null:(new String(field,"UTF-8"));
		} catch (Exception ex) {
			throw new SQLException(ex.getMessage());
		}
	}

	public String	getNString(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		byte[]	field=sqlrcur.getFieldAsByteArray(
						currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrint("  field: ");
		debugPrint(field);
		debugPrint("\n");
		debugPrintln("  was null: "+wasnull);
		try {
			return (wasnull)?null:(new String(field,"UTF-8"));
		} catch (Exception ex) {
			throw new SQLException(ex.getMessage());
		}
	}

	public Object	getObject(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public <T> T	getObject(int columnindex, Class<T> type)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Object	getObject(int columnindex, Map<String,Class<?>> map)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Object	getObject(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public <T> T	getObject(String columnlabel, Class<T> type)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Object	getObject(String columnlabel, Map<String,Class<?>> map)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Ref	getRef(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Ref	getRef(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public int	getRow() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		debugPrintln("  current row: "+currentrow);
		return (int)currentrow;
	}

	public RowId	getRowId(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public RowId	getRowId(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public short	getShort(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		short	field=(short)sqlrcur.getFieldAsInteger(
						currentrow-1,columnindex-1);
		wasnull=(sqlrcur.getField(currentrow-1,columnindex-1)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		return field;
	}

	public short	getShort(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		short	field=(short)sqlrcur.getFieldAsInteger(
						currentrow-1,columnlabel);
		wasnull=(sqlrcur.getField(currentrow-1,columnlabel)==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		return field;
	}

	public SQLXML	getSQLXML(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public SQLXML	getSQLXML(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		// FIXME: we might be able to support this somehow...
		return null;
	}

	public Statement	getStatement() throws SQLException {
		debugFunction();
		return statement;
	}

	public String	getString(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		return field;
	}

	public String	getString(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		return field;
	}

	public Time	getTime(int columnindex) throws SQLException {
		debugFunction();
		// FIXME: pass in some default calendar
		return getTime(columnindex,null);
	}

	public Time	getTime(int columnindex, Calendar cal)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		// FIXME: use cal
		// FIXME: not guaranteed to be in iso format
		return (wasnull)?null:Time.valueOf(field);
	}

	public Time	getTime(String columnlabel) throws SQLException {
		debugFunction();
		// FIXME: pass in some default calendar
		return getTime(columnlabel,null);
	}

	public Time	getTime(String columnlabel, Calendar cal)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		// FIXME: use cal
		// FIXME: not guaranteed to be in iso format
		return (wasnull)?null:Time.valueOf(field);
	}

	public Timestamp	getTimestamp(int columnindex)
						throws SQLException {
		debugFunction();
		// FIXME: pass in some default calendar
		return getTimestamp(columnindex,null);
	}

	public Timestamp	getTimestamp(int columnindex, Calendar cal)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		// FIXME: use cal
		// FIXME: not guaranteed to be in iso format
		return (wasnull)?null:Timestamp.valueOf(field);
	}

	public Timestamp	getTimestamp(String columnlabel)
							throws SQLException {
		debugFunction();
		// FIXME: pass in some default calendar
		return getTimestamp(columnlabel,null);
	}

	public Timestamp	getTimestamp(String columnlabel, Calendar cal)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		// FIXME: use cal
		// FIXME: not guaranteed to be in iso format
		return (wasnull)?null:Timestamp.valueOf(field);
	}

	public int	getType() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		int	type=ResultSet.TYPE_FORWARD_ONLY;
		debugPrintln("  type: "+type);
		return type;
	}

	public InputStream	getUnicodeStream(int columnindex)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		return new StringBufferInputStream(field);
	}

	public InputStream	getUnicodeStream(String columnlabel)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		return new StringBufferInputStream(field);
	}

	public URL	getURL(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnindex);
		throwNotSupportedException();
		String	field=sqlrcur.getField(currentrow-1,columnindex-1);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		if (wasnull) {
			return null;
		}
		try {
			return new URL(field);
		} catch (MalformedURLException ex) {
			return null;
		}
	}

	public URL	getURL(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwInvalidColumn(columnlabel);
		throwNotSupportedException();
		String	field=sqlrcur.getField(currentrow-1,columnlabel);
		wasnull=(field==null);
		debugPrintln("  field: "+field);
		debugPrintln("  wasnull: "+wasnull);
		if (wasnull) {
			return null;
		}
		try {
			return new URL(field);
		} catch (MalformedURLException ex) {
			return null;
		}
	}

	public SQLWarning	getWarnings() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return null;
	}

	public void	insertRow() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public boolean	isAfterLast() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		debugPrintln("  after last: "+afterlast);
		return afterlast;
	}

	public boolean	isBeforeFirst() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		debugPrintln("  before first: "+beforefirst);
		return beforefirst;
	}

	public boolean	isClosed() throws SQLException {
		debugFunction();
		boolean	isclosed=(sqlrcur==null);
		debugPrintln("  is closed: "+isclosed);
		return isclosed;
	}

	public boolean	isFirst() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		boolean	isfirst=(currentrow==0);
		debugPrintln("  is first: "+isfirst);
		return isfirst;
	}

	public boolean	isLast() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		debugPrintln("  is last: "+islast);
		return islast;
	}

	public boolean	last() throws SQLException {
		debugFunction();
		return absolute(-1);
	}

	public void	moveToCurrentRow() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		// since we don't support updating result sets, then we can't
		// be on the insert row, and we're always on the current row
	}

	public void	moveToInsertRow() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public boolean	next() throws SQLException {
		debugFunction();
		return relative(1);
	}

	public boolean	previous() throws SQLException {
		debugFunction();
		return relative(-1);
	}

	public void	refreshRow() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public boolean	relative(int rows) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		debugPrintln("  rows: "+rows);
		if (rows==0) {
			return true;
		}
		int	newrow=(int)(currentrow+rows);
		debugPrintln("  newrow (before): "+newrow);
		if (newrow<1) {
			newrow=1;
		}
		debugPrintln("  newrow (after): "+newrow);
		return absolute(newrow);
	}

	public boolean	rowDeleted() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return false;
	}

	public boolean	rowInserted() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return false;
	}

	public boolean	rowUpdated() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
		return false;
	}

	public void	setFetchDirection(int direction) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		debugPrintln("  direction: "+direction);
		fetchdirection=direction;
	}

	public void	setFetchSize(int rows) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		debugPrintln("  fetch size: "+rows);
		sqlrcur.setResultSetBufferSize(rows);
	}

	public void	updateArray(int columnindex, Array x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateArray(String columnlabel, Array x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(int columnindex,
						InputStream x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(int columnindex,
						InputStream x,
						int length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(int columnindex,
						InputStream x,
						long length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(String columnlabel,
						InputStream x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(String columnlabel,
						InputStream x,
						int length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateAsciiStream(String columnlabel,
						InputStream x,
						long length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBigDecimal(int columnindex,
						BigDecimal x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBigDecimal(String columnlabel,
						BigDecimal x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(int columnindex,
						InputStream x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(int columnindex,
						InputStream x,
						int length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(int columnindex,
						InputStream x,
						long length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(String columnlabel,
						InputStream x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(String columnlabel,
						InputStream x,
						int length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBinaryStream(String columnlabel,
						InputStream x,
						long length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(int columnindex, Blob x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(int columnindex,
					InputStream inputStream)
					throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(int columnindex,
					InputStream inputStream,
					long length)
					throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(String columnlabel,
					Blob x)
					throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(String columnlabel,
					InputStream inputStream)
					throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBlob(String columnlabel,
					InputStream inputStream,
					long length)
					throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBoolean(int columnindex,
						boolean x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBoolean(String columnlabel,
						boolean x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateByte(int columnindex,
						byte x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateByte(String columnlabel,
						byte x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBytes(int columnindex,
						byte[] x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateBytes(String columnlabel,
						byte[] x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(int columnindex,
						Reader x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(int columnindex,
							Reader x,
							int length)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(int columnindex,
							Reader x,
							long length)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(String columnlabel,
							Reader reader)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(String columnlabel,
							Reader reader,
							int length)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateCharacterStream(String columnlabel,
							Reader reader,
							long length)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(int columnindex,
					Clob x)
					throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(int columnindex,
					Reader reader)
					throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(int columnindex,
					Reader reader,
					long length)
					throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(String columnlabel,
						Clob x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(String columnlabel,
						Reader reader)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateClob(String columnlabel,
						Reader reader,
						long length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateDate(int columnindex, Date x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateDate(String columnlabel, Date x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateDouble(int columnindex, double x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateDouble(String columnlabel, double x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateFloat(int columnindex, float x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateFloat(String columnlabel, float x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateInt(int columnindex, int x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateInt(String columnlabel, int x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateLong(int columnindex, long x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateLong(String columnlabel, long x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNCharacterStream(int columnindex,
							Reader x)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNCharacterStream(int columnindex,
							Reader x,
							long length)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNCharacterStream(String columnlabel,
							Reader reader)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNCharacterStream(String columnlabel,
							Reader reader,
							long length)
							throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(int columnindex,
						NClob nClob)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(int columnindex,
						Reader reader)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(int columnindex,
						Reader reader,
						long length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(String columnlabel,
						NClob nClob)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(String columnlabel,
						Reader reader)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNClob(String columnlabel,
						Reader reader,
						long length)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNString(int columnindex,
						String nString)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNString(String columnlabel,
						String nString)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNull(int columnindex) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateNull(String columnlabel) throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateObject(int columnindex,
						Object x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateObject(int columnindex,
						Object x,
						int scaleOrLength)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateObject(String columnlabel,
						Object x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateObject(String columnlabel,
						Object x,
						int scaleOrLength)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRef(int columnindex, Ref x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRef(String columnlabel, Ref x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRow() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRowId(int columnindex, RowId x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateRowId(String columnlabel, RowId x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateShort(int columnindex, short x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateShort(String columnlabel, short x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateSQLXML(int columnindex,
						SQLXML xmlObject)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateSQLXML(String columnlabel,
						SQLXML xmlObject)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateString(int columnindex,
						String x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateString(String columnlabel,
						String x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateTime(int columnindex,
						Time x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateTime(String columnlabel,
						Time x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateTimestamp(int columnindex,
						Timestamp x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void	updateTimestamp(String columnlabel,
						Timestamp x)
						throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public boolean	wasNull() throws SQLException {
		debugFunction();
		throwExceptionIfClosed();
		return wasnull;
	}

	public boolean	isWrapperFor(Class<?> iface) throws SQLException {
		debugFunction();
		return (iface==SQLRCursor.class);
	}

	@SuppressWarnings({"unchecked"})
	public <T> T	unwrap(Class<T> iface) throws SQLException {
		debugFunction();
		return (T)((iface==SQLRCursor.class)?sqlrcur:null);
	}

	private void throwExceptionIfClosed() throws SQLException {
		if (sqlrcur==null) {
			throw new SQLException("FIXME: ResultSet is closed");
		}
	}

	private void throwInvalidColumn(int columnindex) throws SQLException {
		if (columnindex<1 || columnindex>sqlrcur.colCount()) {
			debugFunction();
			String	ex="FIXME: invalid column index";
			debugPrintln("  exception: "+ex);
			throw new SQLException(ex);
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
		debugFunction();
		String	ex="FIXME: invalid column label";
		debugPrintln("  exception: "+ex);
		throw new SQLException(ex);
	}

	private void throwErrorMessageException() throws SQLException {
		debugFunction();
		throw new SQLException(sqlrcur.errorMessage());
	}

	private void throwNotSupportedException() throws SQLException {
		debugFunction();
		throw new SQLFeatureNotSupportedException();
	}
}
