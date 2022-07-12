// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrcrud.h>
#include <rudiments/mvc.h>
#include <rudiments/httprequest.h>
#include <rudiments/scalar.h>

#define HTTP_MODULE_NAME mvccrud
#define HTTP_MODULE_CGI
#include <rudiments/httpserverapimain.h>

class testview : public mvcview {
	public:
		virtual void	setPath(char *path)=0;
		virtual bool	run(bool *handled)=0;
};

class defaulttestview : public testview {
	public:
		void	setPath(char *path);
		bool	run(bool *handled);
	private:
		char	*path;
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

class defaulttestdao : public testdao {
	public:
		void	createTest(jsondom *request, mvcresult *response);
		void	readTest(jsondom *request, mvcresult *response);
		void	updateTest(jsondom *request, mvcresult *response);
		void	deleteTest(jsondom *request, mvcresult *response);
};

class testcontroller : public mvccontroller {
	public:
		void	createTest(jsondom *request, mvcresult *response);
		void	readTest(jsondom *request, mvcresult *response);
		void	updateTest(jsondom *request, mvcresult *response);
		void	deleteTest(jsondom *request, mvcresult *response);
};

class factory {
	public:
		static testview		*getTestView(mvcproperties *prop);
		static testservice	*getTestService(mvcproperties *prop);
		static testdao		*getTestDao(mvcproperties *prop);
		static mvccrud		*getSqlrCrud(mvcproperties *prop,
							mvcresult *response);
};



testview *factory::getTestView(mvcproperties *prop) {
	const char	*impl=prop->getValue("testview.impl");
	if (!charstring::compare(impl,"default")) {
		return new defaulttestview();
	}
	return NULL;
}

testservice *factory::getTestService(mvcproperties *prop) {
	const char	*impl=prop->getValue("testservice.impl");
	if (!charstring::compare(impl,"default")) {
		return new defaulttestservice();
	}
	return NULL;
}

testdao *factory::getTestDao(mvcproperties *prop) {
	const char	*impl=prop->getValue("testdao.impl");
	if (!charstring::compare(impl,"default")) {
		return new defaulttestdao();
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



void defaulttestview::setPath(char *path) {
	this->path=path;
}

bool defaulttestview::run(bool *handled) {

	// get a controller
	// FIXME: the view shouldn't create the controller,
	// the controller should already exist and the view
	// should just get a handle to it
	testcontroller	tc;
	tc.setProperties(getProperties());

	// get posted json
	jsondom	request;
	request.parseString(getRequest()->getJson());

	// ... reformat request as appropriate for the backend ...

	// run the appropriate controller method and get the result
	mvcresult	response;
	*handled=true;
	if (!charstring::compare(path,"/create.html")) {
		tc.createTest(&request,&response);
	} else if (!charstring::compare(path,"/read.html")) {
		tc.readTest(&request,&response);
	} else if (!charstring::compare(path,"/update.html")) {
		tc.updateTest(&request,&response);
	} else if (!charstring::compare(path,"/delete.html")) {
		tc.deleteTest(&request,&response);
	} else {
		*handled=false;
	}

	// ... reformat response as appropriate for the frontend ...

	// respond
	getResponse()->textHtml();
	response.writeJson(getResponse());

	// clean up
	response.getWastebasket()->empty();

	return true;
}



void testcontroller::createTest(jsondom *request, mvcresult *response) {
	testservice	*ts=factory::getTestService(getProperties());
	ts->setProperties(getProperties());
	ts->createTest(request,response);
	delete ts;
}

void testcontroller::readTest(jsondom *request, mvcresult *response) {
	testservice	*ts=factory::getTestService(getProperties());
	ts->setProperties(getProperties());
	ts->readTest(request,response);
	delete ts;
}

void testcontroller::updateTest(jsondom *request, mvcresult *response) {
	testservice	*ts=factory::getTestService(getProperties());
	ts->setProperties(getProperties());
	ts->updateTest(request,response);
	delete ts;
}

void testcontroller::deleteTest(jsondom *request, mvcresult *response) {
	testservice	*ts=factory::getTestService(getProperties());
	ts->setProperties(getProperties());
	ts->deleteTest(request,response);
	delete ts;
}



void defaulttestservice::createTest(jsondom *request, mvcresult *response) {
	testdao	*td=factory::getTestDao(getProperties());
	td->setProperties(getProperties());
	td->createTest(request,response);
	delete td;
}

void defaulttestservice::readTest(jsondom *request, mvcresult *response) {
	testdao	*td=factory::getTestDao(getProperties());
	td->setProperties(getProperties());
	td->readTest(request,response);
	delete td;
}

void defaulttestservice::updateTest(jsondom *request, mvcresult *response) {
	testdao	*td=factory::getTestDao(getProperties());
	td->setProperties(getProperties());
	td->updateTest(request,response);
	delete td;
}

void defaulttestservice::deleteTest(jsondom *request, mvcresult *response) {
	testdao	*td=factory::getTestDao(getProperties());
	td->setProperties(getProperties());
	td->deleteTest(request,response);
	delete td;
}



void defaulttestdao::createTest(jsondom *request, mvcresult *response) {
	mvccrud	*crud=factory::getSqlrCrud(getProperties(),response);
	if (crud->doCreate(request)) {
		response->setSuccess();
		response->setData("ar",crud->getAffectedRowsDictionary());
	} else {
		response->setFailed(crud->getErrorCode(),
					crud->getErrorMessage());
	}
}

void defaulttestdao::readTest(jsondom *request, mvcresult *response) {
	mvccrud	*crud=factory::getSqlrCrud(getProperties(),response);
	if (crud->doRead(request)) {
		response->setSuccess();
		response->setData("rs",crud->getResultSetTable());
	} else {
		response->setFailed(crud->getErrorCode(),
					crud->getErrorMessage());
	}
}

void defaulttestdao::updateTest(jsondom *request, mvcresult *response) {
	mvccrud	*crud=factory::getSqlrCrud(getProperties(),response);
	if (crud->doUpdate(request)) {
		response->setSuccess();
		response->setData("ar",crud->getAffectedRowsDictionary());
	} else {
		response->setFailed(crud->getErrorCode(),
					crud->getErrorMessage());
	}
}

void defaulttestdao::deleteTest(jsondom *request, mvcresult *response) {
	mvccrud	*crud=factory::getSqlrCrud(getProperties(),response);
	if (crud->doDelete(request)) {
		response->setSuccess();
		response->setData("ar",crud->getAffectedRowsDictionary());
	} else {
		response->setFailed(crud->getErrorCode(),
					crud->getErrorMessage());
	}
}



bool httpModuleMain(httpserverapi *sapi) {

	// set up properties (normally these would be in a file)
	mvcproperties	prop;
	prop.parseString(
		"sqlr.host=localhost\n"
		"sqlr.port=9000\n"
		"sqlr.socket=\n"
		"sqlr.user=test\n"
		"sqlr.password=test\n"
		"table=testtable\n"
		"testview.impl=default\n"
		"testservice.impl=default\n"
		"testdao.impl=default\n");

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
	tv->setProperties(&prop);
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
		stdoutput.printf("URL unhandled!\n");
	}
	return true;
}
