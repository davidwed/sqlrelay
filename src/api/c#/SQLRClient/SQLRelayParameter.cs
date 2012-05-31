using System;
using System.Data;

namespace SQLRClient
{
    public class SQLRelayParameter : IDataParameter
    {

        #region member variables

        Boolean _dbtypeset = false;
        DbType _dbtype = DbType.Object;
        SQLRelayType _sqlrelaytype = SQLRelayType.Object;
        ParameterDirection _paramaterdirection = ParameterDirection.Input;
        Boolean _nullable = false;
        String _parametername = null;
        String _sourcecolumn = null;
        DataRowVersion _sourceversion = DataRowVersion.Current;
        Object _value = null;
        UInt32 _size = 0;
        Boolean _isnull = true;

        #endregion


        #region constructors and destructors

        public SQLRelayParameter()
        {
        }

        public SQLRelayParameter(String parametername, DbType dbtype)
        {
            _parametername = parametername;
            DbType = dbtype;
        }

        public SQLRelayParameter(String parametername, Object value)
        {
            _parametername = parametername;
            Value = value;
        }

        public SQLRelayParameter(String parametername, DbType dbtype, String sourcecolumn)
        {
            _parametername = parametername;
            DbType = dbtype;
            _sourcecolumn = sourcecolumn;
        }

        #endregion


        #region parameters

        public DbType DbType
        {
            get
            {
                determineDbType();
                return _dbtype;
            }
            set
            {
                _dbtype = value;
                _dbtypeset = true;
            }
        }

        public SQLRelayType SQLRelayType
        {
            get
            {
                return _sqlrelaytype;
            }
            set
            {
                _sqlrelaytype = value;
            }
        }

        public ParameterDirection Direction
        {
            get
            {
                return _paramaterdirection;
            }
            set
            {
                _paramaterdirection = value;
            }
        }

        public Boolean IsNullable
        {
            get
            {
                return _nullable;
            }
        }

        public String ParameterName
        {
            get
            {
                return _parametername;
            }
            set
            {
                _parametername = value;
            }
        }

        public String SourceColumn
        {
            get
            {
                return _sourcecolumn;
            }
            set
            {
                _sourcecolumn = value;
            }
        }

        public DataRowVersion SourceVersion
        {
            get
            {
                return _sourceversion;
            }
            set
            {
                _sourceversion = value;
            }
        }

        public Object Value
        {
            get
            {
                return _value;
            }
            set
            {
                _value = value;
                _isnull = (_value == null);
                determineDbType();
            }
        }

        public UInt32 Size
        {
            get
            {
                return _size;
            }
            set
            {
                _size = value;
            }
        }

        public Boolean IsNull
        {
            get
            {
                return _isnull;
            }
        }

        #endregion


        #region private methods

        private void determineDbType()
        {
            // dont change the type if it's already been set manually
            if (_dbtypeset == true)
            {
                return;
            }

            _dbtypeset = true;

            if (_value == null)
            {
                _dbtype = DbType.Object;
                return;
            }

            switch (Type.GetTypeCode(_value.GetType()))
            {
                case TypeCode.Empty:
                case TypeCode.Object:
                case TypeCode.DBNull:
                    _dbtype = DbType.Object;
                    return;

                case TypeCode.Char:
                    _dbtype = DbType.UInt16;
                    return;

                case TypeCode.SByte:
                    _dbtype = DbType.Int16;
                    return;

                case TypeCode.UInt16:
                    _dbtype = DbType.UInt16;
                    return;

                case TypeCode.UInt32:
                    _dbtype = DbType.UInt32;
                    return;

                case TypeCode.UInt64 :
                    _dbtype = DbType.UInt64 ;
                    return;

                case TypeCode.Boolean:
                    _dbtype = DbType.Boolean;
                    return;

                case TypeCode.Byte:
                    _dbtype = DbType.Byte;
                    return;

                case TypeCode.Int16:
                    _dbtype = DbType.Int16;
                    return;

                case TypeCode.Int32:
                    _dbtype = DbType.Int32;
                    return;

                case TypeCode.Int64:
                    _dbtype = DbType.Int64;
                    return;

                case TypeCode.Single:
                    _dbtype = DbType.Single;
                    return;

                case TypeCode.Double:
                    _dbtype = DbType.Double;
                    return;

                case TypeCode.Decimal:
                    _dbtype = DbType.Decimal;
                    return;

                case TypeCode.DateTime:
                    _dbtype = DbType.DateTime;
                    return;

                case TypeCode.String:
                    _dbtype = DbType.String;
                    return;

                default:
                    _dbtype = DbType.Object;
                    return;
            }
        }

        #endregion
    }
}
