	if (!charstring::compare(module,"normalize")) {
		tr=new_sqlrtranslation_normalize(pvt->_cont,this,translation);
	} else
	if (!charstring::compare(module,"patterns")) {
		tr=new_sqlrtranslation_patterns(pvt->_cont,this,translation);
	} else
