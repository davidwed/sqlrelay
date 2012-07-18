// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#include <sqlwriter.h>
#include <debugprint.h>

bool sqlwriter::createQuery(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("create");
	return true;
}

bool sqlwriter::table(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("table");
	return true;
}

bool sqlwriter::global(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("global");
	return true;
}

bool sqlwriter::temporary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("temporary");
	return true;
}

bool sqlwriter::ifNotExists(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("if not exists");
	return true;
}

bool sqlwriter::columns(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endColumns(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::column(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::endColumn(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	if (hasSibling(node)) {
		comma(output);
	}
	return true;
}

bool sqlwriter::values(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	leftParen(output);
	return true;
}

bool sqlwriter::endValues(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	rightParen(output);
	return true;
}

bool sqlwriter::length(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::scale(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	comma(output);
	outputValue(node,output);
	return true;
}

bool sqlwriter::constraints(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::unsignedConstraint(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unsigned");
	return true;
}

bool sqlwriter::zerofill(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("zerofill");
	return true;
}

bool sqlwriter::binary(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("binary");
	return true;
}

bool sqlwriter::characterSet(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("character set ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::collate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("collate ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::nullable(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("null");
	return true;
}

bool sqlwriter::notNull(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("not null");
	return true;
}

bool sqlwriter::defaultValue(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("default ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::autoIncrement(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("auto_increment");
	return true;
}

bool sqlwriter::uniqueKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("unique key");
	return true;
}

bool sqlwriter::primaryKey(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("primary key");
	return true;
}

bool sqlwriter::key(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("key");
	return true;
}

bool sqlwriter::comment(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("comment ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::columnFormat(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("column_format ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::references(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("references ");
	return true;
}

bool sqlwriter::endReferences(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	return true;
}

bool sqlwriter::match(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("match ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::onDelete(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on delete ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::onUpdate(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on update ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::onCommit(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("on commit ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::as(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("as ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::fulltext(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("fulltext ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::spatial(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("spatial ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::index(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("index ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::indexName(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	outputValue(node,output);
	return true;
}

bool sqlwriter::btree(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("btree ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::hash(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("hash ");
	outputValue(node,output);
	return true;
}

bool sqlwriter::synonym(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("synonym ");
	return true;
}

bool sqlwriter::forClause(xmldomnode *node, stringbuffer *output) {
	debugFunction();
	output->append("for ");
	return true;
}
