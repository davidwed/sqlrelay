#ifndef MYSQLSQLWRITER_H
#define MYSQLSQLWRITER_H

#include <sqlwriter.h>

using namespace rudiments;

class mysqlsqlwriter : public sqlwriter {
	public:
			mysqlsqlwriter();
		virtual	~mysqlsqlwriter();

	private:
		virtual const char * const *additionalElements();
};

#endif
