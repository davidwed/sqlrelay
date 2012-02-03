// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <rudiments/character.h>

void sqlrcursor_svr::printQueryTree(xmldom *tree) {
	const char	*xml=tree->getRootNode()->xml()->getString();
	int16_t		indent=0;
	bool		endtag;
	for (const char *ptr=xml; *ptr; ptr++) {
		if (*ptr=='<') {
			if (*(ptr+1)=='/') {
				indent=indent-2;
				endtag=true;
			}
			for (uint16_t i=0; i<indent; i++) {
				printf(" ");
			}
		}
		printf("%c",*ptr);
		if (*ptr=='>') {
			printf("\n");
			if (*(ptr-1)!='/' && !endtag) {
				indent=indent+2;
			}
			endtag=false;
		}
	}
}

bool sqlrcursor_svr::translateQuery() {

	if (conn->debugsqltranslation) {
		printf("original:\n\"%s\"\n\n",querybuffer);
	}

	// parse the query
	bool	parsed=conn->sqlp->parse(querybuffer);

	// get the parsed tree
	delete querytree;
	querytree=conn->sqlp->detachTree();
	if (!querytree) {
		return false;
	}

	if (conn->debugsqltranslation) {
		printf("before translation:\n");
		printQueryTree(querytree);
		printf("\n");
	}

	if (!parsed) {
		if (conn->debugsqltranslation) {
			printf("parse failed, using original:\n\"%s\"\n\n",
								querybuffer);
		}
		delete querytree;
		querytree=NULL;
		return false;
	}

	// apply translation rules
	if (!conn->sqlt->applyRules(conn,this,querytree)) {
		return false;
	}

	if (conn->debugsqltranslation) {
		printf("after translation:\n");
		printQueryTree(querytree);
		printf("\n");
	}

	// write the query back out
	stringbuffer	translatedquery;
	if (!conn->sqlw->write(conn,this,querytree,&translatedquery)) {
		return false;
	}

	if (conn->debugsqltranslation) {
		printf("translated:\n\"%s\"\n\n",
				translatedquery.getString());
	}

	// copy the translated query into query buffer
	if (translatedquery.getStringLength()>conn->maxquerysize) {
		// the translated query was too large
		return false;
	}
	charstring::copy(querybuffer,
			translatedquery.getString(),
			translatedquery.getStringLength());
	querylength=translatedquery.getStringLength();
	querybuffer[querylength]='\0';
	return true;
}
