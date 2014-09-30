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
        private Boolean _debug = false;

        #endregion


        #region constructors and destructors

        /** Initializes a new instance of the SQLRelayConnection class. */
        public SQLRelayConnection()
        {
        }

        /** Initializes a new instance of the SQLRelayConnection class when
         *  given a string that contains the connection string.  The connection
         *  string must be of the following format:
         *  "variable=value;variable=value;"  Valid variables include:
         *
         *  Data Source - The SQL Relay server to connect to.  This may be
         *  specified as host:port, host:port:socket or just socket.  If host,
         *  port and socket are all three specified, then a connection will
         *  first be attempted to the local socket and then to the host/port.
         *
         *  User ID - The username to use when logging into SQL Relay.
         *
         *  Password - The password to use when logging into SQL Relay.
         *
         *  Retry Time - If a connection fails, it will be retried on this
         *  interval (in seconds).
         *
         *  Tries - If a connection fails, it will be retried this many times.
         *
         *  Initial Catalog - The database/schema to switch to after logging in.
         *  Optional.
         *
         *  Debug - If this is set to true then debug is enabled. */
        public SQLRelayConnection(String connectstring)
        {
            ConnectionString = connectstring;
        }

        private void Dispose(Boolean disposing)
        {
            if (_connectionstate == ConnectionState.Open)
            {
                Close();
            }
        }

        /** Releases all resources used by the SQLRelayConnection. */
        void IDisposable.Dispose()
        {
            Dispose(true);
            System.GC.SuppressFinalize(this);
        }

        #endregion


        #region properties

        /** Gets or set the string used to open a connection to SQL Relay.
         *  See the constructor for a descripton of the connection string. */
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

        /** Gets Retry Time * Tries.  See the constructor for a descripton
         *  of Retry Time and Tries. */
        public Int32 ConnectionTimeout
        {
            get
            {
                return _retrytime * _tries;
            }
        }

        /** Gets the Initial Catalog, if one was set.  See the constructor for
         *  a descripton of the Initial Catalog. */
        public String Database
        {
            get
            {
                return _db;
            }
        }

        /** Indicates the state of the SQLRelayConnection when the most recent
         *  network operation was performed on the connection. */
        public ConnectionState State
        {
            get
            {
                return _connectionstate;
            }
        }

        /** Gets or sets whether debug is enabled. */
        public Boolean Debug
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

        /** Opens a database connection with the property settings specified by
         *  the ConnectionString.  */
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

        /** Closes the connection to the database.  This is the preferred
         *  method of closing any open connection. */
        public void Close()
        {
            validConnection();

            _connectionstate = ConnectionState.Closed;
            _sqlrcon.endSession();
        }

        /** Starts a database transaction. */
        IDbTransaction IDbConnection.BeginTransaction()
        {
            return BeginTransaction();
        }

        /** Starts a database transaction. */
        public SQLRelayTransaction BeginTransaction()
        {
            validConnection();

            if (!_sqlrcon.begin())
            {
                return null;
            }

            SQLRelayTransaction trans = new SQLRelayTransaction();
            trans.Connection = this;
            return trans;
        }

        /** Starts a database transaction with the specified isolation level.
         *  This method is implemented because it is required by the interface
         *  but SQL Relay doesn't currently support setting the isolation level
         *  here. */
        IDbTransaction IDbConnection.BeginTransaction(IsolationLevel isolationlevel)
        {
            return BeginTransaction(isolationlevel);
        }

        /** Starts a database transaction with the specified isolation level.
         *  This method is implemented because it is required by the interface
         *  but SQL Relay doesn't currently support setting the isolation level
         *  here. */
        public SQLRelayTransaction BeginTransaction(IsolationLevel isolationlevel)
        {
            throw new InvalidOperationException();
        }

        /** Changes the current database for an open SQLRelayConnection. */
        public void ChangeDatabase(String db)
        {
            validConnection();
            _db = db;
            _sqlrcon.selectDatabase(db);
        }

        /** Creates and returns a SQLRelayCommand object associated with the
         *  SQLRelayConnection. */
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
