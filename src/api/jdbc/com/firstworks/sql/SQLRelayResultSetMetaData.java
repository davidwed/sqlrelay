package com.firstworks.sql;

import java.sql.*;

import com.firstworks.sqlrelay.*;

public class SQLRelayResultSetMetaData extends SQLRelayDebug implements ResultSetMetaData {

	private	SQLRCursor	sqlrcur;

	public SQLRelayResultSetMetaData() {
		debugFunction();
		sqlrcur=null;
	}

	public void	setSQLRCursor(SQLRCursor sqlrcur) {
		debugFunction();
		this.sqlrcur=sqlrcur;
	}

	public String 	getCatalogName(int column) {
		debugFunction();
		String	catalogname="";
		debugPrintln("  column: "+column);
		debugPrintln("  catalog name: "+catalogname);
		return catalogname;
	}

	public String 	getColumnClassName(int column) {
		debugFunction();
		debugPrintln("  column: "+column);
		String	retval=null;
		String	ctype=sqlrcur.getColumnType(column-1);
		debugPrintln("  ctype: "+ctype);
		if (ctype.equals("UNKNOWN")) {
			retval=null;
		}
		if (ctype.equals("CHAR")) {
			retval="java.lang.String";
		}
		if (ctype.equals("INT")) {
			retval="java.lang.Integer";
		}
		if (ctype.equals("SMALLINT")) {
			retval="java.lang.Integer";
		}
		if (ctype.equals("TINYINT")) {
			retval="java.lang.Integer";
		}
		if (ctype.equals("MONEY")) {
			retval="java.lang.String";
		}
		if (ctype.equals("DATETIME")) {
			// FIXME: need parameter indicating whether
			// to map this to Types.DATE or SQL_TIMESTAMP.
			// MySQL, for example, may use DATE for dates and
			// TIMESTAMP for datetimes.
			retval="java.sql.Timestamp";
		}
		if (ctype.equals("NUMERIC")) {
			retval="java.lang.String";
		}
		if (ctype.equals("DECIMAL")) {
			retval="java.lang.BigDecimal";
		}
		if (ctype.equals("SMALLDATETIME")) {
			retval="java.sql.Timestamp";
		}
		if (ctype.equals("SMALLMONEY")) {
			retval="java.lang.String";
		}
		if (ctype.equals("IMAGE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BINARY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BIT")) {
			retval="java.lang.Boolean";
		}
		if (ctype.equals("REAL")) {
			retval="java.lang.Double";
		}
		if (ctype.equals("FLOAT")) {
			retval="java.lang.Float";
		}
		if (ctype.equals("TEXT")) {
			retval="java.lang.String";
		}
		if (ctype.equals("VARCHAR")) {
			retval="java.lang.String";
		}
		if (ctype.equals("VARBINARY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LONGCHAR")) {
			retval="java.lang.String";
		}
		if (ctype.equals("LONGBINARY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LONG")) {
			retval="java.lang.String";
		}
		if (ctype.equals("ILLEGAL")) {
			retval="java.lang.String";
		}
		if (ctype.equals("SENSITIVITY")) {
			retval="java.lang.String";
		}
		if (ctype.equals("BOUNDARY")) {
			retval="java.lang.String";
		}
		if (ctype.equals("VOID")) {
			retval="java.lang.String";
		}
		if (ctype.equals("USHORT")) {
			retval="java.lang.Short";
		}
	
		// added by lago
		if (ctype.equals("UNDEFINED")) {
			retval=null;
		}
		if (ctype.equals("DOUBLE")) {
			retval="java.lang.Double";
		}
		if (ctype.equals("DATE")) {
			// FIXME: optionally map to "java.sql.Timestamp"?
			retval="java.sql.Date";
		}
		if (ctype.equals("TIME")) {
			retval="java.sql.Time";
		}
		if (ctype.equals("TIMESTAMP")) {
			retval="java.sql.Timestamp";
		}
	
		// added by msql
		if (ctype.equals("UINT")) {
			retval="java.lang.Integer";
		}
		if (ctype.equals("LASTREAL")) {
			retval="java.lang.String";
		}
	
		// added by mysql
		if (ctype.equals("STRING")) {
			retval="java.lang.String";
		}
		if (ctype.equals("VARSTRING")) {
			retval="java.lang.String";
		}
		if (ctype.equals("LONGLONG")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("MEDIUMINT")) {
			retval="java.lang.Integer";
		}
		if (ctype.equals("YEAR")) {
			retval="java.lang.Short";
		}
		if (ctype.equals("NEWDATE")) {
			// FIXME: optionally map to "java.sql.Timestamp"?
			retval="java.sql.Date";
		}
		if (ctype.equals("NULL")) {
			retval="java.lang.String";
		}
		if (ctype.equals("ENUM")) {
			retval="java.lang.String";
		}
		if (ctype.equals("SET")) {
			retval="java.lang.String";
		}
		if (ctype.equals("TINYBLOB") ||
			ctype.equals("MEDIUMBLOB") ||
			ctype.equals("LONGBLOB") ||
			ctype.equals("BLOB")) {
			boolean	binary=sqlrcur.getColumnIsBinary(column-1);
			debugPrintln("  is binary: "+binary);
			retval=(binary)?"java.lang.Byte":"java.lang.String";
		}
	
		// added by oracle
		if (ctype.equals("VARCHAR2")) {
			retval="java.lang.String";
		}
		if (ctype.equals("NUMBER")) {
			retval="java.lang.String";
		}
		if (ctype.equals("ROWID")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("RAW")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LONG_RAW")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("MLSLABEL")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("CLOB")) {
			retval="java.lang.String";
		}
		if (ctype.equals("BFILE")) {
			retval="java.lang.Byte";
		}
	
		// added by odbc
		if (ctype.equals("BIGINT")) {
			retval="java.lang.Long";
		}
		if (ctype.equals("INTEGER")) {
			retval="java.lang.Integer";
		}
		if (ctype.equals("LONGVARBINARY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LONGVARCHAR")) {
			retval="java.lang.String";
		}
	
		// added by db2
		if (ctype.equals("GRAPHIC")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("VARGRAPHIC")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LONGVARGRAPHIC")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("DBCLOB")) {
			retval="java.lang.String";
		}
		if (ctype.equals("DATALINK")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("USER_DEFINED_TYPE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("SHORT_DATATYPE")) {
			retval="java.lang.Short";
		}
		if (ctype.equals("TINY_DATATYPE")) {
			retval="java.lang.Short";
		}
	
		// added by firebird
		if (ctype.equals("D_FLOAT")) {
			retval="java.lang.Double";
		}
		if (ctype.equals("ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("QUAD")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("INT64")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("DOUBLE PRECISION")) {
			retval="java.lang.Double";
		}
	
		// added by postgresql
		if (ctype.equals("BOOL")) {
			retval="java.lang.String";
		}
		if (ctype.equals("BYTEA")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("NAME")) {
			retval="java.lang.String";
		}
		if (ctype.equals("INT8")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("INT2")) {
			retval="java.lang.Short";
		}
		if (ctype.equals("INT2VECTOR")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INT4")) {
			retval="java.lang.Integer";
		}
		if (ctype.equals("REGPROC")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("OID")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("TID")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("XID")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("CID")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("OIDVECTOR")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("SMGR")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("POINT")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LSEG")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("PATH")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BOX")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("POLYGON")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LINE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LINE_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("FLOAT4")) {
			retval="java.lang.Float";
		}
		if (ctype.equals("FLOAT8")) {
			retval="java.lang.Double";
		}
		if (ctype.equals("ABSTIME")) {
			retval="java.lang.Integer";
		}
		if (ctype.equals("RELTIME")) {
			retval="java.lang.Integer";
		}
		if (ctype.equals("TINTERVAL")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("CIRCLE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("CIRCLE_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("MONEY_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("MACADDR")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INET")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("CIDR")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BOOL_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BYTEA_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("CHAR_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("NAME_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INT2_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INT2VECTOR_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INT4_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGPROC_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("TEXT_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("OID_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("TID_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("XID_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("CID_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("OIDVECTOR_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BPCHAR_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("VARCHAR_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INT8_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("POINT_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LSEG_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("PATH_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BOX_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("FLOAT4_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("FLOAT8_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("ABSTIME_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("RELTIME_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("TINTERVAL_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("POLYGON_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("ACLITEM")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("ACLITEM_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("MACADDR_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INET_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("CIDR_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BPCHAR")) {
			retval="java.lang.String";
		}
		if (ctype.equals("TIMESTAMP_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("DATE_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("TIME_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("TIMESTAMPTZ")) {
			retval="java.sql.Timestamp";
		}
		if (ctype.equals("TIMESTAMPTZ_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INTERVAL")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INTERVAL_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("NUMERIC_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("TIMETZ")) {
			retval="java.sql.Time";
		}
		if (ctype.equals("TIMETZ_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BIT_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("VARBIT")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("VARBIT_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REFCURSOR")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REFCURSOR_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGPROCEDURE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGOPER")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGOPERATOR")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGCLASS")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGTYPE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGPROCEDURE_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGOPER_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGOPERATOR_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGCLASS_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("REGTYPE_ARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("RECORD")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("CSTRING")) {
			retval="java.lang.String";
		}
		if (ctype.equals("ANY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("ANYARRAY")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("TRIGGER")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("LANGUAGE_HANDLER")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("INTERNAL")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("OPAQUE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("ANYELEMENT")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("PG_TYPE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("PG_ATTRIBUTE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("PG_PROC")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("PG_CLASS")) {
			retval="java.lang.Byte";
		}
		// none added by sqlite
		// added by sqlserver
		if (ctype.equals("UBIGINT")) {
			retval="java.lang.BigInteger";
		}
		if (ctype.equals("UNIQUEIDENTIFIER")) {
			retval="java.lang.Byte";
		}
		// added by informix
		if (ctype.equals("SMALLFLOAT")) {
			retval="java.lang.Float";
		}
		if (ctype.equals("BYTE")) {
			retval="java.lang.Byte";
		}
		if (ctype.equals("BOOLEAN")) {
			retval="java.lang.String";
		}
		// also added by mysql
		if (ctype.equals("TINYTEXT")) {
			retval="java.lang.String";
		}
		if (ctype.equals("MEDIUMTEXT")) {
			retval="java.lang.String";
		}
		if (ctype.equals("LONGTEXT")) {
			retval="java.lang.String";
		}
		if (ctype.equals("JSON")) {
			retval="java.lang.String";
		}
		if (ctype.equals("GEOMETRY")) {
			retval="java.lang.Byte";
		}
		// also added by oracle
		if (ctype.equals("SDO_GEOMETRY")) {
			retval="java.lang.Byte";
		}
		// added by mssql
		if (ctype.equals("NCHAR")) {
			retval="java.lang.String";
		}
		if (ctype.equals("NVARCHAR")) {
			retval="java.lang.String";
		}
		if (ctype.equals("NTEXT")) {
			retval="java.lang.String";
		}
		if (ctype.equals("XML")) {
			retval="java.lang.String";
		}
		if (ctype.equals("DATETIMEOFFSET")) {
			retval="java.sql.Timestamp";
		}
		debugPrintln("  class type: "+retval);
		return retval;
	}

	public int 	getColumnCount() {
		debugFunction();
		int	colcount=sqlrcur.colCount();
		debugPrintln("  colcount: "+colcount);
		return colcount;
	}

	public int 	getColumnDisplaySize(int column) {
		debugFunction();
		int	longest=sqlrcur.getLongest(column-1);
		debugPrintln("  column: "+column);
		debugPrintln("  longest (before): "+longest);
		if (longest==-1) {
			longest=2147483647;
		}
		debugPrintln("  longest (after): "+longest);
		return longest;
	}

	public String 	getColumnLabel(int column) {
		debugFunction();
		String	label=sqlrcur.getColumnName(column-1);
		debugPrintln("  column: "+column);
		debugPrintln("  label: "+label);
		return label;
	}

	public String 	getColumnName(int column) {
		debugFunction();
		String	columnname=sqlrcur.getColumnName(column-1);
		debugPrintln("  column: "+column);
		debugPrintln("  column name: "+columnname);
		return columnname;
	}

	public int 	getColumnType(int column) {
		debugFunction();
		debugPrintln("  column: "+column);
		int	retval=0;
		String	ctype=sqlrcur.getColumnType(column-1);
		debugPrintln("  ctype: "+ctype);
		if (ctype.equals("UNKNOWN")) {
			retval=Types.OTHER;
		}
		if (ctype.equals("CHAR")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("INT")) {
			retval=Types.INTEGER;
		}
		if (ctype.equals("SMALLINT")) {
			retval=Types.SMALLINT;
		}
		if (ctype.equals("TINYINT")) {
			retval=Types.TINYINT;
		}
		if (ctype.equals("MONEY")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("DATETIME")) {
			// FIXME: need parameter indicating whether
			// to map this to Types.DATE or SQL_TIMESTAMP.
			// MySQL, for example, may use DATE for dates and
			// TIMESTAMP for datetimes.
			retval=Types.TIMESTAMP;
		}
		if (ctype.equals("NUMERIC")) {
			retval=Types.NUMERIC;
		}
		if (ctype.equals("DECIMAL")) {
			retval=Types.DECIMAL;
		}
		if (ctype.equals("SMALLDATETIME")) {
			retval=Types.TIMESTAMP;
		}
		if (ctype.equals("SMALLMONEY")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("IMAGE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BINARY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BIT")) {
			retval=Types.BIT;
		}
		if (ctype.equals("REAL")) {
			retval=Types.REAL;
		}
		if (ctype.equals("FLOAT")) {
			retval=Types.FLOAT;
		}
		if (ctype.equals("TEXT")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("VARCHAR")) {
			retval=Types.VARCHAR;
		}
		if (ctype.equals("VARBINARY")) {
			retval=Types.VARBINARY;
		}
		if (ctype.equals("LONGCHAR")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("LONGBINARY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("LONG")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("ILLEGAL")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("SENSITIVITY")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("BOUNDARY")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("VOID")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("USHORT")) {
			retval=Types.SMALLINT;
		}
	
		// added by lago
		if (ctype.equals("UNDEFINED")) {
			retval=Types.OTHER;
		}
		if (ctype.equals("DOUBLE")) {
			retval=Types.DOUBLE;
		}
		if (ctype.equals("DATE")) {
			// FIXME: optionally map to Types.TIMESTAMP?
			retval=Types.DATE;
		}
		if (ctype.equals("TIME")) {
			retval=Types.TIME;
		}
		if (ctype.equals("TIMESTAMP")) {
			retval=Types.TIMESTAMP;
		}
	
		// added by msql
		if (ctype.equals("UINT")) {
			retval=Types.INTEGER;
		}
		if (ctype.equals("LASTREAL")) {
			retval=Types.REAL;
		}
	
		// added by mysql
		if (ctype.equals("STRING")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("VARSTRING")) {
			retval=Types.VARCHAR;
		}
		if (ctype.equals("LONGLONG")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("MEDIUMINT")) {
			retval=Types.INTEGER;
		}
		if (ctype.equals("YEAR")) {
			retval=Types.SMALLINT;
		}
		if (ctype.equals("NEWDATE")) {
			// FIXME: optionally map to Types.TIMESTAMP?
			retval=Types.DATE;
		}
		if (ctype.equals("NULL")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("ENUM")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("SET")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("TINYBLOB") ||
			ctype.equals("MEDIUMBLOB") ||
			ctype.equals("LONGBLOB") ||
			ctype.equals("BLOB")) {
			boolean	binary=sqlrcur.getColumnIsBinary(column-1);
			debugPrintln("  is binary: "+binary);
			retval=(binary)?Types.BINARY:Types.LONGVARCHAR;
		}
	
		// added by oracle
		if (ctype.equals("VARCHAR2")) {
			retval=Types.VARCHAR;
		}
		if (ctype.equals("NUMBER")) {
			retval=Types.NUMERIC;
		}
		if (ctype.equals("ROWID")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("RAW")) {
			retval=Types.VARBINARY;
		}
		if (ctype.equals("LONG_RAW")) {
			retval=Types.LONGVARBINARY;
		}
		if (ctype.equals("MLSLABEL")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("CLOB")) {
			retval=Types.LONGVARCHAR;
		}
		if (ctype.equals("BFILE")) {
			retval=Types.LONGVARBINARY;
		}
	
		// added by odbc
		if (ctype.equals("BIGINT")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("INTEGER")) {
			retval=Types.INTEGER;
		}
		if (ctype.equals("LONGVARBINARY")) {
			retval=Types.LONGVARBINARY;
		}
		if (ctype.equals("LONGVARCHAR")) {
			retval=Types.LONGVARCHAR;
		}
	
		// added by db2
		if (ctype.equals("GRAPHIC")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("VARGRAPHIC")) {
			retval=Types.VARBINARY;
		}
		if (ctype.equals("LONGVARGRAPHIC")) {
			retval=Types.LONGVARBINARY;
		}
		if (ctype.equals("DBCLOB")) {
			retval=Types.LONGVARCHAR;
		}
		if (ctype.equals("DATALINK")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("USER_DEFINED_TYPE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("SHORT_DATATYPE")) {
			retval=Types.SMALLINT;
		}
		if (ctype.equals("TINY_DATATYPE")) {
			retval=Types.TINYINT;
		}
	
		// added by firebird
		if (ctype.equals("D_FLOAT")) {
			retval=Types.DOUBLE;
		}
		if (ctype.equals("ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("QUAD")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("INT64")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("DOUBLE PRECISION")) {
			retval=Types.DOUBLE;
		}
	
		// added by postgresql
		if (ctype.equals("BOOL")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("BYTEA")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("NAME")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("INT8")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("INT2")) {
			retval=Types.SMALLINT;
		}
		if (ctype.equals("INT2VECTOR")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INT4")) {
			retval=Types.INTEGER;
		}
		if (ctype.equals("REGPROC")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("OID")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("TID")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("XID")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("CID")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("OIDVECTOR")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("SMGR")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("POINT")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("LSEG")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("PATH")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BOX")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("POLYGON")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("LINE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("LINE_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("FLOAT4")) {
			retval=Types.FLOAT;
		}
		if (ctype.equals("FLOAT8")) {
			retval=Types.DOUBLE;
		}
		if (ctype.equals("ABSTIME")) {
			retval=Types.INTEGER;
		}
		if (ctype.equals("RELTIME")) {
			retval=Types.INTEGER;
		}
		if (ctype.equals("TINTERVAL")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("CIRCLE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("CIRCLE_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("MONEY_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("MACADDR")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INET")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("CIDR")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BOOL_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BYTEA_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("CHAR_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("NAME_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INT2_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INT2VECTOR_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INT4_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGPROC_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("TEXT_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("OID_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("TID_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("XID_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("CID_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("OIDVECTOR_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BPCHAR_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("VARCHAR_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INT8_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("POINT_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("LSEG_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("PATH_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BOX_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("FLOAT4_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("FLOAT8_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("ABSTIME_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("RELTIME_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("TINTERVAL_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("POLYGON_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("ACLITEM")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("ACLITEM_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("MACADDR_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INET_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("CIDR_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BPCHAR")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("TIMESTAMP_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("DATE_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("TIME_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("TIMESTAMPTZ")) {
			retval=Types.TIMESTAMP;
		}
		if (ctype.equals("TIMESTAMPTZ_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INTERVAL")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INTERVAL_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("NUMERIC_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("TIMETZ")) {
			retval=Types.TIME;
		}
		if (ctype.equals("TIMETZ_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BIT_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("VARBIT")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("VARBIT_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REFCURSOR")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REFCURSOR_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGPROCEDURE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGOPER")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGOPERATOR")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGCLASS")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGTYPE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGPROCEDURE_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGOPER_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGOPERATOR_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGCLASS_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("REGTYPE_ARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("RECORD")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("CSTRING")) {
			retval=Types.CHAR;
		}
		if (ctype.equals("ANY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("ANYARRAY")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("TRIGGER")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("LANGUAGE_HANDLER")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("INTERNAL")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("OPAQUE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("ANYELEMENT")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("PG_TYPE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("PG_ATTRIBUTE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("PG_PROC")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("PG_CLASS")) {
			retval=Types.BINARY;
		}
		// none added by sqlite
		// added by sqlserver
		if (ctype.equals("UBIGINT")) {
			retval=Types.BIGINT;
		}
		if (ctype.equals("UNIQUEIDENTIFIER")) {
			retval=Types.BINARY;
		}
		// added by informix
		if (ctype.equals("SMALLFLOAT")) {
			retval=Types.FLOAT;
		}
		if (ctype.equals("BYTE")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BOOLEAN")) {
			retval=Types.CHAR;
		}
		// also added by mysql
		if (ctype.equals("TINYTEXT")) {
			retval=Types.LONGVARCHAR;
		}
		if (ctype.equals("MEDIUMTEXT")) {
			retval=Types.LONGVARCHAR;
		}
		if (ctype.equals("LONGTEXT")) {
			retval=Types.LONGVARCHAR;
		}
		if (ctype.equals("JSON")) {
			retval=Types.LONGVARCHAR;
		}
		if (ctype.equals("GEOMETRY")) {
			retval=Types.BINARY;
		}
		// also added by oracle
		if (ctype.equals("SDO_GEOMETRY")) {
			retval=Types.BINARY;
		}
		// added by mssql
		if (ctype.equals("NCHAR")) {
			retval=Types.NCHAR;
		}
		if (ctype.equals("NVARCHAR")) {
			retval=Types.NVARCHAR;
		}
		if (ctype.equals("NTEXT")) {
			retval=Types.LONGNVARCHAR;
		}
		if (ctype.equals("XML")) {
			retval=Types.LONGVARCHAR;
		}
		if (ctype.equals("DATETIMEOFFSET")) {
			retval=Types.TIMESTAMP;
		}
		debugPrintln("  sql type: "+retval);
		return retval;
	}

	public String 	getColumnTypeName(int column) {
		debugFunction();
		String	typename=sqlrcur.getColumnType(column-1);
		debugPrintln("  column: "+column);
		debugPrintln("  type name: "+typename);
		return typename;
	}

	public int 	getPrecision(int column) {
		debugFunction();
		int	precision=(int)sqlrcur.getColumnPrecision(column-1);
		debugPrintln("  column: "+column);
		debugPrintln("  precision (before): "+precision);
		if (precision==-1) {
			precision=2147483647;
		}
		debugPrintln("  precision (after): "+precision);
		return precision;
	}

	public int 	getScale(int column) {
		debugFunction();
		int	scale=(int)sqlrcur.getColumnScale(column-1);
		debugPrintln("  column: "+column);
		debugPrintln("  scale: "+scale);
		return scale;
	}

	public String 	getSchemaName(int column) {
		debugFunction();
		String	schemaname="";
		debugPrintln("  column: "+column);
		debugPrintln("  schema name: "+schemaname);
		return schemaname;
	}

	public String 	getTableName(int column) {
		debugFunction();
		String	tablename="";
		debugPrintln("  column: "+column);
		debugPrintln("  table name: "+tablename);
		// FIXME: this could be implemented if
		// getColumnTable was exposed
		return tablename;
	}

	public boolean 	isAutoIncrement(int column) {
		debugFunction();
		boolean	isautoinc=sqlrcur.getColumnIsAutoIncrement(column-1);
		debugPrintln("  column: "+column);
		debugPrintln("  is auto increment: "+isautoinc);
		return isautoinc;
	}

	public boolean 	isCaseSensitive(int column) {
		debugFunction();
		// FIXME: can db type tell us this?
		boolean	iscasesensitive=false;
		debugPrintln("  column: "+column);
		debugPrintln("  is case sensitive: "+iscasesensitive);
		return iscasesensitive;
	}

	public boolean 	isCurrency(int column) {
		debugFunction();
		String	ctype=sqlrcur.getColumnType(column-1);
		boolean	iscurrency=ctype.equals("MONEY") ||
					ctype.equals("SMALLMONEY") ||
					ctype.equals("MONEY_ARRAY");
		debugPrintln("  column: "+column);
		debugPrintln("  ctype: "+ctype);
		debugPrintln("  is currency: "+iscurrency);
		return iscurrency;
	}

	public boolean 	isDefinitelyWritable(int column) {
		debugFunction();
		boolean	isdefinitelywriteable=false;
		debugPrintln("  column: "+column);
		debugPrintln("  is definitely writeable: "+
					isdefinitelywriteable);
		return isdefinitelywriteable;
	}

	public int 	isNullable(int column) {
		debugFunction();
		int	isnullable=(sqlrcur.getColumnIsNullable(column-1))?
						columnNullable:columnNoNulls;
		debugPrintln("  column: "+column);
		debugPrintln("  is nullable: "+isnullable);
		return isnullable;
	}

	public boolean 	isReadOnly(int column) {
		debugFunction();
		boolean	isreadonly=false;
		debugPrintln("  column: "+column);
		debugPrintln("  is read-only: "+isreadonly);
		return isreadonly;
	}

	public boolean 	isSearchable(int column) {
		debugFunction();
		boolean	issearchable=true;
		debugPrintln("  column: "+column);
		debugPrintln("  is searchable: "+issearchable);
		return issearchable;
	}

	public boolean 	isSigned(int column) {
		debugFunction();
		boolean	issigned=!sqlrcur.getColumnIsUnsigned(column-1);
		debugPrintln("  column: "+column);
		debugPrintln("  is signed: "+issigned);
		return issigned;
	}

	public boolean 	isWritable(int column) {
		debugFunction();
		boolean	iswriteable=true;
		debugPrintln("  column: "+column);
		debugPrintln("  is writeable: "+iswriteable);
		return iswriteable;
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
