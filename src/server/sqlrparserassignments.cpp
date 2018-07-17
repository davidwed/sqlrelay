	if (!charstring::compare(parsername.getString(),"default")) {
		parser=new_sqlrparser_default(this,pvt->_sqlrpnode);
	} else
