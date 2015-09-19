// Copyright (c) 2012-2015  David Muse
// See the file COPYING for more information

using System;
using System.Data;
using System.Globalization;
using System.Collections.Generic;

namespace SQLRClient
{
    public class SQLRelayDataReader : IDataReader
    {

        #region member variables

        private Boolean _open = true;
        private SQLRelayConnection _sqlrelaycon = null;
        private SQLRCursor _sqlrcur = null;
        private Boolean _endsession = false;
        private Boolean _unfetched = true;
        private UInt64 _currentrow = 0;
        private Boolean[] _havevalues = null;
        private Object[] _values = null;
        private Queue<SQLRCursor> _sqlrcurlist = new Queue<SQLRCursor>();

        #endregion

      
        #region constructors and destructors

        internal SQLRelayDataReader(SQLRelayConnection sqlrelaycon, SQLRCursor sqlrcur, Boolean endsession)
        {
            _sqlrelaycon = sqlrelaycon;
            _sqlrcur = sqlrcur;
            _endsession = endsession;
        }

        /** Releases all resources used by the SQLRelayDataReader. */
        ~SQLRelayDataReader()
        {
            Dispose(false);
        }

        /** Performs application-defined tasks associated with freeing,
         *  releasing or resetting unmanaged resources. */
        void IDisposable.Dispose()
        {
            Dispose(true);
            System.GC.SuppressFinalize(this);
        }

        private void Dispose(Boolean disposing)
        {
            if (disposing)
            {
                try
                {
                    if (IsClosed == false)
                    {
                        Close();
                    }
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

        /** Gets a value indicating the depth of nesting for the current row. */
        public Int32 Depth
        {
            // FIXME: Return 0 if nesting isn't supported.  What is nesting?
            get
            {
                return 0;
            }
        }

        /** Gets the number of columns in the current row. */
        public Int32 FieldCount
        {
            get
            {
                return (Int32)_sqlrcur.colCount();
            }
        }

        /** Gets a value indicating whether the data reader is closed. */
        public Boolean IsClosed
        {
            get
            {
                return !_open;
            }
        }

        /** Gets the number of rows changed, inserted, or deleted by execution
         *  of the SQL statement. */
        public Int32 RecordsAffected
        {
            get
            {
                return (Int32)_sqlrcur.affectedRows();
            }
        }

        /** Gets true or false, indicating whether the result set contains any 
         *  rows at all. */
        public Boolean HasRows
        {
            get
            {
                return (_sqlrcur.rowCount() > 0);
            }
        }

        internal SQLRCursor Cursor
        {
            get
            {
                return _sqlrcur;
            }
        }

        /** Return the value of the specified field. */
        public Object this[Int32 i]
        {
            get
            {
                return GetValue(i);
            }
        }

        /** Return the value of the specified field. */
        public Object this[String name]
        {
            get
            {
                return GetValue(GetOrdinal(name));
            }
        }

        #endregion


        #region public methods

        /** Closes the SQLRelayDataReader object. */
        public void Close()
        {
            _open = false;
            _sqlrcur.closeResultSet();
        }

        /** Advances the data reader to the next result, when reading the
         *  results a query which returns multiple result sets. */
        public Boolean NextResult()
        {

            // If a query returns multiple output bind cursors then
            // they will be queued up in the _sqlrcurlist.  Try to
            // dequeue one.  If it fails then there are no more
            // results.
            SQLRCursor sqlrcur = null;
            try
            {
                sqlrcur = _sqlrcurlist.Dequeue();
            }
            catch (Exception)
            {
                return false;
            }

            // If there was another result set then close the current
            // one, switch cursors and reset the various flags.
            Close();
            _sqlrcur = sqlrcur;
            _open = true;
            _unfetched = true;
            _currentrow = 0;
            _havevalues = null;
            _values = null;
            return true;
        }

        /** Advances the SQLRelayDataReader to the next record. */
        public Boolean Read()
        {

            if (IsClosed == true)
            {
                throw new InvalidOperationException("Reader must be open");
            }

            // if the _endsession flag was set and we've received the
            // entire result set (of all result sets) then end the session
            if (_endsession && _sqlrcurlist.Count > 0 && _sqlrcur.endOfResultSet())
            {
                _sqlrelaycon.SQLRConnection.endSession();
            }

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
            _havevalues = new Boolean [FieldCount];
            for (Int32 i = 0; i < FieldCount; i++)
            {
                _havevalues[i] = false;
            }
            _values = new Object[FieldCount];

            // return whether or not we've read past the end of the result set
            return (!_sqlrcur.endOfResultSet() || _currentrow < _sqlrcur.rowCount());
        }

        /** Returns a DataTable that describes the colum metadata of the
         *  SQLRelayDataReader. */
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
            for (UInt32 i=0; i<FieldCount; i++)
            {

                DataRow row = datatable.NewRow();
                
                row["ColumnName"] = GetName((Int32)i);
                row["ColumnOrdinal"] = i;
                row["ColumnSize"] = _sqlrcur.getColumnLength(i);
                row["NumericPrecision"] = _sqlrcur.getColumnPrecision(i);
                row["NumericScale"] = _sqlrcur.getColumnScale(i);
                row["IsUnique"] = _sqlrcur.getColumnIsUnique(i);
                row["IsKey"] = _sqlrcur.getColumnIsPrimaryKey(i) || _sqlrcur.getColumnIsPartOfKey(i);
                row["BaseServerName"] = null;
                row["BaseCatalogName"] = null;
                row["BaseColumnName"] = GetName((Int32)i);
                row["BaseSchemaName"] = null;
                row["BaseTableName"] = null;
                row["DataType"] = GetFieldType((Int32)i);
                row["AllowDBNull"] = _sqlrcur.getColumnIsNullable(i);
                row["ProviderType"] = _sqlrcur.getColumnType(i);
                row["IsAliased"] = false;
                row["IsExpression"] = false;
                row["IsIdentity"] = false;
                row["IsAutoIncrement"] = _sqlrcur.getColumnIsAutoIncrement(i);
                row["IsRowVersion"] = false;
                row["IsHidden"] = false;
                row["IsLong"] = (GetFieldType((Int32)i) == typeof(Byte[]));
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

        /** Returns the name for the specified field. */
        public String GetName(Int32 i)
        {
            invalidColumnIndex(i);
            return _sqlrcur.getColumnName((UInt32)i);
        }

        /** Gets the data type information for the specified field. */
        public String GetDataTypeName(Int32 i)
        {
            invalidColumnIndex(i);
            return _sqlrcur.getColumnType((UInt32)i);
        }

        /** Gets the Type information corresponding to the type of Object that
         *  would be returned from GetValue. */
        public Type GetFieldType(Int32 i)
        {
            String type = GetDataTypeName(i);

            if (type == "UNKNOWN")
            {
                return typeof(String);
            }
            // addded by freetds
            else if (type == "CHAR")                        // 1
            {
                return typeof(String);
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
                return typeof(Byte[]);
            }
            else if (type == "BINARY")
            {
                return typeof(Byte[]);
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
                return typeof(String);
            }
            else if (type == "VARCHAR")
            {
                return typeof(String);
            }
            else if (type == "VARBINARY")
            {
                return typeof(Byte[]);
            }
            else if (type == "LONGCHAR")
            {
                return typeof(String);
            }
            else if (type == "LONGBINARY")
            {
                return typeof(Byte[]);
            }
            else if (type == "LONG")
            {
                return typeof(Byte[]);
            }
            else if (type == "ILLEGAL")
            {
                return typeof(Byte[]);
            }
            else if (type == "SENSITIVITY")
            {
                return typeof(Byte[]);
            }
            else if (type == "BOUNDARY")
            {
                return typeof(Byte[]);
            }
            else if (type == "VOID")
            {
                return typeof(Byte[]);
            }
            else if (type == "USHORT")
            {
                return typeof(UInt16);
            }
            // added by lago
            else if (type == "UNDEFINED")            // 27
            {
                return typeof(Byte[]);
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
                return typeof(String);
            }
            else if (type == "VARSTRING")
            {
                return typeof(String);
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
                return typeof(Byte[]);
            }
            else if (type == "SET")
            {
                return typeof(Byte[]);
            }
            else if (type == "TINYBLOB")
            {
                return typeof(Byte[]);
            }
            else if (type == "MEDIUMBLOB")
            {
                return typeof(Byte[]);
            }
            else if (type == "LONGBLOB")
            {
                return typeof(Byte[]);
            }
            else if (type == "BLOB")
            {
                return typeof(Byte[]);
            }
            // added by oracle
            else if (type == "VARCHAR2")            // 47
            {
                return typeof(String);
            }
            else if (type == "NUMBER")
            {
                // Numbers witout scale are integers.  However,
                // occasionally integers will come back with length
                // and scale but no precision.  Oracle does this.
                // So, check for lack of either.
                if (_sqlrcur.getColumnScale((UInt32)i) == 0 ||
                    _sqlrcur.getColumnPrecision((UInt32)i) == 0)
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
                return typeof(UInt64 );
            }
            else if (type == "RAW")
            {
                return typeof(Byte[]);
            }
            else if (type == "LONG_RAW")
            {
                return typeof(Byte[]);
            }
            else if (type == "MLSLABEL")
            {
                return typeof(Byte[]);
            }
            else if (type == "CLOB")
            {
                return typeof(String);
            }
            else if (type == "BFILE")
            {
                return typeof(Byte[]);
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
                return typeof(Byte[]);
            }
            else if (type == "LONGVARCHAR")
            {
                return typeof(String);
            }
            // added by db2
            else if (type == "GRAPHIC")            // 59
            {
                return typeof(Byte[]);
            }
            else if (type == "VARGRAPHIC")
            {
                return typeof(Byte[]);
            }
            else if (type == "LONGVARGRAPHIC")
            {
                return typeof(Byte[]);
            }
            else if (type == "DBCLOB")
            {
                return typeof(String);
            }
            else if (type == "DATALINK")
            {
                return typeof(Byte[]);
            }
            else if (type == "USER_DEFINED_TYPE")
            {
                return typeof(Byte[]);
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
                return typeof(Byte[]);
            }
            else if (type == "QUAD")
            {
                return typeof(UInt64 );
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
            else if (type == "Boolean ")
            {
                return typeof(Boolean);
            }
            else if (type == "BYTEA")
            {
                return typeof(Byte[]);
            }
            else if (type == "NAME")
            {
                return typeof(String);
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
                return typeof(Byte[]);
            }
            else if (type == "INT4")
            {
                return typeof(Int32);
            }
            else if (type == "REGPROC")
            {
                return typeof(Byte[]);
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
                return typeof(Byte[]);
            }
            else if (type == "SMGR")
            {
                return typeof(Byte[]);
            }
            else if (type == "POINT")
            {
                return typeof(Byte[]);
            }
            else if (type == "LSEG")
            {
                return typeof(Byte[]);
            }
            else if (type == "PATH")
            {
                return typeof(Byte[]);
            }
            else if (type == "BOX")
            {
                return typeof(Byte[]);
            }
            else if (type == "POLYGON")
            {
                return typeof(Byte[]);
            }
            else if (type == "LINE")
            {
                return typeof(Byte[]);
            }
            else if (type == "LINE_ARRAY")
            {
                return typeof(Byte[]);
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
                return typeof(Byte[]);
            }
            else if (type == "CIRCLE_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "MONEY_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "MACADDR")
            {
                // not sure what to do with a mac addr, presumably it's a 4 Byte array
                return typeof(Byte[]);
            }
            else if (type == "INET")
            {
                return typeof(Byte[]);
            }
            else if (type == "CIDR")
            {
                return typeof(Byte[]);
            }
            else if (type == "Boolean _ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "BYTEA_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "CHAR_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "NAME_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "INT2_ARRAY")
            {
                // ???
                return typeof(Byte[]);
            }
            else if (type == "INT2VECTOR_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "INT4_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGPROC_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "TEXT_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "OID_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "TID_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "XID_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "CID_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "OIDVECTOR_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "BPCHAR_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "VARCHAR_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "INT8_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "POINT_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "LSEG_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "PATH_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "BOX_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "FLOAT4_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "FLOAT8_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "ABSTIME_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "RELTIME_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "TINTERVAL_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "POLYGON_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "ACLITEM")
            {
                return typeof(Byte[]);
            }
            else if (type == "ACLITEM_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "MACADDR_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "INET_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "CIDR_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "BPCHAR")
            {
                return typeof(String);
            }
            else if (type == "TIMESTAMP_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "DATE_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "TIME_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "TIMESTAMPTZ")
            {
                return typeof(Byte[]);
            }
            else if (type == "TIMESTAMPTZ_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "INTERVAL")
            {
                return typeof(Byte[]);
            }
            else if (type == "INTERVAL_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "NUMERIC_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "TIMETZ")
            {
                return typeof(Byte[]);
            }
            else if (type == "TIMETZ_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "BIT_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "VARBIT")
            {
                return typeof(Byte[]);
            }
            else if (type == "VARBIT_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "REFCURSOR")
            {
                return typeof(Byte[]);
            }
            else if (type == "REFCURSOR_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGPROCEDURE")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGOPER")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGOPERATOR")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGCLASS")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGTYPE")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGPROCEDURE_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGOPER_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGOPERATOR_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGCLASS_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "REGTYPE_ARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "RECORD")
            {
                return typeof(Byte[]);
            }
            else if (type == "CSTRING")
            {
                return typeof(String);
            }
            else if (type == "ANY")
            {
                return typeof(Byte[]);
            }
            else if (type == "ANYARRAY")
            {
                return typeof(Byte[]);
            }
            else if (type == "TRIGGER")
            {
                return typeof(Byte[]);
            }
            else if (type == "LANGUAGE_HANDLER")
            {
                return typeof(Byte[]);
            }
            else if (type == "INTERNAL")
            {
                return typeof(Byte[]);
            }
            else if (type == "OPAQUE")
            {
                return typeof(Byte[]);
            }
            else if (type == "ANYELEMENT")
            {
                return typeof(Byte[]);
            }
            else if (type == "PG_TYPE")
            {
                return typeof(Byte[]);
            }
            else if (type == "PG_ATTRIBUTE")
            {
                return typeof(Byte[]);
            }
            else if (type == "PG_PROC")
            {
                return typeof(Byte[]);
            }
            else if (type == "PG_CLASS")
            {
                return typeof(Byte[]);
            }
            // none added by sqlite
            // added by sqlserver
            else if (type == "UBIGINT")
            {
                return typeof(UInt64);
            }
            else if (type == "UNIQUEIDENTIFIER")
            {
                return typeof(Byte[]);
            }
            // added by informix
            else if (type == "SMALLFLOAT")
            {
                return typeof(Single);
            }
            else if (type == "BYTE")
            {
                return typeof(Byte[]);
            }
            else if (type == "BOOLEAN")
            {
                return typeof(Boolean);
            }

            // unrecognized type
            return typeof(Byte[]);
        }

        /** Return the value of the specified field. */
        public Object GetValue(Int32 i)
        {
            invalidColumnIndex(i);
            
            // return the value from the cache, if we have it
            if (_havevalues[i])
            {
                return _values[i];
            }

            // get the field
            Object retval = convertField(_sqlrcur.getFieldAsByteArray(_currentrow, (UInt32)i), GetDataTypeName(i), _sqlrcur.getColumnPrecision((UInt32)i), _sqlrcur.getColumnScale((UInt32)i));

            // cache the value
            _havevalues[i] = true;
            _values[i] = retval;

            // return the value
            return retval;
        }

        public static Object convertField(Byte[] field, String type, UInt32 precision, UInt32 scale)
        {
            if (field == null)
            {
                return null;
            }

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
                return Convert.ToUInt64 (System.Text.Encoding.Default.GetString(field));
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
                return Convert.ToUInt64 (System.Text.Encoding.Default.GetString(field));
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
            else if (type == "Boolean ")
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
                // not sure what to do with a mac addr, presumably it's a 4 Byte array
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
            else if (type == "Boolean _ARRAY")
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

        /** Populates an array of obects with the column values
         *  of the current record. */
        public Int32 GetValues(Object[] values)
        {
            Int32 colcount=(Int32)_sqlrcur.colCount();
            Int32 i = 0;
            for (; i < colcount && i < values.Length; i++)
            {
                values[i] = GetValue(i);
            }
            return i;
        }

        /** Return the index of the named field. */
        public Int32 GetOrdinal(String name)
        {
            UInt32 colcount = _sqlrcur.colCount();
            for (UInt32 i = 0; i < colcount; i++)
            {
                if (cultureAwareCompare(name, _sqlrcur.getColumnName(i)) == 0)
                {
                    return (Int32)i;
                }
            }
            throw new IndexOutOfRangeException("Could not find specified column in results");
        }

        /** Gets the value of the specified column as a Boolean. */
        public Boolean GetBoolean(Int32 i)
        {
            return Convert.ToBoolean(GetValue(i));
        }

        /** Gets the 8-bit unsigned integer value of the specified column. */
        public Byte GetByte(Int32 i)
        {
            return Convert.ToByte(GetValue(i));
        }

        /** Reads a stream of bytes from the specified column offset into the
         *  buffer as an array, starting at the given buffer offset. */
        public Int64 GetBytes(Int32 i, Int64 fieldoffset, Byte[] buffer, Int32 bufferoffset, Int32 length)
        {

            // get the field
            Byte[] field = _sqlrcur.getFieldAsByteArray(_currentrow,(UInt32)i);

            // copy chars from the field into the buffer
            UInt32 j = 0;
            while (j < length && fieldoffset + j < field.Length)
            {
                buffer[bufferoffset + i] = field[(Int32)(fieldoffset + j)];
                j++;
            }
            return (Int64)j;
        }

        /** Gets the character value of the specified column. */
        public Char GetChar(Int32 i)
        {
            return Convert.ToChar(GetValue(i));
        }

        /** Reads a stream of characters from the specified column offset into
         *  the buffer as an array, starting at the given buffer offset. */
        public Int64 GetChars(Int32 i, Int64 fieldoffset, Char[] buffer, Int32 bufferoffset, Int32 length)
        {

            // get the field
            String field = GetString(i);

            // copy chars from the field into the buffer
            UInt32 j = 0;
            while (j < length && fieldoffset + j < field.Length)
            {
                buffer[bufferoffset + i] = field[(Int32)(fieldoffset + j)];
                j++;
            }
            return (Int64)j;
        }

        /** Returns the GUID value of the specified field. */
        public Guid GetGuid(Int32 i)
        {
            return (Guid)GetValue(i);
        }

        /** Gets the 16-bit signed integer value of the specified field. */
        public Int16 GetInt16(Int32 i)
        {
            return (Int16)_sqlrcur.getFieldAsInteger(_currentrow, (UInt32)i);
        }

        /** Gets the 32-bit signed integer value of the specified field. */
        public Int32 GetInt32(Int32 i)
        {
            return (Int32)_sqlrcur.getFieldAsInteger(_currentrow, (UInt32)i);
        }

        /** Gets the 64-bit signed integer value of the specified field. */
        public Int64 GetInt64(Int32 i)
        {
            return _sqlrcur.getFieldAsInteger(_currentrow, (UInt32)i);
        }

        /** Gets the single-precision floating point number of the specified
         *  field. */
        public float GetFloat(Int32 i)
        {
            return (float)_sqlrcur.getFieldAsDouble(_currentrow, (UInt32)i);
        }

        /** Gets the double-precision floating point number of the specified
         *  field. */
        public Double GetDouble(Int32 i)
        {
            return _sqlrcur.getFieldAsDouble(_currentrow, (UInt32)i);
        }

        /** Gets the string value of the specified field. */
        public String GetString(Int32 i)
        {
            return _sqlrcur.getField(_currentrow, (UInt32)i);
        }

        /** Gets the fixed position numeric value of the specified field. */
        public Decimal GetDecimal(Int32 i)
        {
            return Convert.ToDecimal(GetValue(i));
        }

        /** Gets the date and time data value of the specified field. */
        public DateTime GetDateTime(Int32 i)
        {
            return Convert.ToDateTime(GetValue(i));
        }

        /** Returns an IDataReader for the specified column ordinal.  This
         *  method is included because it is required by the interface, but
         *  since nested tables and other heirarchical data are currently
         *  unsupported by SQL Relay, it just throws a NotSupportedException. */
        public IDataReader GetData(Int32 i)
        {
            // Normally, this would be used to expose nested tables and other hierarchical data.
            throw new NotSupportedException("GetData not supported.");
        }

        /** Returns whether the specified field is set to null. */
        public Boolean IsDBNull(Int32 i)
        {
            // FIXME: this will need to be modified if getNullsAsNulls is exposed
            return (GetString(i) == "");
        }

        #endregion


        #region private methods

        private Int32 cultureAwareCompare(String strA, String strB)
        {
            return CultureInfo.CurrentCulture.CompareInfo.Compare(strA, strB, CompareOptions.IgnoreKanaType | CompareOptions.IgnoreWidth | CompareOptions.IgnoreCase);
        }

        private void invalidColumnIndex(Int32 i)
        {
            if (i < 0 || i > FieldCount)
            {
                throw new IndexOutOfRangeException();
            }
        }

        internal void appendCursor(SQLRCursor sqlrcursor)
        {
            _sqlrcurlist.Enqueue(sqlrcursor);
        }

        #endregion
    }
}
