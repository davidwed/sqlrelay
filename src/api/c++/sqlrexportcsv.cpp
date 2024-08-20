// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportcsv.h>

sqlrexportcsv::sqlrexportcsv() : sqlrexportfile() {
}

sqlrexportcsv::~sqlrexportcsv() {
}

bool sqlrexportcsv::exportData() {
	return sqlrexportfile::exportData();
}

bool sqlrexportcsv::exportColumnName(bool first) {

	if (!sqlrexport::exportColumnName(first)) {
		return false;
	}

	// get the file descriptor
	filedescriptor	*fd=getFileDescriptor();

	// export the column name
	if (!first) {
		if (fd->write(',')!=sizeof(char)) {
			return systemError();
		}
	}
	bool	isnumber=charstring::isNumber(getCurrentField());
	if (!isnumber) {
		if (fd->write('"')!=sizeof(char)) {
			return systemError();
		}
	}
	if (!escapeValue(fd,getCurrentField())) {
		return false;
	}
	if (!isnumber) {
		if (fd->write('"')!=sizeof(char)) {
			return systemError();
		}
	}
	return true;
}

bool sqlrexportcsv::endProcessingColumns() {

	// (we call this before closing the columns in case an
	// overridden columnsEnd() wants to add mroe columns or
	// something)
	if (!sqlrexportfile::endProcessingColumns()) {
		return false;
	}

	if (!getExcludeColumns()) {
		if (getFileDescriptor()->write('\n')!=sizeof(char)) {
			return systemError();
		}
	}
	return true;
}

bool sqlrexportcsv::exportField(bool first) {

	if (!sqlrexportfile::exportField(first)) {
		return false;
	}

	// get the file descriptor
	filedescriptor	*fd=getFileDescriptor();

	// we need to quote the field if it's not a
	// number, or if it is a number, but has more
	// than 12 digits.  Excel (and presumably other
	// spreadsheet apps) likes to convert 12+
	// digit numbers to scientific notation.
	bool	quote=(!getIsNumericColumn(getCurrentColumn()) ||
			charstring::getLength(getCurrentField())>=12);

	// export the field
	if (first) {
		first=false;
	} else {
		if (fd->write(',')!=sizeof(char)) {
			return systemError();
		}
	}
	if (quote) {
		if (fd->write('"')!=sizeof(char)) {
			return systemError();
		}
	}
	if (!escapeValue(fd,getCurrentField())) {
		return false;
	}
	if (quote) {
		if (fd->write('"')!=sizeof(char)) {
			return systemError();
		}
	}
	return true;
}

bool sqlrexportcsv::endProcessingRow() {

	// (we call this before closing the row in case an overridden
	// rowEnd() wants to add mroe fields or something)
	if (!sqlrexportfile::endProcessingRow()) {
		return false;
	}

	if (!getExcludeRow()) {
		if (getFileDescriptor()->write('\n')!=sizeof(char)) {
			return systemError();
		}
	}
	return true;
}

bool sqlrexportcsv::escapeValue(filedescriptor *fd, const char *field) {
	if (!field) {
		return true;
	}
	for (const char *f=field; *f; f++) {
		// escape double quotes
		if (*f=='"') {
			if (fd->write("\"\"")!=2) {
				return systemError();
			}
		} else {
			if (fd->write(*f)!=sizeof(char)) {
				return systemError();
			}
		}
	}
	return true;
}
