// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrcrud.h>
#include <rudiments/mvc.h>
#include <rudiments/httprequest.h>
#include <rudiments/scalar.h>

#define HTTP_MODULE_NAME mvccrud
#define HTTP_MODULE_CGI
#include <rudiments/httpserverapimain.h>

class factory {
	public:
		static sqlrcrud	*getSqlrCrud(mvcproperties *p, mvcresult *r);
};

class testview : public mvcview {
	public:
		void	setPath(char *path);
		bool	run(bool *handled);
	private:
		char	*path;
};

class testcontroller : public mvccontroller {
	public:
		void	createTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
		void	readTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
		void	updateTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
		void	deleteTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
};

class testservice : public mvcservice {
	public:
		void	createTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
		void	readTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
		void	updateTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
		void	deleteTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
};

class testdao : public mvcdao {
	public:
		void	createTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
		void	readTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
		void	updateTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
		void	deleteTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r);
};



sqlrcrud *factory::getSqlrCrud(mvcproperties *p, mvcresult *r) {

	// create connection, cursor, and crud
	sqlrconnection	*con=new sqlrconnection(
				p->getValue("sqlr.host"),
				charstring::toInteger(p->getValue("sqlr.port")),
				p->getValue("sqlr.socket"),
				p->getValue("sqlr.user"),
				p->getValue("sqlr.password"),0,1);
	sqlrcursor	*cur=new sqlrcursor(con);
	sqlrcrud	*crud=new sqlrcrud();

	// initialize crud
	crud->setSqlrConnection(con);
	crud->setSqlrCursor(cur);
	crud->setTable(p->getValue("table"));
	crud->buildQueries();

	// attach everything to the wastebasket
	r->getWastebasket()->attach(crud);
	r->getWastebasket()->attach(cur);
	r->getWastebasket()->attach(con);

	return crud;
}



void testview::setPath(char *path) {
	this->path=path;
}

bool testview::run(bool *handled) {

	// get a controller
	testcontroller	tc;
	tc.setProperties(getProperties());

	// build params
	dictionary<const char *, const char *>	params;
	params.setValues(getRequest()->getAllVariables(),
				getRequest()->getAllValues());

	// run the appropriate controller method
	mvcresult	r;
	*handled=true;
	if (!charstring::compare(path,"/create.html")) {

		// normally we wouldn't pass params directly down
		// from the view, as-is, but it's fine for this test
		tc.createTest(&params,&r);
		// FIXME: do something with the result

	} else if (!charstring::compare(path,"/read.html")) {

		// normally we wouldn't pass params directly down
		// from the view, as-is, but it's fine for this test
		tc.readTest(&params,&r);
		// FIXME: do something with the result

	} else if (!charstring::compare(path,"/update.html")) {

		// normally we wouldn't pass params directly down
		// from the view, as-is, but it's fine for this test
		tc.updateTest(&params,&r);
		// FIXME: do something with the result

	} else if (!charstring::compare(path,"/delete.html")) {

		// normally we wouldn't pass params directly down
		// from the view, as-is, but it's fine for this test
		tc.deleteTest(&params,&r);
		// FIXME: do something with the result

	} else {
		*handled=false;
	}
	return true;
}



void testcontroller::createTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r) {

	// normally we'd get ts from a factory based on an
	// impl type, but it's fine to do this for this test
	testservice	ts;
	ts.setProperties(getProperties());
	ts.createTest(kvp,r);
}

void testcontroller::readTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r) {

	// normally we'd get ts from a factory based on an
	// impl type, but it's fine to do this for this test
	testservice	ts;
	ts.setProperties(getProperties());
	ts.readTest(kvp,r);
}

void testcontroller::updateTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r) {

	// normally we'd get ts from a factory based on an
	// impl type, but it's fine to do this for this test
	testservice	ts;
	ts.setProperties(getProperties());
	ts.updateTest(kvp,r);
}

void testcontroller::deleteTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r) {

	// normally we'd get ts from a factory based on an
	// impl type, but it's fine to do this for this test
	testservice	ts;
	ts.setProperties(getProperties());
	ts.deleteTest(kvp,r);
}



void testservice::createTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r) {

	// normally we'd get td from a factory based on an
	// impl type, but it's fine to do this for this test
	testdao	td;
	td.setProperties(getProperties());
	td.createTest(kvp,r);
}

void testservice::readTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r) {

	// normally we'd get td from a factory based on an
	// impl type, but it's fine to do this for this test
	testdao	td;
	td.setProperties(getProperties());
	td.readTest(kvp,r);
}

void testservice::updateTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r) {

	// normally we'd get td from a factory based on an
	// impl type, but it's fine to do this for this test
	testdao	td;
	td.setProperties(getProperties());
	td.updateTest(kvp,r);
}

void testservice::deleteTest(
			dictionary<const char *, const char *> *kvp,
			mvcresult *r) {

	// normally we'd get td from a factory based on an
	// impl type, but it's fine to do this for this test
	testdao	td;
	td.setProperties(getProperties());
	td.deleteTest(kvp,r);
}



void testdao::createTest(dictionary<const char *, const char *> *kvp,
							mvcresult *r) {
	stdoutput.printf("create!\n");

	sqlrcrud	*crud=factory::getSqlrCrud(getProperties(),r);

	// FIXME: I need a way to quickly decompose a dictionary into keys
	// and values, or a way to pass doCreate a dictionary
	const char * const *cols=NULL;
	const char * const *vals=NULL;

	if (crud->doCreate(cols,vals)) {
		r->setSuccess();

		scalar<uint64_t>	*s=new scalar<uint64_t>();
		s->setValue(crud->getAffectedRows());
		r->attachData("affectedrows","scalar",s);
	} else {
		r->setFailed(crud->getErrorCode(),crud->getErrorMessage());
	}
}

void testdao::readTest(dictionary<const char *, const char *> *kvp,
							mvcresult *r) {
	stdoutput.printf("read!\n");

	sqlrcrud	*crud=factory::getSqlrCrud(getProperties(),r);

	// FIXME: build these
	const char	*criteria=NULL;
	const char	*sort=NULL;

	if (crud->doRead(criteria,sort,0)) {
		r->setSuccess();

		scalar<uint64_t>	*s=new scalar<uint64_t>();
		s->setValue(crud->getAffectedRows());
		r->attachData("affectedrows","scalar",s);
	} else {
		r->setFailed(crud->getErrorCode(),crud->getErrorMessage());
	}
}

void testdao::updateTest(dictionary<const char *, const char *> *kvp,
							mvcresult *r) {
	stdoutput.printf("update!\n");

	sqlrcrud	*crud=factory::getSqlrCrud(getProperties(),r);

	// FIXME: I need a way to quickly decompose a dictionary into keys
	// and values, or a way to pass doUpdate a dictionary
	const char * const *cols=NULL;
	const char * const *vals=NULL;

	// FIXME: build this
	const char	*criteria=NULL;

	if (crud->doUpdate(cols,vals,criteria)) {
		r->setSuccess();

		scalar<uint64_t>	*s=new scalar<uint64_t>();
		s->setValue(crud->getAffectedRows());
		r->attachData("affectedrows","scalar",s);
	} else {
		r->setFailed(crud->getErrorCode(),crud->getErrorMessage());
	}
}

void testdao::deleteTest(dictionary<const char *, const char *> *kvp,
							mvcresult *r) {
	stdoutput.printf("delete!\n");

	sqlrcrud	*crud=factory::getSqlrCrud(getProperties(),r);

	// FIXME: build this
	const char	*criteria=NULL;

	if (crud->doDelete(criteria)) {
		r->setSuccess();

		scalar<uint64_t>	*s=new scalar<uint64_t>();
		s->setValue(crud->getAffectedRows());
		r->attachData("affectedrows","scalar",s);
	} else {
		r->setFailed(crud->getErrorCode(),crud->getErrorMessage());
	}
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

	// set up properties (normally these would be in a file)
	mvcproperties	prop;
	prop.parseString(
		"sqlr.host=localhost\n"
		"sqlr.port=9000\n"
		"sqlr.socket=\n"
		"sqlr.user=test\n"
		"sqlr.password=test\n"
		"table=testtable");

	// set up the view
	testview	tv;
	tv.setRequest(&req);
	tv.setResponse(&resp);
	tv.setProperties(&prop);
	tv.setPath(path);

	// run the view
	bool	handled=false;
	bool	result=tv.run(&handled);

	// clean up
	delete[] path;

	// handle success/error conditions
	if (!result) {
		return false;
	}
	if (!handled) {
		// FIXME: handle with an errorview
	}
	return true;
}
