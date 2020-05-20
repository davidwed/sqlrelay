// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportxml.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>

#define NEED_IS_NUMBER_TYPE_CHAR
#include <datatypes.h>

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

	// export header
	fd->printf("<?xml version=\"1.0\"?>\n");

	// export table name
	if (!charstring::isNullOrEmpty(table)) {
		fd->printf("<table name=\"%s\">\n",table);
	}

	// export columns
	uint32_t	cols=sqlrcur->colCount();
	delete[] numbercolumns;
	numbercolumns=new bool[cols];
	for (uint32_t j=0; j<cols; j++) {
		numbercolumns[j]=isNumberTypeChar(sqlrcur->getColumnType(j));
		if (charstring::inSet(sqlrcur->getColumnName(j),
						fieldstoignore)) {
			cols--;
		}
	}
	if (!ignorecolumns) {
		fd->printf("<columns count=\"%d\">\n",cols);
	}
	cols=sqlrcur->colCount();
	if (!ignorecolumns) {
		for (uint32_t j=0; j<cols; j++) {
			const char	*name=sqlrcur->getColumnName(j);
			if (charstring::inSet(name,fieldstoignore)) {
				continue;
			}
			fd->printf("	<column name=\"%s\" type=\"%s\"/>\n",
						name,sqlrcur->getColumnType(j));
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

				// ignore particular fields
				if (fieldstoignore) {
					if (charstring::inSet(
						sqlrcur->getColumnName(
								currentcol),
						fieldstoignore)) {
						continue;
					}
				}

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
