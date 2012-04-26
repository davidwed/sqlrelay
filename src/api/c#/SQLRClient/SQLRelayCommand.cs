using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;

namespace SQLRClient
{
    public class SQLRelayCommand : IDbCommand
    {
        private SQLRelayConnection _sqlrelaycon = null;
        private SQLRelayTransaction _sqlrelaytran = null;
        private SQLRCursor _sqlrcur = null;
        private string _commandtext = null;
        private bool _prepared = false;
        private UpdateRowSource _updaterowsource = UpdateRowSource.None;
        private SQLRelayParameterCollection _sqlrelayparams = new SQLRelayParameterCollection();

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
            // huh?
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

        public void Cancel()
        {
            throw new NotSupportedException();
        }

        public IDbDataParameter CreateParameter()
        {
            return (IDbDataParameter)(new SQLRelayParameter());
        }

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

            return (_prepared && _sqlrcur.executeQuery()) || (!_prepared && _sqlrcur.sendQuery(_commandtext));
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
                return (object)_sqlrcur.getField(0, 0);
            }
            return null;
        }

        public void Prepare()
        {
            validConnection();

            getCursor().prepareQuery(_commandtext);
            _prepared = true;
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
    }
}
