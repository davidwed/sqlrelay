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
	setExportRow(true);
	setCurrentRow(0);
	setCurrentColumn(0);
	setCurrentField(NULL);

	// output to stdoutput or create/open file
	setFileDescriptor(&stdoutput);
	file	f;
	if (!charstring::isNullOrEmpty(filename)) {
		if (!f.create(filename,
				permissions::evalPermString("rw-r--r--"))) {
			// FIXME: report error
			return false;
		}
		setFileDescriptor(&f);
	}
	filedescriptor	*fd=getFileDescriptor();

	// export header
	fd->printf("<?xml version=\"1.0\"?>\n");

	// export table name
	if (!charstring::isNullOrEmpty(table)) {
		fd->printf("<table name=\"%s\">\n",table);
	}

	sqlrcursor	*sqlrcur=getSqlrCursor();
	const char * const *fieldstoignore=getFieldsToIgnore();

	// export columns
	uint32_t	cols=sqlrcur->colCount();
	clearNumberColumns();
	for (uint32_t j=0; j<cols; j++) {
		setNumberColumn(j,isNumberTypeChar(sqlrcur->getColumnType(j)));
		if (charstring::inSet(sqlrcur->getColumnName(j),
						fieldstoignore)) {
			cols--;
		}
	}
	if (!getIgnoreColumns()) {
		fd->printf("<columns count=\"%d\">\n",cols);
	}
	cols=sqlrcur->colCount();
	if (!getIgnoreColumns()) {
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
		setExportRow(true);

		// call the pre-row event
		if (!rowStart()) {
			return false;
		}

		// if rowStart() didn't disable export of this row...
		if (getExportRow()) {
			fd->printf("	<row>\n");
			for (setCurrentColumn(0);
				getCurrentColumn()<cols;
				setCurrentColumn(getCurrentColumn()+1)) {

				// ignore particular fields
				if (fieldstoignore) {
					if (charstring::inSet(
						sqlrcur->getColumnName(
							getCurrentColumn()),
						fieldstoignore)) {
						continue;
					}
				}

				// get the field
				setCurrentField(sqlrcur->getField(
							getCurrentRow(),
							getCurrentColumn()));
				if (!getCurrentField()) {
					break;
				}

				// call the pre-field event
				if (!fieldStart()) {
					return false;
				}

				// export the field
				fd->printf("	<field>");
				escapeField(fd,getCurrentField());
				fd->printf("</field>\n");

				// call the post-field event
				if (!fieldEnd()) {
					return false;
				}
			}
		}

		// call the post-row event
		// (we call this before closing the row in case an overridden
		// rowEnd() wants to add more fields or something)
		if (!rowEnd()) {
			return false;
		}

		// if rowStart() didn't disable export of this row...
		if (getExportRow()) {
			fd->printf("	</row>\n");
		}

		setCurrentRow(getCurrentRow()+1);

	} while (!sqlrcur->endOfResultSet() ||
			getCurrentRow()<sqlrcur->rowCount());

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
