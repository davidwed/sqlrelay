using System;
using System.Data;
using System.Globalization;

namespace SQLRClient
{
    public class SQLRelayDataReader : IDataReader
    {

        #region member variables

        private bool _open = true;
        private SQLRelayConnection _sqlrelaycon = null;
        private SQLRCursor _sqlrcur = null;
        private bool _endsession = false;
        private bool _unfetched = true;
        private ulong _currentrow = 0;
        private bool[] _havevalues = null;
        private object[] _values = null;

        #endregion


        #region constructors and destructors

        internal SQLRelayDataReader(SQLRelayConnection sqlrelaycon, SQLRCursor sqlrcur, bool endsession)
        {
            _sqlrelaycon = sqlrelaycon;
            _sqlrcur = sqlrcur;
            _endsession = endsession;
        }

        void IDisposable.Dispose()
        {
            this.Dispose(true);
            System.GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (disposing)
            {
                try
                {
                    this.Close();
                }
                catch (Exception e)
                {
                    throw new SystemException("An exception of type " + e.GetType() +
                                              " was encountered while closing the SQLRelayDataReader.");
                }
            }
        }

        #endregion


        #region properties

        public int Depth
        {
            // FIXME: Return 0 if nesting isn't supported.  What is nesting?
            get
            {
                return 0;
            }
        }

        public bool IsClosed
        {
            get
            {
                return !_open;
            }
        }

        public int RecordsAffected
        {
            get
            {
                return (int)_sqlrcur.affectedRows();
            }
        }

        #endregion


        #region public methods

        public void Close()
        {
            // FIXME: clean up anything?
            _open = false;
        }

        public bool NextResult()
        {
            // SQL Relay doesn't support multiple result sets
            return false;
        }

        public bool Read()
        {
            // we need to move to the next row and see if it's valid
            // if we haven't fetched anything yet then the next row is 0
            // if we have, then the next row is just the current row + 1
            if (_unfetched)
            {
                _unfetched = false;
                _currentrow = 0;
            }
            else
            {
                _currentrow++;
            }
            
            // re-init the value cache
            _havevalues = new bool[FieldCount];
            for (int i = 0; i < FieldCount; i++)
            {
                _havevalues[i] = false;
            }
            _values = new object[FieldCount];

            // this will need to be updated if getNullsAsNulls is exposed
            return (_sqlrcur.getField(_currentrow, (uint)0) != null);
        }

        public DataTable GetSchemaTable()
        {

            DataTable datatable = new DataTable();

            datatable.Columns.Add("ColumnName", typeof(String));
            datatable.Columns.Add("ColumnOrdinal", typeof(UInt32));
            datatable.Columns.Add("ColumnSize", typeof(UInt32));
            datatable.Columns.Add("NumericPrecision", typeof(UInt32));
            datatable.Columns.Add("NumericScale", typeof(UInt32));
            datatable.Columns.Add("IsUnique", typeof(Boolean));
            datatable.Columns.Add("IsKey", typeof(Boolean));
            datatable.Columns.Add("BaseServerName", typeof(String));
            datatable.Columns.Add("BaseCatalogName", typeof(String));
            datatable.Columns.Add("BaseColumnName", typeof(String));
            datatable.Columns.Add("BaseSchemaName", typeof(String));
            datatable.Columns.Add("BaseTableName", typeof(String));
            datatable.Columns.Add("DataType", typeof(Type));
            datatable.Columns.Add("AllowDBNull", typeof(Boolean));
            datatable.Columns.Add("ProviderType", typeof(String));
            datatable.Columns.Add("IsAliased", typeof(Boolean));
            datatable.Columns.Add("IsExpression", typeof(Boolean));
            datatable.Columns.Add("IsIdentity", typeof(Boolean));
            datatable.Columns.Add("IsAutoIncrement", typeof(Boolean));
            datatable.Columns.Add("IsRowVersion", typeof(Boolean));
            datatable.Columns.Add("IsHidden", typeof(Boolean));
            datatable.Columns.Add("IsLong", typeof(Boolean));
            datatable.Columns.Add("IsReadOnly", typeof(Boolean));
            datatable.Columns.Add("ProviderSpecificDataType", typeof(String));
            datatable.Columns.Add("DataTypeName", typeof(String));
            datatable.Columns.Add("XmlSchemaCollectionDatabase", typeof(String));
            datatable.Columns.Add("XmlSchemaCollectionOwningSchema", typeof(String));
            datatable.Columns.Add("XmlSchemaCollectionName", typeof(String));

            datatable.BeginLoadData();
            for (uint i=0; i<FieldCount; i++)
            {

                DataRow row = datatable.NewRow();
                
                row["ColumnName"] = GetName((int)i);
                row["ColumnOrdinal"] = i;
                row["ColumnSize"] = _sqlrcur.getColumnLength(i);
                row["NumericPrecision"] = _sqlrcur.getColumnPrecision(i);
                row["NumericScale"] = _sqlrcur.getColumnScale(i);
                row["IsUnique"] = _sqlrcur.getColumnIsUnique(i);
                row["IsKey"] = _sqlrcur.getColumnIsPrimaryKey(i) || _sqlrcur.getColumnIsPartOfKey(i);
                row["BaseServerName"] = null;
                row["BaseCatalogName"] = null;
                row["BaseColumnName"] = GetName((int)i);
                row["BaseSchemaName"] = null;
                row["BaseTableName"] = null;
                row["DataType"] = null; // FIXME
                row["AllowDBNull"] = _sqlrcur.getColumnIsNullable(i);
                row["ProviderType"] = _sqlrcur.getColumnType(i);
                row["IsAliased"] = false;
                row["IsExpression"] = false;
                row["IsIdentity"] = false;
                row["IsAutoIncrement"] = _sqlrcur.getColumnIsAutoIncrement(i);
                row["IsRowVersion"] = false;
                row["IsHidden"] = false;
                row["IsLong"] = false; // FIXME
                row["IsReadOnly"] = false;
                row["ProviderSpecificDataType"] = _sqlrcur.getColumnType(i);
                row["DataTypeName"] = _sqlrcur.getColumnType(i);
                row["XmlSchemaCollectionDatabase"] = null;
                row["XmlSchemaCollectionOwningSchema"] = null;
                row["XmlSchemaCollectionName"] = null;

                datatable.Rows.Add(row);
                datatable.AcceptChanges();
            }
            datatable.EndLoadData();

            return datatable;
        }

        public int FieldCount
        {
            get
            {
                return (int)_sqlrcur.colCount();
            }
        }

        public String GetName(int i)
        {
            invalidColumnIndex(i);
            return _sqlrcur.getColumnName((uint)i);
        }

        public String GetDataTypeName(int i)
        {
            invalidColumnIndex(i);
            return _sqlrcur.getColumnType((uint)i);
        }

        public Type GetFieldType(int i)
        {
            String type = GetDataTypeName(i);

            if (type == "UNKNOWN")
            {
                return typeof(string);
            }
            // addded by freetds
            else if (type == "CHAR")                        // 1
            {
                return typeof(string);
            }
            else if (type == "INT")
            {
                return typeof(Int32);
            }
            else if (type == "SMALLINT")
            {
                return typeof(Int16);
            }
            else if (type == "TINYINT")
            {
                return typeof(Int16);
            }
            else if (type == "MONEY")
            {
                return typeof(Decimal);
            }
            else if (type == "DATETIME")
            {
                return typeof(DateTime);
            }
            else if (type == "NUMERIC")
            {
                return typeof(Decimal);
            }
            else if (type == "DECIMAL")
            {
                return typeof(Decimal);
            }
            else if (type == "SMALLDATETIME")
            {
                return typeof(DateTime);
            }
            else if (type == "SMALLMONEY")
            {
                return typeof(Decimal);
            }
            else if (type == "IMAGE")
            {
                return typeof(byte[]);
            }
            else if (type == "BINARY")
            {
                return typeof(byte[]);
            }
            else if (type == "BIT")
            {
                return typeof(Boolean);
            }
            else if (type == "REAL")
            {
                return typeof(Double);
            }
            else if (type == "FLOAT")
            {
                return typeof(Single);
            }
            else if (type == "TEXT")
            {
                return typeof(string);
            }
            else if (type == "VARCHAR")
            {
                return typeof(string);
            }
            else if (type == "VARBINARY")
            {
                return typeof(byte[]);
            }
            else if (type == "LONGCHAR")
            {
                return typeof(string);
            }
            else if (type == "LONGBINARY")
            {
                return typeof(byte[]);
            }
            else if (type == "LONG")
            {
                return typeof(byte[]);
            }
            else if (type == "ILLEGAL")
            {
                return typeof(byte[]);
            }
            else if (type == "SENSITIVITY")
            {
                return typeof(byte[]);
            }
            else if (type == "BOUNDARY")
            {
                return typeof(byte[]);
            }
            else if (type == "VOID")
            {
                return typeof(byte[]);
            }
            else if (type == "USHORT")
            {
                return typeof(UInt16);
            }
            // added by lago
            else if (type == "UNDEFINED")            // 27
            {
                return typeof(byte[]);
            }
            else if (type == "DOUBLE")
            {
                return typeof(Double);
            }
            else if (type == "DATE")
            {
                return typeof(DateTime);
            }
            else if (type == "TIME")
            {
                return typeof(DateTime);
            }
            else if (type == "TIMESTAMP")
            {
                return typeof(DateTime);
            }
            // added by msql
            else if (type == "UINT")                        // 32
            {
                return typeof(UInt32);
            }
            else if (type == "LASTREAL")
            {
                return typeof(Double);
            }
            // added by mysql
            else if (type == "STRING")            // 34
            {
                return typeof(string);
            }
            else if (type == "VARSTRING")
            {
                return typeof(string);
            }
            else if (type == "LONGLONG")
            {
                return typeof(Int64);
            }
            else if (type == "MEDIUMINT")
            {
                return typeof(Int32);
            }
            else if (type == "YEAR")
            {
                return typeof(Int64);
            }
            else if (type == "NEWDATE")
            {
                return typeof(DateTime);
            }
            else if (type == "NULL")
            {
                return null;
            }
            else if (type == "ENUM")
            {
                return typeof(byte[]);
            }
            else if (type == "SET")
            {
                return typeof(byte[]);
            }
            else if (type == "TINYBLOB")
            {
                return typeof(byte[]);
            }
            else if (type == "MEDIUMBLOB")
            {
                return typeof(byte[]);
            }
            else if (type == "LONGBLOB")
            {
                return typeof(byte[]);
            }
            else if (type == "BLOB")
            {
                return typeof(byte[]);
            }
            // added by oracle
            else if (type == "VARCHAR2")            // 47
            {
                return typeof(string);
            }
            else if (type == "NUMBER")
            {
                // Numbers witout scale are integers.  However,
                // occasionally integers will come back with length
                // and scale but no precision.  Oracle does this.
                // So, check for lack of either.
                if (_sqlrcur.getColumnScale((uint)i) == 0 ||
                    _sqlrcur.getColumnPrecision((uint)i) == 0)
                {
                    return typeof(Int64);
                }
                else
                {
                    return typeof(Decimal);
                }
            }
            else if (type == "ROWID")
            {
                return typeof(UInt64);
            }
            else if (type == "RAW")
            {
                return typeof(byte[]);
            }
            else if (type == "LONG_RAW")
            {
                return typeof(byte[]);
            }
            else if (type == "MLSLABEL")
            {
                return typeof(byte[]);
            }
            else if (type == "CLOB")
            {
                return typeof(string);
            }
            else if (type == "BFILE")
            {
                return typeof(byte[]);
            }
            // added by odbc
            else if (type == "BIGINT")            // 55
            {
                return typeof(Int64);
            }
            else if (type == "INTEGER")
            {
                return typeof(Int32);
            }
            else if (type == "LONGVARBINARY")
            {
                return typeof(byte[]);
            }
            else if (type == "LONGVARCHAR")
            {
                return typeof(string);
            }
            // added by db2
            else if (type == "GRAPHIC")            // 59
            {
                return typeof(byte[]);
            }
            else if (type == "VARGRAPHIC")
            {
                return typeof(byte[]);
            }
            else if (type == "LONGVARGRAPHIC")
            {
                return typeof(byte[]);
            }
            else if (type == "DBCLOB")
            {
                return typeof(string);
            }
            else if (type == "DATALINK")
            {
                return typeof(byte[]);
            }
            else if (type == "USER_DEFINED_TYPE")
            {
                return typeof(byte[]);
            }
            else if (type == "SHORT_DATATYPE")
            {
                return typeof(Int16);
            }
            else if (type == "TINY_DATATYPE")
            {
                return typeof(Int16);
            }
            // added by firebird
            else if (type == "D_FLOAT")            // 67
            {
                return typeof(Single);
            }
            else if (type == "ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "QUAD")
            {
                return typeof(UInt64);
            }
            else if (type == "INT64")
            {
                return typeof(Int64);
            }
            else if (type == "DOUBLE PRECISION")
            {
                return typeof(Double);
            }
            // added by postgresql
            else if (type == "BOOL")
            {
                return typeof(Boolean);
            }
            else if (type == "BYTEA")
            {
                return typeof(byte[]);
            }
            else if (type == "NAME")
            {
                return typeof(string);
            }
            else if (type == "INT8")
            {
                return typeof(Int64);
            }
            else if (type == "INT2")
            {
                return typeof(Int16);
            }
            else if (type == "INT2VECTOR")
            {
                return typeof(byte[]);
            }
            else if (type == "INT4")
            {
                return typeof(Int32);
            }
            else if (type == "REGPROC")
            {
                return typeof(byte[]);
            }
            else if (type == "OID")
            {
                return typeof(Int64);
            }
            else if (type == "TID")
            {
                return typeof(Int64);
            }
            else if (type == "XID")
            {
                return typeof(Int64);
            }
            else if (type == "CID")
            {
                return typeof(Int64);
            }
            else if (type == "OIDVECTOR")
            {
                return typeof(byte[]);
            }
            else if (type == "SMGR")
            {
                return typeof(byte[]);
            }
            else if (type == "POINT")
            {
                return typeof(byte[]);
            }
            else if (type == "LSEG")
            {
                return typeof(byte[]);
            }
            else if (type == "PATH")
            {
                return typeof(byte[]);
            }
            else if (type == "BOX")
            {
                return typeof(byte[]);
            }
            else if (type == "POLYGON")
            {
                return typeof(byte[]);
            }
            else if (type == "LINE")
            {
                return typeof(byte[]);
            }
            else if (type == "LINE_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "FLOAT4")
            {
                return typeof(Single);
            }
            else if (type == "FLOAT8")
            {
                return typeof(Double);
            }
            else if (type == "ABSTIME")
            {
                // I guess...
                return typeof(Int64);
            }
            else if (type == "RELTIME")
            {
                // I guess...
                return typeof(Int64);
            }
            else if (type == "TINTERVAL")
            {
                return typeof(Int64);
            }
            else if (type == "CIRCLE")
            {
                return typeof(byte[]);
            }
            else if (type == "CIRCLE_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "MONEY_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "MACADDR")
            {
                // not sure what to do with a mac addr, presumably it's a 4 byte array
                return typeof(byte[]);
            }
            else if (type == "INET")
            {
                return typeof(byte[]);
            }
            else if (type == "CIDR")
            {
                return typeof(byte[]);
            }
            else if (type == "BOOL_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "BYTEA_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "CHAR_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "NAME_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "INT2_ARRAY")
            {
                // ???
                return typeof(byte[]);
            }
            else if (type == "INT2VECTOR_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "INT4_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "REGPROC_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "TEXT_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "OID_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "TID_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "XID_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "CID_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "OIDVECTOR_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "BPCHAR_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "VARCHAR_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "INT8_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "POINT_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "LSEG_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "PATH_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "BOX_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "FLOAT4_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "FLOAT8_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "ABSTIME_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "RELTIME_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "TINTERVAL_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "POLYGON_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "ACLITEM")
            {
                return typeof(byte[]);
            }
            else if (type == "ACLITEM_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "MACADDR_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "INET_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "CIDR_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "BPCHAR")
            {
                return typeof(string);
            }
            else if (type == "TIMESTAMP_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "DATE_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "TIME_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "TIMESTAMPTZ")
            {
                return typeof(byte[]);
            }
            else if (type == "TIMESTAMPTZ_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "INTERVAL")
            {
                return typeof(byte[]);
            }
            else if (type == "INTERVAL_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "NUMERIC_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "TIMETZ")
            {
                return typeof(byte[]);
            }
            else if (type == "TIMETZ_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "BIT_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "VARBIT")
            {
                return typeof(byte[]);
            }
            else if (type == "VARBIT_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "REFCURSOR")
            {
                return typeof(byte[]);
            }
            else if (type == "REFCURSOR_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "REGPROCEDURE")
            {
                return typeof(byte[]);
            }
            else if (type == "REGOPER")
            {
                return typeof(byte[]);
            }
            else if (type == "REGOPERATOR")
            {
                return typeof(byte[]);
            }
            else if (type == "REGCLASS")
            {
                return typeof(byte[]);
            }
            else if (type == "REGTYPE")
            {
                return typeof(byte[]);
            }
            else if (type == "REGPROCEDURE_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "REGOPER_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "REGOPERATOR_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "REGCLASS_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "REGTYPE_ARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "RECORD")
            {
                return typeof(byte[]);
            }
            else if (type == "CSTRING")
            {
                return typeof(string);
            }
            else if (type == "ANY")
            {
                return typeof(byte[]);
            }
            else if (type == "ANYARRAY")
            {
                return typeof(byte[]);
            }
            else if (type == "TRIGGER")
            {
                return typeof(byte[]);
            }
            else if (type == "LANGUAGE_HANDLER")
            {
                return typeof(byte[]);
            }
            else if (type == "INTERNAL")
            {
                return typeof(byte[]);
            }
            else if (type == "OPAQUE")
            {
                return typeof(byte[]);
            }
            else if (type == "ANYELEMENT")
            {
                return typeof(byte[]);
            }
            else if (type == "PG_TYPE")
            {
                return typeof(byte[]);
            }
            else if (type == "PG_ATTRIBUTE")
            {
                return typeof(byte[]);
            }
            else if (type == "PG_PROC")
            {
                return typeof(byte[]);
            }
            else if (type == "PG_CLASS")
            {
                return typeof(byte[]);
            }
            // none added by sqlite

            // unrecognized type
            return typeof(byte[]);
        }

        public Object GetValue(int i)
        {
            invalidColumnIndex(i);
            
            // return the value from the cache, if we have it
            if (_havevalues[i])
            {
                return _values[i];
            }

            // get the field
            object retval = convertField(_sqlrcur.getFieldAsByteArray(_currentrow, (uint)i), GetDataTypeName(i), _sqlrcur.getColumnPrecision((uint)i), _sqlrcur.getColumnScale((uint)i));

            // cache the value
            _havevalues[i] = true;
            _values[i] = retval;

            // return the value
            return retval;
        }

        public static Object convertField(byte[] field, string type, uint precision, uint scale)
        {

            // convert the field to a native type...

            if (type == "UNKNOWN")
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            // addded by freetds
            else if (type == "CHAR")                        // 1
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "INT")
            {
                return Convert.ToInt32(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "SMALLINT")
            {
                return Convert.ToInt16(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "TINYINT")
            {
                return Convert.ToInt16(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "MONEY")
            {
                return Convert.ToDecimal(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "DATETIME")
            {
                return DateTime.Parse(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "NUMERIC")
            {
                return Convert.ToDecimal(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "DECIMAL")
            {
                return Convert.ToDecimal(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "SMALLDATETIME")
            {
                return DateTime.Parse(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "SMALLMONEY")
            {
                return Convert.ToDecimal(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "IMAGE")
            {
                return field;
            }
            else if (type == "BINARY")
            {
                return field;
            }
            else if (type == "BIT")
            {
                return Convert.ToBoolean(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "REAL")
            {
                return Convert.ToDouble(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "FLOAT")
            {
                return Convert.ToSingle(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "TEXT")
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "VARCHAR")
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "VARBINARY")
            {
                return field;
            }
            else if (type == "LONGCHAR")
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "LONGBINARY")
            {
                return field;
            }
            else if (type == "LONG")
            {
                return field;
            }
            else if (type == "ILLEGAL")
            {
                return field;
            }
            else if (type == "SENSITIVITY")
            {
                return field;
            }
            else if (type == "BOUNDARY")
            {
                return field;
            }
            else if (type == "VOID")
            {
                return null;
            }
            else if (type == "USHORT")
            {
                return Convert.ToUInt16(System.Text.Encoding.Default.GetString(field));
            }
            // added by lago
            else if (type == "UNDEFINED")            // 27
            {
                return field;
            }
            else if (type == "DOUBLE")
            {
                return Convert.ToDouble(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "DATE")
            {
                return DateTime.Parse(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "TIME")
            {
                return DateTime.Parse(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "TIMESTAMP")
            {
                return DateTime.Parse(System.Text.Encoding.Default.GetString(field));
            }
            // added by msql
            else if (type == "UINT")                        // 32
            {
                return Convert.ToUInt32(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "LASTREAL")
            {
                return Convert.ToDouble(System.Text.Encoding.Default.GetString(field));
            }
            // added by mysql
            else if (type == "STRING")            // 34
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "VARSTRING")
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "LONGLONG")
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "MEDIUMINT")
            {
                return Convert.ToInt32(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "YEAR")
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "NEWDATE")
            {
                return DateTime.Parse(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "NULL")
            {
                return null;
            }
            else if (type == "ENUM")
            {
                return field;
            }
            else if (type == "SET")
            {
                return field;
            }
            else if (type == "TINYBLOB")
            {
                return field;
            }
            else if (type == "MEDIUMBLOB")
            {
                return field;
            }
            else if (type == "LONGBLOB")
            {
                return field;
            }
            else if (type == "BLOB")
            {
                return field;
            }
            // added by oracle
            else if (type == "VARCHAR2")            // 47
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "NUMBER")
            {
                // Numbers witout scale are integers.  However,
                // occasionally integers will come back with length
                // and scale but no precision.  Oracle does this.
                // So, check for lack of either.
                if (scale == 0 || precision == 0)
                {
                    return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
                }
                else
                {
                    return Convert.ToDecimal(System.Text.Encoding.Default.GetString(field));
                }
            }
            else if (type == "ROWID")
            {
                return Convert.ToUInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "RAW")
            {
                return field;
            }
            else if (type == "LONG_RAW")
            {
                return field;
            }
            else if (type == "MLSLABEL")
            {
                return field;
            }
            else if (type == "CLOB")
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "BFILE")
            {
                return field;
            }
            // added by odbc
            else if (type == "BIGINT")            // 55
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "INTEGER")
            {
                return Convert.ToInt32(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "LONGVARBINARY")
            {
                return field;
            }
            else if (type == "LONGVARCHAR")
            {
                return Convert.ToString(System.Text.Encoding.Default.GetString(field));
            }
            // added by db2
            else if (type == "GRAPHIC")            // 59
            {
                return field;
            }
            else if (type == "VARGRAPHIC")
            {
                return field;
            }
            else if (type == "LONGVARGRAPHIC")
            {
                return field;
            }
            else if (type == "DBCLOB")
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "DATALINK")
            {
                return field;
            }
            else if (type == "USER_DEFINED_TYPE")
            {
                return field;
            }
            else if (type == "SHORT_DATATYPE")
            {
                return Convert.ToInt16(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "TINY_DATATYPE")
            {
                return Convert.ToInt16(System.Text.Encoding.Default.GetString(field));
            }
            // added by firebird
            else if (type == "D_FLOAT")            // 67
            {
                return Convert.ToSingle(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "ARRAY")
            {
                return field;
            }
            else if (type == "QUAD")
            {
                return Convert.ToUInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "INT64")
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "DOUBLE PRECISION")
            {
                return Convert.ToDouble(System.Text.Encoding.Default.GetString(field));
            }
            // added by postgresql
            else if (type == "BOOL")
            {
                return Convert.ToBoolean(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "BYTEA")
            {
                return field;
            }
            else if (type == "NAME")
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "INT8")
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "INT2")
            {
                return Convert.ToInt16(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "INT2VECTOR")
            {
                return field;
            }
            else if (type == "INT4")
            {
                return Convert.ToInt32(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "REGPROC")
            {
                return field;
            }
            else if (type == "OID")
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "TID")
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "XID")
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "CID")
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "OIDVECTOR")
            {
                return field;
            }
            else if (type == "SMGR")
            {
                return field;
            }
            else if (type == "POINT")
            {
                return field;
            }
            else if (type == "LSEG")
            {
                return field;
            }
            else if (type == "PATH")
            {
                return field;
            }
            else if (type == "BOX")
            {
                return field;
            }
            else if (type == "POLYGON")
            {
                return field;
            }
            else if (type == "LINE")
            {
                return field;
            }
            else if (type == "LINE_ARRAY")
            {
                return field;
            }
            else if (type == "FLOAT4")
            {
                return Convert.ToSingle(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "FLOAT8")
            {
                return Convert.ToDouble(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "ABSTIME")
            {
                // I guess...
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "RELTIME")
            {
                // I guess...
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "TINTERVAL")
            {
                return Convert.ToInt64(System.Text.Encoding.Default.GetString(field));
            }
            else if (type == "CIRCLE")
            {
                return field;
            }
            else if (type == "CIRCLE_ARRAY")
            {
                return field;
            }
            else if (type == "MONEY_ARRAY")
            {
                return field;
            }
            else if (type == "MACADDR")
            {
                // not sure what to do with a mac addr, presumably it's a 4 byte array
                return field;
            }
            else if (type == "INET")
            {
                return field;
            }
            else if (type == "CIDR")
            {
                return field;
            }
            else if (type == "BOOL_ARRAY")
            {
                return field;
            }
            else if (type == "BYTEA_ARRAY")
            {
                return field;
            }
            else if (type == "CHAR_ARRAY")
            {
                return field;
            }
            else if (type == "NAME_ARRAY")
            {
                return field;
            }
            else if (type == "INT2_ARRAY")
            {
                // ???
                return field;
            }
            else if (type == "INT2VECTOR_ARRAY")
            {
                return field;
            }
            else if (type == "INT4_ARRAY")
            {
                return field;
            }
            else if (type == "REGPROC_ARRAY")
            {
                return field;
            }
            else if (type == "TEXT_ARRAY")
            {
                return field;
            }
            else if (type == "OID_ARRAY")
            {
                return field;
            }
            else if (type == "TID_ARRAY")
            {
                return field;
            }
            else if (type == "XID_ARRAY")
            {
                return field;
            }
            else if (type == "CID_ARRAY")
            {
                return field;
            }
            else if (type == "OIDVECTOR_ARRAY")
            {
                return field;
            }
            else if (type == "BPCHAR_ARRAY")
            {
                return field;
            }
            else if (type == "VARCHAR_ARRAY")
            {
                return field;
            }
            else if (type == "INT8_ARRAY")
            {
                return field;
            }
            else if (type == "POINT_ARRAY")
            {
                return field;
            }
            else if (type == "LSEG_ARRAY")
            {
                return field;
            }
            else if (type == "PATH_ARRAY")
            {
                return field;
            }
            else if (type == "BOX_ARRAY")
            {
                return field;
            }
            else if (type == "FLOAT4_ARRAY")
            {
                return field;
            }
            else if (type == "FLOAT8_ARRAY")
            {
                return field;
            }
            else if (type == "ABSTIME_ARRAY")
            {
                return field;
            }
            else if (type == "RELTIME_ARRAY")
            {
                return field;
            }
            else if (type == "TINTERVAL_ARRAY")
            {
                return field;
            }
            else if (type == "POLYGON_ARRAY")
            {
                return field;
            }
            else if (type == "ACLITEM")
            {
                return field;
            }
            else if (type == "ACLITEM_ARRAY")
            {
                return field;
            }
            else if (type == "MACADDR_ARRAY")
            {
                return field;
            }
            else if (type == "INET_ARRAY")
            {
                return field;
            }
            else if (type == "CIDR_ARRAY")
            {
                return field;
            }
            else if (type == "BPCHAR")
            {
                return System.Text.Encoding.Default.GetString(field);
            }
            else if (type == "TIMESTAMP_ARRAY")
            {
                return field;
            }
            else if (type == "DATE_ARRAY")
            {
                return field;
            }
            else if (type == "TIME_ARRAY")
            {
                return field;
            }
            else if (type == "TIMESTAMPTZ")
            {
                return field;
            }
            else if (type == "TIMESTAMPTZ_ARRAY")
            {
                return field;
            }
            else if (type == "INTERVAL")
            {
                return field;
            }
            else if (type == "INTERVAL_ARRAY")
            {
                return field;
            }
            else if (type == "NUMERIC_ARRAY")
            {
                return field;
            }
            else if (type == "TIMETZ")
            {
                return field;
            }
            else if (type == "TIMETZ_ARRAY")
            {
                return field;
            }
            else if (type == "BIT_ARRAY")
            {
                return field;
            }
            else if (type == "VARBIT")
            {
                return field;
            }
            else if (type == "VARBIT_ARRAY")
            {
                return field;
            }
            else if (type == "REFCURSOR")
            {
                return field;
            }
            else if (type == "REFCURSOR_ARRAY")
            {
                return field;
            }
            else if (type == "REGPROCEDURE")
            {
                return field;
            }
            else if (type == "REGOPER")
            {
                return field;
            }
            else if (type == "REGOPERATOR")
            {
                return field;
            }
            else if (type == "REGCLASS")
            {
                return field;
            }
            else if (type == "REGTYPE")
            {
                return field;
            }
            else if (type == "REGPROCEDURE_ARRAY")
            {
                return field;
            }
            else if (type == "REGOPER_ARRAY")
            {
                return field;
            }
            else if (type == "REGOPERATOR_ARRAY")
            {
                return field;
            }
            else if (type == "REGCLASS_ARRAY")
            {
                return field;
            }
            else if (type == "REGTYPE_ARRAY")
            {
                return field;
            }
            else if (type == "RECORD")
            {
                return field;
            }
            else if (type == "CSTRING")
            {
                return Convert.ToString(field);
            }
            else if (type == "ANY")
            {
                return field;
            }
            else if (type == "ANYARRAY")
            {
                return field;
            }
            else if (type == "TRIGGER")
            {
                return field;
            }
            else if (type == "LANGUAGE_HANDLER")
            {
                return field;
            }
            else if (type == "INTERNAL")
            {
                return field;
            }
            else if (type == "OPAQUE")
            {
                return field;
            }
            else if (type == "ANYELEMENT")
            {
                return field;
            }
            else if (type == "PG_TYPE")
            {
                return field;
            }
            else if (type == "PG_ATTRIBUTE")
            {
                return field;
            }
            else if (type == "PG_PROC")
            {
                return field;
            }
            else if (type == "PG_CLASS")
            {
                return field;
            }
            // none added by sqlite

            // unrecognized type
            return field;
        }

        public int GetValues(object[] values)
        {
            int colcount=(int)_sqlrcur.colCount();
            int i = 0;
            for (; i < colcount && i < values.Length; i++)
            {
                values[i] = GetValue(i);
            }
            return i;
        }

        public int GetOrdinal(string name)
        {
            uint colcount = _sqlrcur.colCount();
            for (uint i = 0; i < colcount; i++)
            {
                if (cultureAwareCompare(name, _sqlrcur.getColumnName(i)) == 0)
                {
                    return (int)i;
                }
            }
            throw new IndexOutOfRangeException("Could not find specified column in results");
        }

        public object this[int i]
        {
            get
            {
                return GetValue(i);
            }
        }

        public object this[String name]
        {
            get
            {
                return GetValue(GetOrdinal(name));
            }
        }

        public bool GetBoolean(int i)
        {
            return Convert.ToBoolean(GetValue(i));
        }

        public byte GetByte(int i)
        {
            return Convert.ToByte(GetValue(i));
        }

        public long GetBytes(int i, long fieldoffset, byte[] buffer, int bufferoffset, int length)
        {

            // get the field
            byte[] field = _sqlrcur.getFieldAsByteArray(_currentrow,(uint)i);

            // copy chars from the field into the buffer
            uint j = 0;
            while (j < length && fieldoffset + j < field.Length)
            {
                buffer[bufferoffset + i] = field[(int)(fieldoffset + j)];
                j++;
            }
            return (long)j;
        }

        public char GetChar(int i)
        {
            return Convert.ToChar(GetValue(i));
        }

        public long GetChars(int i, long fieldoffset, char[] buffer, int bufferoffset, int length)
        {

            // get the field
            string field = GetString(i);

            // copy chars from the field into the buffer
            uint j = 0;
            while (j < length && fieldoffset + j < field.Length)
            {
                buffer[bufferoffset + i] = field[(int)(fieldoffset + j)];
                j++;
            }
            return (long)j;
        }

        public Guid GetGuid(int i)
        {
            return (Guid)GetValue(i);
        }

        public Int16 GetInt16(int i)
        {
            //return Convert.ToInt16(GetValue(i));
            return (Int16)_sqlrcur.getFieldAsInteger(_currentrow, (uint)i);
        }

        public Int32 GetInt32(int i)
        {
            //return Convert.ToInt32(GetValue(i));
            return (Int32)_sqlrcur.getFieldAsInteger(_currentrow, (uint)i);
        }

        public Int64 GetInt64(int i)
        {
            //return Convert.ToInt64(GetValue(i));
            return _sqlrcur.getFieldAsInteger(_currentrow, (uint)i);
        }

        public float GetFloat(int i)
        {
            //return (float)Convert.ToSingle(GetValue(i));
            return (float)_sqlrcur.getFieldAsDouble(_currentrow, (uint)i);
        }

        public double GetDouble(int i)
        {
            //return Convert.ToDouble(GetValue(i));
            return _sqlrcur.getFieldAsDouble(_currentrow, (uint)i);
        }

        public String GetString(int i)
        {
            //return Convert.ToString(GetValue(i));
            return _sqlrcur.getField(_currentrow, (uint)i);
        }

        public Decimal GetDecimal(int i)
        {
            return Convert.ToDecimal(GetValue(i));
        }

        public DateTime GetDateTime(int i)
        {
            return Convert.ToDateTime(GetValue(i));
        }

        public IDataReader GetData(int i)
        {
            // Normally, this would be used to expose nested tables and other hierarchical data.
            throw new NotSupportedException("GetData not supported.");
        }

        public bool IsDBNull(int i)
        {
            // FIXME: this will need to be modified if getNullsAsNulls is exposed
            return (GetString(i) == "");
        }

        #endregion


        #region private methods

        private int cultureAwareCompare(string strA, string strB)
        {
            return CultureInfo.CurrentCulture.CompareInfo.Compare(strA, strB, CompareOptions.IgnoreKanaType | CompareOptions.IgnoreWidth | CompareOptions.IgnoreCase);
        }

        private void invalidColumnIndex(int i)
        {
            if (i < 0 || i > FieldCount)
            {
                throw new IndexOutOfRangeException();
            }
        }

        #endregion
    }
}
