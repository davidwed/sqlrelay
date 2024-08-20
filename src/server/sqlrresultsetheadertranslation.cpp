// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrserver.h>

class sqlrresultsetheadertranslationprivate {
	friend class sqlrresultsetheadertranslation;
	private:
};

sqlrresultsetheadertranslation::sqlrresultsetheadertranslation(
				sqlrservercontroller *cont,
				domnode *parameters) :
				sqlrservermodule(cont,parameters) {
	pvt=new sqlrresultsetheadertranslationprivate;
}

sqlrresultsetheadertranslation::~sqlrresultsetheadertranslation() {
	delete pvt;
}

bool sqlrresultsetheadertranslation::run(sqlrserverconnection *sqlrcon,
					sqlrservercursor *sqlrcur,
					uint32_t colcount,
					const char ***columnnames,
					uint16_t **columnnamesizes,
					uint16_t **columntypes,
					const char ***columntypenames,
					uint16_t **columntypenamesizes,
					uint32_t **columnsizes,
					uint32_t **columnprecisions,
					uint32_t **columnscales,
					uint16_t **columnisnullables,
					uint16_t **columnisprimarykeys,
					uint16_t **columnisuniques,
					uint16_t **columnispartofkeys,
					uint16_t **columnisunsigneds,
					uint16_t **columniszerofilleds,
					uint16_t **columnisbinarys,
					uint16_t **columnisautoincrements,
					const char ***columntables,
					uint16_t **columntablesizes) {
	return true;
}

const char *sqlrresultsetheadertranslation::getError() {
	return NULL;
}
