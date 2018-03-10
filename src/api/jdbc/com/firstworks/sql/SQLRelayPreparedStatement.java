package com.firstworks.sql;

import java.sql.*;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.util.Calendar;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import com.firstworks.sqlrelay.*;

public class SQLRelayPreparedStatement
		extends SQLRelayStatement
		implements PreparedStatement {

	private ArrayList<HashMap<Integer,SQLRelayParameter>>	batch;
	private HashMap<Integer,SQLRelayParameter>		parameters;

	SQLRelayPreparedStatement() {
		super();
		batch=new ArrayList<HashMap<Integer,SQLRelayParameter>>();
		parameters=new HashMap<Integer,SQLRelayParameter>();
	}

	public void 	addBatch() throws SQLException {
		throwExceptionIfClosed();
		batch.add(parameters);
		parameters=new HashMap<Integer,SQLRelayParameter>();
	}

	public void 	clearParameters() throws SQLException {
		throwExceptionIfClosed();
		sqlrcur.clearBinds();
		batch.clear();
		parameters.clear();
	}

	public boolean 	execute() throws SQLException {
		throwExceptionIfClosed();
		// FIXME: handle timeout
		resultset=null;
		updatecount=-1;
		sqlrcur.clearBinds();
		if (batch.size()==0) {
			bind(parameters);
		} else {
			bind(batch.get(0));
		}
		boolean	result=sqlrcur.executeQuery();
		if (result) {
			resultset=new SQLRelayResultSet();
			resultset.setStatement(this);
			resultset.setSQLRCursor(sqlrcur);
		} else {
			throwErrorMessageException();
		}
		return result;
	}

	public int[] 	executeBatch() throws SQLException {
		throwExceptionIfClosed();
		// FIXME: handle timeout
		int[]	results=new int[batch.size()];
		int	count=0;
		for (HashMap<Integer,SQLRelayParameter> params: batch) {
			sqlrcur.clearBinds();
			bind(params);
			results[count++]=executeUpdate();
		}
		return results;
	}

	public ResultSet 	executeQuery() throws SQLException {
		throwExceptionIfClosed();
		// FIXME: handle timeout
		resultset=null;
		updatecount=-1;
		sqlrcur.clearBinds();
		if (batch.size()==0) {
			bind(parameters);
		} else {
			bind(batch.get(0));
		}
		if (sqlrcur.executeQuery()) {
			resultset=new SQLRelayResultSet();
			resultset.setStatement(this);
			resultset.setSQLRCursor(sqlrcur);
		} else {
			throwErrorMessageException();
		}
		return resultset;
	}

	public int 	executeUpdate() throws SQLException {
		throwExceptionIfClosed();
		// FIXME: handle timeout
		resultset=null;
		updatecount=-1;
		sqlrcur.clearBinds();
		if (batch.size()==0) {
			bind(parameters);
		} else {
			bind(batch.get(0));
		}
		if (sqlrcur.executeQuery()) {
			updatecount=(int)sqlrcur.affectedRows();
		} else {
			throwErrorMessageException();
		}
		return updatecount;
	}

	private void	bind(HashMap<Integer,SQLRelayParameter> params)
							throws SQLException {

		if (params==null) {
			return;
		}

		for (Map.Entry<Integer,SQLRelayParameter> entry:
							params.entrySet()) {

			Integer			key=entry.getKey();
			SQLRelayParameter	value=entry.getValue();

			if (value.getObject()==null) {

			} else if (value.getObject() instanceof Array) {
				// not supported
			} else if (value.getObject() instanceof InputStream) {
				if (value.getIsLob()) {
					if (value.getIsBinary()) {
						if (value.getLength()==-1) {
						} else {
						}
					} else {
						if (value.getLength()==-1) {
						} else {
						}
					}
				} else {
					if (value.getIsBinary()) {
						if (value.getLength()==-1) {
						} else {
						}
					} else {
						if (value.getLength()==-1) {
						} else {
						}
					}
				}
			} else if (value.getObject() instanceof BigDecimal) {
			} else if (value.getObject() instanceof Blob) {
			} else if (value.getObject() instanceof Boolean) {
			} else if (value.getObject() instanceof Byte) {
			} else if (value.getObject() instanceof Byte[]) {
			} else if (value.getObject() instanceof Reader) {
			} else if (value.getObject() instanceof Clob) {
			} else if (value.getObject() instanceof Date) {
			} else if (value.getObject() instanceof Double) {
			} else if (value.getObject() instanceof Float) {
			} else if (value.getObject() instanceof Integer) {
			} else if (value.getObject() instanceof Long) {
			} else if (value.getObject() instanceof Object) {
			} else if (value.getObject() instanceof Ref) {
			} else if (value.getObject() instanceof RowId) {
			} else if (value.getObject() instanceof Short) {
			} else if (value.getObject() instanceof SQLXML) {
			} else if (value.getObject() instanceof String) {
			} else if (value.getObject() instanceof Time) {
			} else if (value.getObject() instanceof Timestamp) {
			} else if (value.getObject() instanceof URL) {
			}
		}
	}

	public ResultSetMetaData 	getMetaData() throws SQLException {
		throwExceptionIfClosed();
		return (resultset!=null)?resultset.getMetaData():null;
	}

	public ParameterMetaData 	getParameterMetaData()
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameterMetaData	pmd=
					new SQLRelayParameterMetaData();
		if (batch.size()==0) {
			pmd.setParameters(parameters);
		} else {
			pmd.setParameters(batch.get(0));
		}
		return pmd;
	}

	public void 	setArray(int parameterIndex, Array x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setAsciiStream(int parameterIndex, InputStream x)
							throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(true);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setAsciiStream(int parameterIndex,
						InputStream x,
						int length)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(length);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(true);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setAsciiStream(int parameterIndex,
						InputStream x,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(length);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(true);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setBigDecimal(int parameterIndex,
						BigDecimal x)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(true);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setBinaryStream(int parameterIndex,
						InputStream x)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(true);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setBinaryStream(int parameterIndex,
						InputStream x,
						int length)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(length);
		param.setIsBinary(true);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setBinaryStream(int parameterIndex,
						InputStream x,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(length);
		param.setIsBinary(true);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setBlob(int parameterIndex, Blob x)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(true);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setBlob(int parameterIndex,
					InputStream inputStream)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(inputStream);
		param.setLength(-1);
		param.setIsBinary(true);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setBlob(int parameterIndex,
					InputStream inputStream,
					long length)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(inputStream);
		param.setLength(length);
		param.setIsBinary(true);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setBoolean(int parameterIndex,
					boolean x)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(new Boolean(x));
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setByte(int parameterIndex,
					byte x)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(new Byte(x));
		param.setLength(-1);
		param.setIsBinary(true);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setBytes(int parameterIndex,
					byte[] x)
					throws SQLException {
		throwExceptionIfClosed();
		Byte[]	bytes=new Byte[x.length];
		for (int i=0; i<x.length; i++) {
			bytes[i]=x[i];
		}
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(bytes);
		param.setLength(-1);
		param.setIsBinary(true);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setCharacterStream(int parameterIndex,
					Reader reader)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(reader);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setCharacterStream(int parameterIndex,
						Reader reader,
						int length)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(reader);
		param.setLength(length);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setCharacterStream(int parameterIndex,
						Reader reader,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(reader);
		param.setLength(length);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setClob(int parameterIndex,
					Clob x)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setClob(int parameterIndex,
					Reader reader)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(reader);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setClob(int parameterIndex,
					Reader reader,
					long length)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(reader);
		param.setLength(length);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setDate(int parameterIndex,
					Date x)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setDate(int parameterIndex,
					Date x,
					Calendar cal)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(cal);
		parameters.put(parameterIndex,param);
	}

	public void 	setDouble(int parameterIndex,
					double x)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(true);
		param.setObject(new Double(x));
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setFloat(int parameterIndex,
					float x)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(true);
		param.setObject(new Float(x));
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setInt(int parameterIndex,
					int x)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(true);
		param.setObject(new Integer(x));
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setLong(int parameterIndex,
					long x)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(true);
		param.setObject(new Long(x));
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setNCharacterStream(int parameterIndex,
						Reader value)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(value);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setNCharacterStream(int parameterIndex,
						Reader value,
						long length)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(value);
		param.setLength(length);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setNClob(int parameterIndex,
					NClob value)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(value);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(true);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setNClob(int parameterIndex,
					Reader reader)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(reader);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(true);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setNClob(int parameterIndex,
					Reader reader,
					long length)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(reader);
		param.setLength(length);
		param.setIsBinary(false);
		param.setIsLob(true);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setNString(int parameterIndex,
					String value)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(value);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setNull(int parameterIndex,
					int sqlType)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(null);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setNull(int parameterIndex,
					int sqlType,
					String typeName)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(null);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setObject(int parameterIndex,
					Object x)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setObject(int parameterIndex,
					Object x,
					int targetSqlType)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setObject(int parameterIndex,
					Object x,
					int targetSqlType,
					int scaleOrLength)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setRef(int parameterIndex, Ref x)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setRowId(int parameterIndex, RowId x)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setShort(int parameterIndex, short x)
					throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(true);
		param.setObject(new Short(x));
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setSQLXML(int parameterIndex,
					SQLXML xmlObject)
					throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}

	public void 	setString(int parameterIndex, String x)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setTime(int parameterIndex, Time x)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setTime(int parameterIndex,
						Time x,
						Calendar cal)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(cal);
		parameters.put(parameterIndex,param);
	}

	public void 	setTimestamp(int parameterIndex,
						Timestamp x)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setTimestamp(int parameterIndex,
						Timestamp x,
						Calendar cal)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(-1);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(cal);
		parameters.put(parameterIndex,param);
	}

	public void 	setUnicodeStream(int parameterIndex,
						InputStream x,
						int length)
						throws SQLException {
		throwExceptionIfClosed();
		SQLRelayParameter	param=new SQLRelayParameter();
		param.setClassName("FIXME");
		param.setMode(ParameterMetaData.parameterModeIn);
		param.setTypeName("FIXME");
		param.setPrecision(0);
		param.setScale(0);
		param.setIsNullable(ParameterMetaData.parameterNullable);
		param.setIsSigned(false);
		param.setObject(x);
		param.setLength(length);
		param.setIsBinary(false);
		param.setIsLob(false);
		param.setIsAscii(false);
		param.setCalendar(null);
		parameters.put(parameterIndex,param);
	}

	public void 	setURL(int parameterIndex, URL x)
						throws SQLException {
		throwExceptionIfClosed();
		throwNotSupportedException();
	}
}
