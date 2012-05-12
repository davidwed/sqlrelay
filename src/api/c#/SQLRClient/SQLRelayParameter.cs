using System;
using System.Data;

namespace SQLRClient
{
    public class SQLRelayParameter : IDataParameter
    {

        #region member variables

        DbType _dbtype = DbType.Object;
        SQLRelayType _sqlrelaytype = SQLRelayType.Object;
        ParameterDirection _paramaterdirection = ParameterDirection.Input;
        bool _nullable = false;
        string _parametername = null;
        string _sourcecolumn = null;
        DataRowVersion _sourceversion = DataRowVersion.Current;
        object _value = null;

        #endregion


        #region constructors and destructors

        public SQLRelayParameter()
        {
        }

        public SQLRelayParameter(string parametername, DbType dbtype)
        {
            _parametername = parametername;
            _dbtype = dbtype;
        }

        public SQLRelayParameter(string parametername, object value)
        {
            _parametername = parametername;
            _value = value;
        }

        public SQLRelayParameter(string parametername, DbType dbtype, string sourcecolumn)
        {
            _parametername = parametername;
            _dbtype = dbtype;
            _sourcecolumn = sourcecolumn;
        }

        #endregion


        #region parameters

        public DbType DbType
        {
            get
            {
                return _dbtype;
            }
            set
            {
                _dbtype = value;
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

        public bool IsNullable
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

        public object Value
        {
            get
            {
                return _value;
            }
            set
            {
                _value = value;
                _dbtype = inferType(_value);
            }
        }

        #endregion


        #region private methods

        private DbType inferType(object value)
        {
            switch (Type.GetTypeCode(value.GetType()))
            {
                case TypeCode.Empty:
                    throw new SystemException("Invalid data type");

                case TypeCode.Object:
                    return DbType.Object;

                case TypeCode.DBNull:
                case TypeCode.Char:
                case TypeCode.SByte:
                case TypeCode.UInt16:
                case TypeCode.UInt32:
                case TypeCode.UInt64:
                    // Throw a SystemException for unsupported data types.
                    throw new SystemException("Invalid data type");

                case TypeCode.Boolean:
                    return DbType.Boolean;

                case TypeCode.Byte:
                    return DbType.Byte;

                case TypeCode.Int16:
                    return DbType.Int16;

                case TypeCode.Int32:
                    return DbType.Int32;

                case TypeCode.Int64:
                    return DbType.Int64;

                case TypeCode.Single:
                    return DbType.Single;

                case TypeCode.Double:
                    return DbType.Double;

                case TypeCode.Decimal:
                    return DbType.Decimal;

                case TypeCode.DateTime:
                    return DbType.DateTime;

                case TypeCode.String:
                    return DbType.String;

                default:
                    throw new SystemException("Value is of unknown data type");
            }
        }

        #endregion
    }
}
