package com.firstworks.sql;

import java.sql.*;

import java.util.HashMap;

import com.firstworks.sqlrelay.*;

public class SQLRelayParameterMetaData implements ParameterMetaData {

	private HashMap<Integer,SQLRelayParameter>	parameters;

	public SQLRelayParameterMetaData() {
		parameters=null;
	}

	public void	setParameters(
				HashMap<Integer,SQLRelayParameter> parameters) {
		this.parameters=parameters;
	}

	public String 	getParameterClassName(int param) {
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getClassName():null;
	}

	private SQLRelayParameter getParameter(int param) {
		return (parameters!=null)?parameters.get(param):null;
	}

	public int 	getParameterCount() {
		return (parameters!=null)?parameters.size():0;
	}

	public int 	getParameterMode(int param) {
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getMode():parameterModeUnknown;
	}

	public int 	getParameterType(int param) {
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getType():0;
	}

	public String 	getParameterTypeName(int param) {
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getTypeName():null;
	}

	public int 	getPrecision(int param) {
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getPrecision():0;
	}

	public int 	getScale(int param) {
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getScale():0;
	}

	public int 	isNullable(int param) {
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getIsNullable():parameterNullableUnknown;
	}

	public boolean 	isSigned(int param) {
		SQLRelayParameter	p=getParameter(param);
		return (p!=null)?p.getIsSigned():true;
	}

	public boolean	isWrapperFor(Class<?> iface) throws SQLException {
		return false;
	}

	public <T> T	unwrap(Class<T> iface) throws SQLException {
		return null;
	}
}
