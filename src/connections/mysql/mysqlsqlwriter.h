#ifndef MYSQLSQLWRITER_H
#define MYSQLSQLWRITER_H

#include <sqlwriter.h>

class mysqlsqlwriter : public sqlwriter {
	public:
			mysqlsqlwriter();
		virtual	~mysqlsqlwriter();

	private:
		virtual const char * const *additionalElements();
		virtual const char * const *unsupportedElements();
};

#endif
