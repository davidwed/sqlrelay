using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;

namespace SQLRClient
{
    class SQLRelayConnection : IDbConnection
    {
        private ConnectionState _connectionstate;
        private SQLRConnection _sqlrcon;
        private string _connectstring;
        private string _host;
        private ushort _port;
        private string _socket;
        private string _user;
        private string _password;
        private int _retrytime;
        private int _tries;
        private string _db;


        public SQLRelayConnection()
        {
            init(null, null, 0, null, null, null, 0, 0, null);
        }

        public SQLRelayConnection(string connectstring)
        {

            // parse the connectstring
            string host = null;
            ushort port = 0;
            string socket = null;
            string user = null;
            string password = null;
            int retrytime = 0;
            int tries = 0;
            string db = null;

            string[] parts = connectstring.Split(";".ToCharArray());

            foreach (string part in parts)
            {
                string[] subparts = part.Split("=".ToCharArray());
                if (subparts[0] == "host")
                {
                    host = subparts[1];
                }
                else if (subparts[0] == "port")
                {
                    port = ushort.Parse(subparts[1]);
                }
                else if (subparts[0] == "socket")
                {
                    socket = subparts[1];
                }
                else if (subparts[0] == "user")
                {
                    user = subparts[1];
                }
                else if (subparts[0] == "password")
                {
                    password = subparts[1];
                }
                else if (subparts[0] == "retrytime")
                {
                    retrytime = int.Parse(subparts[1]);
                }
                else if (subparts[0] == "tries")
                {
                    tries = int.Parse(subparts[1]);
                }
                else if (subparts[0] == "db")
                {
                    db = subparts[1];
                }
            }

            // init class variables
            init(connectstring, host, port, socket, user, password, retrytime, tries, db);

            // create SQLRconnection
            _sqlrcon = new SQLRConnection(_host, _port, _socket, _user, _password, _retrytime, _tries);

            // set db
            ChangeDatabase(_db);
        }

        private void init(string connectstring, string host, ushort port, string socket, string user, string password, int retrytime, int tries, string db)
        {
            _connectionstate = ConnectionState.Closed;
            _sqlrcon = null;
            _connectstring = connectstring;
            _host = host;
            _port = port;
            _socket = socket;
            _user = user;
            _password = password;
            _retrytime = retrytime;
            _tries = tries;
            _db = db;
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

        private void Dispose(bool disposing)
        {
            if (_connectionstate == ConnectionState.Open)
            {
                Close();
            }
        }

        public void IDisposable.Dispose()
        {
            Dispose(true);
            System.GC.SuppressFinalize(this);
        }
    }
}
