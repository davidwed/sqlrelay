	if (!charstring::compare(module,"sqlrcmdcstat")) {
		qr=new_sqlrquery_sqlrcmdcstat(pvt->_cont,this,query);
	} else
	if (!charstring::compare(module,"sqlrcmdgstat")) {
		qr=new_sqlrquery_sqlrcmdgstat(pvt->_cont,this,query);
	} else
