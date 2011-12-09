#include <oracle8sqlwriter.h>
#include <sqlelement.h>
#include <sqltranslatordebug.h>

oracle8sqlwriter::oracle8sqlwriter() : sqlwriter() {
	debugFunction();
}

oracle8sqlwriter::~oracle8sqlwriter() {
	debugFunction();
}

const char * const *oracle8sqlwriter::additionalElements() {
	debugFunction();
	static const char *additionalelements[]={
		// on commit...
		sqlelement::on_commit,
		NULL
	};
	return additionalelements;
}

const char * const *oracle8sqlwriter::unsupportedElements() {
	debugFunction();
	static const char *unsupportedelements[]={
		// constraints...
		sqlelement::unsigned_constraint,
		sqlelement::zerofill,
		sqlelement::binary,
		sqlelement::character_set,
		sqlelement::collate,
		sqlelement::auto_increment,
		sqlelement::key,
		sqlelement::comment,
		sqlelement::column_format,
		sqlelement::match,
		sqlelement::on_delete,
		sqlelement::on_update,
		NULL
	};
	return unsupportedelements;
}

bool oracle8sqlwriter::temporary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("global temporary");
	return true;
}

bool oracle8sqlwriter::uniqueKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique");
	return true;
}
