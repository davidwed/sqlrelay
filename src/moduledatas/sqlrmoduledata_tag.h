// Copyright (c) 1999-2019 David Muse
// See the file COPYING for more information

#ifndef SQLRMODULEDATA_TAG_H
#define SQLRMODULEDATA_TAG_H

#include <sqlrelay/sqlrserver.h>
#include <rudiments/dictionary.h>
#include <rudiments/avltree.h>

class SQLRSERVER_DLLSPEC sqlrmd_tag : public sqlrmoduledata {
	public:
		sqlrmd_tag(domnode *parameters);
		~sqlrmd_tag();
		
		void	addTag(uint16_t cursorid, const char *tag);
		void	addTag(uint16_t cursorid, const char *tag, size_t size);
		avltree<char *>	*getTags(uint16_t cursorid);
		bool	tagExists(uint16_t cursorid, const char *tag);

	private:
		dictionary<uint16_t, avltree<char *> *>	tags;
};

#endif
