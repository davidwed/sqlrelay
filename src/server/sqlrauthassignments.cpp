	if (!charstring::compare(module,"database")) {
		au=new_sqlrauth_database(pvt->_cont,this,sqlrpe,auth);
	} else
	if (!charstring::compare(module,"proxied")) {
		au=new_sqlrauth_proxied(pvt->_cont,this,sqlrpe,auth);
	} else
	if (!charstring::compare(module,"sqlrelay")) {
		au=new_sqlrauth_sqlrelay(pvt->_cont,this,sqlrpe,auth);
	} else
	if (!charstring::compare(module,"userlist")) {
		au=new_sqlrauth_userlist(pvt->_cont,this,sqlrpe,auth);
	} else
