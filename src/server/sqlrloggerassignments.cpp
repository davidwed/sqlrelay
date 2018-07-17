	if (!charstring::compare(module,"custom_nw")) {
		lg=new_sqlrlogger_custom_nw(this,logger);
	} else
	if (!charstring::compare(module,"custom_sc")) {
		lg=new_sqlrlogger_custom_sc(this,logger);
	} else
	if (!charstring::compare(module,"debug")) {
		lg=new_sqlrlogger_debug(this,logger);
	} else
	if (!charstring::compare(module,"slowqueries")) {
		lg=new_sqlrlogger_slowqueries(this,logger);
	} else
