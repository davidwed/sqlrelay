using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SQLRClient
{
    [Serializable]
    public enum SQLRelayType
    {
        Object = 0,
        Clob,
        Blob,
        Cursor
    }
}
