Attribute VB_Name = "modSqlrconnection"

Declare Function sqlrconnect Lib "libsqlrclientwrapper.dll" Alias "sqlrcon_alloc" ( _
ByVal server As String, _
ByVal port As Long, _
ByVal socket As String, _
ByVal user As String, _
ByVal password As String, _
ByVal retrytime As Long, _
ByVal tries As Long) As Long


Declare Function sqlrcursor Lib "libsqlrclientwrapper.dll" Alias "sqlrcur_alloc" (ByVal sqlrcon As Long) As Long


Declare Function sqlrcon_identify Lib "libsqlrclientwrapper.dll" (ByVal sqlrcon As Long) As String

Declare Function sqlrcon_ping Lib "libsqlrclientwrapper.dll" (ByVal sqlrcon As Long) As Long

Declare Sub sqlrcon_free Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long)
Declare Function sqlrcon_endSession Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long) As Long

Declare Function sqlrcon_suspendSession Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long) As Long

Declare Function sqlrcon_getConnectionPort Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long) As Long

Declare Function sqlrcon_getConnectionSocket Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long) As String

Declare Function sqlrcon_resumeSession Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long, _
ByVal port As Long, ByVal socket As String) As Long

Declare Function sqlrcon_autoCommit Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long) As Long

Declare Function sqlrcon_autoCommitOff Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long) As Long

Declare Function sqlrcon_commit Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long) As Long

Declare Function sqlrcon_rollback Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long) As Long

Declare Sub sqlrcon_debugOn Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long)

Declare Sub sqlrcon_debugOff Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long)

Declare Function sqlrcon_getDebug Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long) As Long

Declare Sub sqlrcur_copyReferences Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Sub sqlrcur_free Lib "libsqlrclientwrapper.dll" (ByVal sqlrconref As Long)

Declare Sub sqlrcur_setResultSetBufferSize Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, ByVal rows As Long)
     
Declare Function sqlrcur_getResultSetBufferSize Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref) As Long

Declare Sub sqlrcur_dontGetColumnInfo Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Sub sqlrcur_getColumnInfo Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Sub sqlrcur_mixedCaseColumnNames Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Sub sqlrcur_upperCaseColumnNames Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Sub sqlrcur_lowerCaseColumnNames Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Sub sqlrcur_cacheToFile Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal filename As String)

Declare Sub sqlrcur_setCacheTtl Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal ttl As Long)

Declare Function sqlrcur_getCacheFileName Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long _
) As String

Declare Sub sqlrcur_cacheOff Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Function sqlrcur_sendQuery Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal query As String) As Long

Declare Function sqlrcur_sendQueryWithLength Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal query As String, ByVal length As Long) As Long

Declare Function sqlrcur_sendFileQuery Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal path As String, ByVal filename As String) As Long

Declare Sub sqlrcur_prepareQuery Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal query As String)

Declare Sub sqlrcur_prepareQueryWithLength Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal query As String, ByVal length As Long)

Declare Sub sqlrcur_subString Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, ByVal value As String)

Declare Sub sqlrcur_subLong Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, ByVal value As Long)

Declare Sub sqlrcur_subDouble Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, _
ByVal value As Double, _
ByVal precision As Integer, _
ByVal v_scale As Integer)

Declare Sub sqlrcur_clearBinds Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Sub sqlrcur_inputBindString Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, ByVal value As String)

Declare Sub sqlrcur_inputBindLong Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, ByVal value As Long)

Declare Sub sqlrcur_inputBindDouble Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, ByVal value As Double, _
ByVal precision As Integer, ByVal v_scale As Integer)

Declare Sub sqlrcur_inputBindBlob Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, ByVal value As String, ByVal size As Long)

Declare Sub sqlrcur_inputBindClob Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, ByVal value As String, ByVal size As Long)

Declare Sub sqlrcur_subStrings Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, ByVal values As String)

Declare Sub sqlrcur_subLongs Lib "libsqlrclientwrappers.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String, ByVal values As Long)

Declare Sub sqlrcur_subDoubles Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variables As String, ByVal values As Long, ByVal precisions As Long, _
ByVal scales As Long)

Declare Sub sqlrcur_inputBindStrings Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByRef variables() As String, ByRef values() As String)

Declare Sub sqlrcur_inputBindLongs Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variables As Long, ByVal values As Long)

Declare Sub sqlrcur_inputBindDoubles Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variables As Long, ByVal values As Long, ByVal precisions As Long, ByVal scales As Long)

Declare Sub sqlrcur_validateBinds Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Function sqlrcur_executeQuery Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long) As Long

Declare Function sqlrcur_fetchFromBindCursor Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long) As Long

Declare Sub sqlrcur_defineOutputBind Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variabl As Long, ByVal length As Long)

Declare Sub sqlrcur_defineOutputBindBlob Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variabl As Long)

Declare Sub sqlrcur_defineOutputBindClob Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As Long)

Declare Sub sqlrcur_defineOutputBindCursor Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As Long)

Declare Function sqlrcur_getOutputBind Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String) As String

Declare Function sqlrcur_getOutputBindLength Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String) As Long

Declare Function sqlrcur_getOutputBindCursor Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal variable As String) As Long

Declare Function sqlrcur_openCachedResultSet Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal filename As String) As Long

Declare Function sqlrcur_rowCount Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long) As Long

Declare Function sqlrcur_colCount Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long) As Long

Declare Function sqlrcur_totalRows Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long) As Long

Declare Function sqlrcur_affectedRows Lib "libsqlrclientwrapper.dll" (ByVal sqrcurref As Long) As Long

Declare Function sqlrcur_firstRowIndex Lib "libsqlrclientwrapper.dll" (ByVal sqrcurref As Long) As Long

Declare Function sqlrcur_endOfResultSet Lib "libsqlrclientwrapper.dll" (ByVal sqrcurref As Long) As Long

Declare Function sqlrcur_errorMessage Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long) As String

Declare Sub sqlrcur_getNullsAsEmptyStrings Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Sub sqlrcur_getNullsAsNulls Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)


Declare Function sqlrcur_getFieldByIndex Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal row As Long, ByVal col As Long) As String

Declare Function sqlrcur_getFieldByName Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal row As Long, ByVal col As String) As String

Declare Function sqlrcur_getFieldLengthByIndex Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal row As Long, ByVal col As Long) As Long

Declare Function sqlrcur_getFieldLengthByName Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal row As Long, ByVal col As String) As Long

Declare Function sqlrcur_getRow Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, ByVal row As Long) _
As Long

Declare Function sqlrcur_getRowLengths Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal row As Long) As Long

Declare Function sqlrcur_getColumnNames Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long) As Long

Declare Function sqlrcur_getColumnName Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal col As Long) As Long

Declare Function sqlrcur_getColumnTypeByIndex Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal col As Long) As Long

Declare Function sqlrcur_getColumnLengthByIndex Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal col As Long) As Long

Declare Function sqlrcur_getColumnTypeByName Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal col As String) As String

Declare Function sqlrcur_getColumnLengthByName Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal col As String) As Long

Declare Function sqlrcur_getLongestByName Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal col As String) As Long

Declare Function sqlrcur_getLongestByIndex Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal col As Long) As Long

Declare Function sqlrcur_getResultSetId Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long) As Long

Declare Sub sqlrcur_suspendResultSet Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long)

Declare Function sqlrcur_resumeResultSet Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal col As Long) As Long

Declare Function sqlrcur_resumeCachedResultSet Lib "libsqlrclientwrapper.dll" (ByVal sqlrcurref As Long, _
ByVal id As Long, ByVal filename As String) As Long
