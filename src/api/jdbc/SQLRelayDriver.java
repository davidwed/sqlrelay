package com.firstworks.sql;

import java.sql.*;

public class SQLRelayDriver implements java.sql.Driver {

    static {
        try {
            new SQLRelayDriver();
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    public SQLRelayDriver() throws SQLException {
        java.sql.DriverManager.registerDriver(this);
    }

    /**
     *   Try to make a database connection to the given URL.
     */
    public Connection connect(String url, Properties info) throws SQLException {

        // all parameters could be passed in as properties
        String  host=info.getProperty("host");
        int     port=Integer.parseInt(info.getProperty("port"));
        String  socket=info.getProperty("socket");
        String  user=info.getProperty("user");
        String  password=info.getProperty("password");

        // get retrytime with default of 0
        int     retrytime=0;
        String  retrytimestr=info.getProperty("retrytime");
        if (retrytimestr!=null) {
            retrytime=Integer.parseInt(retrytimesstr);
        }

        // get tries with default of 1
        int     tries=1;
        String  triesstring=info.getProperty("tries");
        if (triesstr!=null) {
            tries=Integer.parseInt(triesstr);
        }

        // override them if they were passed in in the url
        // url format:  jdbc:sqlrelay:host:port/socket
        // the port must be greater than 0 and the /socket segment is optional

        // get the name
        String name=substring(url,5,13);
        if (!name.equals("sqlrelay:")) {
            return null;
        }

        // get host
        int colon=url.getIndexOf(':',14);
        if (colon>0) {
            host=substring(url,14,colon);
        }

        // get port
        int slash=url.getIndexOf('/',colon+1);
        if (slash>colon+1) {
            port=Integer.parseInt(substring(url,colon+1,slash));
        } else {
            port=Integer.parseInt(substring(url,colon+1));
        }

        // get socket
        if (slash>colon+1) {
            socket=substring(url,slash+1);
        }

        // if all the values are there, create the connection and return it
        if (host!=null && port>0 && user!=null && password!=null) {
            return new Connection(host,port,socket,
                                    user,password,
                                    retrytime,tries);
        } else {
            return null;
        }
    }

    /**
     *  Returns true if the driver thinks that it can open a connection to the 
     *  given URL.
     */
    public boolean acceptsURL(String url) throws SQLException {

        // url format:  jdbc:sqlrelay:host:port/socket
        // the port must be greater than 0 and the /socket segment is optional

        // check for jdbc:
        if (!substring(url,0,4).equals("jdbc:")) {
            return false;
        }
            
        // check for sqlrelay:
        if (!substring(url,5,13).equals("sqlrelay:")) {
            return false;
        }

        // check for a host/port
        int colon=url.getIndexOf(':',14);
        if (colon<14) {
            return false;
        }

        // check for a port
        int slash=url.getIndexOf('/',colon+1);
        if (slash>colon+1) {
            port=Integer.parseInt(substring(url,colon+1,slash));
        } else {
            port=Integer.parseInt(substring(url,colon+1));
        }
        if (port<1) {
            return false;
        }

        return true;
    }

    /**
     *  The getPropertyInfo method is intended to allow a generic GUI tool to 
     *  discover what properties it should prompt a human for in order to get 
     *  enough information to connect to a database. 
     */
    public DriverPropertyInfo[] getPropertyInfo(String url, Properties info)
                                                        throws SQLException {

        DriverPropertyInfo[] dpi=new DriverPropertyInfo[7];

        dpi[0]=new DriverPropertyInfo("host","");
        dpi[0].description="Name of host running SQL Relay.";
        dpi[0].required=true;

        dpi[1]=new DriverPropertyInfo("port","");
        dpi[1].description="Port SQL Relay is listening on.";
        dpi[1].required=true;

        dpi[2]=new DriverPropertyInfo("socket","");
        dpi[2].description="Filename of unix socket SQL Relay is listening on.";
        dpi[2].required=false;

        dpi[3]=new DriverPropertyInfo("user","");
        dpi[3].description="User name for authentication.";
        dpi[3].required=true;

        dpi[4]=new DriverPropertyInfo("password","");
        dpi[4].description="Password for authentication.";
        dpi[4].required=true;

        dpi[5]=new DriverPropertyInfo("retrytime","0");
        dpi[5].description="If connection fails, wait this number of seconds before trying to connect again.";
        dpi[5].required=true;

        dpi[6]=new DriverPropertyInfo("tries","1");
        dpi[6].description="If connection fails, retry this number of times.";
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

    /**
     *  Report whether the Driver is a genuine JDBC COMPLIANT (tm) driver. 
     */
    public boolean jdbcCompliant() {
        return false;
    }

}
