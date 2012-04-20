using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;

namespace SQLRClient
{
    class SQLRelayCommand : IDbCommand
    {
        private SQLRelayConnection _sqlrelaycon;
        private SQLRelayTransaction _sqlrelaytran;
        private string _commandtext;
        private UpdateRowSource _updaterowsource = UpdateRowSource.None;
        private SQLRelayParameterCollection _sqlrelayparams = new SQLRelayParameterCollection();

        public SQLRelayCommand()
        {
        }

        public SQLRelayCommand(string commandtext)
        {
            init(commandtext, null, null);
        }

        public SQLRelayCommand(string commandtext, SQLRelayConnection sqlrelaycon)
        {
            init(commandtext, sqlrelaycon, null);
        }

        public SQLRelayCommand(string commandtext, SQLRelayConnection sqlrelaycon, SQLRelayTransaction sqlrelaytran)
        {
            init(commandtext, sqlrelaycon, sqlrelaytran);
        }

        private void init(string commandtext, SQLRelayConnection sqlrelaycon, SQLRelayTransaction sqlrelaytran)
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
                // set the tranaction to null if the connection is changed
                if (value != _sqlrelaycon)
                {
                    _sqlrelaytran = null;
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

        private IDataParameterCollection IDbCommand.Parameters
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

        public UpdateRowSource UpdateRowSource
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

        public IDataParameter CreateParameter()
        {
            return (IDataParameter)(new SQLRelayParameter());
        }

        public int ExecuteNonQuery()
        {

            validConnection();

            // FIXME: execute the query and return the number of affected rows
            return 0;
        }

        private void validConnection()
        {
            if (_sqlrelaycon == null || _sqlrelaycon.State != ConnectionState.Open)
            {
                throw new InvalidOperationException("Connection must be valid and open.");
            }
        }

        public IDataReader ExecuteReader()
        {
            validConnection();

            // FIXME: execute the query and return a result set
            return new SQLRelayDataReader();
        }

        public IDataReader ExecuteReader(CommandBehavior commandbehavior)
        {
            validConnection();

            // FIXME: execute the query and return a result set

            if (commandbehavior == CommandBehavior.CloseConnection)
            {
                // FIXME: set a flag so endSession gets called
                return new SQLRelayDataReader();
            }
            else
            {
                return new SQLRelayDataReader();
            }
        }

        public object ExecuteScalar()
        {
            validConnection();

            // FIXME: execute the query and return the first column of the first row
            return null;
        }

        public void Prepare()
        {
            validConnection();

            // FIXME: prepare query
        }

        public void IDisposable.Dispose()
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
