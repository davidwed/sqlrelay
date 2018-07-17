	if (!charstring::compare(module,"clientinfolist")) {
		r=new_sqlrrouter_clientinfolist(pvt->_cont,this,router);
	} else
	if (!charstring::compare(module,"clientiplist")) {
		r=new_sqlrrouter_clientiplist(pvt->_cont,this,router);
	} else
	if (!charstring::compare(module,"regex")) {
		r=new_sqlrrouter_regex(pvt->_cont,this,router);
	} else
	if (!charstring::compare(module,"usedatabase")) {
		r=new_sqlrrouter_usedatabase(pvt->_cont,this,router);
	} else
	if (!charstring::compare(module,"userlist")) {
		r=new_sqlrrouter_userlist(pvt->_cont,this,router);
	} else
