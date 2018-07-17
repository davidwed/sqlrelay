	if (!charstring::compare(module,"sqlrclient")) {
		pr=new_sqlrprotocol_sqlrclient(pvt->_cont,this,listener);
	} else
