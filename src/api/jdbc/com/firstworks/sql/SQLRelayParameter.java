package com.firstworks.sql;

import java.sql.*;

import java.util.Calendar;

import com.firstworks.sqlrelay.*;

public class SQLRelayParameter {

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
		return classname;
	}

	public int 	getMode() {
		return mode;
	}

	public int 	getType() {
		return type;
	}

	public String 	getTypeName() {
		return typename;
	}

	public int 	getPrecision() {
		return precision;
	}

	public int 	getScale() {
		return scale;
	}

	public int 	getIsNullable() {
		return nullable;
	}

	public boolean 	getIsSigned() {
		return signed;
	}

	public Object	getObject() {
		return object;
	}

	public long	getLength() {
		return length;
	}

	public boolean	getIsBinary() {
		return binary;
	}

	public boolean	getIsLob() {
		return lob;
	}

	public boolean	getIsAscii() {
		return ascii;
	}

	public Calendar getCalendar() {
		return cal;
	}

	public BindType getBindType() {
		return bindtype;
	}

	public void 	setClassName(String classname) {
		this.classname=classname;
	}

	public void 	setMode(int mode) {
		this.mode=mode;
	}

	public void 	setType(int type) {
		this.type=type;
	}

	public void 	setTypeName(String typename) {
		this.typename=typename;
	}

	public void 	setPrecision(int precision) {
		this.precision=precision;
	}

	public void 	setScale(int scale) {
		this.scale=scale;
	}

	public void 	setIsNullable(int nullable) {
		this.nullable=nullable;
	}

	public void 	setIsSigned(boolean signed) {
		this.signed=signed;
	}

	public void	setObject(Object object) {
		this.object=object;
	}

	public void	setLength(long length) {
		this.length=length;
	}

	public void	setIsBinary(boolean binary) {
		this.binary=binary;
	}

	public void	setIsLob(boolean lob) {
		this.lob=lob;
	}

	public void	setIsAscii(boolean ascii) {
		this.ascii=ascii;
	}

	public void	setCalendar(Calendar cal) {
		this.cal=cal;
	}

	public void	setBindType(BindType bindtype) {
		this.bindtype=bindtype;
	}
}
