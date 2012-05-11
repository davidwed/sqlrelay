using System;
using System.Data;

namespace SQLRClient
{
    public class SQLRelayConnection : IDbConnection
    {

        #region member variables

        private ConnectionState _connectionstate = ConnectionState.Closed;
        private SQLRConnection _sqlrcon = null;
        private string _connectionstring = null;
        private string _host = null;
        private ushort _port = 0;
        private string _socket = null;
        private string _user = null;
        private string _password = null;
        private int _retrytime = 0;
        private int _tries = 1;
        private string _db = null;
        private bool _debug = false;

        #endregion


        #region constructors and destructors

        public SQLRelayConnection()
        {
        }

        public SQLRelayConnection(string connectstring)
        {
            ConnectionString = connectstring;
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

        #endregion


        #region properties

        public string ConnectionString
        {
            get
            {
                return _connectionstring;
            }
            set
            {
                _connectionstring = value;

                // reset connection arguments
                _host = null;
                _port = 0;
                _socket = null;
                _user = null;
                _password = null;
                _retrytime = 0;
                _tries = 0;
                _db = null;
                _debug = false;

                // parse the connection string
                string[] parts = ConnectionString.Split(";".ToCharArray());

                foreach (string part in parts)
                {
                    string[] subparts = part.Split("=".ToCharArray());
                    if (subparts[0] == "Data Source")
                    {
                        string datasource = subparts[1];

                        // split the data source, look for one of the following formats:
                        // host:port:socket
                        // host:port
                        // socket
                        string[] subsubparts = datasource.Split(":".ToCharArray());
                        if (subsubparts.Length == 1)
                        {
                            _socket = subsubparts[0];
                        }
                        else
                        {
                            _host = subsubparts[0];
                            _port = ushort.Parse(subsubparts[1]);
                            if (subsubparts.Length > 2)
                            {
                                _socket = subsubparts[2];
                            }
                        }
                    }
                    else if (subparts[0] == "User ID")
                    {
                        _user = subparts[1];
                    }
                    else if (subparts[0] == "Password")
                    {
                        _password = subparts[1];
                    }
                    else if (subparts[0] == "Retry Time")
                    {
                        _retrytime = int.Parse(subparts[1]);
                    }
                    else if (subparts[0] == "Tries")
                    {
                        _tries = int.Parse(subparts[1]);
                    }
                    else if (subparts[0] == "Initial Catalog")
                    {
                        _db = subparts[1];
                    }
                    else if (subparts[0] == "Debug")
                    {
                        _debug = (subparts[1] == "true");
                    }
                }
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

        public bool Debug
        {
            get
            {
                return _debug;
            }
            set
            {
                _debug = value;
                if (_debug)
                {
                    _sqlrcon.debugOn();
                }
                else
                {
                    _sqlrcon.debugOff();
                }
            }
        }

        #endregion


        #region public methods

        public void Open()
        {
            if (_connectionstring == null)
            {
                throw new InvalidOperationException("ConnectionString must be non-null");
            }

            if (_connectionstate == ConnectionState.Open)
            {
                Close();
            }

            _connectionstate = ConnectionState.Open;
            _sqlrcon = new SQLRConnection(_host, _port, _socket, _user, _password, _retrytime, _tries);
            ChangeDatabase(_db);

            if (_debug)
            {
                _sqlrcon.debugOn();
            }
        }

        public void Close()
        {
            validConnection();

            _connectionstate = ConnectionState.Closed;
            _sqlrcon.endSession();
        }

        public IDbTransaction BeginTransaction()
        {
            validConnection();

            SQLRelayTransaction trans = new SQLRelayTransaction();
            trans.Connection = this;
            // FIXME: some db's require a "begin"
            // maybe I need to add a method to the c++ api for that
            return trans;
        }

        public IDbTransaction BeginTransaction(IsolationLevel isolationlevel)
        {
            throw new InvalidOperationException();
        }

        public void ChangeDatabase(string db)
        {
            validConnection();
            _db = db;
            _sqlrcon.selectDatabase(db);
        }

        public IDbCommand CreateCommand()
        {
            return new SQLRelayCommand(null, this);
        }

        internal SQLRConnection SQLRConnection
        {
            get
            {
                return _sqlrcon;
            }
        }

        #endregion

        #region private methods
        private void validConnection()
        {
            if (_connectionstate != ConnectionState.Open)
            {
                throw new InvalidOperationException("Connection must be open");
            }
        }
        #endregion
    }
}
