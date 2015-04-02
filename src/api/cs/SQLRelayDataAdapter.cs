using System;
using System.Data;
using System.Data.Common;

namespace SQLRClient
{
    public class SQLRelayDataAdapter : DbDataAdapter, IDbDataAdapter
    {
        private SQLRelayCommand _selectcommand;
        private SQLRelayCommand _insertcommand;
        private SQLRelayCommand _updatecommand;
        private SQLRelayCommand _deletecommand;

        static private readonly Object EventRowUpdating = new Object();
        static private readonly Object EventRowUpdated = new Object();

        /** Initializes a new instance of the SQLRelayDataAdapter class. */
        public SQLRelayDataAdapter()
        {
        }

        /** Gets or set the query used to select records in the data source. */
        new public SQLRelayCommand SelectCommand
        {
            get
            {
                return _selectcommand;
            }
            set
            {
                _selectcommand = value;
            }
        }

        /** Gets or set the query used to select records in the data source. */
        IDbCommand IDbDataAdapter.SelectCommand
        {
            get
            {
                return _selectcommand;
            }
            set
            {
                _selectcommand = (SQLRelayCommand)value;
            }
        }

        /** Gets or set the query used to insert records into the data
         *  source. */
        new public SQLRelayCommand InsertCommand
        {
            get
            {
                return _insertcommand;
            }
            set
            {
                _insertcommand = value;
            }
        }

        /** Gets or set the query used to insert records into the data
         *  source. */
        IDbCommand IDbDataAdapter.InsertCommand
        {
            get
            {
                return _insertcommand;
            }
            set
            {
                _insertcommand = (SQLRelayCommand)value;
            }
        }

        /** Gets or set the query used to update records in the data source. */
        new public SQLRelayCommand UpdateCommand
        {
            get
            {
                return _updatecommand;
            }
            set
            {
                _updatecommand = value;
            }
        }

        /** Gets or set the query used to update records in the data source. */
        IDbCommand IDbDataAdapter.UpdateCommand
        {
            get
            {
                return _updatecommand;
            }
            set
            {
                _updatecommand = (SQLRelayCommand)value;
            }
        }

        /** Gets or set the query used to delete records from the data
         *  source. */
        new public SQLRelayCommand DeleteCommand
        {
            get
            {
                return _deletecommand;
            }
            set
            {
                _deletecommand = value;
            }
        }

        /** Gets or set the query used to delete records from the data
         *  source. */
        IDbCommand IDbDataAdapter.DeleteCommand
        {
            get
            {
                return _deletecommand;
            }
            set
            {
                _deletecommand = (SQLRelayCommand)value;
            }
        }

        /** Initializes a new instance of the RowUpdatingEventArgs class. */
        override protected RowUpdatingEventArgs CreateRowUpdatingEvent(DataRow datarow, IDbCommand command, StatementType statementtype, DataTableMapping datatablemapping)
        {
            return new SQLRelayRowUpdatingEventArgs(datarow, command, statementtype, datatablemapping);
        }

        /** Initializes a new instance of the RowUpdatingEventArgs class. */
        override protected RowUpdatedEventArgs CreateRowUpdatedEvent(DataRow datarow, IDbCommand command, StatementType statementtype, DataTableMapping datatablemapping)
        {
            return new SQLRelayRowUpdatedEventArgs(datarow, command, statementtype, datatablemapping);
        }

        /** Raises the RowUpdating event of a .NET Framework data provider. */
        protected override void OnRowUpdating(RowUpdatingEventArgs value)
        {
            SQLRelayRowUpdatingEventHandler handler = (SQLRelayRowUpdatingEventHandler)Events[EventRowUpdating];
            if (handler != null && value is SQLRelayRowUpdatingEventArgs)
            {
                handler(this, (SQLRelayRowUpdatingEventArgs)value);
            }
        }

        /** Raises the RowUpdated event of a .NET Framework data provider. */
        protected override void OnRowUpdated(RowUpdatedEventArgs value)
        {
            SQLRelayRowUpdatedEventHandler handler = (SQLRelayRowUpdatedEventHandler)Events[EventRowUpdated];
            if (handler != null && value is SQLRelayRowUpdatedEventArgs)
            {
                handler(this, (SQLRelayRowUpdatedEventArgs)value);
            }
        }

        /** Occurs during Update before a command is executed against the
         *  data source.  The attempt to update is made, so the even fires. */
        public event SQLRelayRowUpdatingEventHandler RowUpdating
        {
            add
            {
                Events.AddHandler(EventRowUpdating, value);
            }
            remove
            {
                Events.RemoveHandler(EventRowUpdating, value);
            }
        }

        /** Occurs during Update after a command is executed against the
         *  data source.  The attempt to update is made, so the even fires. */
        public event SQLRelayRowUpdatedEventHandler RowUpdated
        {
            add
            {
                Events.AddHandler(EventRowUpdated, value);
            }
            remove
            {
                Events.RemoveHandler(EventRowUpdated, value);
            }
        }
    }

    public delegate void SQLRelayRowUpdatingEventHandler(Object sender, SQLRelayRowUpdatingEventArgs e);
    public delegate void SQLRelayRowUpdatedEventHandler(Object sender, SQLRelayRowUpdatedEventArgs e);

    public class SQLRelayRowUpdatingEventArgs : RowUpdatingEventArgs
    {
        public SQLRelayRowUpdatingEventArgs(DataRow datarow, IDbCommand command, StatementType statementtype, DataTableMapping datatablemapping)
            : base(datarow, command, statementtype, datatablemapping)
        {
        }

        new public SQLRelayCommand Command
        {
            get
            {
                return (SQLRelayCommand)base.Command;
            }
            set
            {
                base.Command = value;
            }
        }
    }

    public class SQLRelayRowUpdatedEventArgs : RowUpdatedEventArgs
    {
        public SQLRelayRowUpdatedEventArgs(DataRow datarow, IDbCommand command, StatementType statementtype, DataTableMapping datatablemapping)
            : base(datarow, command, statementtype, datatablemapping)
        {
        }

        new public SQLRelayCommand Command
        {
            get
            {
                return (SQLRelayCommand)base.Command;
            }
        }
    }
}
