// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>
#include <rudiments/charstring.h>

class sqlrmoduledataprivate {
	friend class sqlrmoduledata;
	private:
		domnode		*_parameters;
		const char	*_moduletype;
		const char	*_id;
};

sqlrmoduledata::sqlrmoduledata(domnode *parameters) {
	pvt=new sqlrmoduledataprivate;
	pvt->_parameters=parameters;
	pvt->_moduletype=parameters->getAttributeValue("module");
	if (charstring::isNullOrEmpty(pvt->_moduletype)) {
		pvt->_moduletype=parameters->getAttributeValue("file");
	}
	pvt->_id=parameters->getAttributeValue("id");
}

sqlrmoduledata::~sqlrmoduledata() {
	delete pvt;
}

const char *sqlrmoduledata::getModuleType() {
	return pvt->_moduletype;
}

const char *sqlrmoduledata::getId() {
	return pvt->_id;
}

domnode *sqlrmoduledata::getParameters() {
	return pvt->_parameters;
}

void sqlrmoduledata::closeResultSet(sqlrservercursor *sqlrcur) {
}

void sqlrmoduledata::endTransaction(bool commit) {
}

void sqlrmoduledata::endSession() {
}

sqlrmoduledata_tag::sqlrmoduledata_tag(domnode *parameters) :
					sqlrmoduledata(parameters) {
}

sqlrmoduledata_tag::~sqlrmoduledata_tag() {
	for (listnode<uint16_t> *node=tags.getKeys()->getFirst();
						node; node->getNext()) {
		tags.getValue(node->getValue())->clearAndArrayDelete();
	}
	tags.clearAndDeleteValues();
}

void sqlrmoduledata_tag::addTag(uint16_t cursorid, const char *tag) {
	avltree<char *>	*tree=tags.getValue(cursorid);
	if (tree && tree->find((char *)tag)) {
		return;
	}
	if (!tree) {
		tree=new avltree<char *>();
		tags.setValue(cursorid,tree);
	}
	tree->insert(charstring::duplicate(tag));
}

void sqlrmoduledata_tag::addTag(uint16_t cursorid,
				const char *tag, size_t size) {
	avltree<char *>	*tree=tags.getValue(cursorid);
	if (tree && tree->find((char *)tag)) {
		return;
	}
	if (!tree) {
		tree=new avltree<char *>();
		tags.setValue(cursorid,tree);
	}
	tree->insert(charstring::duplicate(tag,size));
}

avltree<char *> *sqlrmoduledata_tag::getTags(uint16_t cursorid) {
	return tags.getValue(cursorid);
}

bool sqlrmoduledata_tag::tagExists(uint16_t cursorid, const char *tag) {
	avltree<char *>	*tree=tags.getValue(cursorid);
	return (tree && tree->find((char *)tag));
}

void sqlrmoduledata_tag::closeResultSet(sqlrservercursor *sqlrcur) {
	avltree<char *>	*tree=tags.getValue(sqlrcur->getId());
	if (tree) {
		tree->clearAndArrayDelete();
		tags.removeAndDeleteValue(sqlrcur->getId());
	}
}
