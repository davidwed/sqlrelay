// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

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
