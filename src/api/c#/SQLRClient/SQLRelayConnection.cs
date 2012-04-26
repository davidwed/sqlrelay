using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;

namespace SQLRClient
{
    public class SQLRelayConnection : IDbConnection
    {
        private ConnectionState _connectionstate = ConnectionState.Closed;
        private SQLRConnection _sqlrcon = null;
        private string _connectstring = null;
        private string _host = null;
        private ushort _port = 0;
        private string _socket = null;
        private string _user = null;
        private string _password = null;
        private int _retrytime = 0;
        private int _tries = 1;
        private string _db = null;


        public SQLRelayConnection()
        {
        }

        public SQLRelayConnection(string connectstring)
        {

            // parse the connect string
            _connectstring = connectstring;

            string[] parts = connectstring.Split(";".ToCharArray());

            foreach (string part in parts)
            {
                string[] subparts = part.Split("=".ToCharArray());
                if (subparts[0] == "host")
                {
                    _host = subparts[1];
                }
                else if (subparts[0] == "port")
                {
                    _port = ushort.Parse(subparts[1]);
                }
                else if (subparts[0] == "socket")
                {
                    _socket = subparts[1];
                }
                else if (subparts[0] == "user")
                {
                    _user = subparts[1];
                }
                else if (subparts[0] == "password")
                {
                    _password = subparts[1];
                }
                else if (subparts[0] == "retrytime")
                {
                    _retrytime = int.Parse(subparts[1]);
                }
                else if (subparts[0] == "tries")
                {
                    _tries = int.Parse(subparts[1]);
                }
                else if (subparts[0] == "db")
                {
                    _db = subparts[1];
                }
            }

            // create SQLRconnection
            _sqlrcon = new SQLRConnection(_host, _port, _socket, _user, _password, _retrytime, _tries);

            // set db
            ChangeDatabase(_db);
        }

        public string ConnectionString
        {
            get
            {
                return _connectstring;
            }
            set
            {
                _connectstring = value;
            }
        }

        public int ConnectionTimeout
        {
            get
            {
                return _retrytime * _tries;
            }
        }

        public string Database
        {
            get
            {
                return _db;
            }
        }

        public ConnectionState State
        {
            get
            {
                return _connectionstate;
            }
        }

        public IDbTransaction BeginTransaction()
        {
            throw new NotSupportedException();
        }

        public IDbTransaction BeginTransaction(IsolationLevel isolationlevel)
        {
            throw new NotSupportedException();
        }

        public void ChangeDatabase(string db)
        {
            _sqlrcon.selectDatabase(db);
        }

        public void Open()
        {
            _connectionstate = ConnectionState.Open;
        }

        public void Close()
        {
            _connectionstate = ConnectionState.Closed;
        }

        public IDbCommand CreateCommand()
        {
            return new SQLRelayCommand();
        }

        internal SQLRConnection SQLRConnection
        {
            get
            {
                return _sqlrcon;
            }
        }

        private void Dispose(bool disposing)
        {
            if (_connectionstate == ConnectionState.Open)
            {
                Close();
            }
        }

        void IDisposable.Dispose()
        {
            Dispose(true);
            System.GC.SuppressFinalize(this);
        }
    }
}
