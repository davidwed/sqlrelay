// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTCSV_H
#define SQLRIMPORTCSV_H

#include <sqlrelay/private/sqlrimportcsvincludes.h>

class sqlrimportcsv : public sqlrimport, public csvsax {
	public:
			sqlrimportcsv();
			~sqlrimportcsv();

		void	setPrimaryKeyPosition(uint32_t primarykeyposition);
		void	setPrimaryKeyName(const char *primarykeyname);
		void	setPrimaryKeySequence(const char *primarykeysequence);

		bool	importFromFile(const char *filename);

	#include <sqlrelay/private/sqlrimportcsv.h>
};

#endif
