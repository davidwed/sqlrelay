using System;
using System.Data;

namespace SQLRClient
{
    public class SQLRelayTransaction : IDbTransaction
    {

        #region member variables

        private Boolean  _open = false;

        #endregion


        #region constructors and destructors

        internal SQLRelayTransaction()
        {
            _open = true;
        }

        private void Dispose(Boolean  disposing)
        {
            if (disposing)
            {
                if (_open && Connection != null)
                {
                    Rollback();
                }
            }
        }

        void IDisposable.Dispose()
        {
            this.Dispose(true);
            System.GC.SuppressFinalize(this);
        }

        #endregion


        #region properties

        public IDbConnection Connection
        {
            get;
            set;
        }

        public IsolationLevel IsolationLevel
        {
            // FIXME: ideally this would do something
            // I may need to add some methods to the C++ API
            get
            {
                return IsolationLevel.ReadCommitted;
            }
        }

        #endregion


        #region public methods

        public void Commit()
        {
            validTransaction();
            ((SQLRelayConnection)Connection).SQLRConnection.commit();
            _open = false;
        }

        public void Rollback()
        {
            validTransaction();
            ((SQLRelayConnection)Connection).SQLRConnection.rollback();
            _open = false;
        }

        #endregion


        #region private methods

        private void validTransaction()
        {
            if (!_open)
            {
                throw new InvalidOperationException("Transaction must be open");
            }
            if (Connection == null)
            {
                throw new InvalidOperationException("Connection must be non-null");
            }
            if (Connection.State != ConnectionState.Open)
            {
                throw new InvalidOperationException("Connection must be open");
            }
        }

        #endregion
    }
}
