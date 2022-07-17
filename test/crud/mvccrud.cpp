// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrcrud.h>
#include <rudiments/mvc.h>
#include <rudiments/httprequest.h>
#include <rudiments/scalar.h>

#define HTTP_MODULE_NAME mvccrud
#ifdef APACHE
	#define HTTP_MODULE_APACHE
#else
	#define HTTP_MODULE_CGI
#endif
#include <rudiments/httpserverapimain.h>

class testview : public mvcview {
	public:
		virtual void	setPath(char *path)=0;
		virtual bool	run(bool *handled)=0;
};

class ajaxtestview : public testview {
	public:
		void	setPath(char *path);
		bool	run(bool *handled);
	private:
		char	*path;
};

class testcontroller : public mvccontroller {
	public:
		void	createTest(jsondom *request, mvcresult *response);
		void	readTest(jsondom *request, mvcresult *response);
		void	updateTest(jsondom *request, mvcresult *response);
		void	deleteTest(jsondom *request, mvcresult *response);
};

class testservice : public mvcservice {
	public:
		virtual void	createTest(jsondom *request,
						mvcresult *response)=0;
		virtual void	readTest(jsondom *request,
						mvcresult *response)=0;
		virtual void	updateTest(jsondom *request,
						mvcresult *response)=0;
		virtual void	deleteTest(jsondom *request,
						mvcresult *response)=0;
};

class defaulttestservice : public testservice {
	public:
		void	createTest(jsondom *request, mvcresult *response);
		void	readTest(jsondom *request, mvcresult *response);
		void	updateTest(jsondom *request, mvcresult *response);
		void	deleteTest(jsondom *request, mvcresult *response);
};

class testdao : public mvcdao {
	public:
		virtual void	createTest(jsondom *request,
						mvcresult *response)=0;
		virtual void	readTest(jsondom *request,
						mvcresult *response)=0;
		virtual void	updateTest(jsondom *request,
						mvcresult *response)=0;
		virtual void	deleteTest(jsondom *request,
						mvcresult *response)=0;
};

class sqlrtestdao : public testdao {
	public:
		void	createTest(jsondom *request, mvcresult *response);
		void	readTest(jsondom *request, mvcresult *response);
		void	updateTest(jsondom *request, mvcresult *response);
		void	deleteTest(jsondom *request, mvcresult *response);
};

class factory {
	public:
		static testcontroller	*getTestController(mvcproperties *prop);
		static testview		*getTestView(mvcproperties *prop);
		static testservice	*getTestService(mvcproperties *prop);
		static testdao		*getTestDao(mvcproperties *prop);
		static mvccrud		*getSqlrCrud(mvcproperties *prop,
							mvcresult *response);
};



testcontroller *factory::getTestController(mvcproperties *prop) {
	// FIXME: use pools
	testcontroller	*c=new testcontroller();
	c->setProperties(prop);
	return c;
}

testview *factory::getTestView(mvcproperties *prop) {
	// FIXME: use pools
	const char	*impl=prop->getValue("testview.impl");
	if (!charstring::compare(impl,"ajax")) {
		testview	*v=new ajaxtestview();
		v->setProperties(prop);
		return v;
	}
	return NULL;
}

testservice *factory::getTestService(mvcproperties *prop) {
	// FIXME: use pools
	const char	*impl=prop->getValue("testservice.impl");
	if (!charstring::compare(impl,"default")) {
		testservice	*s=new defaulttestservice();
		s->setProperties(prop);
		return s;
	}
	return NULL;
}

testdao *factory::getTestDao(mvcproperties *prop) {
	// FIXME: use pools
	const char	*impl=prop->getValue("testdao.impl");
	if (!charstring::compare(impl,"sqlr")) {
		testdao	*d=new sqlrtestdao();
		d->setProperties(prop);
		return d;
	}
	return NULL;
}

mvccrud *factory::getSqlrCrud(mvcproperties *prop, mvcresult *response) {

	// create connection, cursor, and crud
	sqlrconnection	*con=new sqlrconnection(
				prop->getValue("sqlr.host"),
				charstring::toInteger(
					prop->getValue("sqlr.port")),
				prop->getValue("sqlr.socket"),
				prop->getValue("sqlr.user"),
				prop->getValue("sqlr.password"),0,1);
	sqlrcursor	*cur=new sqlrcursor(con);
	sqlrcrud	*crud=new sqlrcrud();

	// initialize crud
	crud->setSqlrConnection(con);
	crud->setSqlrCursor(cur);
	crud->setTable(prop->getValue("table"));
	crud->buildQueries();

	// attach everything to the wastebasket
	response->getWastebasket()->attach(crud);
	response->getWastebasket()->attach(cur);
	response->getWastebasket()->attach(con);

	return crud;
}



void ajaxtestview::setPath(char *path) {
	this->path=path;
}

bool ajaxtestview::run(bool *handled) {

	// get a controller
	testcontroller	*tc=factory::getTestController(getProperties());

	// get posted json
	jsondom	request;
	request.parseString(getRequest()->getJson());

	// ... reformat request as appropriate for the backend ...

	// run the appropriate controller method and get the result
	mvcresult	response;
	*handled=true;
	if (!charstring::compare(path,"/create")) {
		tc->createTest(&request,&response);
	} else if (!charstring::compare(path,"/read")) {
		tc->readTest(&request,&response);
	} else if (!charstring::compare(path,"/update")) {
		tc->updateTest(&request,&response);
	} else if (!charstring::compare(path,"/delete")) {
		tc->deleteTest(&request,&response);
	} else {
		*handled=false;
	}

	// respond
	if (*handled) {

		// ... reformat response as appropriate for the frontend ...

		getResponse()->textHtml();
		response.writeJson(getResponse());
	}

	// clean up
	response.getWastebasket()->empty();
	delete tc;

	return true;
}



void testcontroller::createTest(jsondom *request, mvcresult *response) {
	testservice	*ts=factory::getTestService(getProperties());
	ts->createTest(request,response);
	delete ts;
}

void testcontroller::readTest(jsondom *request, mvcresult *response) {
	testservice	*ts=factory::getTestService(getProperties());
	ts->readTest(request,response);
	delete ts;
}

void testcontroller::updateTest(jsondom *request, mvcresult *response) {
	testservice	*ts=factory::getTestService(getProperties());
	ts->updateTest(request,response);
	delete ts;
}

void testcontroller::deleteTest(jsondom *request, mvcresult *response) {
	testservice	*ts=factory::getTestService(getProperties());
	ts->deleteTest(request,response);
	delete ts;
}



void defaulttestservice::createTest(jsondom *request, mvcresult *response) {
	testdao	*td=factory::getTestDao(getProperties());
	td->createTest(request,response);
	delete td;
}

void defaulttestservice::readTest(jsondom *request, mvcresult *response) {
	testdao	*td=factory::getTestDao(getProperties());
	td->readTest(request,response);
	delete td;
}

void defaulttestservice::updateTest(jsondom *request, mvcresult *response) {
	testdao	*td=factory::getTestDao(getProperties());
	td->updateTest(request,response);
	delete td;
}

void defaulttestservice::deleteTest(jsondom *request, mvcresult *response) {
	testdao	*td=factory::getTestDao(getProperties());
	td->deleteTest(request,response);
	delete td;
}



void sqlrtestdao::createTest(jsondom *request, mvcresult *response) {
	mvccrud	*crud=factory::getSqlrCrud(getProperties(),response);
	if (crud->doCreate(request)) {
		response->setSuccess();
		response->setData("ar",crud->getAffectedRowsDictionary());
	} else {
		response->setFailed(crud->getErrorCode(),
					crud->getErrorMessage());
	}
}

void sqlrtestdao::readTest(jsondom *request, mvcresult *response) {
	mvccrud	*crud=factory::getSqlrCrud(getProperties(),response);
	if (crud->doRead(request)) {
		response->setSuccess();
		response->setData("rs",crud->getResultSetTable());
	} else {
		response->setFailed(crud->getErrorCode(),
					crud->getErrorMessage());
	}
}

void sqlrtestdao::updateTest(jsondom *request, mvcresult *response) {
	mvccrud	*crud=factory::getSqlrCrud(getProperties(),response);
	if (crud->doUpdate(request)) {
		response->setSuccess();
		response->setData("ar",crud->getAffectedRowsDictionary());
	} else {
		response->setFailed(crud->getErrorCode(),
					crud->getErrorMessage());
	}
}

void sqlrtestdao::deleteTest(jsondom *request, mvcresult *response) {
	mvccrud	*crud=factory::getSqlrCrud(getProperties(),response);
	if (crud->doDelete(request)) {
		response->setSuccess();
		response->setData("ar",crud->getAffectedRowsDictionary());
	} else {
		response->setFailed(crud->getErrorCode(),
					crud->getErrorMessage());
	}
}

mvcproperties	prop;

bool httpModuleInit(httpserverapi *sapi) {

	// set up properties (normally these would be in a file)
	prop.parseString(
		"sqlr.host=localhost\n"
		"sqlr.port=9000\n"
		"sqlr.socket=\n"
		"sqlr.user=test\n"
		"sqlr.password=test\n"
		"table=testtable\n"
		"testview.impl=ajax\n"
		"testservice.impl=default\n"
		"testdao.impl=sqlr\n");

	// initialize pools
	uint64_t	tpp=sapi->getThreadsPerProcess();
	for (uint64_t i=0; i<tpp; i++) {
		// FIXME: create one controller, view, service,
		// and dao for each thread
	}

	return true;
}

bool httpModuleMain(httpserverapi *sapi) {

	// set up request/response
	httprequest	req(sapi);
	httpresponse	resp(sapi);

	// get the path from the request and truncate any params
	char	*path=charstring::duplicate(
				req.getEnvironmentVariable("PATH_INFO"));
	char	*query=charstring::findFirst(path,'?');
	if (query) {
		*query='\0';
	}

	// set up the testview
	testview	*tv=factory::getTestView(&prop);
	tv->setRequest(&req);
	tv->setResponse(&resp);
	tv->setPath(path);

	// run the view
	bool	handled=false;
	bool	result=tv->run(&handled);

	// clean up
	delete[] path;
	delete tv;

	// handle success/error conditions
	if (!result) {
		return false;
	}
	if (!handled) {
		// normally an errorview would handle this
		resp.textHtml();
		resp.write("URL unhandled!\n");
	}
	return true;
}

bool httpModuleExit(httpserverapi *sapi) {
	return true;
}
