// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#include <rudiments/charstring.h>

#include "sqlrmoduledata_tag.h"

sqlrmd_tag::sqlrmd_tag(domnode *parameters) : sqlrmoduledata(parameters) {
}

sqlrmd_tag::~sqlrmd_tag() {
	avltree<dictionarynode<uint16_t, avltree<char *> *> *>
						*tree=tags.getTree();
	for (avltreenode<dictionarynode<uint16_t, avltree<char *> *> *>
						*node=tree->getFirst();
					node; node=tree->getNext(node)) {
		delete[] node->getValue()->getValue();
	}
	tags.clearAndArrayDeleteValues();
}

void sqlrmd_tag::addTag(uint16_t cursorid, const char *tag) {
	avltree<char *>	*tree=tags.getValue(cursorid);
	if (tree && tree->find((char *)tag)) {
		return;
	}
	if (!tree) {
		avltree<char *>	*tree=new avltree<char *>();
		tags.setValue(cursorid,tree);
	}
	tree->insert(charstring::duplicate(tag));
}

void sqlrmd_tag::addTag(uint16_t cursorid, const char *tag, size_t size) {
	avltree<char *>	*tree=tags.getValue(cursorid);
	if (tree && tree->find((char *)tag)) {
		return;
	}
	if (!tree) {
		avltree<char *>	*tree=new avltree<char *>();
		tags.setValue(cursorid,tree);
	}
	tree->insert(charstring::duplicate(tag,size));
}

avltree<char *> *sqlrmd_tag::getTags(uint16_t cursorid) {
	return tags.getValue(cursorid);
}

bool sqlrmd_tag::tagExists(uint16_t cursorid, const char *tag) {
	avltree<char *>	*tree=tags.getValue(cursorid);
	return (tree && tree->find((char *)tag));
}

extern "C" {
	SQLRSERVER_DLLSPEC
	sqlrmoduledata	*new_sqlrmoduledata_tag(domnode *parameters) {
		return new sqlrmd_tag(parameters);
	}
}
