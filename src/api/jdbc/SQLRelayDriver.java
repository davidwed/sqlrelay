package com.firstworks.sql;

import java.sql.*;

import java.util.Properties;
import java.util.logging.Logger;

public class SQLRelayDriver implements Driver {

	static {
		try {
			DriverManager.registerDriver(new SQLRelayDriver());
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}

	public SQLRelayDriver() throws SQLException {
	}

	/**
	 *   Try to make a database connection to the given URL.
	 */
	public Connection connect(String url,
					Properties info)
					throws SQLException {

		// all parameters could be passed in as properties
		String	host=info.getProperty("host");
		int	port=Integer.parseInt(info.getProperty("port"));
		String	socket=info.getProperty("socket");
		String	user=info.getProperty("user");
		String	password=info.getProperty("password");

		// get retrytime with default of 0
		int	retrytime=0;
		String	retrytimestr=info.getProperty("retrytime");
		if (retrytimestr!=null) {
			retrytime=Integer.parseInt(retrytimestr);
		}

		// get tries with default of 1
		int	tries=1;
		String	triesstr=info.getProperty("tries");
		if (triesstr!=null) {
			tries=Integer.parseInt(triesstr);
		}

		// override them if they were passed in in the url
		// url format:
		// jdbc:sqlrelay://[user:password@]host:[port][:socket]

		// check for jdbc:sqlrelay://
		if (url.substring(0,15).equals("jdbc:sqlrelay://")) {

			// split the rest of the string on @ and get the parts
			String[]	parts=url.substring(16).split("@");
			String		cred=null;
			String		conn=null;
			if (parts.length==1) {
				conn=parts[0];
			} else if (parts.length>=2) {
				cred=parts[0];
				conn=parts[1];
			}

			// split the cred on : and get the parts
			if (cred!=null) {
				parts=cred.split(":");
				user=parts[0];
				if (parts.length>=2) {
					password=parts[1];
				}
			}

			// split the conn on : and get the parts
			if (conn!=null) {
				parts=conn.split(":");
				if (parts.length>=1) {
					host=parts[0];
				}
				if (parts.length>=2) {
					port=Integer.parseInt(parts[1]);
				}
				if (parts.length>=3) {
					socket=parts[2];
				}
			}
		}

		// finagle the port
		if (port==0) {
			port=9000;
		}

		// if all the values are there then create
		// the connection and return it
		if (host!=null && port>0 && user!=null && password!=null) {
			return new SQLRelayConnection(host,port,socket,
							user,password,
							retrytime,tries);
		}
		return null;
	}

	/**
	 *  Returns true if the driver thinks that it can open a connection to
	 *  the given URL.
	 */
	public boolean acceptsURL(String url) throws SQLException {

		// url format:
		// jdbc:sqlrelay://[user:password@]host:[port][:socket]

		// check for jdbc:sqlrelay://
		if (!url.substring(0,15).equals("jdbc:sqlrelay://")) {
			return false;
		}

		// split the rest of the string on @ and get the parts
		String[]	parts=url.substring(16).split("@");
		String		cred=null;
		String		conn=null;
		if (parts.length==1) {
			conn=parts[0];
		} else if (parts.length==2) {
			cred=parts[0];
			conn=parts[1];
		}

		// split the conn on : and get the parts
		String	host=null;
		int	port=0;
		String	socket=null;
		if (conn!=null) {
			parts=conn.split(":");
			host=(parts.length>=1)?parts[0]:null;
			port=(parts.length>=2)?Integer.parseInt(parts[1]):9000;
			socket=(parts.length>=3)?parts[2]:null;
		}

		// host/port or socket must be valid
		return ((host!=null && port>0) || socket!=null);
	}

	/**
	 *  The getPropertyInfo method is intended to allow a generic GUI tool
	 *  to discover what properties it should prompt a human for in order
	 *  to get enough information to connect to a database. 
	 */
	public DriverPropertyInfo[] getPropertyInfo(String url,
							Properties info)
							throws SQLException {

		DriverPropertyInfo[] dpi=new DriverPropertyInfo[7];

		dpi[0]=new DriverPropertyInfo("host","");
		dpi[0].description="Name of host running SQL Relay.";
		dpi[0].required=true;

		dpi[1]=new DriverPropertyInfo("port","");
		dpi[1].description="Port SQL Relay is listening on.";
		dpi[1].required=true;

		dpi[2]=new DriverPropertyInfo("socket","");
		dpi[2].description="Filename of unix socket SQL Relay is "+
					"listening on.";
		dpi[2].required=false;

		dpi[3]=new DriverPropertyInfo("user","");
		dpi[3].description="User name for authentication.";
		dpi[3].required=true;

		dpi[4]=new DriverPropertyInfo("password","");
		dpi[4].description="Password for authentication.";
		dpi[4].required=true;

		dpi[5]=new DriverPropertyInfo("retrytime","0");
		dpi[5].description="If connection fails, wait this number "+
					"of seconds before trying to connect "+
					"again.";
		dpi[5].required=true;

		dpi[6]=new DriverPropertyInfo("tries","1");
		dpi[6].description="If connection fails, retry this number "+
					"of times.";
		dpi[6].required=true;

		return dpi;
	}

	/**
	 *   Get the driver's major version number.
	 */
	public int getMajorVersion() {
		return 0;
	}

	/**
	 *   Get the driver's minor version number.
	 */
	public int getMinorVersion() {
		return 0;
	}

	public Logger getParentLogger() {
		return null;
	}

	/**
	 *  Report whether the Driver is a genuine JDBC COMPLIANT (tm) driver. 
	 */
	public boolean jdbcCompliant() {
		return false;
	}

}
