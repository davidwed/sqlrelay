package com.firstworks.sql;

import java.sql.*;

import java.util.Calendar;

import com.firstworks.sqlrelay.*;

public class SQLRelayParameter extends SQLRelayDebug {

	public enum BindType {
		Array,
		AsciiStream,
		AsciiStreamWithIntLength,
		AsciiStreamWithLongLength,
		BigDecimal,
		BinaryStream,
		BinaryStreamWithIntLength,
		BinaryStreamWithLongLength,
		Blob,
		BlobInputStream,
		BlobInputStreamWithLongLength,
		Boolean,
		Byte,
		Bytes,
		CharacterStream,
		CharacterStreamWithIntLength,
		CharacterStreamWithLongLength,
		Clob,
		ClobReader,
		ClobReaderWithLength,
		Date,
		DateWithCalendar,
		Double,
		Float,
		Int,
		Long,
		NCharStream,
		NCharStreamWithLength,
		NClob,
		NClobReader,
		NClobReaderWithLength,
		NString,
		Null,
		NullWithTypeName,
		Object,
		ObjectWithTargetType,
		ObjectWithTargetTypeAndScaleOrLength,
		Ref,
		RowId,
		Short,
		String,
		SQLXML,
		Time,
		TimeWithCalendar,
		Timestamp,
		TimestampWithCalendar,
		UnicodeStream,
		URL
	};

	private String	classname;
	private int	mode;
	private int	type;
	private String	typename;
	private int	precision;
	private int	scale;
	private int	nullable;
	private boolean	signed;

	private Object		object;
	private long		length;
	private boolean		binary;
	private boolean		lob;
	private boolean		ascii;
	private Calendar	cal;
	private	BindType	bindtype;

	public SQLRelayParameter() {
		debugFunction();
		classname=null;
		mode=ParameterMetaData.parameterModeIn;
		type=0;
		typename=null;
		precision=0;
		scale=0;
		nullable=ParameterMetaData.parameterNullableUnknown;
		signed=true;
		object=null;
		length=0;
		binary=false;
		lob=false;
		ascii=false;
		cal=null;
		bindtype=BindType.Null;
	}

	public String 	getClassName() {
		debugFunction();
		return classname;
	}

	public int 	getMode() {
		debugFunction();
		return mode;
	}

	public int 	getType() {
		debugFunction();
		return type;
	}

	public String 	getTypeName() {
		debugFunction();
		return typename;
	}

	public int 	getPrecision() {
		debugFunction();
		return precision;
	}

	public int 	getScale() {
		debugFunction();
		return scale;
	}

	public int 	getIsNullable() {
		debugFunction();
		return nullable;
	}

	public boolean 	getIsSigned() {
		debugFunction();
		return signed;
	}

	public Object	getObject() {
		debugFunction();
		return object;
	}

	public long	getLength() {
		debugFunction();
		return length;
	}

	public boolean	getIsBinary() {
		debugFunction();
		return binary;
	}

	public boolean	getIsLob() {
		debugFunction();
		return lob;
	}

	public boolean	getIsAscii() {
		debugFunction();
		return ascii;
	}

	public Calendar getCalendar() {
		debugFunction();
		return cal;
	}

	public BindType getBindType() {
		debugFunction();
		return bindtype;
	}

	public void 	setClassName(String classname) {
		debugFunction();
		this.classname=classname;
	}

	public void 	setMode(int mode) {
		debugFunction();
		this.mode=mode;
	}

	public void 	setType(int type) {
		debugFunction();
		this.type=type;
	}

	public void 	setTypeName(String typename) {
		debugFunction();
		this.typename=typename;
	}

	public void 	setPrecision(int precision) {
		debugFunction();
		this.precision=precision;
	}

	public void 	setScale(int scale) {
		debugFunction();
		this.scale=scale;
	}

	public void 	setIsNullable(int nullable) {
		debugFunction();
		this.nullable=nullable;
	}

	public void 	setIsSigned(boolean signed) {
		debugFunction();
		this.signed=signed;
	}

	public void	setObject(Object object) {
		debugFunction();
		this.object=object;
	}

	public void	setLength(long length) {
		debugFunction();
		this.length=length;
	}

	public void	setIsBinary(boolean binary) {
		debugFunction();
		this.binary=binary;
	}

	public void	setIsLob(boolean lob) {
		debugFunction();
		this.lob=lob;
	}

	public void	setIsAscii(boolean ascii) {
		debugFunction();
		this.ascii=ascii;
	}

	public void	setCalendar(Calendar cal) {
		debugFunction();
		this.cal=cal;
	}

	public void	setBindType(BindType bindtype) {
		debugFunction();
		this.bindtype=bindtype;
	}
}
