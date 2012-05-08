using System;
using System.Data;
using System.Globalization;

namespace SQLRClient
{
    public class SQLRelayDataReader : IDataReader
    {
        private bool _open = true;
        private SQLRelayConnection _sqlrelaycon = null;
        private SQLRCursor _sqlrcur = null;
        private bool _endsession = false;
        private bool _unfetched = true;
        private ulong _currentrow = 0;

        private bool[] _havevalues = null;
        private object[] _values = null;
        

        internal SQLRelayDataReader(SQLRelayConnection sqlrelaycon, SQLRCursor sqlrcur, bool endsession)
        {
            _sqlrelaycon = sqlrelaycon;
            _sqlrcur = sqlrcur;
            _endsession = endsession;
        }

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
            return (_values[0] != null);
        }

        public DataTable GetSchemaTable()
        {
            // FIXME: implement this
            throw new NotSupportedException();
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
            invalidColumnIndex(i);
            return GetValue(i).GetType();
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
            object field = _sqlrcur.getField(_currentrow, (uint)i);

            // convert as appropriate
            string type = GetDataTypeName(i);

            // map the field type a value in the .NET Type class
            object retval = null;
            if (type == "UNKNOWN")
            {
                retval = Convert.ToString(field);
            }
            // addded by freetds
            if (type == "CHAR")                        // 1
            {
                retval = Convert.ToString(field);
            }
            if (type == "INT")
            {
                retval = Convert.ToInt32(field);
            }
            if (type == "SMALLINT")
            {
                retval = Convert.ToInt16(field);
            }
            if (type == "TINYINT")
            {
                retval = Convert.ToChar(field);
            }
            if (type == "MONEY")
            {
                retval = Convert.ToDecimal(field);
            }
            if (type == "DATETIME")
            {
                retval = DateTime.Parse(Convert.ToString(field));
            }
            if (type == "NUMERIC")
            {
                retval = Convert.ToDecimal(retval);
            }
            if (type == "DECIMAL")
            {
                retval = Convert.ToDecimal(retval);
            }
            if (type == "SMALLDATETIME")
            {
                retval = DateTime.Parse(Convert.ToString(field));
            }
            if (type == "SMALLMONEY")
            {
                retval = Convert.ToDecimal(retval);
            }
            if (type == "IMAGE")
            {
                retval = field;
            }
            if (type == "BINARY")
            {
                retval = field;
            }
            if (type == "BIT")
            {
                retval = Convert.ToBoolean(retval);
            }
            if (type == "REAL")
            {
                retval = Convert.ToDouble(retval);
            }
            if (type == "FLOAT")
            {
                retval = Convert.ToSingle(retval);
            }
            if (type == "TEXT")
            {
                retval = Convert.ToString(retval);
            }
            if (type == "VARCHAR")
            {
                retval = Convert.ToString(retval);
            }
            if (type == "VARBINARY")
            {
                retval = field;
            }
            if (type == "LONGCHAR")
            {
                retval = Convert.ToString(retval);
            }
            if (type == "LONGBINARY")
            {
                retval = field;
            }
            if (type == "LONG")
            {
                retval = field;
            }
            if (type == "ILLEGAL")
            {
                retval = field;
            }
            if (type == "SENSITIVITY")
            {
                retval = field;
            }
            if (type == "BOUNDARY")
            {
                retval = field;
            }
            if (type == "VOID")
            {
                retval = null;
            }
            if (type == "USHORT")
            {
                retval = Convert.ToUInt16(field);
            }
            // added by lago
            if (type == "UNDEFINED")            // 27
            {
                retval = field;
            }
            if (type == "DOUBLE")
            {
                retval = Convert.ToDouble(field);
            }
            if (type == "DATE")
            {
                retval = DateTime.Parse(Convert.ToString(field));
            }
            if (type == "TIME")
            {
                retval = DateTime.Parse(Convert.ToString(field));
            }
            if (type == "TIMESTAMP")
            {
                retval = DateTime.Parse(Convert.ToString(field));
            }
            // added by msql
            if (type == "UINT")                        // 32
            {
                retval = Convert.ToUInt32(field);
            }
            if (type == "LASTREAL")
            {
                retval = Convert.ToDouble(field);
            }
            // added by mysql
            if (type == "STRING")            // 34
            {
                retval = Convert.ToString(field);
            }
            if (type == "VARSTRING")
            {
                retval = Convert.ToString(field);
            }
            if (type == "LONGLONG")
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "MEDIUMINT")
            {
                retval = Convert.ToInt32(field);
            }
            if (type == "YEAR")
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "NEWDATE")
            {
                retval = DateTime.Parse(Convert.ToString(field));
            }
            if (type == "NULL")
            {
                retval = null;
            }
            if (type == "ENUM")
            {
                retval = field;
            }
            if (type == "SET")
            {
                retval = field;
            }
            if (type == "TINYBLOB")
            {
                retval = field;
            }
            if (type == "MEDIUMBLOB")
            {
                retval = field;
            }
            if (type == "LONGBLOB")
            {
                retval = field;
            }
            if (type == "BLOB")
            {
                retval = field;
            }
            // added by oracle
            if (type == "VARCHAR2")            // 47
            {
                retval = Convert.ToString(field);
            }
            if (type == "NUMBER")
            {
                // FIXME: use precision and scale to convert this differently
                retval = Convert.ToDecimal(field);
            }
            if (type == "ROWID")
            {
                retval = Convert.ToUInt64(field);
            }
            if (type == "RAW")
            {
                retval = field;
            }
            if (type == "LONG_RAW")
            {
                retval = field;
            }
            if (type == "MLSLABEL")
            {
                retval = field;
            }
            if (type == "CLOB")
            {
                retval = Convert.ToString(field);
            }
            if (type == "BFILE")
            {
                retval = field;
            }
            // added by odbc
            if (type == "BIGINT")            // 55
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "INTEGER")
            {
                retval = Convert.ToInt32(field);
            }
            if (type == "LONGVARBINARY")
            {
                retval = field;
            }
            if (type == "LONGVARCHAR")
            {
                retval = Convert.ToString(field);
            }
            // added by db2
            if (type == "GRAPHIC")            // 59
            {
                retval = field;
            }
            if (type == "VARGRAPHIC")
            {
                retval = field;
            }
            if (type == "LONGVARGRAPHIC")
            {
                retval = field;
            }
            if (type == "DBCLOB")
            {
                retval = Convert.ToString(field);
            }
            if (type == "DATALINK")
            {
                retval = field;
            }
            if (type == "USER_DEFINED_TYPE")
            {
                retval = field;
            }
            if (type == "SHORT_DATATYPE")
            {
                retval = Convert.ToInt16(field);
            }
            if (type == "TINY_DATATYPE")
            {
                retval = Convert.ToChar(field);
            }
            // added by firebird
            if (type == "D_FLOAT")            // 67
            {
                retval = Convert.ToSingle(field);
            }
            if (type == "ARRAY")
            {
                retval = field;
            }
            if (type == "QUAD")
            {
                retval = Convert.ToUInt64(field);
            }
            if (type == "INT64")
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "DOUBLE PRECISION")
            {
                retval = Convert.ToDouble(field);
            }
            // added by postgresql
            if (type == "BOOL")
            {
                retval = Convert.ToBoolean(field);
            }
            if (type == "BYTEA")
            {
                retval = field;
            }
            if (type == "NAME")
            {
                retval = Convert.ToString(field);
            }
            if (type == "INT8")
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "INT2")
            {
                retval = Convert.ToInt16(field);
            }
            if (type == "INT2VECTOR")
            {
                retval = field;
            }
            if (type == "INT4")
            {
                retval = Convert.ToInt32(field);
            }
            if (type == "REGPROC")
            {
                retval = field;
            }
            if (type == "OID")
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "TID")
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "XID")
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "CID")
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "OIDVECTOR")
            {
                retval = field;
            }
            if (type == "SMGR")
            {
                retval = field;
            }
            if (type == "POINT")
            {
                retval = field;
            }
            if (type == "LSEG")
            {
                retval = field;
            }
            if (type == "PATH")
            {
                retval = field;
            }
            if (type == "BOX")
            {
                retval = field;
            }
            if (type == "POLYGON")
            {
                retval = field;
            }
            if (type == "LINE")
            {
                retval = field;
            }
            if (type == "LINE_ARRAY")
            {
                retval = field;
            }
            if (type == "FLOAT4")
            {
                retval = Convert.ToSingle(field);
            }
            if (type == "FLOAT8")
            {
                retval = Convert.ToDouble(field);
            }
            if (type == "ABSTIME")
            {
                // I guess...
                retval = Convert.ToInt64(field);
            }
            if (type == "RELTIME")
            {
                // I guess...
                retval = Convert.ToInt64(field);
            }
            if (type == "TINTERVAL")
            {
                retval = Convert.ToInt64(field);
            }
            if (type == "CIRCLE")
            {
                retval = field;
            }
            if (type == "CIRCLE_ARRAY")
            {
                retval = field;
            }
            if (type == "MONEY_ARRAY")
            {
                retval = field;
            }
            if (type == "MACADDR")
            {
                // not sure what to do with a mac addr, presumably it's a 4 byte array
                retval = field;
            }
            if (type == "INET")
            {
                retval = field;
            }
            if (type == "CIDR")
            {
                retval = field;
            }
            if (type == "BOOL_ARRAY")
            {
                retval = field;
            }
            if (type == "BYTEA_ARRAY")
            {
                retval = field;
            }
            if (type == "CHAR_ARRAY")
            {
                retval = field;
            }
            if (type == "NAME_ARRAY")
            {
                retval = field;
            }
            if (type == "INT2_ARRAY")
            {
                // ???
                retval = field;
            }
            if (type == "INT2VECTOR_ARRAY")
            {
                retval = field;
            }
            if (type == "INT4_ARRAY")
            {
                retval = field;
            }
            if (type == "REGPROC_ARRAY")
            {
                retval = field;
            }
            if (type == "TEXT_ARRAY")
            {
                retval = field;
            }
            if (type == "OID_ARRAY")
            {
                retval = field;
            }
            if (type == "TID_ARRAY")
            {
                retval = field;
            }
            if (type == "XID_ARRAY")
            {
                retval = field;
            }
            if (type == "CID_ARRAY")
            {
                retval = field;
            }
            if (type == "OIDVECTOR_ARRAY")
            {
                retval = field;
            }
            if (type == "BPCHAR_ARRAY")
            {
                retval = field;
            }
            if (type == "VARCHAR_ARRAY")
            {
                retval = field;
            }
            if (type == "INT8_ARRAY")
            {
                retval = field;
            }
            if (type == "POINT_ARRAY")
            {
                retval = field;
            }
            if (type == "LSEG_ARRAY")
            {
                retval = field;
            }
            if (type == "PATH_ARRAY")
            {
                retval = field;
            }
            if (type == "BOX_ARRAY")
            {
                retval = field;
            }
            if (type == "FLOAT4_ARRAY")
            {
                retval = field;
            }
            if (type == "FLOAT8_ARRAY")
            {
                retval = field;
            }
            if (type == "ABSTIME_ARRAY")
            {
                retval = field;
            }
            if (type == "RELTIME_ARRAY")
            {
                retval = field;
            }
            if (type == "TINTERVAL_ARRAY")
            {
                retval = field;
            }
            if (type == "POLYGON_ARRAY")
            {
                retval = field;
            }
            if (type == "ACLITEM")
            {
                retval = field;
            }
            if (type == "ACLITEM_ARRAY")
            {
                retval = field;
            }
            if (type == "MACADDR_ARRAY")
            {
                retval = field;
            }
            if (type == "INET_ARRAY")
            {
                retval = field;
            }
            if (type == "CIDR_ARRAY")
            {
                retval = field;
            }
            if (type == "BPCHAR")
            {
                retval = Convert.ToString(field);
            }
            if (type == "TIMESTAMP_ARRAY")
            {
                retval = field;
            }
            if (type == "DATE_ARRAY")
            {
                retval = field;
            }
            if (type == "TIME_ARRAY")
            {
                retval = field;
            }
            if (type == "TIMESTAMPTZ")
            {
                retval = field;
            }
            if (type == "TIMESTAMPTZ_ARRAY")
            {
                retval = field;
            }
            if (type == "INTERVAL")
            {
                retval = field;
            }
            if (type == "INTERVAL_ARRAY")
            {
                retval = field;
            }
            if (type == "NUMERIC_ARRAY")
            {
                retval = field;
            }
            if (type == "TIMETZ")
            {
                retval = field;
            }
            if (type == "TIMETZ_ARRAY")
            {
                retval = field;
            }
            if (type == "BIT_ARRAY")
            {
                retval = field;
            }
            if (type == "VARBIT")
            {
                retval = field;
            }
            if (type == "VARBIT_ARRAY")
            {
                retval = field;
            }
            if (type == "REFCURSOR")
            {
                retval = field;
            }
            if (type == "REFCURSOR_ARRAY")
            {
                retval = field;
            }
            if (type == "REGPROCEDURE")
            {
                retval = field;
            }
            if (type == "REGOPER")
            {
                retval = field;
            }
            if (type == "REGOPERATOR")
            {
                retval = field;
            }
            if (type == "REGCLASS")
            {
                retval = field;
            }
            if (type == "REGTYPE")
            {
                retval = field;
            }
            if (type == "REGPROCEDURE_ARRAY")
            {
                retval = field;
            }
            if (type == "REGOPER_ARRAY")
            {
                retval = field;
            }
            if (type == "REGOPERATOR_ARRAY")
            {
                retval = field;
            }
            if (type == "REGCLASS_ARRAY")
            {
                retval = field;
            }
            if (type == "REGTYPE_ARRAY")
            {
                retval = field;
            }
            if (type == "RECORD")
            {
                retval = field;
            }
            if (type == "CSTRING")
            {
                retval = Convert.ToString(field);
            }
            if (type == "ANY")
            {
                retval = field;
            }
            if (type == "ANYARRAY")
            {
                retval = field;
            }
            if (type == "TRIGGER")
            {
                retval = field;
            }
            if (type == "LANGUAGE_HANDLER")
            {
                retval = field;
            }
            if (type == "INTERNAL")
            {
                retval = field;
            }
            if (type == "OPAQUE")
            {
                retval = field;
            }
            if (type == "ANYELEMENT")
            {
                retval = field;
            }
            if (type == "PG_TYPE")
            {
                retval = field;
            }
            if (type == "PG_ATTRIBUTE")
            {
                retval = field;
            }
            if (type == "PG_PROC")
            {
                retval = field;
            }
            if (type == "PG_CLASS")
            {
                retval = field;
            }
            // none added by sqlite

            // cache the value
            _havevalues[i] = true;
            _values[i] = retval;

            // return the value
            return retval;
        }

        public int GetValues(object[] values)
        {
            uint colcount=_sqlrcur.colCount();
            for (uint i = 0; i < colcount; i++)
            {
                values[i] = GetValue((int)i);
            }
            return (int)colcount;
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
            return (bool)GetValue(i);
        }

        public byte GetByte(int i)
        {
            return (byte)GetValue(i);
        }

        public long GetBytes(int i, long fieldOffset, byte[] buffer, int bufferoffset, int length)
        {
            // FIXME: This is tricky because the C# wrapper for getField converts the char (byte) array into a string which
            // may be different from the char array due to the encoding.  I may need to add a method to the C# wrapper that
            // returns the field as a byte array.
            throw new NotSupportedException("GetBytes not supported.");
        }

        public char GetChar(int i)
        {
            return (char)GetValue(i);
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
            return (Int16)GetValue(i);
        }

        public Int32 GetInt32(int i)
        {
            return (Int32)GetValue(i);
        }

        public Int64 GetInt64(int i)
        {
            return (Int64)GetValue(i);
        }

        public float GetFloat(int i)
        {
            return (float)GetValue(i);
        }

        public double GetDouble(int i)
        {
            return (double)GetValue(i);
        }

        public String GetString(int i)
        {
            return (String)GetValue(i);
        }

        public Decimal GetDecimal(int i)
        {
            return (Decimal)GetValue(i);
        }

        public DateTime GetDateTime(int i)
        {
            return (DateTime)GetValue(i);
        }

        public IDataReader GetData(int i)
        {
            // Normally, this would be used to expose nested tables and other hierarchical data.
            throw new NotSupportedException("GetData not supported.");
        }

        public bool IsDBNull(int i)
        {
            // FIXME: this will need to be modified if getNullsAsNulls is exposed
            return ((string)GetValue(i) == "");
        }

        private int cultureAwareCompare(string strA, string strB)
        {
            return CultureInfo.CurrentCulture.CompareInfo.Compare(strA, strB, CompareOptions.IgnoreKanaType | CompareOptions.IgnoreWidth | CompareOptions.IgnoreCase);
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

        private void invalidColumnIndex(int i)
        {
            if (i < 0 || i > FieldCount)
            {
                throw new IndexOutOfRangeException();
            }
        }
    }
}
