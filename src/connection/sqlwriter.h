// Copyright (c) 1999-2011  David Muse
// See the file COPYING for more information

#ifndef SQLWRITER_H
#define SQLWRITER_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>

using namespace rudiments;

class sqlwriter {
	public:
			sqlwriter();
		virtual	~sqlwriter();

		virtual bool	write(xmldom *tree, stringbuffer *output);
	protected:
		virtual bool	write(xmldomnode *tree, stringbuffer *output);

		virtual const char * const *baseElements();
		virtual const char * const *additionalElements();
		virtual const char * const *unsupportedElements();

		virtual	bool	elementSupported(const char *element);
		virtual bool	handleStart(xmldomnode *node,
						stringbuffer *output);
		virtual bool	handleEnd(xmldomnode *node,
						stringbuffer *output);

		// generic
		virtual bool	name(xmldomnode *node,
						stringbuffer *output);
		virtual bool	type(xmldomnode *node,
						stringbuffer *output);
		virtual bool	endType(xmldomnode *node,
						stringbuffer *output);
		virtual bool	size(xmldomnode *node,
						stringbuffer *output);
		virtual bool	endSize(xmldomnode *node,
						stringbuffer *output);
		virtual bool	value(xmldomnode *node,
						stringbuffer *output);
		virtual bool	options(xmldomnode *node,
						stringbuffer *output);
		virtual bool	endOptions(xmldomnode *node,
						stringbuffer *output);
		virtual bool	verbatim(xmldomnode *node,
						stringbuffer *output);

		// create query...
		virtual bool	createQuery(xmldomnode *node,
						stringbuffer *output);

		// table...
		virtual bool	table(xmldomnode *node,
						stringbuffer *output);
		virtual bool	temporary(xmldomnode *node,
						stringbuffer *output);
		virtual bool	ifNotExists(xmldomnode *node,
						stringbuffer *output);

		// column definitions...
		virtual bool	columns(xmldomnode *node,
						stringbuffer *output);
		virtual bool	endColumns(xmldomnode *node,
						stringbuffer *output);
		virtual bool	column(xmldomnode *node,
						stringbuffer *output);
		virtual bool	endColumn(xmldomnode *node,
						stringbuffer *output);
		virtual bool	values(xmldomnode *node,
						stringbuffer *output);
		virtual bool	endValues(xmldomnode *node,
						stringbuffer *output);
		virtual bool	length(xmldomnode *node,
						stringbuffer *output);
		virtual bool	scale(xmldomnode *node,
						stringbuffer *output);

		// constraints...
		virtual bool	constraints(xmldomnode *node,
						stringbuffer *output);
		virtual bool	unsignedConstraint(xmldomnode *node,
						stringbuffer *output);
		virtual bool	zerofill(xmldomnode *node,
						stringbuffer *output);
		virtual bool	binary(xmldomnode *node,
						stringbuffer *output);
		virtual bool	characterSet(xmldomnode *node,
						stringbuffer *output);
		virtual bool	collate(xmldomnode *node,
						stringbuffer *output);
		virtual bool	nullable(xmldomnode *node,
						stringbuffer *output);
		virtual bool	notNullable(xmldomnode *node,
						stringbuffer *output);
		virtual bool	defaultValue(xmldomnode *node,
						stringbuffer *output);
		virtual bool	autoIncrement(xmldomnode *node,
						stringbuffer *output);
		virtual bool	uniqueKey(xmldomnode *node,
						stringbuffer *output);
		virtual bool	primaryKey(xmldomnode *node,
						stringbuffer *output);
		virtual bool	key(xmldomnode *node,
						stringbuffer *output);
		virtual bool	comment(xmldomnode *node,
						stringbuffer *output);
		virtual bool	columnFormat(xmldomnode *node,
						stringbuffer *output);
		virtual bool	references(xmldomnode *node,
						stringbuffer *output);
		virtual bool	endReferences(xmldomnode *node,
						stringbuffer *output);
		virtual bool	match(xmldomnode *node,
						stringbuffer *output);
		virtual bool	onDelete(xmldomnode *node,
						stringbuffer *output);
		virtual bool	onUpdate(xmldomnode *node,
						stringbuffer *output);
		virtual bool	onCommit(xmldomnode *node,
						stringbuffer *output);


		// drop...
		virtual bool	dropQuery(xmldomnode *node,
						stringbuffer *output);


		// insert...
		virtual bool	insertQuery(xmldomnode *node,
						stringbuffer *output);
		virtual bool	into(xmldomnode *node,
						stringbuffer *output);


		// update...
		virtual bool	updateQuery(xmldomnode *node,
						stringbuffer *output);


		// delete...
		virtual bool	deleteQuery(xmldomnode *node,
						stringbuffer *output);


		// select...
		virtual bool	selectQuery(xmldomnode *node,
						stringbuffer *output);
		virtual bool	unique(xmldomnode *node,
						stringbuffer *output);
		virtual bool	distinct(xmldomnode *node,
						stringbuffer *output);



		// helper methods
		virtual bool	outputValue(xmldomnode *node,
						stringbuffer *output);
		virtual bool	space(stringbuffer *output);
		virtual bool	comma(stringbuffer *output);
		virtual bool	leftParen(stringbuffer *output);
		virtual bool	rightParen(stringbuffer *output);
		virtual bool	hasSibling(xmldomnode *node);
};

#endif
