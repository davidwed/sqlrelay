// Connect, Authorize, and Initialize Functions
	//OCIConnectionPoolCreate()
	//OCIConnectionPoolDestroy()
	OCIEnvCreate()
	OCIEnvInit()
	OCIEnvNlsCreate() - call OCIEnvCreate()
	OCIInitialize()
	OCILogoff()
	OCILogon()
	OCILogon2() - call OCILogon()
	OCIServerAttach()
	OCIServerDetach()
	OCISessionBegin()
	OCISessionEnd()
	OCISessionGet()
	//OCISessionPoolCreate()
	//OCISessionPoolDestroy()
	OCISessionRelease()
	OCITerminate()

// Handle and Descriptor Functions
	OCIAttrGet()
	OCIAttrSet()
	OCIDescriptorAlloc()
	OCIDescriptorFree()
	OCIHandleAlloc()
	OCIHandleFree()
	OCIParamGet()
	OCIParamSet()

// Bind, Define, and Describe Functions
	//OCIBindArrayOfStruct()
	OCIBindByName()
	OCIBindByPos()
	//OCIBindDynamic()
	//OCIBindObject()
	//OCIDefineArrayOfStruct()
	OCIDefineByPos()
	//OCIDefineDynamic()
	//OCIDefineObject
	//OCIDescribeAny()
	//OCIStmtGetBindInfo()

// Statement Functions
	OCIStmtExecute()
	OCIStmtFetch()
	OCIStmtFetch2() - call OCIStmtFetch()
	OCIStmtGetPieceInfo()
	OCIStmtPrepare()
	OCIStmtPrepare2() - call OCIStmtPrepare()
	OCIStmtRelease()
	//OCIStmtSetPieceInfo()

// LOB Functions
	//OCIDurationBegin()
	//OCIDurationEnd()
	//OCILobAppend()
	//OCILobAssign()
	//OCILobCharSetForm()
	//OCILobCharSetId()
	OCILobClose()
	//OCILobCopy()
	OCILobCreateTemporary()
	//OCILobDisableBuffering()
	//OCILobEnableBuffering()
	//OCILobErase()
	//OCILobFileClose()
	//OCILobFileCloseAll()
	//OCILobFileExists()
	//OCILobFileGetName()
	//OCILobFileIsOpen()
	//OCILobFileOpen()
	//OCILobFileSetName()
	//OCILobFlushBuffer()
	OCILobFreeTemporary()
	//OCILobGetChunkSize()
	OCILobGetLength()
	//OCILobIsEqual()
	//OCILobIsOpen()
	OCILobIsTemporary()
	//OCILobLoadFromFile()
	//OCILobLocatorAssign()
	//OCILobLocatorIsInit()
	OCILobOpen()
	OCILobRead()
	//OCILobTrim()
	OCILobWrite()
	//OCILobWriteAppend()

// Advanced Queuing and Publish-Subscribe Functions
// Direct Path Loading Functions
// Thread Management Functions

// Transaction Functions
	OCITransCommit()
	//OCITransDetach()
	//OCITransForget()
	//OCITransMultiPrepare()
	//OCITransPrepare()
	OCITransRollback()
	//OCITransStart()

// Miscellaneous Functions
	//OCIBreak()
	OCIErrorGet()
	//OCILdaToSvcCtx()
	//OCINlsEnvironmentVariableGet()
	//OCIPasswordChange()
	//OCIReset()
	//OCIRowidToChar()
	OCIServerVersion()
	//OCISvcCtxToLda()
	//OCIUserCallbackGet()
	//OCIUserCallbackRegister()


// OCI Flush or Refresh Functions
// OCI Mark or Unmark Object and Cache Functions
// OCI Get Object Status Functions
// OCI Miscellaneous Object Functions
// OCI Pin, Unpin, and Free Functions
// OCI Type Information Accessor Functions
// OCI Collection and Iterator Functions
// OCI Date, Datetime, and Interval Functions
// OCI Number Functions
// OCI Raw Functions
// OCI Ref Functions
// OCI String Functions
// OCI Table Functions
// Cartridge Services -- OCI External Procedures
// Cartridge Services -- Memory Services
// Cartridge Services -- Maintaining Context
// Cartridge Services -- Parameter Manager Interface
// Cartridge Services -- File I/O Interface
// Cartridge Services -- String Formatting Interface
// OCI Type Interface Functions
// OCI Any Data Interface Functions
// OCI Any Data Set Interface Functions


#ifdef HAVE_ORACLE_8i
OCIEnvCreate((OCIEnv **)&env,OCI_DEFAULT,(dvoid *)0,
				(dvoid *(*)(dvoid *, size_t))0,
				(dvoid *(*)(dvoid *, dvoid *, size_t))0,
				(void (*)(dvoid *, dvoid *))0,
				(size_t)0,(dvoid **)0) {
}

#else
OCIInitialize(OCI_DEFAULT,NULL,NULL,NULL,NULL) {
}

OCIEnvInit((OCIEnv **)&env,OCI_DEFAULT,0,(dvoid **)0) {
}

#endif

OCIHandleAlloc((dvoid *)env,(dvoid **)&err,OCI_HTYPE_ERROR,0,NULL) {
	OCI_HTYPE_ERROR
	OCI_HTYPE_SERVER
	OCI_HTYPE_SVCCTX
	OCI_HTYPE_SERVER
	OCI_HTYPE_TRANS
	OCI_HTYPE_SESSION
	OCI_HTYPE_STMT
}

OCIHandleFree(svc,OCI_HTYPE_SVCCTX) {
	OCI_HTYPE_ERROR
	OCI_HTYPE_SERVER
	OCI_HTYPE_SVCCTX
	OCI_HTYPE_SERVER
	OCI_HTYPE_TRANS
	OCI_HTYPE_SESSION
	OCI_HTYPE_STMT
}

OCIServerAttach(srv,err,(text *)sid,charstring::length(sid),0) {
}

OCIServerDetach(srv,err,OCI_DEFAULT) {
}

OCIAttrSet((dvoid *)svc,OCI_HTYPE_SVCCTX,
				(dvoid *)srv,(ub4)0,
				OCI_ATTR_SERVER,(OCIError *)err) {
	OCI_HTYPE_SVCCTX
		OCI_ATTR_SERVER
		OCI_ATTR_SESSION
		OCI_ATTR_TRANS
	OCI_HTYPE_SESSION
		OCI_ATTR_USERNAME
		OCI_ATTR_PSSSWORD
}

OCIAttrGet((dvoid *)stmt,OCI_HTYPE_STMT,
				(dvoid *)&ncols,(ub4 *)NULL,
				OCI_ATTR_PARAM_COUNT,
				oracleconn->err) {
	OCI_HTYPE_STMT
		OCI_ATTR_STMT_TYPE
		OCI_ATTR_PARAM_COUNT
		OCI_ATTR_ROW_COUNT
	OCI_DTYPE_PARAM
		OCI_ATTR_NAME
		OCI_ATTR_DATA_TYPE
		OCI_ATTR_PRECISION
		OCI_ATTR_SCALE
		OCI_ATTR_IS_NULL
		OCI_ATTR_DATA_SIZE
}


OCISessionBegin(svc,err,session,OCI_CRED_RDBMS,(ub4)OCI_DEFAULT) {
}


OCISessionEnd(svc,err,session,OCI_DEFAULT) {
}

OCIServerVersion((dvoid *)svc,err,(text *)versionbuf,
				sizeof(versionbuf),OCI_HTYPE_SVCCTX) {
}


OCIErrorGet((dvoid *)err,1,(text *)0,&errcode,
				message,sizeof(message),OCI_HTYPE_ERROR) {
}

OCITransCommit(svc,err,OCI_DEFAULT) {
}


OCITransRollback(svc,err,OCI_DEFAULT) {
}



OCIStmtPrepare(stmt,oracleconn->err,
				(text *)query,(ub4)length,
				(ub4)OCI_NTV_SYNTAX,
				(ub4)OCI_DEFAULT) {
}


OCIBindByPos(stmt,&inbindpp[inbindcount],
				oracleconn->err,
				(ub4)charstring::toInteger(variable+1),
				(dvoid *)value,(sb4)valuesize+1,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT) {
	SQLT_STR
	SQLT_FLT
	SQLT_BLOB
	SQLT_CLOB
}


OCIBindByName(stmt,&inbindpp[inbindcount],
				oracleconn->err,
				(text *)variable,(sb4)variablesize,
				(dvoid *)value,(sb4)valuesize+1,
				SQLT_STR,
				(dvoid *)isnull,(ub2 *)0,
				(ub2 *)0,0,(ub4 *)0,
				OCI_DEFAULT) {
	SQLT_STR
	SQLT_FLT
	SQLT_BLOB
	SQLT_CLOB
}



OCIDescriptorAlloc((dvoid *)oracleconn->env,
			(dvoid **)&inbind_lob[inbindlobcount],
			(ub4)OCI_DTYPE_LOB,
			(size_t)0,(dvoid **)0) {
	OCI_DTYPE_LOB
}

OCIDescriptorFree(inbind_lob[inbindlobcount],OCI_DTYPE_LOB) {
	OCI_DTYPE_LOB
}

OCILobCreateTemporary(oracleconn->svc,oracleconn->err,
			inbind_lob[inbindlobcount],
			//(ub2)0,SQLCS_IMPLICIT,
			(ub2)OCI_DEFAULT,OCI_DEFAULT,
			temptype,OCI_ATTR_NOCACHE,
			OCI_DURATION_SESSION) {
}

OCILobIsTemporary(oracleconn->env,oracleconn->err,
					def_lob[col][row],
					&templob) {
}

OCILobFreeTemporary(oracleconn->svc,oracleconn->err,
					inbind_lob[inbindlobcount]) {
}

OCILobOpen(oracleconn->svc,oracleconn->err,
			inbind_lob[inbindlobcount],
			OCI_LOB_READWRITE) {
}


OCILobWrite(oracleconn->svc,oracleconn->err,
			inbind_lob[inbindlobcount],&size,1,
			(void *)value,valuesize,
			OCI_ONE_PIECE,(dvoid *)0,
			(sb4 (*)(dvoid*,dvoid*,ub4*,ub1 *))0,
			0,SQLCS_IMPLICIT) {
}


OCILobRead(oracleconn->svc,oracleconn->err,
				outbind_lob[index],
				&retlen,
				offset,
				(dvoid *)buf,
				oracleconn->maxitembuffersize,
				(dvoid *)NULL,
				(sb4(*)(dvoid *,CONST dvoid *,ub4,ub1))NULL,
				(ub2)0,
				(ub1)SQLCS_IMPLICIT) {
}

OCILobClose(oracleconn->svc,oracleconn->err,inbind_lob[inbindlobcount]) {
}

OCILobGetLength(oracleconn->svc,
			oracleconn->err,
			outbind_lob[index],
			&loblength) {
}


OCIStmtExecute(oracleconn->svc,stmt,
				oracleconn->err,iters,
				(ub4)0,NULL,NULL,
				oracleconn->statementmode) {
}


OCIParamGet(stmt,OCI_HTYPE_STMT,oracleconn->err,
				(dvoid **)&desc[i].paramd,
				i+1) {
}


OCIDefineByPos(stmt,&def[i],oracleconn->err,
					i+1,
					(dvoid *)def_buf[i],
					(sb4)oracleconn->maxitembuffersize,
					SQLT_STR,
					(dvoid *)def_indp[i],
					(ub2 *)def_col_retlen[i],
					def_col_retcode[i],
					OCI_DEFAULT) {
}


OCIStmtFetch(stmt,oracleconn->err,oracleconn->fetchatonce,
						OCI_FETCH_NEXT,OCI_DEFAULT) {
	OCI_FETCH_NEXT
}
