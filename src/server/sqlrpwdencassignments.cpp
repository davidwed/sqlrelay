	if (!charstring::compare(module,"crypt")) {
		pe=new_sqlrpwdenc_crypt(pwdenc,pvt->_debug);
	} else
	if (!charstring::compare(module,"md5")) {
		pe=new_sqlrpwdenc_md5(pwdenc,pvt->_debug);
	} else
	if (!charstring::compare(module,"rot")) {
		pe=new_sqlrpwdenc_rot(pwdenc,pvt->_debug);
	} else
