// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <cachemanager.h>
#include <rudiments/commandline.h>
#include <rudiments/snooze.h>
#include <rudiments/charstring.h>
#include <rudiments/datetime.h>
#include <rudiments/file.h>
#include <rudiments/directory.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

#include <stdio.h>
#include <config.h>

dirnode::dirnode(const char *dirname) {
	this->dirname=charstring::duplicate(dirname);
	next=NULL;
}

dirnode::dirnode(const char *start, const char *end) {
	dirname=charstring::duplicate(start,end-start);
	next=NULL;
}

dirnode::~dirnode() {
	delete[] dirname;
}


cachemanager::cachemanager(int argc, const char **argv) {

	// read the commandline
	commandline	cmdl(argc,argv);

	// get the scaninterval
	const char	*scanint=cmdl.value("-scaninterval");
	if (scanint && scanint[0]) {
		scaninterval=charstring::toInteger(scanint);
	} else {
		scaninterval=DEFAULT_INTERVAL;
	}

	// get the directories to scan
	const char	*cachedirs=cmdl.value("-cachedirs");
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

void cachemanager::scan() {

	// detach from the controlling tty
	detach();

	directory	dir;

	for (;;) {

		// start with the first dir in the list
		currentdir=firstdir;

		while (currentdir) {

			// open directory
			if (dir.open(currentdir->dirname)) {

				// loop through directory, erasing
				uint32_t	index=0;
				for (;;) {
					char	*name=dir.getChildName(index);
					if (!name) {
						break;
					}
					if (charstring::compare(
							name,".") &&
						charstring::compare(
							name,"..")) {
						erase(currentdir->dirname,name);
					}
					delete[] name;
					index++;
				}

				// close the directory
				dir.close();
			}

			// move to the next dir in the list
			currentdir=currentdir->next;
		}

		// wait...
		snooze::macrosnooze(scaninterval);
	}

}

void cachemanager::erase(const char *dirname, const char *filename) {

	// derive the full pathname
	char	*fullpathname=new char[charstring::length(dirname)+1+
					charstring::length(filename)+1];
	sprintf(fullpathname,"%s/%s",dirname,filename);

	// open the file
	file	fl;
	if (fl.open(fullpathname,O_RDONLY)) {

		// get the "magic" identifier
		char	magicid[13];
		fl.read(magicid,13);
		if (!charstring::compare(magicid,"SQLRELAYCACHE",13)) {

			// get the ttl
			int32_t ttl;
			fl.read(&ttl);
	
			fl.close();
	
			// delete the file if the ttl has expired
			datetime	dt;
			dt.getSystemDateAndTime();
			if (ttl<dt.getEpoch()) {
				file::remove(fullpathname);
			}
		} else {
			fl.close();
		}
	}
	delete[] fullpathname;
}

void cachemanager::parseCacheDirs(const char *cachedirs) {

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
