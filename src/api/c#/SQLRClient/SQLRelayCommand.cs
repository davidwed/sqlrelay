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
        private string _commandtext = null;
        private bool _prepared = false;
        private UpdateRowSource _updaterowsource = UpdateRowSource.None;
        private SQLRelayParameterCollection _sqlrelayparams = new SQLRelayParameterCollection();

        #endregion


        #region constructors and destructors

        public SQLRelayCommand()
        {
        }

        public SQLRelayCommand(string commandtext)
        {
            _commandtext = commandtext;
        }

        public SQLRelayCommand(string commandtext, SQLRelayConnection sqlrelaycon)
        {
            _commandtext = commandtext;
            _sqlrelaycon = sqlrelaycon;
        }

        public SQLRelayCommand(string commandtext, SQLRelayConnection sqlrelaycon, SQLRelayTransaction sqlrelaytran)
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

        private void Dispose(bool disposing)
        {
            // FIXME: do anything?
        }

        #endregion


        #region properties

        public string CommandText
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

        public int CommandTimeout
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

        public int ExecuteNonQuery()
        {
            return (runQuery()) ? (int)_sqlrcur.affectedRows() : 0;
        }

        public IDataReader ExecuteReader()
        {
            return ExecuteReader(CommandBehavior.Default);
        }

        public IDataReader ExecuteReader(CommandBehavior commandbehavior)
        {
            return (runQuery()) ? new SQLRelayDataReader(_sqlrelaycon, _sqlrcur, commandbehavior == CommandBehavior.CloseConnection) : null;
        }

        public object ExecuteScalar()
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

        private bool runQuery()
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

            for (int i = 0; i < Parameters.Count; i++)
            {

                SQLRelayParameter param = (SQLRelayParameter)Parameters[i];

                if (param.Direction == ParameterDirection.Input)
                {

                    switch (param.SQLRelayType)
                    {
                        case SQLRelayType.Clob:
                            Console.WriteLine("binding clob");
                            _sqlrcur.inputBindClob(param.ParameterName, (string)param.Value, (uint)((string)param.Value).Length);
                            continue;
                        case SQLRelayType.Blob:
                            //_sqlrcur.inputBindBlob(param.ParameterName, (byte[])param.Value, (uint)(byte[])param.Value).Length);
                            continue;
                        case SQLRelayType.Cursor:
                            // FIXME: not implemented yet
                            throw new NotSupportedException();
                    }

                    switch (param.DbType)
                    {
                        case DbType.AnsiString:
                        case DbType.AnsiStringFixedLength:
                        case DbType.Date:
                        case DbType.DateTime:
                        case DbType.DateTime2:
                        case DbType.DateTimeOffset:
                        case DbType.String:
                        case DbType.StringFixedLength:
                        case DbType.Time:
                        case DbType.Guid:
                            _sqlrcur.inputBind(param.ParameterName, (string)param.Value);
                            continue;

                        case DbType.Binary:
                            //_sqlrcur.inputBindBlob(param.ParameterName, (byte[])param.Value, (uint)(byte[])param.Value).Length);
                            continue;

                        case DbType.Boolean:
                            _sqlrcur.inputBind(param.ParameterName, (((bool)param.Value)==true) ? 1 : 0);
                            continue;

                        case DbType.Currency:
                        case DbType.Decimal:
                        case DbType.Single:
                        case DbType.Double:
                        case DbType.VarNumeric:
                            _sqlrcur.inputBind(param.ParameterName, (double)param.Value, 0, 0);
                            continue;

                        case DbType.Byte:
                        case DbType.Int16:
                        case DbType.Int32:
                        case DbType.Int64:
                        case DbType.SByte:
                        case DbType.UInt16:
                        case DbType.UInt32:
                        case DbType.UInt64:
                            _sqlrcur.inputBind(param.ParameterName, (long)param.Value);
                            continue;

                        case DbType.Object:
                        case DbType.Xml:
                            _sqlrcur.inputBind(param.ParameterName, param.Value.ToString());
                            continue;
                    }

                }
                else if (param.Direction == ParameterDirection.Output)
                {
                    switch (param.DbType)
                    {
                        case DbType.AnsiString:
                        case DbType.AnsiStringFixedLength:
                        case DbType.Date:
                        case DbType.DateTime:
                        case DbType.DateTime2:
                        case DbType.DateTimeOffset:
                        case DbType.String:
                        case DbType.StringFixedLength:
                        case DbType.Time:
                        case DbType.Guid:
                            // FIXME: length?
                            _sqlrcur.defineOutputBindString(param.ParameterName, 32768);
                            continue;

                        case DbType.Binary:
                            // FIXME: I should use inputBindBlob but how do i get the size?
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
                        case DbType.UInt64:
                            _sqlrcur.defineOutputBindInteger(param.ParameterName);
                            continue;

                        case DbType.Object:
                        case DbType.Xml:
                            // FIXME: length?
                            _sqlrcur.defineOutputBindString(param.ParameterName, 32768);
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
            for (int i = 0; i < Parameters.Count; i++)
            {

                SQLRelayParameter param = (SQLRelayParameter)Parameters[i];

                if (param.Direction == ParameterDirection.Output)
                {
                    switch (param.DbType)
                    {
                        case DbType.AnsiString:
                        case DbType.AnsiStringFixedLength:
                        case DbType.Date:
                        case DbType.DateTime:
                        case DbType.DateTime2:
                        case DbType.DateTimeOffset:
                        case DbType.String:
                        case DbType.StringFixedLength:
                        case DbType.Time:
                        case DbType.Guid:
                            param.Value = _sqlrcur.getOutputBindString(param.ParameterName);
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
                        case DbType.UInt64:
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
