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
		String	catalogname=null;
		debugPrintln("  column: "+column);
		debugPrintln("  catalog name: "+catalogname);
		return catalogname;
	}

	public String 	getColumnClassName(int column) {
		debugFunction();
		String	columnclassname=null;
		debugPrintln("  column: "+column);
		debugPrintln("  column class name: "+columnclassname);
		return columnclassname;
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
		debugPrintln("  longest: "+longest);
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
		if (ctype.equals("TINYBLOB")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("MEDIUMBLOB")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("LONGBLOB")) {
			retval=Types.BINARY;
		}
		if (ctype.equals("BLOB")) {
			retval=Types.BINARY;
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
		debugPrintln("  precision: "+precision);
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
		String	schemaname=null;
		debugPrintln("  column: "+column);
		debugPrintln("  schema name: "+schemaname);
		return schemaname;
	}

	public String 	getTableName(int column) {
		debugFunction();
		String	tablename=null;
		debugPrintln("  column: "+column);
		debugPrintln("  table name: "+tablename);
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
		// FIXME: map this
		boolean	iscurrency=false;
		debugPrintln("  column: "+column);
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
		boolean	issearchable=false;
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
		boolean	iswriteable=false;
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
