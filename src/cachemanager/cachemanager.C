// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <cachemanager.h>
#include <rudiments/commandline.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <config.h>

dirnode::dirnode(const char *dirname) {
	this->dirname=strdup(dirname);
	next=NULL;
}

dirnode::dirnode(const char *start, const char *end) {
	dirname=new char[end-start+1];
	char	*ptr=(char *)start;
	int	index=0;
	while (ptr<end) {
		dirname[index]=*ptr;
		index++;
		ptr++;
	}
	dirname[index]=(char)NULL;
	next=NULL;
}

dirnode::~dirnode() {
	delete[] dirname;
}


cachemanager::cachemanager(int argc, const char **argv) {

	// read the commandline
	commandline	cmdl(argc,argv);

	// get the scaninterval
	char	*scanint=cmdl.value("-scaninterval");
	if (scanint && scanint[0]) {
		scaninterval=atoi(scanint);
	} else {
		scaninterval=DEFAULT_INTERVAL;
	}

	// get the directories to scan
	char	*cachedirs=cmdl.value("-cachedirs");
	parseCacheDirs(cachedirs);
}

cachemanager::~cachemanager() {

	// delete the list of dirnames
	currentdir=firstdir;
	while (currentdir) {
		firstdir=currentdir;
		currentdir=currentdir->next;
		delete firstdir;
	}
}

void	cachemanager::scan() {

	// detach from the controlling tty
	detach();

	// some useful vars
	DIR	*dir;
	#ifdef HAVE_DIRENT_H
		dirent	*current;
	#else
		direct	*current;
	#endif

	for (;;) {

		// start with the first dir in the list
		currentdir=firstdir;

		while (currentdir) {

			// open directory
			if (dir=opendir(currentdir->dirname)) {

				// loop through directory, erasing
				while (current=readdir(dir)) {
					if (strcmp(current->d_name,".") &&
						strcmp(current->d_name,"..")) {
						erase(currentdir->dirname,
							current->d_name);
					}
				}

				// close the directory
				closedir(dir);
			}

			// move to the next dir in the list
			currentdir=currentdir->next;
		}

		// wait...
		sleep(scaninterval);
	}

}

void	cachemanager::erase(const char *dirname, const char *filename) {

	// derive the full pathname
	char	*fullpathname=new char[strlen(dirname)+1+strlen(filename)+1];
	sprintf(fullpathname,"%s/%s",dirname,filename);

	// open the file
	int	file=open(fullpathname,O_RDONLY);
	if (file>-1) {

		// get the "magic" identifier
		char	magicid[13];
		read(file,(void *)magicid,13);
		if (!strncmp(magicid,"SQLRELAYCACHE",13)) {

			// get the ttl
			long ttl;
			read(file,(void *)&ttl,sizeof(long));
	
			close(file);
	
			// delete the file if the ttl has expired
			if (ttl<time(NULL)) {
				unlink(fullpathname);
			}
		} else {
			close(file);
		}

	}

	// clean up
	delete[] fullpathname;
}

void	cachemanager::parseCacheDirs(const char *cachedirs) {

	if (cachedirs && cachedirs[0]) {

		// parse the colon delimited cachedirs string, 
		// create a new dirnode for each directory
		char	*ptr1=(char *)cachedirs;
		char	*ptr2=(char *)cachedirs;
		firstdir=NULL;
		for (;;) {
			if (*ptr2==':' || !(*ptr2)) {
				if (firstdir) {
					currentdir->next=new 
						dirnode(ptr1,ptr2);
					currentdir=currentdir->next;
				} else {
					firstdir=new dirnode(ptr1,ptr2);
					currentdir=firstdir;
				}
				if (*ptr2==':') {
					ptr1=ptr2+1;
				}
			}
			if (!(*ptr2)) {
				return;
			}
			ptr2++;
		}

	} else {

		// in the event that there is no dirnames 
		// string then use the default CACHE_DIR 
		firstdir=new dirnode(CACHE_DIR);
		currentdir=firstdir;
	}
}
