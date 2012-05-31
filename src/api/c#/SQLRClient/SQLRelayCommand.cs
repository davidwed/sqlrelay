using System;
using System.Data;

namespace SQLRClient
{
    public class SQLRelayCommand : IDbCommand
    {

        #region member variables

        private SQLRelayConnection _sqlrelaycon = null;
        private SQLRelayTransaction _sqlrelaytran = null;
        private SQLRCursor _sqlrcur = null;
        private String _commandtext = null;
        private Boolean  _prepared = false;
        private UpdateRowSource _updaterowsource = UpdateRowSource.None;
        private SQLRelayParameterCollection _sqlrelayparams = new SQLRelayParameterCollection();

        #endregion


        #region constructors and destructors

        public SQLRelayCommand()
        {
        }

        public SQLRelayCommand(String commandtext)
        {
            _commandtext = commandtext;
        }

        public SQLRelayCommand(String commandtext, SQLRelayConnection sqlrelaycon)
        {
            _commandtext = commandtext;
            _sqlrelaycon = sqlrelaycon;
        }

        public SQLRelayCommand(String commandtext, SQLRelayConnection sqlrelaycon, SQLRelayTransaction sqlrelaytran)
        {
            _commandtext = commandtext;
            _sqlrelaycon = sqlrelaycon;
            _sqlrelaytran = sqlrelaytran;
        }

        void IDisposable.Dispose()
        {
            this.Dispose(true);
            System.GC.SuppressFinalize(this);
        }

        private void Dispose(Boolean  disposing)
        {
            // FIXME: do anything?
        }

        #endregion


        #region properties

        public String CommandText
        {
            get
            {
                return _commandtext;
            }
            set
            {
                _commandtext = value;
                _prepared = false;
            }
        }

        public Int32 CommandTimeout
        {
            get
            {
                return 0;
            }
            set
            {
                if (value != 0)
                {
                    throw new NotSupportedException();
                }
            }
        }

        public CommandType CommandType
        {
            get
            {
                return CommandType.Text;
            }
            set
            {
                if (value != CommandType.Text)
                {
                    throw new NotSupportedException();
                }
            }
        }

        public IDbConnection Connection
        {
            get
            {
                return _sqlrelaycon;
            }
            set
            {
                // set the tranaction and cursor to null if the connection is changed
                if (value != _sqlrelaycon)
                {
                    _sqlrelaytran = null;
                    _sqlrcur = null;
                    _prepared = false;
                }
                _sqlrelaycon = (SQLRelayConnection)value;
            }   
        }

        public SQLRelayParameterCollection Parameters
        {
            get
            {
                return _sqlrelayparams;
            }
        }

        IDataParameterCollection IDbCommand.Parameters
        {
            get
            {
                return _sqlrelayparams;
            }
        }

        public IDbTransaction Transaction
        {
            get
            {
                return _sqlrelaytran;
            }
            set
            {
                _sqlrelaytran = (SQLRelayTransaction)value;
            }
        }

        public UpdateRowSource UpdatedRowSource
        {
            get
            {
                return _updaterowsource;
            }
            set
            {
                _updaterowsource = value;
            }
        }

        #endregion


        #region public methods

        public void Cancel()
        {
            throw new NotSupportedException();
        }

        public IDbDataParameter CreateParameter()
        {
            return (IDbDataParameter)(new SQLRelayParameter());
        }

        public Int32 ExecuteNonQuery()
        {
            return (runQuery()) ? (Int32)_sqlrcur.affectedRows() : 0;
        }

        public IDataReader ExecuteReader()
        {
            return ExecuteReader(CommandBehavior.Default);
        }

        public IDataReader ExecuteReader(CommandBehavior commandbehavior)
        {
            return (runQuery()) ? new SQLRelayDataReader(_sqlrelaycon, _sqlrcur, commandbehavior == CommandBehavior.CloseConnection) : null;
        }

        public Object ExecuteScalar()
        {
            if (runQuery())
            {
                if (_sqlrcur.rowCount() == 0)
                {
                    return null;
                }
                return SQLRelayDataReader.convertField(_sqlrcur.getFieldAsByteArray(0, 0), _sqlrcur.getColumnType(0), _sqlrcur.getColumnPrecision(0), _sqlrcur.getColumnScale(0));
            }
            return null;
        }

        public void Prepare()
        {
            validConnection();

            getCursor().prepareQuery(_commandtext);
            _prepared = true;
        }

        #endregion


        #region private methods

        private void validConnection()
        {
            if (_sqlrelaycon == null || _sqlrelaycon.State != ConnectionState.Open)
            {
                throw new InvalidOperationException("Connection must be valid and open.");
            }
        }

        private SQLRCursor getCursor()
        {
            if (_sqlrcur == null)
            {
                _sqlrcur = new SQLRCursor(_sqlrelaycon.SQLRConnection);
            }
            return _sqlrcur;
        }

        private Boolean  runQuery()
        {
            if (_commandtext == null)
            {
                return false;
            }

            validConnection();
            getCursor();

            if (Parameters.Count == 0)
            {
                if ((_prepared) ? _sqlrcur.executeQuery() : _sqlrcur.sendQuery(_commandtext))
                {
                    return true;
                }
            }
            else
            {
                if (!_prepared)
                {
                    Prepare();
                }

                bindParameters();

                if (_sqlrcur.executeQuery())
                {
                    copyOutBindValues();
                    return true;
                }
            }

            throw new SQLRelayException(_sqlrcur.errorNumber(),_sqlrcur.errorMessage());
        }

        private void bindParameters()
        {

            _sqlrcur.clearBinds();

            for (Int32 i = 0; i < Parameters.Count; i++)
            {

                SQLRelayParameter param = (SQLRelayParameter)Parameters[i];

                if (param.Direction == ParameterDirection.Input)
                {

                    if (param.IsNull)
                    {
                        _sqlrcur.inputBind(param.ParameterName, null);
                        continue;
                    }

                    switch (param.SQLRelayType)
                    {
                        case SQLRelayType.Clob:
                            _sqlrcur.inputBindClob(param.ParameterName, Convert.ToString(param.Value), (UInt32)Convert.ToString(param.Value).Length);
                            continue;
                        case SQLRelayType.Blob:
                            _sqlrcur.inputBindBlob(param.ParameterName, (Byte[])param.Value, (UInt32)((Byte[])param.Value).Length);
                            continue;
                        case SQLRelayType.Cursor:
                            // FIXME: not implemented yet
                            throw new NotSupportedException();
                    }

                    switch (param.DbType)
                    {
                        case DbType.AnsiString:
                        case DbType.AnsiStringFixedLength:
                        case DbType.String:
                        case DbType.StringFixedLength:
                        case DbType.Time:
                        case DbType.Guid:
                            _sqlrcur.inputBind(param.ParameterName, Convert.ToString(param.Value));
                            continue;

                        case DbType.Date:
                        case DbType.DateTime:
                        case DbType.DateTime2:
                        case DbType.DateTimeOffset:
                            DateTime dt = Convert.ToDateTime(param.Value);
                            _sqlrcur.inputBind(param.ParameterName, Convert.ToInt16(dt.Year), Convert.ToInt16(dt.Month), Convert.ToInt16(dt.Day), Convert.ToInt16(dt.Hour), Convert.ToInt16(dt.Minute), Convert.ToInt16(dt.Second), null);
                            continue;

                        case DbType.Binary:
                            _sqlrcur.inputBindBlob(param.ParameterName, (Byte[])param.Value, (UInt32)((Byte[])param.Value).Length);
                            continue;

                        case DbType.Boolean:
                            _sqlrcur.inputBind(param.ParameterName, (Convert.ToBoolean(param.Value)==true) ? 1 : 0);
                            continue;

                        case DbType.Currency:
                        case DbType.Decimal:
                        case DbType.Single:
                        case DbType.Double:
                        case DbType.VarNumeric:
                            _sqlrcur.inputBind(param.ParameterName, Convert.ToDouble(param.Value), 0, 0);
                            continue;

                        case DbType.Byte:
                        case DbType.Int16:
                        case DbType.Int32:
                        case DbType.Int64:
                        case DbType.SByte:
                        case DbType.UInt16:
                        case DbType.UInt32:
                        case DbType.UInt64 :
                            _sqlrcur.inputBind(param.ParameterName, Convert.ToInt64(param.Value));
                            continue;

                        case DbType.Object:
                        case DbType.Xml:
                            _sqlrcur.inputBind(param.ParameterName, Convert.ToString(param.Value));
                            continue;
                    }

                }
                else if (param.Direction == ParameterDirection.Output)
                {
                    switch (param.SQLRelayType)
                    {
                        case SQLRelayType.Clob:
                            _sqlrcur.defineOutputBindClob(param.ParameterName);
                            continue;
                        case SQLRelayType.Blob:
                            _sqlrcur.defineOutputBindBlob(param.ParameterName);
                            continue;
                        case SQLRelayType.Cursor:
                            // FIXME: not implemented yet
                            throw new NotSupportedException();
                    }

                    switch (param.DbType)
                    {
                        case DbType.AnsiString:
                        case DbType.AnsiStringFixedLength:
                        case DbType.String:
                        case DbType.StringFixedLength:
                        case DbType.Time:
                        case DbType.Guid:
                            _sqlrcur.defineOutputBindString(param.ParameterName, param.Size);
                            continue;

                        case DbType.Date:
                        case DbType.DateTime:
                        case DbType.DateTime2:
                        case DbType.DateTimeOffset:
                            _sqlrcur.defineOutputBindDate(param.ParameterName);
                            continue;

                        case DbType.Binary:
                            _sqlrcur.defineOutputBindBlob(param.ParameterName);
                            continue;

                        case DbType.Boolean:
                            _sqlrcur.defineOutputBindInteger(param.ParameterName);
                            continue;

                        case DbType.Currency:
                        case DbType.Decimal:
                        case DbType.Single:
                        case DbType.Double:
                        case DbType.VarNumeric:
                            _sqlrcur.defineOutputBindDouble(param.ParameterName);
                            continue;

                        case DbType.Byte:
                        case DbType.Int16:
                        case DbType.Int32:
                        case DbType.Int64:
                        case DbType.SByte:
                        case DbType.UInt16:
                        case DbType.UInt32:
                        case DbType.UInt64 :
                            _sqlrcur.defineOutputBindInteger(param.ParameterName);
                            continue;

                        case DbType.Object:
                        case DbType.Xml:
                            _sqlrcur.defineOutputBindString(param.ParameterName, param.Size);
                            continue;
                    }
                }
                else if (param.Direction == ParameterDirection.InputOutput)
                {
                    // FIXME: SQL Relay doesn't currently support in/out parameters
                    throw new NotSupportedException();
                }
            }
        }

        private void copyOutBindValues()
        {
            for (Int32 i = 0; i < Parameters.Count; i++)
            {

                SQLRelayParameter param = (SQLRelayParameter)Parameters[i];

                if (param.Direction == ParameterDirection.Output)
                {

                    param.Size = _sqlrcur.getOutputBindLength(param.ParameterName);

                    switch (param.SQLRelayType)
                    {
                        case SQLRelayType.Clob:
                            param.Value = _sqlrcur.getOutputBindClob(param.ParameterName);
                            continue;
                        case SQLRelayType.Blob:
                            param.Value = _sqlrcur.getOutputBindBlob(param.ParameterName);
                            continue;
                        case SQLRelayType.Cursor:
                            // FIXME: not implemented yet
                            throw new NotSupportedException();
                    }

                    switch (param.DbType)
                    {
                        case DbType.AnsiString:
                        case DbType.AnsiStringFixedLength:
                        case DbType.String:
                        case DbType.StringFixedLength:
                        case DbType.Time:
                        case DbType.Guid:
                            param.Value = _sqlrcur.getOutputBindString(param.ParameterName);
                            break;

                        case DbType.Date:
                        case DbType.DateTime:
                        case DbType.DateTime2:
                        case DbType.DateTimeOffset:
                            Int16 year = 0;
                            Int16 month = 0;
                            Int16 day = 0;
                            Int16 hour = 0;
                            Int16 minute = 0;
                            Int16 second = 0;
                            String tz = null;
                            _sqlrcur.getOutputBindDate(param.ParameterName, out year, out month, out day, out hour, out minute, out second, out tz);
                            if (year == -1)
                            {
                                year = 0;
                            }
                            if (month == -1)
                            {
                                month = 0;
                            }
                            if (day == -1)
                            {
                                day = 0;
                            }
                            if (hour == -1)
                            {
                                hour = 0;
                            }
                            if (minute == -1)
                            {
                                minute = 0;
                            }
                            if (second == -1)
                            {
                                second = 0;
                            }
                            param.Value = new DateTime(year, month, day, hour, minute, second);
                            break;

                        case DbType.Binary:
                            param.Value = _sqlrcur.getOutputBindBlob(param.ParameterName);
                            break;

                        case DbType.Boolean:
                            param.Value = _sqlrcur.getOutputBindInteger(param.ParameterName);
                            break;

                        case DbType.Currency:
                        case DbType.Decimal:
                        case DbType.Single:
                        case DbType.Double:
                        case DbType.VarNumeric:
                            param.Value = _sqlrcur.getOutputBindDouble(param.ParameterName);
                            break;

                        case DbType.Byte:
                        case DbType.Int16:
                        case DbType.Int32:
                        case DbType.Int64:
                        case DbType.SByte:
                        case DbType.UInt16:
                        case DbType.UInt32:
                        case DbType.UInt64 :
                            param.Value = _sqlrcur.getOutputBindInteger(param.ParameterName);
                            break;

                        case DbType.Object:
                        case DbType.Xml:
                            param.Value = _sqlrcur.getOutputBindString(param.ParameterName);
                            break;
                    }
                }
            }
        }

        #endregion
    }
}
