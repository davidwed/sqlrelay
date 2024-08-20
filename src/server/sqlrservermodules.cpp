// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

#include <rudiments/domnode.h>
#include <rudiments/process.h>
#include <rudiments/stdio.h>

#include <config.h>

sqlrservermodules::sqlrservermodules(sqlrservercontroller *cont) :
							sqlrserverbase() {
	this->cont=cont;
}

sqlrservermodules::~sqlrservermodules() {
	unload();
}

bool sqlrservermodules::isModuleDisabled(domnode *parameters) {
	if (charstring::isNo(parameters->getAttributeValue("enabled"))) {
		debugWrite("not loading %s - disabled",
				getModuleName(parameters));
		return true;
	}
	return false;
}

bool sqlrservermodules::load(domnode *parameters) {

	unload();

	// run through the module tags
	for (domnode *moduledata=parameters->getFirstTagChild();
				!moduledata->isNullNode();
				moduledata=moduledata->getNextTagSibling()) {

		// skip disabled modules
		if (isModuleDisabled(moduledata)) {
			continue;
		}

		// load the module
		loadModule(moduledata);
	}
	return true;
}

void sqlrservermodules::loadModule(domnode *parameters) {
	// by default, do nothing
}

const char *sqlrservermodules::getModuleName(domnode *parameters) {

	const char	*module=parameters->getAttributeValue("module");

	if (charstring::isNullOrEmpty(module)) {
		// try "file", that's what it used to be called
		module=parameters->getAttributeValue("file");
	}

	return module;
}

void sqlrservermodules::unload() {

	// before list...
	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {
		sqlrmoduleplugin	*sqlrmp=node->getValue();
		delete sqlrmp->m;
		delete sqlrmp->dl;
		delete sqlrmp;
	}
	blist.clear();

	// after list...
	for (listnode< sqlrmoduleplugin * > *node=alist.getFirst();
						node; node=node->getNext()) {
		sqlrmoduleplugin	*sqlrmp=node->getValue();
		delete sqlrmp->m;
		delete sqlrmp->dl;
		delete sqlrmp;
	}
	alist.clear();
}

void sqlrservermodules::endTransaction(bool commit) {

	// before list...
	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->m->endTransaction(commit);
	}

	// after list...
	for (listnode< sqlrmoduleplugin * > *node=alist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->m->endTransaction(commit);
	}
}

void sqlrservermodules::endSession() {

	// before list...
	for (listnode< sqlrmoduleplugin * > *node=blist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->m->endSession();
	}

	// after list...
	for (listnode< sqlrmoduleplugin * > *node=alist.getFirst();
						node; node=node->getNext()) {
		node->getValue()->m->endSession();
	}
}
