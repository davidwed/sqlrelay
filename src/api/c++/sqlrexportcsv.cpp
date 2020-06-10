// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportcsv.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>

#define NEED_IS_NUMBER_TYPE_CHAR
#include <datatypes.h>

sqlrexportcsv::sqlrexportcsv() {
}

sqlrexportcsv::~sqlrexportcsv() {
}

bool sqlrexportcsv::exportToFile(const char *filename, const char *table) {

	// reset flags
	setExportRow(true);
	setCurrentRow(0);
	setCurrentColumn(0);
	setCurrentField(NULL);

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

	sqlrcursor	*sqlrcur=getSqlrCursor();
	const char * const *fieldstoignore=getFieldsToIgnore();

	// export header
	uint32_t	cols=sqlrcur->colCount();
	clearNumberColumns();
	bool	first=true;
	for (uint32_t j=0; j<cols; j++) {
		setNumberColumn(j,isNumberTypeChar(sqlrcur->getColumnType(j)));
		const char	*name=sqlrcur->getColumnName(j);
		if (charstring::inSet(name,fieldstoignore)) {
			continue;
		}
		if (!getIgnoreColumns()) {
			if (first) {
				first=false;
			} else {
				fd->printf(",");
			}
			bool	isnumber=charstring::isNumber(name);
			if (!isnumber) {
				fd->write('"');
			}
			escapeField(fd,name);
			if (!isnumber) {
				fd->write('"');
			}
		}
	}
	if (!getIgnoreColumns()) {
		fd->printf("\n");
	}

	// call the pre-rows event
	if (!rowsStart()) {
		return false;
	}

	// export rows...
	do {

		// reset export-row flag
		setExportRow(true);

		// call the pre-row event
		if (!rowStart()) {
			return false;
		}

		// if rowStart() didn't disable export of this row...
		if (getExportRow()) {
			bool	first=true;
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

				// prepend a comma if necessary
				if (first) {
					first=false;
				} else {
					fd->write(',');
				}

				// call the pre-column event
				if (!colStart()) {
					return false;
				}

				// export the field
				bool	isnumber=
					getNumberColumn(getCurrentColumn());
				if (!isnumber) {
					fd->write('"');
				}
				escapeField(fd,getCurrentField());
				if (!isnumber) {
					fd->write('"');
				}

				// call the post-column event
				if (!colEnd()) {
					return false;
				}
			}
			fd->write('\n');
		}

		// call the post-row event
		if (!rowEnd()) {
			return false;
		}

		setCurrentRow(getCurrentRow()+1);

	} while  (!sqlrcur->endOfResultSet() ||
			getCurrentRow()<sqlrcur->rowCount());

	// call the post-rows event
	if (!rowsEnd()) {
		return false;
	}

	return true;
}

void sqlrexportcsv::escapeField(filedescriptor *fd, const char *field) {
	for (const char *f=field; *f; f++) {
		if (*f=='"') {
			fd->write("\"\"");
		} else {
			fd->write(*f);
		}
	}
}
