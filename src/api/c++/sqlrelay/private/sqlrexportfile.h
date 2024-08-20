// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

		virtual void	clearFlagsAndCounts();

		virtual bool	sanityCheck();
		virtual bool	flush();

	private:

		const char		*filename;
		file			f;
		filedescriptor		*fd;
