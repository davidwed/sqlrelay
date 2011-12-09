#ifndef ORACLE8SQLWRITER_H
#define ORACLE8SQLWRITER_H

#include <sqlwriter.h>

using namespace rudiments;

class oracle8sqlwriter : public sqlwriter {
	public:
			oracle8sqlwriter();
		virtual	~oracle8sqlwriter();

	private:
		virtual const char * const *additionalElements();
		virtual const char * const *unsupportedElements();

		virtual bool	temporary(xmldomnode *node,
						stringbuffer *output);
		virtual bool	uniqueKey(xmldomnode *node,
						stringbuffer *output);
};

#endif
