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

	// call the pre-header event
	if (!headerStart()) {
		return false;
	}

	sqlrcursor	*sqlrcur=getSqlrCursor();
	const char * const *fieldstoignore=getFieldsToIgnore();

	// export header
	uint32_t	cols=sqlrcur->colCount();
	clearNumberColumns();
	bool	first=true;
	for (setCurrentColumn(0);
		getCurrentColumn()<cols;
		setCurrentColumn(getCurrentColumn()+1)) {

		setNumberColumn(getCurrentColumn(),
			isNumberTypeChar(sqlrcur->getColumnType(
						getCurrentColumn())));

		setCurrentField(sqlrcur->getColumnName(getCurrentColumn()));
		if (charstring::inSet(getCurrentField(),fieldstoignore)) {
			continue;
		}

		if (!getIgnoreColumns()) {

			if (first) {
				first=false;
			} else {
				fd->printf(",");
			}

			// call the pre-column event
			if (!columnStart()) {
				return false;
			}

			bool	isnumber=
				charstring::isNumber(getCurrentField());
			if (!isnumber) {
				fd->write('"');
			}
			escapeField(fd,getCurrentField());
			if (!isnumber) {
				fd->write('"');
			}

			// call the post-column event
			if (!columnEnd()) {
				return false;
			}
		}
	}

	// call the post-header event
	// (we call this before closing the header in case an overridden
	// headerEnd() wants to add more columns or something)
	if (!headerEnd()) {
		return false;
	}

	if (!getIgnoreColumns()) {
		fd->write('\n');
	}

	// call the pre-rows event
	if (!rowsStart()) {
		return false;
	}

	// export rows...
	while (!sqlrcur->endOfResultSet() ||
			getCurrentRow()<sqlrcur->rowCount()) {

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

				// call the pre-field event
				if (!fieldStart()) {
					return false;
				}

				// we need to quote the field if it's not a
				// number, or if it is a number, but has more
				// than 12 digits.  Excel (and presumably other
				// spreadsheet apps) likes to convert 12+
				// digit numbers to scientific notation.
				bool	quote=
					(!getNumberColumn(getCurrentColumn()) ||
					charstring::length(
						getCurrentField())>=12);

				// export the field
				if (quote) {
					fd->write('"');
				}
				escapeField(fd,getCurrentField());
				if (quote) {
					fd->write('"');
				}

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
			fd->write('\n');
		}

		setCurrentRow(getCurrentRow()+1);
	}

	// call the post-rows event
	if (!rowsEnd()) {
		return false;
	}

	return true;
}

void sqlrexportcsv::escapeField(filedescriptor *fd, const char *field) {
	for (const char *f=field; *f; f++) {
		// escape double quotes and ignore non-ascii characters
		if (*f=='"') {
			fd->write("\"\"");
		} else if (*f>=' ' && *f<='~') {
			fd->write(*f);
		}
	}
}

bool sqlrexportcsv::exportToJsonDomNode(domnode *jsondomnode) {

	// reset flags
	setExportRow(true);
	setCurrentRow(0);
	setCurrentColumn(0);
	setCurrentField(NULL);

	// call the pre-header event
	if (!headerStart()) {
		return false;
	}

	sqlrcursor	*sqlrcur=getSqlrCursor();
	const char * const *fieldstoignore=getFieldsToIgnore();

	domnode	*columns=jsondomnode->appendTag("columns");
	columns->setAttributeValue("t","a");

	// export header
	uint32_t	cols=sqlrcur->colCount();
	clearNumberColumns();
	for (setCurrentColumn(0);
		getCurrentColumn()<cols;
		setCurrentColumn(getCurrentColumn()+1)) {

		setNumberColumn(getCurrentColumn(),
			isNumberTypeChar(sqlrcur->getColumnType(
						getCurrentColumn())));

		setCurrentField(sqlrcur->getColumnName(getCurrentColumn()));
		if (charstring::inSet(getCurrentField(),fieldstoignore)) {
			continue;
		}

		if (!getIgnoreColumns()) {

			// call the pre-column event
			if (!columnStart()) {
				return false;
			}

			bool	isnumber=
				charstring::isNumber(getCurrentField());
			domnode	*column=columns->appendTag("v");
			if (isnumber) {
				column->setAttributeValue("t","n");
				column->setAttributeValue("v",
						getCurrentField());
			} else {
				column->setAttributeValue("t","s");
				column->setAttributeValue("v",
						getCurrentField());
			}

			// call the post-column event
			if (!columnEnd()) {
				return false;
			}
		}
	}

	// call the post-header event
	// (we call this before closing the header in case an overridden
	// headerEnd() wants to add more columns or something)
	if (!headerEnd()) {
		return false;
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

			domnode	*row=jsondomnode->appendTag("row");
			row->setAttributeValue("t","a");

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

				domnode	*field=row->appendTag("v");
				if (getNumberColumn(getCurrentColumn())) {
					field->setAttributeValue("t","n");
					field->setAttributeValue("v",
							getCurrentField());
				} else {
					field->setAttributeValue("t","s");
					field->setAttributeValue("v",
							getCurrentField());
				}

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

		setCurrentRow(getCurrentRow()+1);

	} while  (!sqlrcur->endOfResultSet() ||
			getCurrentRow()<sqlrcur->rowCount());

	// call the post-rows event
	if (!rowsEnd()) {
		return false;
	}

	return true;
}
