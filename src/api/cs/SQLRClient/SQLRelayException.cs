using System;
using System.Runtime.Serialization;

namespace SQLRClient
{
    [Serializable]
    public sealed class SQLRelayException : SystemException
    {
        #region member variables
        Int64 number = 0;
        #endregion

        #region constructors and destructors
        internal SQLRelayException(Int64 number, String message)
            : base(message)
        {
            this.number = number;
        }

        private SQLRelayException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            number = info.GetInt64("number");
        }
        #endregion

        #region properties
        public Int64 Number
        {
            get
            {
                return number;
            }
        }
        #endregion

        #region public methods
        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("number", number);
            base.GetObjectData(info, context);
        }
        #endregion
    }
}
