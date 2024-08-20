// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportcsvjsondomnode.h>

sqlrexportcsvjsondomnode::sqlrexportcsvjsondomnode() : sqlrexportdomnode() {
}

sqlrexportcsvjsondomnode::~sqlrexportcsvjsondomnode() {
}

bool sqlrexportcsvjsondomnode::exportData() {
	return sqlrexportdomnode::exportData();
}

bool sqlrexportcsvjsondomnode::startProcessingColumns() {

	// start processing columns
	if (!sqlrexportdomnode::startProcessingColumns()) {
		return false;
	}

	// export columns node
	if (!getExcludeColumns()) {
		// "h" for header, like in csvdom
		setColumnsDomNode(getDomNode()->appendTag("h"));
		getColumnsDomNode()->setAttributeValue("t","a");
	}
	return true;
}

bool sqlrexportcsvjsondomnode::exportColumnName(bool first) {

	if (!sqlrexportdomnode::exportColumnName(first)) {
		return false;
	}

	// export the column name
	// "v" for value, all jsondom arrays
	// consist of "v"alues
	bool	isnumber=charstring::isNumber(getCurrentField());
	domnode	*column=getColumnsDomNode()->appendTag("v");
	setCurrentColumnDomNode(column);
	if (isnumber) {
		column->setAttributeValue("t","n");
		column->setAttributeValue("v",getCurrentField());
	} else {
		column->setAttributeValue("t","s");
		column->setAttributeValue("v",getCurrentField());
	}
	return true;
}

bool sqlrexportcsvjsondomnode::startProcessingRow() {

	// start processing row
	if (!sqlrexportdomnode::startProcessingRow()) {
		return false;
	}

	// if rowStart() didn't disable export of this row...
	if (!getExcludeRow()) {
		// "r" for record, like in csvdom
		setCurrentRowDomNode(getDomNode()->appendTag("r"));
		getCurrentRowDomNode()->setAttributeValue("t","a");
	}
	return true;
}

bool sqlrexportcsvjsondomnode::exportField(bool first) {

	if (!sqlrexportdomnode::exportField(first)) {
		return false;
	}

	// export the field
	// "v" for value, all jsondom arrays consist of "v"alues
	domnode	*field=getCurrentRowDomNode()->appendTag("v");
	setCurrentFieldDomNode(field);
	if (getIsNumericColumn(getCurrentColumn())) {
		field->setAttributeValue("t","n");
		field->setAttributeValue("v",getCurrentField());
	} else {
		field->setAttributeValue("t","s");
		field->setAttributeValue("v",getCurrentField());
	}
	return true;
}
