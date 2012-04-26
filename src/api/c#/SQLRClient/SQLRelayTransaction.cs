using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;

namespace SQLRClient
{
    public class SQLRelayTransaction : IDbTransaction
    {
        private SQLRelayConnection _sqlrelaycon = null;
        private SQLRConnection _sqlrcon = null;

        public IsolationLevel IsolationLevel
        {
            get
            {
                // FIXME: is there an interface for getting this?
                return IsolationLevel.ReadCommitted;
            }
        }

        public void Commit()
        {
            _sqlrcon.commit();
        }

        public void Rollback()
        {
            _sqlrcon.rollback();
        }

        public IDbConnection Connection
        {
            get
            {
                return this.Connection;
            }
        }

        private void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (this.Connection != null)
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
    }
}
