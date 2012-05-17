// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <rudiments/file.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/process.h>
#include <rudiments/environment.h>
#include <sqlrconfigfile.h>

// for clock()
#include <time.h>

// for printf, fflush
#include <stdio.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif


#ifdef HAVE_READLINE
	#include <rudiments/charstring.h>
	// This is an interesting story...
	// readline 2's include files don't list any parameters for any of
	// the functions.  This is fine for C, but not C++, at least not with
	// the compiler I'm using, even with the extern "C" {} bit.
	// This is fixed in readline 4, but, to maintain compatibility with 
	// readline 2, I define the functions myself.
	extern "C" {
		extern char *readline(char *prompt);
		extern void add_history(char *line);
		extern void read_history(char *file);
		extern void write_history(char *file);
		extern void history_truncate_file(char *file, int line);
	}
#endif

class sqlrshbindvalue {
	public:
		union {
			char	*stringval;
			int64_t	integerval;
			struct {
				double		value;
				uint32_t	precision;
				uint32_t	scale;
			} doubleval;
		};
		bindtype	type;
		uint32_t	outputstringbindlength;
};

class sqlrshenv {
	public:
			sqlrshenv();
			~sqlrshenv();
		void	 clearbinds(
			stringdictionary< sqlrshbindvalue * > *binds);
		bool	color;
		bool	headers;
		bool	stats;
		bool	debug;
		bool	final;
		bool	autocommit;
		char	delimiter;
		stringdictionary< sqlrshbindvalue * >	inputbinds;
		stringdictionary< sqlrshbindvalue * >	outputbinds;
};

sqlrshenv::sqlrshenv() {
	color=false;
	headers=true;
	stats=true;
	debug=false;
	final=false;
	autocommit=false;
	delimiter=';';
}

sqlrshenv::~sqlrshenv() {
	clearbinds(&inputbinds);
	clearbinds(&outputbinds);
}

void sqlrshenv::clearbinds(stringdictionary< sqlrshbindvalue * > *binds) {

	for (stringdictionarylistnode< sqlrshbindvalue * > *node=
		(stringdictionarylistnode< sqlrshbindvalue *> *)
		binds->getList()->getFirstNode(); node;
		node=(stringdictionarylistnode< sqlrshbindvalue *> *)
							node->getNext()) {

		delete[] node->getData()->getKey();
		sqlrshbindvalue	*bv=node->getData()->getData();
		if (bv->type==STRING_BIND) {
			delete[] bv->stringval;
		}
		delete node->getData();
	}
	binds->clear();
}

enum querytype_t {
	SHOW_DATABASES_QUERY=0,
	SHOW_TABLES_QUERY,
	SHOW_COLUMNS_QUERY,
	DESCRIBE_QUERY
};

class	sqlrsh {
	public:
#ifndef HAVE_READLINE
			sqlrsh();
#endif
		void	execute(int argc, const char **argv);
	private:
		void	startupMessage(sqlrshenv *env,
					const char *host, uint16_t port,
					const char *user);
		void	systemRcFile(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					sqlrshenv *env);
		void	userRcFile(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					sqlrshenv *env);
		void	runScript(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, sqlrshenv *env, 
					const char *filename, bool returnerror,
					bool displaycommand);
		bool	getCommandFromFile(file *fl,
					stringbuffer *cmdbuffer,
					sqlrshenv *env);
		bool	runCommand(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					sqlrshenv *env, const char *command);
		int	commandType(const char *command);
		void	internalCommand(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, sqlrshenv *env,
					const char *command);
		void	externalCommand(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, sqlrshenv *env, 
					const char *command);
		void	executeQuery(sqlrcursor *sqlrcur, sqlrshenv *env);
		char	*getWild(const char *command);
		char	*getTable(enum querytype_t querytype,
					const char *command);
		void	initStats(sqlrshenv *env);
		void	displayError(sqlrshenv *env,
					const char *message,
					const char *error,
					int64_t errornumber);
		void	displayHeader(sqlrcursor *sqlrcur, sqlrshenv *env);
		void	displayResultSet(sqlrcursor *sqlrcur, sqlrshenv *env);
		void	displayStats(sqlrcursor *sqlrcur, sqlrshenv *env);
		void	ping(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	identify(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	dbversion(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	clientversion(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		void	serverversion(sqlrconnection *sqlrcon,
						sqlrshenv *env);
		bool	lastinsertid(sqlrconnection *sqlrcon, sqlrshenv *env);
		void	inputbind(sqlrcursor *sqlrcur,
						sqlrshenv *env,
						const char *command);
		void	outputbind(sqlrcursor *sqlrcur,
						sqlrshenv *env,
						const char *command);
		void	printbinds(const char *type,
				stringdictionary< sqlrshbindvalue * >
								*binds);
		void	clearbinds(stringdictionary< sqlrshbindvalue * >
								*binds);
		void	displayHelp(sqlrshenv *env);
		void	interactWithUser(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, sqlrshenv *env);
		void	prompt(unsigned long promptcount);
		void	error(const char *errstring);

		void	setColor(sqlrshenv *env, int value);
		void	black(sqlrshenv *env);
		void	red(sqlrshenv *env);
		void	green(sqlrshenv *env);
		void	yellow(sqlrshenv *env);
		void	blue(sqlrshenv *env);
		void	magenta(sqlrshenv *env);
		void	cyan(sqlrshenv *env);
		void	white(sqlrshenv *env);

#ifndef HAVE_READLINE
		filedescriptor	standardin;
#endif
};

#ifndef HAVE_READLINE
sqlrsh::sqlrsh() {
	standardin.setFileDescriptor(0);
	standardin.allowShortReads();
}
#endif

void sqlrsh::systemRcFile(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
						sqlrshenv *env) {
	runScript(sqlrcon,sqlrcur,env,SYSTEM_SQLRSHRC,false,false);
}

void sqlrsh::userRcFile(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
						sqlrshenv *env) {

	// get user's home directory
	const char	*home=environment::getValue("HOME");
	if (!home) {
		home="~";
	}

	// build rcfilename
	size_t	userrcfilelen=charstring::length(home)+10+1;
	char	*userrcfile=new char[userrcfilelen];
	charstring::copy(userrcfile,home);
	charstring::append(userrcfile,"/.sqlrshrc");

	// process the file
	runScript(sqlrcon,sqlrcur,env,userrcfile,false,false);
	delete[] userrcfile;
}

void sqlrsh::runScript(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
			sqlrshenv *env, const char *filename,
			bool returnerror, bool displaycommand) {

	char	*trimmedfilename=charstring::duplicate(filename);
	charstring::bothTrim(trimmedfilename);

	// open the file
	file	scriptfile;
	if (scriptfile.open(trimmedfilename,O_RDONLY)) {

		for (;;) {
		
			// get a command
			stringbuffer	command;
			if (!getCommandFromFile(&scriptfile,&command,env)) {
				break;
			}

			if (displaycommand) {
				cyan(env);
				printf("%s\n",command.getString());
				white(env);
			}

			// run the command
			runCommand(sqlrcon,sqlrcur,env,command.getString());
		}

		// close the file
		scriptfile.close();
	} else {

		// error message
		if (returnerror) {
			stringbuffer	errmesg;
			errmesg.append("Couldn't open file: ");
			errmesg.append(trimmedfilename);
			error(errmesg.getString());
		}
	}

	delete[] trimmedfilename;
}

bool sqlrsh::getCommandFromFile(file *fl, stringbuffer *cmdbuffer,
						sqlrshenv *env) {

	char	character;
	
	for (;;) {

		// get a character from the file
		if (fl->read(&character)!=sizeof(character)) {
			return false;
		}

		// look for an escape character
		if (character=='\\') {
			if (fl->read(&character)!=sizeof(character)) {
				return false;
			}
			cmdbuffer->append(character);
			if (fl->read(&character)!=sizeof(character)) {
				return false;
			}
		}

		// look for an end of command delimiter
		if (character==env->delimiter) {
			return true;
		}

		// write character to buffer and move on
		cmdbuffer->append(character);
	}
		
}

bool sqlrsh::runCommand(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
					sqlrshenv *env, const char *command) {

	int	cmdtype=commandType(command);

	if (cmdtype>0) {
		// if the command an internal command, run it as one
		internalCommand(sqlrcon,sqlrcur,env,command);
		return true;
	} else if (cmdtype==0) {
		// if the command is not an internal command, 
		// execute it as a query and display the result set
		externalCommand(sqlrcon,sqlrcur,env,command);
		return true;
	} else {
		return false;
	}
}

int sqlrsh::commandType(const char *command) {

	// skip white space
	char	*ptr=(char *)command;
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// compare to known internal commands
	if (!charstring::compareIgnoringCase(ptr,"color",5) ||
		!charstring::compareIgnoringCase(ptr,"headers",7) ||
		!charstring::compareIgnoringCase(ptr,"stats",5) ||
		!charstring::compareIgnoringCase(ptr,"debug",5) ||
		!charstring::compareIgnoringCase(ptr,"autocommit",10) ||
		!charstring::compareIgnoringCase(ptr,"final",5) ||
		!charstring::compareIgnoringCase(ptr,"help",4) ||
		!charstring::compareIgnoringCase(ptr,"ping",4) ||
		!charstring::compareIgnoringCase(ptr,"identify",8) ||
		!charstring::compareIgnoringCase(ptr,"dbversion",9) ||
		!charstring::compareIgnoringCase(ptr,"clientversion",13) ||
		!charstring::compareIgnoringCase(ptr,"serverversion",13) ||
		!charstring::compareIgnoringCase(ptr,"use ",4) ||
		!charstring::compareIgnoringCase(ptr,"currentdb",9) ||
		!charstring::compareIgnoringCase(ptr,"run",3) ||
		!charstring::compareIgnoringCase(ptr,"@",1) ||
		!charstring::compareIgnoringCase(ptr,"delimiter",9) ||
		!charstring::compareIgnoringCase(ptr,"delimeter",9) ||
		!charstring::compareIgnoringCase(ptr,"inputbind ",10) ||
		!charstring::compareIgnoringCase(ptr,"outputbind ",11) ||
		!charstring::compareIgnoringCase(ptr,"printinputbind",14) ||
		!charstring::compareIgnoringCase(ptr,"printoutputbind",15) ||
		!charstring::compareIgnoringCase(ptr,"printbinds",10) ||
		!charstring::compareIgnoringCase(ptr,"clearinputbind",14) ||
		!charstring::compareIgnoringCase(ptr,"clearoutputbind",15) ||
		!charstring::compareIgnoringCase(ptr,"clearbinds",10) ||
		!charstring::compareIgnoringCase(ptr,"lastinsertid",12)) {

		// return value of 1 is internal command
		return 1;
	}

	// look for an exit command
	if (!charstring::compareIgnoringCase(ptr,"quit",4) ||
		!charstring::compareIgnoringCase(ptr,"exit",4)) {
		return -1;
	}

	// return value of 0 is external command
	return 0;
}

void sqlrsh::internalCommand(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur,
					sqlrshenv *env, const char *command) {

	// skip white space
	char	*ptr=(char *)command;
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// compare to known internal commands
	int	cmdtype=0;
	if (!charstring::compareIgnoringCase(ptr,"color",5)) {
		ptr=ptr+5;
		cmdtype=1;
	} else if (!charstring::compareIgnoringCase(ptr,"headers",7)) {
		ptr=ptr+7;
		cmdtype=2;
	} else if (!charstring::compareIgnoringCase(ptr,"stats",5)) {	
		ptr=ptr+5;
		cmdtype=3;
	} else if (!charstring::compareIgnoringCase(ptr,"debug",5)) {	
		ptr=ptr+5;
		cmdtype=4;
	} else if (!charstring::compareIgnoringCase(ptr,"autocommit",10)) {	
		ptr=ptr+10;
		cmdtype=8;
	} else if (!charstring::compareIgnoringCase(ptr,"final",5)) {	
		ptr=ptr+5;
		cmdtype=5;
	} else if (!charstring::compareIgnoringCase(ptr,"help",4)) {	
		displayHelp(env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"ping",4)) {	
		ping(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"use ",4)) {	
		if (!sqlrcon->selectDatabase(ptr+4)) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
		}
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"currentdb",9)) {	
		printf("%s\n",sqlrcon->getCurrentDatabase());
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"run",3)) {	
		ptr=ptr+3;
		cmdtype=6;
	} else if (!charstring::compareIgnoringCase(ptr,"@",1)) {	
		ptr=ptr+1;
		cmdtype=6;
	} else if (!charstring::compareIgnoringCase(ptr,"delimiter",9) ||
			!charstring::compareIgnoringCase(ptr,"delimeter",9)) {	
		ptr=ptr+9;
		cmdtype=7;
	} else if (!charstring::compareIgnoringCase(ptr,"identify",8)) {	
		identify(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"dbversion",9)) {	
		dbversion(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"clientversion",13)) {	
		clientversion(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"serverversion",13)) {	
		serverversion(sqlrcon,env);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"inputbind ",10)) {	
		inputbind(sqlrcur,env,command);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"outputbind ",11)) {	
		outputbind(sqlrcur,env,command);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"printbinds",10)) {	
		printbinds("Input",&env->inputbinds);
		printf("\n");
		printbinds("Output",&env->outputbinds);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"clearinputbind",14)) {	
		env->clearbinds(&env->inputbinds);
		return;
	} else if (!charstring::compareIgnoringCase(
					ptr,"clearoutputbind",15)) {	
		env->clearbinds(&env->outputbinds);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"clearbinds",10)) {	
		env->clearbinds(&env->inputbinds);
		env->clearbinds(&env->outputbinds);
		return;
	} else if (!charstring::compareIgnoringCase(ptr,"lastinsertid",12)) {	
		if (!lastinsertid(sqlrcon,env)) {
			displayError(env,NULL,
					sqlrcon->errorMessage(),
					sqlrcon->errorNumber());
		}
		return;
	} else {
		return;
	}

	// skip white space
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// handle scripts
	if (cmdtype==6) {
		runScript(sqlrcon,sqlrcur,env,ptr,true,false);
		return;
	}

	// on or off?
	bool	toggle=false;
	if (!charstring::compareIgnoringCase(ptr,"on",2)) {
		toggle=true;
	}

	// set parameter
	if (cmdtype==1) {
		env->color=toggle;
	} else if (cmdtype==2) {
		env->headers=toggle;
	} else if (cmdtype==3) {
		env->stats=toggle;
	} else if (cmdtype==4) {
		env->debug=toggle;
	} else if (cmdtype==5) {
		env->final=toggle;
	} else if (cmdtype==7) {
		env->delimiter=ptr[0];
		cyan(env);
		printf("Delimiter set to %c\n",env->delimiter);
		white(env);
	} else if (cmdtype==8) {
		cyan(env);
		printf("Autocommit set ");
		if (toggle) {
			sqlrcon->autoCommitOn();
			printf("on\n");
		} else {
			sqlrcon->autoCommitOff();
			printf("off\n");
		}
		white(env);
	}
}

void sqlrsh::externalCommand(sqlrconnection *sqlrcon,
				sqlrcursor *sqlrcur, sqlrshenv *env, 
				const char *command) {

	// init stats
	initStats(env);

	// handle debug
	if (env->debug) {
		sqlrcon->debugOn();
	}

	// handle begin, commit and rollback
	if (!charstring::compareIgnoringCase(command,"begin",5)) {

		sqlrcon->begin();

	} else if (!charstring::compareIgnoringCase(command,"commit",6)) {

		sqlrcon->commit();

	} else if (!charstring::compareIgnoringCase(command,"rollback",8)) {

		sqlrcon->rollback();

	} else {

		sqlrcur->setResultSetBufferSize(100);

		// send the query
		if (!charstring::compareIgnoringCase(command,
							"show databases",14)) {
			char	*wild=getWild(command);
			sqlrcur->getDatabaseList(wild);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"show tables",11)) {
			char	*wild=getWild(command);
			sqlrcur->getTableList(wild);
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"show columns",12)) {
			char	*table=getTable(SHOW_COLUMNS_QUERY,command);
			char	*wild=getWild(command);
			sqlrcur->getColumnList(table,wild);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"describe ",9)) {
			char	*table=getTable(DESCRIBE_QUERY,command);
			char	*wild=getWild(command);
			sqlrcur->getColumnList(table,wild);
			delete[] table;
			delete[] wild;
		} else if (!charstring::compareIgnoringCase(command,
							"reexecute",9)) {	
			executeQuery(sqlrcur,env);
		} else {
			sqlrcur->prepareQuery(command);
			executeQuery(sqlrcur,env);
		}

		// look for an error
		if (sqlrcur->errorMessage()) {

			// display the error
			displayError(env,NULL,
					sqlrcur->errorMessage(),
					sqlrcur->errorNumber());

		} else {

			// display the header
			displayHeader(sqlrcur,env);

			// display the result set
			displayResultSet(sqlrcur,env);
		}

		if (env->final) {
			sqlrcon->endSession();
		}
	}

	// set debug back off
	sqlrcon->debugOff();

	// display statistics
	displayStats(sqlrcur,env);
}

void sqlrsh::executeQuery(sqlrcursor *sqlrcur, sqlrshenv *env) {

	sqlrcur->clearBinds();

	if (env->inputbinds.getList()->getLength()) {

		for (stringdictionarylistnode< sqlrshbindvalue * > *node=
			(stringdictionarylistnode< sqlrshbindvalue *> *)
			env->inputbinds.getList()->getFirstNode(); node;
			node=(stringdictionarylistnode< sqlrshbindvalue *> *)
							node->getNext()) {

			const char	*name=node->getData()->getKey();
			sqlrshbindvalue	*bv=node->getData()->getData();
			if (bv->type==STRING_BIND) {
				sqlrcur->inputBind(name,bv->stringval);
			} else if (bv->type==INTEGER_BIND) {
				sqlrcur->inputBind(name,bv->integerval);
			} else if (bv->type==DOUBLE_BIND) {
				sqlrcur->inputBind(name,bv->doubleval.value,
							bv->doubleval.precision,
							bv->doubleval.scale);
			} else if (bv->type==NULL_BIND) {
				sqlrcur->inputBind(name,(const char *)NULL);
			}
		}
	}

	if (env->outputbinds.getList()->getLength()) {

		for (stringdictionarylistnode< sqlrshbindvalue * > *node=
			(stringdictionarylistnode< sqlrshbindvalue *> *)
			env->outputbinds.getList()->getFirstNode(); node;
			node=(stringdictionarylistnode< sqlrshbindvalue *> *)
							node->getNext()) {

			const char	*name=node->getData()->getKey();
			sqlrshbindvalue	*bv=node->getData()->getData();
			if (bv->type==STRING_BIND) {
				// FIXME: make buffer length variable
				sqlrcur->defineOutputBindString(name,
						bv->outputstringbindlength);
			} else if (bv->type==INTEGER_BIND) {
				sqlrcur->defineOutputBindInteger(name);
			} else if (bv->type==DOUBLE_BIND) {
				sqlrcur->defineOutputBindDouble(name);
			}
		}
	}

	sqlrcur->executeQuery();

	if (env->outputbinds.getList()->getLength()) {

		for (stringdictionarylistnode< sqlrshbindvalue * > *node=
			(stringdictionarylistnode< sqlrshbindvalue *> *)
			env->outputbinds.getList()->getFirstNode(); node;
			node=(stringdictionarylistnode< sqlrshbindvalue *> *)
							node->getNext()) {

			const char	*name=node->getData()->getKey();
			sqlrshbindvalue	*bv=node->getData()->getData();
			if (bv->type==STRING_BIND) {
				delete[] bv->stringval;
				bv->stringval=charstring::duplicate(
					sqlrcur->getOutputBindString(name));
			} else if (bv->type==INTEGER_BIND) {
				bv->integerval=
					sqlrcur->getOutputBindInteger(name);
			} else if (bv->type==DOUBLE_BIND) {
				bv->doubleval.value=
					sqlrcur->getOutputBindDouble(name);
			}
		}
	}
}

char *sqlrsh::getWild(const char *command) {
	const char	*wildptr=charstring::findFirst(command,"'");
	if (!wildptr) {
		return NULL;
	}
	wildptr++;
	const char	*endptr=charstring::findLast(wildptr,"'");
	if (!endptr) {
		return NULL;
	}
	return charstring::duplicate(wildptr,endptr-wildptr);
}

char *sqlrsh::getTable(querytype_t querytype, const char *command) {
	const char	*tableptr=NULL;
	if (querytype==SHOW_COLUMNS_QUERY) {
		tableptr=charstring::findFirst(command," in ");
		if (!tableptr) {
			return NULL;
		}
		tableptr=tableptr+4;
		const char	*endptr=charstring::findFirst(tableptr," ");
		if (!endptr) {
			return charstring::duplicate(tableptr);
		}
		return charstring::duplicate(tableptr,endptr-tableptr);
	} else if (querytype==DESCRIBE_QUERY) {
		tableptr=charstring::findFirst(command," ");
		if (!tableptr) {
			return NULL;
		}
		return charstring::duplicate(tableptr+1);
	}
	return NULL;
}

void sqlrsh::initStats(sqlrshenv *env) {

	if (!env->stats) {
		return;
	}

	// call clock here or something
	clock();
}

void sqlrsh::displayError(sqlrshenv *env,
				const char *message,
				const char *error,
				int64_t errornumber) {
	cyan(env);
	if (charstring::length(message)) {
		printf("%s\n",message);
	}
	printf("%lld:\n",errornumber);
	if (charstring::length(error)) {
		printf("%s\n\n",error);
	}
	white(env);
}

void sqlrsh::displayHeader(sqlrcursor *sqlrcur, sqlrshenv *env) {

	if (!env->headers) {
		return;
	}

	// display column names
	uint32_t	charcount=0;
	uint32_t	colcount=sqlrcur->colCount();
	const char	*name;
	uint32_t	namelen;
	uint32_t	longest;

	if (!colcount) {
		return;
	}

	// iterate through columns
	for (uint32_t i=0; i<sqlrcur->colCount(); i++) {

		// write the column name
		if (i%2==1) {
			green(env);
		} else {
			yellow(env);
		}
		name=sqlrcur->getColumnName(i);
		printf("%s",name);
		white(env);

		// which is longer, field name or longest field
		namelen=charstring::length(name);
		longest=sqlrcur->getLongest(i);
		if (namelen>longest) {
			longest=namelen;
		}
		charcount=charcount+longest;

		// pad after the name with spaces
		for (uint32_t j=namelen; j<longest; j++) {
			printf(" ");
		}

		// put an extra space between names
		if (i<colcount-1) {
			printf(" ");
			charcount=charcount+1;
		}
	}
	printf("\n");

	// display delimiter
	red(env);
	for (uint32_t i=0; i<charcount; i++) {
		printf("=");
	}
	printf("\n");
	white(env);
}

void sqlrsh::displayResultSet(sqlrcursor *sqlrcur, sqlrshenv *env) {

	// display column names
	uint32_t	colcount=sqlrcur->colCount();
	uint32_t	namelen;
	uint32_t	longest;

	if (!colcount) {
		return;
	}

	uint32_t	i=0;
	const char	*field="";
	while (field && colcount) {
		for (uint32_t j=0; j<colcount; j++) {

			if (!(field=sqlrcur->getField(i,j))) {
				break;
			}

			// write the column value
			if (i%2==1) {
				cyan(env);
			} else {
				white(env);
			}
			printf("%s",field);
			white(env);

			// which is longer, field name or longest field
			longest=sqlrcur->getLongest(j);
			if (env->headers) {
				namelen=charstring::length(
					sqlrcur->getColumnName(j));
				if (namelen>longest) {
					longest=namelen;
				}
			}

			// pad after the name with spaces
			for (uint32_t k=sqlrcur->getFieldLength(i,j); 
							k<longest; k++) {
				printf(" ");
			}

			// put an extra space between names
			if (j<colcount-1) {
				printf(" ");
			}
		}
		if (field) {
			printf("\n");
		}
		i++;
	}
}

void sqlrsh::displayStats(sqlrcursor *sqlrcur, sqlrshenv *env) {

	if (!env->stats) {
		return;
	}

	// call clock again, display results
	red(env);
	printf("	Rows Returned   : ");
	magenta(env);
	printf("%lld\n",sqlrcur->rowCount());
	red(env);
	printf("	Fields Returned : ");
	magenta(env);
	printf("%lld\n",sqlrcur->rowCount()*sqlrcur->colCount());
	red(env);
	printf("	System time     : ");
	magenta(env);
	printf("%ld\n",clock());
	white(env);
	printf("\n");
}

void sqlrsh::ping(sqlrconnection *sqlrcon, sqlrshenv *env) {
	red(env);
	printf((sqlrcon->ping())?"	The database is up.\n":
				"	The database is down.\n");
	white(env);
}

bool sqlrsh::lastinsertid(sqlrconnection *sqlrcon, sqlrshenv *env) {
	red(env);
	bool		retval=false;
	uint64_t	id=sqlrcon->getLastInsertId();
	if (id!=0 || !sqlrcon->errorMessage()) {
		printf("%lld\n",id);
		retval=true;
	}
	white(env);
	return retval;
}

void sqlrsh::identify(sqlrconnection *sqlrcon, sqlrshenv *env) {
	red(env);
	printf("%s\n",sqlrcon->identify());
	white(env);
}

void sqlrsh::dbversion(sqlrconnection *sqlrcon, sqlrshenv *env) {
	red(env);
	printf("%s\n",sqlrcon->dbVersion());
	white(env);
}

void sqlrsh::clientversion(sqlrconnection *sqlrcon, sqlrshenv *env) {
	red(env);
	printf("%s\n",sqlrcon->clientVersion());
	white(env);
}

void sqlrsh::serverversion(sqlrconnection *sqlrcon, sqlrshenv *env) {
	red(env);
	printf("%s\n",sqlrcon->serverVersion());
	white(env);
}

void sqlrsh::inputbind(sqlrcursor *sqlrcur,
				sqlrshenv *env, const char *command) {

	// sanity check
	const char	*ptr=command+10;
	const char	*space=charstring::findFirst(ptr,' ');
	if (!space) {
		printf("usage: inputbind [variable] = [value]\n");
		return;
	}

	// get the variable name
	char	*variable=charstring::duplicate(ptr,space-ptr);

	// move on
	ptr=space;
	if (*(ptr+1)=='=' && *(ptr+2)==' ') {
		ptr=ptr+3;
	} else {
		printf("usage: inputbind [variable] = [value]\n");
		return;
	}
		
	// get the value
	char	*value=charstring::duplicate(ptr);
	charstring::bothTrim(value);
	size_t	valuelen=charstring::length(value);

	// if the bind variable is already defined, clear it...
	sqlrshbindvalue	*bv=NULL;
	if (env->inputbinds.getData(variable,&bv)) {
		delete[] bv;
	}

	// define the variable
	bv=new sqlrshbindvalue;

	// first handle nulls, then...
	// anything enclosed in quotes is a string
	// if it's unquoted, check to see if it's an integer or float
	// if it's not, then it's a string
	if (!value) {
		bv->type=NULL_BIND;
	} else if ((value[0]=='\'' && value[valuelen-1]=='\'') ||
			(value[0]=='"' && value[valuelen-1]=='"')) {

		bv->type=STRING_BIND;

		// trim off quotes
		char	*newvalue=charstring::duplicate(value+1);
		newvalue[valuelen-2]='\0';
		delete[] value;

		// unescape the string
		bv->stringval=charstring::unescape(newvalue);
		delete[] newvalue;

	} else if (charstring::isInteger(value)) {
		bv->type=INTEGER_BIND;
		bv->integerval=charstring::toInteger(value);
		delete[] value;
	} else if (charstring::isNumber(value)) {
		bv->type=DOUBLE_BIND;
		bv->doubleval.value=charstring::toFloat(value);
		bv->doubleval.precision=valuelen-((value[0]=='-')?2:1);
		bv->doubleval.scale=
			charstring::findFirst(value,'.')-value+
			((value[0]=='-')?0:1);
		delete[] value;
	} else {
		bv->type=STRING_BIND;
		bv->stringval=value;
	}

	// put the bind variable in the list
	env->inputbinds.setData(variable,bv);
}

void sqlrsh::outputbind(sqlrcursor *sqlrcur,
				sqlrshenv *env, const char *command) {

	// split the command on ' '
	char		**parts;
	uint64_t	partcount;
	charstring::split(command," ",true,&parts,&partcount);

	// sanity check...
	bool	sane=true;
	if (partcount>2 && !charstring::compare(parts[0],"outputbind")) {

		// if the bind variable is already defined, clear it...
		sqlrshbindvalue	*bv=NULL;
		if (env->outputbinds.getData(parts[1],&bv)) {
			delete[] bv;
		}

		// define the variable
		bv=new sqlrshbindvalue;

		if (!charstring::compareIgnoringCase(
						parts[2],"string") &&
						partcount==4) {
			bv->type=STRING_BIND;
			bv->stringval=NULL;
			bv->outputstringbindlength=
				charstring::toInteger(parts[3]);
		} else if (!charstring::compareIgnoringCase(
						parts[2],"integer") &&
						partcount==3) {
			bv->type=INTEGER_BIND;
			bv->integerval=0;
		} else if (!charstring::compareIgnoringCase(
						parts[2],"double") &&
						partcount==5) {
			bv->type=DOUBLE_BIND;
			bv->doubleval.value=0.0;
			bv->doubleval.precision=
				charstring::toInteger(parts[3]);
			bv->doubleval.scale=
				charstring::toInteger(parts[4]);
		} else {
			sane=false;
		}

		// put the bind variable in the list
		if (sane) {
			env->outputbinds.setData(parts[1],bv);
		}

	} else {
		sane=false;
	}

	// clean up
	if (sane) {
		delete[] parts[0];
	} else {
		printf("usage: outputbind [variable] [type] [length] [scale]\n");
		for (uint64_t i=0; i<partcount; i++) {
			delete[] parts[i];
		}
	}
	delete[] parts;
}

void sqlrsh::printbinds(const char *type,
			stringdictionary< sqlrshbindvalue * > *binds) {

	printf("%s bind variables:\n",type);

	for (stringdictionarylistnode< sqlrshbindvalue * > *node=
		(stringdictionarylistnode< sqlrshbindvalue *> *)
		binds->getList()->getFirstNode();
		node;
		node=(stringdictionarylistnode< sqlrshbindvalue *> *)
						node->getNext()) {

		printf("    %s ",node->getData()->getKey());
		sqlrshbindvalue	*bv=node->getData()->getData();
		if (bv->type==STRING_BIND) {
			printf("(STRING) = %s\n",bv->stringval);
		} else if (bv->type==INTEGER_BIND) {
			printf("(INTEGER) = %lld\n",bv->integerval);
		} else if (bv->type==DOUBLE_BIND) {
			printf("(DOUBLE %d,%d) = %*.*f\n",
						bv->doubleval.precision,
						bv->doubleval.scale,
						bv->doubleval.precision,
						bv->doubleval.scale,
						bv->doubleval.value);
		} else if (bv->type==NULL_BIND) {
			printf("NULL\n");
		}
	}
}

void sqlrsh::displayHelp(sqlrshenv *env) {

	printf("\n");
	yellow(env);
	printf("	To run a query, simply type it at the prompt,\n"
		"	followed by a semicolon.  Queries may be \n"
		"	split over multiple lines.\n\n");
	cyan(env);
	printf("	ping			- ");
	green(env);
	printf("pings the database\n");
	cyan(env);
	printf("	identify		- ");
	green(env);
	printf("returns the type of database\n");
	cyan(env);
	printf("	dbversion		- ");
	green(env);
	printf("returns the version of the database\n");
	cyan(env);
	printf("	clientversion		- ");
	green(env);
	printf("returns the version of the SQL Relay\n");
	printf("\t\t\t\t  client library\n");
	cyan(env);
	printf("	serverversion		- ");
	green(env);
	printf("returns the version of the SQL Relay server\n");
	cyan(env);
	printf("	use [database]		- ");
	green(env);
	printf("change the current database/schema\n");
	cyan(env);
	printf("	currentdb		- ");
	green(env);
	printf("shows the current database/schema\n");
	cyan(env);
	printf("	run script		- ");
	green(env);
	printf("runs commands contained in file \"script\"\n");
	cyan(env);
	printf("	color on/off		- ");
	green(env);
	printf("toggles colorizing\n");
	cyan(env);
	printf("	headers on/off		- ");
	green(env);
	printf("toggles column descriptions before result set\n");
	cyan(env);
	printf("	stats on/off		- ");
	green(env);
	printf("toggles statistics after result set\n");
	cyan(env);
	printf("	debug on/off		- ");
	green(env);
	printf("toggles debug messages\n");
	cyan(env);
	printf("	autocommit on/off	- ");
	green(env);
	printf("toggles autocommit\n");
	cyan(env);
	printf("	final on/off		- ");
	green(env);
	printf("toggles use of one session per query\n");
	cyan(env);
	printf("	delimiter [character]	- ");
	green(env);
	printf("sets delimiter character to [character]\n\n");
	cyan(env);
	printf("	inputbind [variable] = [value] - ");
	green(env);
	printf("defines an input bind variable\n");
	cyan(env);
	printf("	outputbind ...                 - ");
	green(env);
	printf("defines an output bind variable\n");
	cyan(env);
	printf("		outputbind [variable] string [length]\n");
	printf("		outputbind [variable] integer\n");
	printf("		outputbind [variable] double [precision] [scale}\n");
	printf("	printbinds                     - ");
	green(env);
	printf("prints all bind variables\n");
	cyan(env);
	printf("	clearinputbind [variable]      - ");
	green(env);
	printf("clears an input bind variable\n");
	cyan(env);
	printf("	clearoutputbind [variable]     - ");
	green(env);
	printf("clears an output bind variable\n");
	cyan(env);
	printf("	clearbinds                     - ");
	green(env);
	printf("clears all bind variables\n");
	cyan(env);
	printf("	reexecute                      - ");
	green(env);
	printf("reexecutes the previous query\n\n");
	cyan(env);
	printf("	lastinsertid                   - ");
	green(env);
	printf("returns the value of the most recently\n");
	printf("\t\t\t\t\t updated auto-increment or identity\n");
	printf("\t\t\t\t\t column, if the database supports it\n\n");
	cyan(env);
	printf("	show databases [like pattern]		-\n");
	green(env);
	printf("		returns a list of known databases/schemas\n");
	cyan(env);
	printf("	show tables [like pattern]		-\n");
	green(env);
	printf("		returns a list of known tables\n");
	cyan(env);
	printf("	show columns in table [like pattern]	-\n");
	green(env);
	printf("		returns a list of columns in the table \"table\"\n");
	cyan(env);
	printf("	describe table				-\n");
	green(env);
	printf("		returns a list of columns in the table \"table\"\n\n");
	cyan(env);
	printf("	exit/quit		- ");
	green(env);
	printf("exits\n\n");
	yellow(env);
	printf("	All commands must be followed by the delimiter: %c\n",
								env->delimiter);
	white(env);
}

void sqlrsh::startupMessage(sqlrshenv *env, const char *host,
					uint16_t port, const char *user) {

	red(env);
	printf("SQLRShell - ");
	green(env);
	printf("Version 0.22\n");
	yellow(env);
	printf("	Connected to: ");
	blue(env);
	printf("%s:%d as %s\n\n",host,port,user);
	yellow(env);
	printf("	type help; for a help.\n\n");
	white(env);
}

void sqlrsh::interactWithUser(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
							sqlrshenv *env) {

	// init some variables
	stringbuffer	command;
	stringbuffer	prmpt;
	int		exitprogram=0;
	uint32_t	promptcount;

	while (!exitprogram) {

		// prompt the user
		promptcount=0;
		
		// get the command
		bool	done=false;
		while (!done) {
			#ifdef HAVE_READLINE
				prmpt.append(promptcount);
				prmpt.append("> ");
				char	*cmd=readline(const_cast<char *>(
							prmpt.getString()));
				prmpt.clear();
				if (cmd && cmd[0]) {
					charstring::rightTrim(cmd);
					add_history(cmd);
				} else {
					printf("\n");
				}
			#else
				prompt(promptcount);
				char	cmd[1024];
				ssize_t	bytes=standardin.read(cmd,1024);
				cmd[bytes-1]='\0';
			#endif
			size_t	len=charstring::length(cmd);
			done=false;
			for (size_t i=0; i<len; i++) {
				if (i==len-1) {
				       if (cmd[i]==env->delimiter) {
						done=true;
					} else {
						command.append(cmd[i]);
					}
				} else if (cmd[i]>=32 || 
						cmd[i]=='	') {
					command.append(cmd[i]);
				}
			}
			if (!done) {
				promptcount++;
				command.append(" ");
			}
			#ifdef HAVE_READLINE
				delete[] cmd;
			#endif
		}

		// run the command
		if (!runCommand(sqlrcon,sqlrcur,env,command.getString())) {	
			exitprogram=1;
		}

		command.clear();
	}
}

void sqlrsh::prompt(unsigned long promptcount) {

	printf("%ld> ",promptcount);
	fflush(stdout);
}

void sqlrsh::error(const char *errstring) {

	// print the error
	printf("%s\n\n",errstring);
}

void sqlrsh::execute(int argc, const char **argv) {


	commandline	cmdline(argc,argv);
	sqlrconfigfile	cfgfile;
	usercontainer	*currentnode=NULL;
	const char	*host;
	uint16_t	port;
	const char	*socket;
	const char	*user;
	const char	*password;
	const char	*script=NULL;

	const char	*config=cmdline.getValue("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	const char	*id=cmdline.getValue("-id");

	if (!(id && id[0])) {

		if (argc<6) {
			printf("usage: sqlrsh  host port socket "
				"user password [script]\n"
				"  or   sqlrsh  [-config configfile] "
				"-id id [script]\n");
			process::exit(1);
		}

		host=argv[1];
		port=charstring::toInteger(argv[2]);
		socket=argv[3];
		user=argv[4];
		password=argv[5];
		if (argv[6]) {
			script=argv[6];
		}

	} else {

		if (cfgfile.parse(config,id)) {

			// get the host/port/socket/username/password
			host="localhost";
			port=cfgfile.getPort();
			socket=cfgfile.getUnixPort();
			// FIXME: this can return 0
			cfgfile.getUserList()->getDataByIndex(0,&currentnode);
			user=currentnode->getUser();
			password=currentnode->getPassword();
		} else {
			return;
		}

		// find the script if there is one
		for (int i=1; i<argc; i++) {
			if (argv[i][0]=='-') {
				i++;
				continue;
			}
			script=(char *)argv[i];
			break;
		}
	}

	// connect to sql relay
	sqlrconnection	sqlrcon(host,port,socket,user,password,0,1);
	sqlrcursor	sqlrcur(&sqlrcon);

	// set up an sqlrshenv
	sqlrshenv	env;

	// process RC files
	systemRcFile(&sqlrcon,&sqlrcur,&env);
	userRcFile(&sqlrcon,&sqlrcur,&env);


	#ifdef HAVE_READLINE

		// handle the history file
		size_t		filenamelen;
		char		*filename;
		const char	*home=environment::getValue("HOME");
		if (home && home[0]) {
			filenamelen=charstring::length(home)+16+1;
			filename=new char[filenamelen];
			charstring::copy(filename,home);
			charstring::append(filename,"/.sqlrsh_history");

			// create the history file if it doesn't exist now
			FILE	*historyfile=fopen(filename,"a");
			if (historyfile) {
				fclose(historyfile);
				read_history(filename);
			}
		}
	#endif

	// if a script was specified, run it otherwise go into interactive mode
	if (script) {
		runScript(&sqlrcon,&sqlrcur,&env,script,true,false);
	} else {
		startupMessage(&env,host,port,user);
		interactWithUser(&sqlrcon,&sqlrcur,&env);
	}

	// clean up
	#ifdef HAVE_READLINE
		if (home && home[0]) {
			write_history(filename);
			history_truncate_file(filename,100);
			delete[] filename;
		}
	#endif
}

void sqlrsh::setColor(sqlrshenv *env, int value) {
	if (env->color) {
		printf("\033[0;%dm",value);
	}
}

void sqlrsh::black(sqlrshenv *env) {
	setColor(env,30);
}

void sqlrsh::red(sqlrshenv *env) {
	setColor(env,31);
}

void sqlrsh::green(sqlrshenv *env) {
	setColor(env,32);
}

void sqlrsh::yellow(sqlrshenv *env) {
	setColor(env,33);
}

void sqlrsh::blue(sqlrshenv *env) {
	setColor(env,34);
}

void sqlrsh::magenta(sqlrshenv *env) {
	setColor(env,35);
}

void sqlrsh::cyan(sqlrshenv *env) {
	setColor(env,36);
}

void sqlrsh::white(sqlrshenv *env) {
	setColor(env,37);
}



int main(int argc, const char **argv) {

	#include <version.h>

	sqlrsh	s;
	s.execute(argc,argv);
	process::exit(0);
}
