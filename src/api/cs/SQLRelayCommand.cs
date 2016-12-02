// Copyright (c) 2012-2015  David Muse
// See the file COPYING for more information

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
        private Boolean _prepared = false;
        private UpdateRowSource _updaterowsource = UpdateRowSource.None;
        private SQLRelayParameterCollection _sqlrelayparams = new SQLRelayParameterCollection();

        #endregion


        #region constructors and destructors

        /** Initializes a new instance of the SQLRelayCommand class. */
        public SQLRelayCommand()
        {
        }

        /** Initializes a new instance of the SQLRelayCommand class with the
         *  text of the query. */
        public SQLRelayCommand(String commandtext)
        {
            _commandtext = commandtext;
        }

        /** Initializes a new instance of the SQLRelayCommand class with the
         *  text of the query and a SQLRelayConnection */
        public SQLRelayCommand(String commandtext, SQLRelayConnection sqlrelaycon)
        {
            _commandtext = commandtext;
            _sqlrelaycon = sqlrelaycon;
        }

        /** Initializes a new instance of the SQLRelayCommand class with the
         *  text of the query, a SQLRelayConnection, and the
         *  SQLRelayTransaction. */
        public SQLRelayCommand(String commandtext, SQLRelayConnection sqlrelaycon, SQLRelayTransaction sqlrelaytran)
        {
            _commandtext = commandtext;
            _sqlrelaycon = sqlrelaycon;
            _sqlrelaytran = sqlrelaytran;
        }

        /** Releases all resources used by the SQLRelayCommand. */
        ~SQLRelayCommand()
        {
            Dispose(false);
        }

        /** Releases all resources used by the SQLRelayCommand. */
        void IDisposable.Dispose()
        {
            Dispose(true);
            System.GC.SuppressFinalize(this);
        }

        private void Dispose(Boolean disposing)
        {
        }

        #endregion


        #region properties

        /** Gets or sets the text of the query that will be executed when
         *  ExecuteNonQuery, ExecuteScalar or ExecuteReader are called. */
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

        /** Gets or sets the wait time before terminating the attempt to
         *  execute a command and generating an error.  This method is
         *  implemented because it is required by the interface but SQLRelay
         *  does not support command timeouts, so a Get returns 0 and a Set to
         *  any value but 0 throws NotSupportedException. */
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

        /** Gets or sets a value indicating how the CommandText property is to
         *  be interpreted.  This method is implemented because it is required
         *  by the interface but SQLRelay does not support command types, so a
         *  Get returns CommandType.Text and a Set to any type other than
         *  CommandType.Text throws NotSupportedException. */
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

        /** Gets or sets the SQLRelayConnection used by this instance of
         *  SQLRelayCommand. */
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

        /** Gets the SQLRelayParameterCollection. */
        public SQLRelayParameterCollection Parameters
        {
            get
            {
                return _sqlrelayparams;
            }
        }

        /** Gets the SQLRelayParameterCollection. */
        IDataParameterCollection IDbCommand.Parameters
        {
            get
            {
                return _sqlrelayparams;
            }
        }

        /** Gets or sets the SQLRelayTransaction within which the
         *  SQLRelayCommand executes. */
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

        /** Gets or sets how command results are applied to the DataRow when
         *  used by the Update method of the DbDataAdapter. */
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

        /** Gets or sets the number of rows of the result set to buffer at
         *  a time.  0 (the default) means buffer the entire result set. */
        public UInt64 ResultSetBufferSize
        {
            get
            {
                return _sqlrcur.getResultSetBufferSize();
            }
            set
            {
                _sqlrcur.setResultSetBufferSize(value);
            }
        }

        #endregion


        #region public methods

        /** Tries to cancel the executeion of a SQLRelayCommand.  This method is
         *  implemented because it is required by the interface but SQLRelay
         *  does not support cancelling commands and calling it will throw
         *  NotSupportedException. */
        public void Cancel()
        {
            throw new NotSupportedException();
        }

        /** Creates a new instance of an SQLRelayParameter object. */
        public IDbDataParameter CreateParameter()
        {
            return (IDbDataParameter)(new SQLRelayParameter());
        }

        /** Sends the CommandText to the SQLRelayConnection and returns the
         *  number of rows affected. */
        public Int32 ExecuteNonQuery()
        {
            return (runQuery()) ? (Int32)_sqlrcur.affectedRows() : 0;
        }

        /** Sends the CommandText to the SQLRelayConnection and builds and
         *  returns a SQLRelayDataReader. */
        public IDataReader ExecuteReader()
        {
            return ExecuteReader(CommandBehavior.Default);
        }

        /** Sends the CommandText to the SQLRelayConnection and builds and
         *  returns a SQLRelayDataReader using one of the CommandBehavior
         *  values. */
        public IDataReader ExecuteReader(CommandBehavior commandbehavior)
        {

            // run the query...
            if (!runQuery())
            {
                return null;
            }

            // if there were output bind cursors then create a data reader and attach them as multiple result sets
            SQLRelayDataReader retval = null;
            foreach (SQLRelayParameter param in Parameters)
            {
                if (param.Direction == ParameterDirection.Output && param.SQLRelayType == SQLRelayType.Cursor)
                {
                    if (retval == null)
                    {
                        retval = new SQLRelayDataReader(_sqlrelaycon, ((SQLRelayDataReader)param.Value).Cursor, commandbehavior == CommandBehavior.CloseConnection);
                    }
                    else
                    {
                        retval.appendCursor(((SQLRelayDataReader)param.Value).Cursor);
                    }
                }
            }
            if (retval != null)
            {
                return retval;
            }

            // otherwise just return a data reader that uses the current cursor
            return new SQLRelayDataReader(_sqlrelaycon, _sqlrcur, commandbehavior == CommandBehavior.CloseConnection);
        }

        /** Sends the CommandText to the SQLRelayConnection and returns the
         *  first column of the first row in the result set returned by the
         *  query.  Additional columns or rows are ignored. */
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

        /** Creates a prepared version of the command on the database. */
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

                if (_sqlrelaycon._columnnamecase == "upper")
                {
                    _sqlrcur.upperCaseColumnNames();
                }
                else if (_sqlrelaycon._columnnamecase == "lower")
                {
                    _sqlrcur.lowerCaseColumnNames();
                }

                _sqlrcur.setResultSetBufferSize(_sqlrelaycon._resultsetbuffersize);

                if (_sqlrelaycon._dontgetcolumninfo)
                {
                    _sqlrcur.dontGetColumnInfo();
                }

                if (_sqlrelaycon._nullsasnulls)
                {
                    _sqlrcur.getNullsAsNulls();
                }
            }
            return _sqlrcur;
        }

        private Boolean runQuery()
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
                            _sqlrcur.inputBind(param.ParameterName, Convert.ToInt16(dt.Year), Convert.ToInt16(dt.Month), Convert.ToInt16(dt.Day), Convert.ToInt16(dt.Hour), Convert.ToInt16(dt.Minute), Convert.ToInt16(dt.Second), Convert.ToInt16(dt.Millisecond)*1000, null, false);
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
                            _sqlrcur.defineOutputBindCursor(param.ParameterName);
                            continue;
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
                            SQLRCursor cursor = _sqlrcur.getOutputBindCursor(param.ParameterName);
                            if (cursor.fetchFromBindCursor())
                            {
                                param.Value = new SQLRelayDataReader(_sqlrelaycon, cursor, false);
                            }
                            continue;
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
                            Int32 microsecond = 0;
                            String tz = null;
                            Boolean isnegative = false;
                            _sqlrcur.getOutputBindDate(param.ParameterName, out year, out month, out day, out hour, out minute, out second, out microsecond, out tz, out isnegative);
                            param.Value = new DateTime(year, month, day, hour, minute, second, microsecond/1000);
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
