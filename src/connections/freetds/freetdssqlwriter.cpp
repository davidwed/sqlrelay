#include <freetdssqlwriter.h>
#include <sqlrcursor.h>
#include <sqlparser.h>
#include <debugprint.h>

freetdssqlwriter::freetdssqlwriter() : sqlwriter() {
	debugFunction();
}

freetdssqlwriter::~freetdssqlwriter() {
	debugFunction();
}

bool freetdssqlwriter::uniqueKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique");
	return true;
}
