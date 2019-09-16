// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#ifndef SQLRMODULEDATA_TAG_H
#define SQLRMODULEDATA_TAG_H

#include <sqlrelay/sqlrserver.h>

class SQLRSERVER_DLLSPEC sqlrmd_tag : public sqlrmoduledata {
	public:
		sqlrmd_tag(domnode *parameters);
		~sqlrmd_tag();
		
		void		setTag(uint16_t cursorid, const char *tag);
		void		setTag(uint16_t cursorid,
					const char *tag, size_t size);
		const char	*getTag(uint16_t cursorid);

	private:
		dictionary<uint16_t, char *>	tagmap;
};

#endif
