#ifndef ORACLE8SQLWRITER_H
#define ORACLE8SQLWRITER_H

#include <sqlwriter.h>

class oracle8sqlwriter : public sqlwriter {
	public:
			oracle8sqlwriter();
		virtual	~oracle8sqlwriter();

	private:
		virtual const char * const *unsupportedElements();

		virtual bool	uniqueKey(xmldomnode *node,
					stringbuffer *output);
		virtual bool	cascade(xmldomnode *node,
					stringbuffer *output);
		virtual bool	selectQuery(xmldomnode *node,
					stringbuffer *output);
		virtual bool	as(xmldomnode *node,
					stringbuffer *output);
		virtual bool	isolationLevel(xmldomnode *node,
					stringbuffer *output);

		bool	convertDate(const char *date,
					stringbuffer *output);
};

#endif
