using System;
using System.Data;

namespace SQLRClient
{
    public class SQLRelayConnection : IDbConnection
    {

        #region member variables

        private ConnectionState _connectionstate = ConnectionState.Closed;
        private SQLRConnection _sqlrcon = null;
        private String _connectionstring = null;
        private String _host = null;
        private UInt16 _port = 0;
        private String _socket = null;
        private String _user = null;
        private String _password = null;
        private Int32 _retrytime = 0;
        private Int32 _tries = 1;
        private String _db = null;
        private Boolean  _debug = false;

        #endregion


        #region constructors and destructors

        public SQLRelayConnection()
        {
        }

        public SQLRelayConnection(String connectstring)
        {
            ConnectionString = connectstring;
        }

        private void Dispose(Boolean  disposing)
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

        public String ConnectionString
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
                String[] parts = ConnectionString.Split(";".ToCharArray());

                foreach (String part in parts)
                {
                    String[] subparts = part.Split("=".ToCharArray());
                    if (subparts[0] == "Data Source")
                    {
                        String datasource = subparts[1];

                        // split the data source, look for one of the following formats:
                        // host:port:socket
                        // host:port
                        // socket
                        String[] subsubparts = datasource.Split(":".ToCharArray());
                        if (subsubparts.Length == 1)
                        {
                            _socket = subsubparts[0];
                        }
                        else
                        {
                            _host = subsubparts[0];
                            _port = Convert.ToUInt16(subsubparts[1]);
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
                        _retrytime = Convert.ToInt32(subparts[1]);
                    }
                    else if (subparts[0] == "Tries")
                    {
                        _tries = Convert.ToInt32(subparts[1]);
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

        public Int32 ConnectionTimeout
        {
            get
            {
                return _retrytime * _tries;
            }
        }

        public String Database
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

        public Boolean  Debug
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

        public void ChangeDatabase(String db)
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
