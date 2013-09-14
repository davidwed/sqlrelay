#ifndef FREETDSSQLWRITER_H
#define FREETDSSQLWRITER_H

#include <sqlwriter.h>

class freetdssqlwriter : public sqlwriter {
	public:
			freetdssqlwriter();
		virtual	~freetdssqlwriter();

	private:
		virtual bool	uniqueKey(xmldomnode *node,
					stringbuffer *output);
};

#endif
