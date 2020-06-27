package com.firstworks.sql;

import java.sql.*;

import java.util.HashMap;

import com.firstworks.sqlrelay.*;

public class SQLRelayParameterMetaData extends SQLRelayDebug implements ParameterMetaData {

	private HashMap<Integer,SQLRelayParameter>	parameters;

	public SQLRelayParameterMetaData() {
		debugFunction();
		parameters=null;
	}

	public void	setParameters(
				HashMap<Integer,SQLRelayParameter> parameters) {
		debugFunction();
		this.parameters=parameters;
	}

	public String 	getParameterClassName(int param) {
		debugFunction();
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getClassName():null;
	}

	private SQLRelayParameter getParameter(int param) {
		debugFunction();
		return (parameters!=null)?parameters.get(param):null;
	}

	public int 	getParameterCount() {
		debugFunction();
		return (parameters!=null)?parameters.size():0;
	}

	public int 	getParameterMode(int param) {
		debugFunction();
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getMode():parameterModeUnknown;
	}

	public int 	getParameterType(int param) {
		debugFunction();
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getType():0;
	}

	public String 	getParameterTypeName(int param) {
		debugFunction();
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getTypeName():null;
	}

	public int 	getPrecision(int param) {
		debugFunction();
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getPrecision():0;
	}

	public int 	getScale(int param) {
		debugFunction();
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getScale():0;
	}

	public int 	isNullable(int param) {
		debugFunction();
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getIsNullable():parameterNullableUnknown;
	}

	public boolean 	isSigned(int param) {
		debugFunction();
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getIsSigned():true;
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
