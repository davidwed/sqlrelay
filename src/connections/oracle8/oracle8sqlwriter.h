#ifndef ORACLE8SQLWRITER_H
#define ORACLE8SQLWRITER_H

#include <sqlwriter.h>

class oracle8sqlwriter : public sqlwriter {
	public:
			oracle8sqlwriter();
		virtual	~oracle8sqlwriter();

	private:
		virtual const char * const *unsupportedElements();

		virtual bool	temporary(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	uniqueKey(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	cascade(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	selectQuery(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	as(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);
		virtual bool	isolationLevel(rudiments::xmldomnode *node,
					rudiments::stringbuffer *output);

		bool	convertDate(const char *date,
					rudiments::stringbuffer *output);
};

#endif
