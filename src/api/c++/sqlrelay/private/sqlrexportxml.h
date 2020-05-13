// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		friend class sqlrexport;
		bool	exportToFileDescriptor(filedescriptor *fd,
						const char *filename,
						const char *table);
		void	escapeField(filedescriptor *fd,
						const char *field,
						uint32_t length);
