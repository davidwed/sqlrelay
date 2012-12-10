// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrclient.h>
#include <rudiments/environment.h>

void sqlrconnection::debugOn() {
	debug=true;
}

void sqlrconnection::debugOff() {
	debug=false;
}

bool sqlrconnection::getDebug() {
	return debug;
}

void sqlrconnection::debugPreStart() {
	if (webdebug==-1) {
		const char	*docroot=environment::getValue("DOCUMENT_ROOT");
		if (docroot && docroot[0]) {
			webdebug=1;
		} else {
			webdebug=0;
		}
	}
	if (webdebug==1) {
		debugPrint("<pre>\n");
	}
}

void sqlrconnection::debugPreEnd() {
	if (webdebug==1) {
		debugPrint("</pre>\n");
	}
}

void sqlrconnection::debugPrintFunction(
				int (*printfunction)(const char *,...)) {
	this->printfunction=printfunction;
}

void sqlrconnection::debugPrint(const char *string) {
	if (printfunction) {
		(*printfunction)("%s",string);
	} else {
		printf("%s",string);
	}
}

void sqlrconnection::debugPrint(int64_t number) {
	if (printfunction) {
		(*printfunction)("%lld",(long long)number);
	} else {
		printf("%lld",(long long)number);
	}
}

void sqlrconnection::debugPrint(double number) {
	if (printfunction) {
		(*printfunction)("%f",number);
	} else {
		printf("%f",number);
	}
}

void sqlrconnection::debugPrint(char character) {
	if (printfunction) {
		(*printfunction)("%c",character);
	} else {
		printf("%c",character);
	}
}

void sqlrconnection::debugPrintBlob(const char *blob, uint32_t length) {
	debugPrint('\n');
	int	column=0;
	for (uint32_t i=0; i<length; i++) {
		if (blob[i]>=' ' && blob[i]<='~') {
			debugPrint(blob[i]);
		} else {
			debugPrint('.');
		}
		column++;
		if (column==80) {
			debugPrint('\n');
			column=0;
		}
	}
	debugPrint('\n');
}

void sqlrconnection::debugPrintClob(const char *clob, uint32_t length) {
	debugPrint('\n');
	for (uint32_t i=0; i<length; i++) {
		if (clob[i]=='\0') {
			debugPrint("\\0");
		} else {
			debugPrint(clob[i]);
		}
	}
	debugPrint('\n');
}
