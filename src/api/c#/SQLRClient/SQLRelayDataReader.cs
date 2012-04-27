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
            // this will need to be updated if getNullsAsNulls is exposed
            return (this[0] != null);
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
            return _sqlrcur.getColumnName((uint)i);
        }

        public String GetDataTypeName(int i)
        {
            return _sqlrcur.getColumnType((uint)i);
        }

        public Type GetFieldType(int i)
        {
            // FIXME: return the actual Type class the data type
            return null;
        }

        public Object GetValue(int i)
        {
            return this[i];
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
                return _sqlrcur.getField(_currentrow, (uint)i);
            }
        }

        public object this[String name]
        {
            get
            {
                return _sqlrcur.getField(_currentrow, name);
            }
        }

        public bool GetBoolean(int i)
        {
            return (bool)this[i];
        }

        public byte GetByte(int i)
        {
            return (byte)this[i];
        }

        public long GetBytes(int i, long fieldOffset, byte[] buffer, int bufferoffset, int length)
        {
            // FIXME: I should be able to support this
            throw new NotSupportedException("GetBytes not supported.");
        }

        public char GetChar(int i)
        {
            return (char)this[i];
        }

        public long GetChars(int i, long fieldoffset, char[] buffer, int bufferoffset, int length)
        {
            // FIXME: I should be able to support this
            throw new NotSupportedException("GetChars not supported.");
        }

        public Guid GetGuid(int i)
        {
            return (Guid)this[i];
        }

        public Int16 GetInt16(int i)
        {
            return (Int16)this[i];
        }

        public Int32 GetInt32(int i)
        {
            return (Int32)this[i];
        }

        public Int64 GetInt64(int i)
        {
            return (Int64)this[i];
        }

        public float GetFloat(int i)
        {
            return (float)this[i];
        }

        public double GetDouble(int i)
        {
            return (double)this[i];
        }

        public String GetString(int i)
        {
            return (String)this[i];
        }

        public Decimal GetDecimal(int i)
        {
            return (Decimal)this[i];
        }

        public DateTime GetDateTime(int i)
        {
            return (DateTime)this[i];
        }

        public IDataReader GetData(int i)
        {
            // Normally, this would be used to expose nested tables and other hierarchical data.
            throw new NotSupportedException("GetData not supported.");
        }

        public bool IsDBNull(int i)
        {
            // FIXME: this will need to be modified if getNullsAsNulls is exposed
            return ((string)this[i] == "");
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
    }
}
