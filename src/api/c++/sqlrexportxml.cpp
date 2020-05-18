// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportxml.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>

sqlrexportxml::sqlrexportxml() {
}

sqlrexportxml::~sqlrexportxml() {
}

bool sqlrexportxml::exportToFile(const char *filename, const char *table) {

	// reset flags
	exportrow=true;
	currentrow=0;
	currentcol=0;
	currentfield=NULL;

	// output to stdoutput or create/open file
	filedescriptor	*fd=&stdoutput;
	file		f;
	if (!charstring::isNullOrEmpty(filename)) {
		if (!f.create(filename,
				permissions::evalPermString("rw-r--r--"))) {
			// FIXME: report error
			return false;
		}
		fd=&f;
	}

	// print header
	fd->printf("<?xml version=\"1.0\"?>\n");

	// print table name
	if (!charstring::isNullOrEmpty(table)) {
		fd->printf("<table name=\"%s\">\n",table);
	}

	// print columns
	uint32_t	cols=sqlrcur->colCount();
	if (!ignorecolumns) {
		fd->printf("<columns count=\"%d\">\n",cols);
		for (uint32_t j=0; j<cols; j++) {
			fd->printf("	<column name=\"%s\" type=\"%s\"/>\n",
						sqlrcur->getColumnName(j),
						sqlrcur->getColumnType(j));
		}
		fd->printf("</columns>\n");
	}

	// call the pre-rows event
	if (!rowsStart()) {
		return false;
	}

	// export rows
	fd->printf("<rows>\n");
	do {

		// reset export-row flag
		exportrow=true;

		// call the pre-row event
		if (!rowStart()) {
			return false;
		}

		// if rowStart() didn't disable export of this row...
		if (exportrow) {
			fd->printf("	<row>\n");
			for (currentcol=0; currentcol<cols; currentcol++) {

				// get the field
				currentfield=sqlrcur->getField(
						currentrow,currentcol);
				if (!currentfield) {
					break;
				}

				// call the pre-column event
				if (!colStart()) {
					return false;
				}

				// export the field
				fd->printf("	<field>");
				escapeField(fd,currentfield);
				fd->printf("</field>\n");

				// call the post-column event
				if (!colEnd()) {
					return false;
				}
			}
			fd->printf("	</row>\n");
		}

		// call the post-row event
		if (!rowEnd()) {
			return false;
		}

		currentrow++;

	} while (!sqlrcur->endOfResultSet() || currentrow<sqlrcur->rowCount());

	fd->printf("</rows>\n");
	fd->printf("</table>\n");

	// call the post-rows event
	if (!rowsEnd()) {
		return false;
	}

	return true;
}

void sqlrexportxml::escapeField(filedescriptor *fd, const char *field) {
	for (const char *f=field; *f; f++) {
		if (*f=='"') {
			fd->write("\"\"");
		} else if (*f<' ' || *f>'~' || *f=='&' || *f=='<' || *f=='>') {
			fd->printf("&%d;",(uint8_t)*f);
		} else {
			fd->write(*f);
		}
	}
}
