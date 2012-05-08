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

        static private readonly object EventRowUpdating = new object();
        static private readonly object EventRowUpdated = new object();

        public SQLRelayDataAdapter()
        {
        }

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

        override protected RowUpdatingEventArgs CreateRowUpdatingEvent(DataRow datarow, IDbCommand command, StatementType statementtype, DataTableMapping datatablemapping)
        {
            return new SQLRelayRowUpdatingEventArgs(datarow, command, statementtype, datatablemapping);
        }

        override protected RowUpdatedEventArgs CreateRowUpdatedEvent(DataRow datarow, IDbCommand command, StatementType statementtype, DataTableMapping datatablemapping)
        {
            return new SQLRelayRowUpdatedEventArgs(datarow, command, statementtype, datatablemapping);
        }

        protected override void OnRowUpdating(RowUpdatingEventArgs value)
        {
            SQLRelayRowUpdatingEventHandler handler = (SQLRelayRowUpdatingEventHandler)Events[EventRowUpdating];
            if (handler != null && value is SQLRelayRowUpdatingEventArgs)
            {
                handler(this, (SQLRelayRowUpdatingEventArgs)value);
            }
        }

        protected override void OnRowUpdated(RowUpdatedEventArgs value)
        {
            SQLRelayRowUpdatedEventHandler handler = (SQLRelayRowUpdatedEventHandler)Events[EventRowUpdated];
            if (handler != null && value is SQLRelayRowUpdatedEventArgs)
            {
                handler(this, (SQLRelayRowUpdatedEventArgs)value);
            }
        }

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

    public delegate void SQLRelayRowUpdatingEventHandler(object sender, SQLRelayRowUpdatingEventArgs e);
    public delegate void SQLRelayRowUpdatedEventHandler(object sender, SQLRelayRowUpdatedEventArgs e);

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
