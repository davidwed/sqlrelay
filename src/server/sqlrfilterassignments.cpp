	if (!charstring::compare(module,"patterns")) {
		f=new_sqlrfilter_patterns(pvt->_cont,this,filter);
	} else
	if (!charstring::compare(module,"regex")) {
		f=new_sqlrfilter_regex(pvt->_cont,this,filter);
	} else
	if (!charstring::compare(module,"string")) {
		f=new_sqlrfilter_string(pvt->_cont,this,filter);
	} else
