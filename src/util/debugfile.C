// Copyright (c) 2000-2005  David Muse
// See the file COPYING for more information

#include <debugfile.h>
#include <rudiments/stringbuffer.h>
#include <rudiments/process.h>
#include <config.h>

debugfile::debugfile() {
	dbgfile=NULL;
	debuglogger=NULL;
}

debugfile::~debugfile() {
	closeDebugFile();
}

void debugfile::openDebugFile(const char *name, const char *localstatedir) {

	// set the debug file name
	char	*dbgfilename;
	if (localstatedir[0]) {
		dbgfilename=new char[charstring::length(localstatedir)+
					16+5+charstring::length(name)+20+1];
		sprintf(dbgfilename,"%s/sqlrelay/debug/sqlr-%s.%d",
						localstatedir,name,
						process::getProcessId());
	} else {
		dbgfilename=new char[charstring::length(DEBUG_DIR)+5+
					charstring::length(name)+20+1];
		sprintf(dbgfilename,"%s/sqlr-%s.%d",DEBUG_DIR,name,
						process::getProcessId());
	}

	// create the debug file
	mode_t	oldumask=umask(066);
	dbgfile=new filedestination();
	umask(oldumask);

	// open the file
	if (dbgfile->open(dbgfilename)) {
		printf("Debugging to: %s\n",dbgfilename);
		debuglogger=new logger();
		debuglogger->addLogDestination(dbgfile);
	} else {
		fprintf(stderr,"Couldn't open debug file: %s\n",dbgfilename);
		if (dbgfile) {
			dbgfile->close();
			delete dbgfile;
			dbgfile=NULL;
		}
	}

	delete[] dbgfilename;
}

void debugfile::closeDebugFile() {
	if (dbgfile) {
		dbgfile->close();
		delete dbgfile;
		dbgfile=NULL;
		delete debuglogger;
	}
}

void debugfile::debugPrint(const char *name, int32_t tabs, const char *string) {
	char	*header=debuglogger->logHeader(name);
	debuglogger->write(header,tabs,string);
	delete[] header;
}

void debugfile::debugPrint(const char *name, int32_t tabs, int32_t number) {
	char	*header=debuglogger->logHeader(name);
	debuglogger->write(header,tabs,number);
	delete[] header;
}

void debugfile::debugPrint(const char *name, int32_t tabs, double number) {
	char	*header=debuglogger->logHeader(name);
	debuglogger->write(header,tabs,number);
	delete[] header;
}

void debugfile::debugPrintBlob(const char *blob, uint32_t length) {

	// write printable characters from the blob, for all other characters,
	// print a period instead, also print a carriage return every 80 columns
	stringbuffer	*debugstr=new stringbuffer();
	debugstr->append('\n');
	int	column=0;
	for (uint32_t i=0; i<length; i++) {
		if (blob[i]>=' ' && blob[i]<='~') {
			debugstr->append(blob[i]);
		} else {
			debugstr->append('.');
		}
		column++;
		if (column==80) {
			debugstr->append('\n');
			column=0;
		}
	}
	debugstr->append('\n');
	char	*header=logger::logHeader("connection");
	debuglogger->write(header,0,debugstr->getString());
	delete[] header;
	delete debugstr;
}

void debugfile::debugPrintClob(const char *clob, uint32_t length) {

	// write printable characters from the clob, for NULl characters,
	// print a \0 instead
	stringbuffer	*debugstr=new stringbuffer();
	debugstr->append('\n');
	for (uint32_t i=0; i<length; i++) {
		if (clob[i]==(char)NULL) {
			debugstr->append("\\0");
		} else {
			debugstr->append(clob[i]);
		}
	}
	debugstr->append('\n');
	char	*header=logger::logHeader("connection");
	debuglogger->write(header,0,debugstr->getString());
	delete[] header;
	delete debugstr;
}

logger *debugfile::getDebugLogger() {
	return debuglogger;
}
