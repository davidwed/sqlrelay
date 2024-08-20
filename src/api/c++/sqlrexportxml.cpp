// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportxml.h>

sqlrexportxml::sqlrexportxml() : sqlrexportfile() {
}

sqlrexportxml::~sqlrexportxml() {
}

bool sqlrexportxml::exportData() {
	return sqlrexportfile::exportData();
}

bool sqlrexportxml::startProcessingExport() {

	if (!sqlrexportfile::startProcessingExport()) {
		return false;
	}

	// get the file descriptor
	filedescriptor	*fd=getFileDescriptor();

	// export the xml header
	if (fd->write("<?xml version=\"1.0\"?>\n")!=22) {
		return systemError();
	}

	// open the table tag
	if (fd->write("<table")!=6) {
		return systemError();
	}
	if (!charstring::isNullOrEmpty(getTable())) {
	 	if (fd->write(" name=\"")!=7) {
			return systemError();
		}
		if (!escapeValue(fd,getTable())) {
			return false;
		}
		if (fd->write('\"')!=sizeof(char)) {
			return systemError();
		}
	}
	if (fd->write(">\n")!=2) {
		return systemError();
	}

	return true;
}

bool sqlrexportxml::startProcessingColumns() {

	if (!sqlrexportfile::startProcessingColumns()) {
		return false;
	}

	// open the columns tag
	if (!getExcludeColumns()) {
		getFileDescriptor()->printf("<columns count=\"%d\">\n",
						getSqlrCursor()->colCount());
	}
	return true;
}

bool sqlrexportxml::exportColumnName(bool first) {

	if (!sqlrexport::exportColumnName(first)) {
		return false;
	}

	// get the file descriptor
	filedescriptor	*fd=getFileDescriptor();

	// export the column name and type
	if (fd->write("	<column name=\"")!=15) {
		return systemError();
	}
	if (!escapeValue(fd,getCurrentField())) {
		return false;
	}
	if (fd->write("\" type=\"")!=8) {
		return systemError();
	}
	if (!escapeValue(fd,getSqlrCursor()->
				getColumnType(getCurrentColumn()))) {
		return false;
	}
	if (fd->write("\"/>\n")!=4) {
		return systemError();
	}
	return true;
}

bool sqlrexportxml::endProcessingColumns() {

	// (we call this before closing the columns in case an
	// overridden columnsEnd() wants to add more columns or
	// something)
	if (!sqlrexportfile::endProcessingColumns()) {
		return false;
	}

	// close the columns tag
	if (!getExcludeColumns()) {
		if (getFileDescriptor()->write("</columns>\n")!=11) {
			return systemError();
		}
	}
	return true;
}

bool sqlrexportxml::startProcessingRows() {

	if (!sqlrexportfile::startProcessingRows()) {
		return false;
	}

	// open the rows tag
	if (getFileDescriptor()->write("<rows>\n")!=7) {
		return systemError();
	}
	return true;
}

bool sqlrexportxml::startProcessingRow() {

	if (!sqlrexportfile::startProcessingRow()) {
		return false;
	}

	// open the row tag
	if (!getExcludeRow()) {
		if (getFileDescriptor()->write("	<row>\n")!=7) {
			return systemError();
		}
	}
	return true;
}

bool sqlrexportxml::exportField(bool first) {

	if (!sqlrexportfile::exportField(first)) {
		return false;
	}

	// get the file descriptor
	filedescriptor	*fd=getFileDescriptor();

	// export the field
	if (fd->write("	<field>")!=8) {
		return systemError();
	}
	if (!escapeValue(fd,getCurrentField())) {
		return false;
	}
	if (fd->write("</field>\n")!=9) {
		return systemError();
	}
	return true;
}

bool sqlrexportxml::endProcessingRow() {

	// (we call this before closing the row in case an overridden
	// rowEnd() wants to add more fields or something)
	if (!sqlrexportfile::endProcessingRow()) {
		return false;
	}

	// close the row tag
	if (!getExcludeRow()) {
		if (getFileDescriptor()->write("	</row>\n")!=8) {
			return systemError();
		}
	}
	return true;
}

bool sqlrexportxml::endProcessingRows() {

	// end processing rows
	// (we call this before closing the rows in case an overridden
	// rowsEnd() wants to add more rows or something)
	if (!sqlrexportfile::endProcessingRows()) {
		return false;
	}

	// close the rows tag
	if (getFileDescriptor()->write("</rows>\n")!=8) {
		return systemError();
	}
	return true;
}

bool sqlrexportxml::endProcessingExport() {

	// close the table tag
	if (getFileDescriptor()->write("</table>\n")!=9) {
		return systemError();
	}

	return sqlrexportfile::endProcessingExport();
}

bool sqlrexportxml::escapeValue(filedescriptor *fd, const char *field) {
	if (!field) {
		return true;
	}
	for (const char *f=field; *f; f++) {
		if (*f=='"' || *f<' ' || *f>'~' ||
				*f=='&' || *f=='<' || *f=='>') {
			if (fd->printf("&%d;",(uint8_t)*f)!=
						((*f<=9)?3:((*f>=100)?5:4))) {
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
