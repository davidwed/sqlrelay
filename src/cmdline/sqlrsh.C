// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <defaults.h>

#include <sqlrelay/sqlrclient.h>
#include <rudiments/commandline.h>
#include <sqlrconfigfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif
#include <time.h>



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

class	environment {
	public:
			environment();
		int	color;
		int	headers;
		int	stats;
		int	debug;
		int	final;
};

environment::environment() {
	color=0;
	headers=1;
	stats=1;
	debug=0;
	final=0;
}

class	sqlrsh {
	public:
		void	execute(int argc, const char **argv);
	private:
		void	startupMessage(environment *env,
					const char *host, int port,
					const char *user);
		void	systemRcFile(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					environment *env);
		void	userRcFile(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					environment *env);
		void	runScript(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, environment *env, 
					const char *file, int returnerror);
		int	getCommandFromFile(int file, stringbuffer *cmdbuffer);
		int	runCommand(sqlrconnection *sqlrcon, 
					sqlrcursor *sqlrcur, 
					environment *env, const char *command);
		int	commandType(const char *command);
		void	internalCommand(environment *env, const char *command);
		void	externalCommand(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, environment *env, 
					const char *command);
		void	initStats(environment *env);
		void	displayError(sqlrcursor *sqlrcur, environment *env);
		void	displayHeader(sqlrcursor *sqlrcur, environment *env);
		void	displayResultSet(sqlrcursor *sqlrcur, environment *env);
		void	displayStats(sqlrcursor *sqlrcur, environment *env);
		void	displayHelp(environment *env);
		void	interactWithUser(sqlrconnection *sqlrcon,
					sqlrcursor *sqlrcur, environment *env);
		void	prompt(int promptcount);
		void	error(const char *errstring);
		void	addLineToCommand(stringbuffer *command,
						stringbuffer *line,
						int final);

		void	setColor(environment *env, int value);
		void	black(environment *env);
		void	red(environment *env);
		void	green(environment *env);
		void	yellow(environment *env);
		void	blue(environment *env);
		void	magenta(environment *env);
		void	cyan(environment *env);
		void	white(environment *env);
};

void sqlrsh::systemRcFile(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
						environment *env) {
	runScript(sqlrcon,sqlrcur,env,SYSTEM_SQLRSHRC,0);
}

void sqlrsh::userRcFile(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
						environment *env) {

	// get user's home directory
	char	*home=getenv("HOME");
	if (!home) {
		home="~";
	}

	// build rcfilename
	char	*userrcfile=new char[strlen(home)+10+1];
	sprintf(userrcfile,"%s/.sqlrshrc",home);

	// process the file
	runScript(sqlrcon,sqlrcur,env,userrcfile,0);

	// clean up
	delete[] userrcfile;
}

void sqlrsh::runScript(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
			environment *env, const char *file, int returnerror) {

	// open the file
	int scriptfile=open(file,O_RDONLY);
	if (scriptfile>-1) {

		while (1) {
		
			// get a command
			stringbuffer	command;
			if (!getCommandFromFile(scriptfile,&command)) {
				break;
			}

			// run the command
			runCommand(sqlrcon,sqlrcur,env,command.getString());
		}

		// close the file
		close(scriptfile);
	} else {

		// error message
		if (returnerror) {
			stringbuffer	errmesg;
			errmesg.append("Couldn't open file: ")->append(file);
			error(errmesg.getString());
		}
	}
}

int sqlrsh::getCommandFromFile(int file, stringbuffer *cmdbuffer) {

	char	character;
	
	while (1) {

		// get a character from the file
		read(file,(void *)&character,sizeof(char));

		// look for end of file
		if (character==-1) {
			return 0;
		}

		// look for an escape character
		if (character=='\\') {
			read(file,(void *)&character,sizeof(char));
			cmdbuffer->append(character);
			read(file,(void *)&character,sizeof(char));
		}

		// look for an end of command delimiter
		if (character==';') {
			return 1;
		}

		// write character to buffer and move on
		cmdbuffer->append(character);
	}
		
}

int sqlrsh::runCommand(sqlrconnection *sqlrcon, sqlrcursor *sqlrcur, 
					environment *env, const char *command) {

	int	cmdtype=commandType(command);

	if (cmdtype>0) {
		// if the command an internal command, run it as one
		internalCommand(env,command);
		return 1;
	} else if (cmdtype==0) {
		// if the command is not an internal command, 
		// execute it as a query and display the result set
		externalCommand(sqlrcon,sqlrcur,env,command);
		return 1;
	} else {
		return 0;
	}
}

int sqlrsh::commandType(const char *command) {

	// skip white space
	char	*ptr=(char *)command;
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// compare to known internal commands
	if (!strncasecmp(ptr,"color",5) ||
		!strncasecmp(ptr,"headers",7) ||
		!strncasecmp(ptr,"stats",5) ||
		!strncasecmp(ptr,"debug",5) ||
		!strncasecmp(ptr,"final",5) ||
		!strncasecmp(ptr,"help",4)) {

		// return value of 1 is internal command
		return 1;
	}

	// look for an exit command
	if (!strncasecmp(ptr,"quit",4) ||
		!strncasecmp(ptr,"exit",4)) {
		return -1;
	}

	// return value of 0 is external command
	return 0;
}

void sqlrsh::internalCommand(environment *env, const char *command) {

	// skip white space
	char	*ptr=(char *)command;
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// compare to known internal commands
	int	cmdtype=0;
	if (!strncasecmp(ptr,"color",5)) {
		ptr=ptr+5;
		cmdtype=1;
	} else if (!strncasecmp(ptr,"headers",7)) {
		ptr=ptr+7;
		cmdtype=2;
	} else if (!strncasecmp(ptr,"stats",5)) {	
		ptr=ptr+5;
		cmdtype=3;
	} else if (!strncasecmp(ptr,"debug",5)) {	
		ptr=ptr+5;
		cmdtype=4;
	} else if (!strncasecmp(ptr,"final",5)) {	
		ptr=ptr+5;
		cmdtype=5;
	} else if (!strncasecmp(ptr,"help",4)) {	
		displayHelp(env);
		return;
	} else {
		return;
	}

	// skip white space
	while (*ptr==' ' || *ptr=='	' || *ptr=='\n') {
		ptr++;
	}

	// on or off?
	int	toggle;
	if (!strncasecmp(ptr,"on",2)) {
		toggle=1;
	} else if (!strncasecmp(ptr,"off",3)) {
		toggle=0;
	} else {
		return;
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
	}
}

void sqlrsh::externalCommand(sqlrconnection *sqlrcon,
				sqlrcursor *sqlrcur, environment *env, 
				const char *command) {

	// init stats
	initStats(env);

	// handle debug
	if (env->debug) {
		sqlrcon->debugOn();
	}

	// handle a commit/rollback
	if (!strncasecmp(command,"commit",6)) {

		sqlrcon->commit();

	} else if (!strncasecmp(command,"rollback",8)) {

		sqlrcon->rollback();

	} else {

		// send the query
		sqlrcur->setResultSetBufferSize(100);
		sqlrcur->sendQuery(command);

		// look for an error
		if (sqlrcur->errorMessage()) {

			// display the error
			displayError(sqlrcur,env);

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

void sqlrsh::initStats(environment *env) {

	if (!env->stats) {
		return;
	}

	// call clock here or something
	clock();
}

void sqlrsh::displayError(sqlrcursor *sqlrcur, environment *env) {

	cyan(env);
	printf("%s\n",sqlrcur->errorMessage());
	white(env);
}

void sqlrsh::displayHeader(sqlrcursor *sqlrcur, environment *env) {

	if (!env->headers) {
		return;
	}

	// display column names
	int	charcount=0;
	int	colcount=sqlrcur->colCount();
	char	*name;
	int	namelen;
	int	longest;

	// iterate through columns
	for (int i=0; i<sqlrcur->colCount(); i++) {

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
		namelen=strlen(name);
		longest=sqlrcur->getLongest(i);
		if (namelen>longest) {
			longest=namelen;
		}
		charcount=charcount+longest;

		// pad after the name with spaces
		for (int j=namelen; j<longest; j++) {
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
	for (int i=0; i<charcount; i++) {
		printf("=");
	}
	printf("\n");
	white(env);
}

void sqlrsh::displayResultSet(sqlrcursor *sqlrcur, environment *env) {

	// display column names
	int	colcount=sqlrcur->colCount();
	int	namelen;
	int	longest;

	int	i=0;
	char	*field="";
	while (field && colcount) {
		for (int j=0; j<colcount; j++) {

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
				namelen=strlen(sqlrcur->getColumnName(j));
				if (namelen>longest) {
					longest=namelen;
				}
			}

			// pad after the name with spaces
			for (int k=strlen(sqlrcur->getField(i,j)); 
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

void sqlrsh::displayStats(sqlrcursor *sqlrcur, environment *env) {

	if (!env->stats) {
		return;
	}

	// call clock again, display results
	red(env);
	printf("	Rows Returned   : ");
	magenta(env);
	printf("%d\n",sqlrcur->rowCount());
	red(env);
	printf("	Fields Returned : ");
	magenta(env);
	printf("%d\n",sqlrcur->rowCount()*sqlrcur->colCount());
	red(env);
	printf("	System time     : ");
	magenta(env);
	printf("%ld\n",clock());
	white(env);
}

void sqlrsh::displayHelp(environment *env) {

	printf("\n");
	yellow(env);
	printf("	To run a query, simply type it at the prompt,\n");
	printf("	followed by a semicolon.  Queries may be \n");
	printf("	split over multiple lines.\n\n");
	cyan(env);
	printf("		color on/off	- ");
	green(env);
	printf("toggles colorizing\n");
	cyan(env);
	printf("		headers on/off	- ");
	green(env);
	printf("toggles column descriptions before result set\n");
	cyan(env);
	printf("		stats on/off	- ");
	green(env);
	printf("toggles statistics after result set\n");
	cyan(env);
	printf("		debug on/off	- ");
	green(env);
	printf("toggles debug messages\n");
	cyan(env);
	printf("		final on/off	- ");
	green(env);
	printf("toggles use of one session per query\n");
	cyan(env);
	printf("		exit/quit	- ");
	green(env);
	printf("exits\n\n");
	yellow(env);
	printf("	All commands must be followed by a semicolon.\n");
	white(env);
}

void sqlrsh::startupMessage(environment *env,
				const char *host, int port, const char *user) {

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
							environment *env) {

	// init some variables
	stringbuffer	*command;
	int		exitprogram=0;
	int		promptcount;

	while (!exitprogram) {

		// prompt the user
		promptcount=0;
		
		// get the command
		command=new stringbuffer();
		int	done=0;
		while (!done) {
			#ifdef HAVE_READLINE
				stringbuffer	prmpt;
				prmpt.append((long)promptcount);
				prmpt.append("> ");
				char	*cmd=readline(prmpt.getString());
				if (cmd[0]) {
					charstring::rightTrim(cmd);
					add_history(cmd);
				}
			#else
				prompt(promptcount);
				char	cmd[1024];
				ssize_t	bytes=read(0,cmd,1024);
				cmd[bytes-1]=(char)NULL;
			#endif
			int	len=strlen(cmd);
			done=0;
			for (int i=0; i<len; i++) {
				if (i==len-1) {
				       if (cmd[i]==';') {
						done=1;
					} else {
						command->append(cmd[i]);
					}
				} else if (cmd[i]>=32 || 
						cmd[i]=='	') {
					command->append(cmd[i]);
				}
			}
			if (!done) {
				promptcount++;
				command->append(" ");
			}
			#ifdef HAVE_READLINE
				delete[] cmd;
			#endif
		}
		/*#else
			putStdioInRawMode();

			prompt(promptcount);

			while (1) {
				read(0,&buffer1,1);
				if (buffer1==';') {
					read(0,&buffer2,1);
					if (buffer2=='\n') {
						break;
					} else {
						command->append(buffer1);
					}
				} else if (buffer1=='\n') {
					promptcount++;
					command->append(" ");
					prompt(promptcount);
				} else if (buffer1>=32 || buffer1=='	') {
					command->append(buffer1);
				}
			}

			putStdioInCookedMode();
		#endif*/

		// run the command
		if (!runCommand(sqlrcon,sqlrcur,env,command->getString())) {	
			exitprogram=1;
		}

		delete command;
	}
}

void sqlrsh::prompt(int promptcount) {

	printf("%d> ",promptcount);
	fflush(stdout);
}

void sqlrsh::error(const char *errstring) {

	// print the error
	printf("%s\n",errstring);
}

void sqlrsh::execute(int argc, const char **argv) {


	commandline	cmdline(argc,argv);
	sqlrconfigfile	cfgfile;
	usercontainer	*currentnode=NULL;
	char		*host;
	int		port;
	char		*socket;
	char		*user;
	char		*password;
	char		*script=NULL;

	char	*config=cmdline.value("-config");
	if (!(config && config[0])) {
		config=DEFAULT_CONFIG_FILE;
	}
	char	*id=cmdline.value("-id");

	if (!(id && id[0])) {

		if (argc<6) {
			printf("usage: sqlrsh  host port socket ");
			printf("user password [script]\n");
			printf("  or   sqlrsh  [-config configfile] ");
			printf("-id id [script]\n");
			exit(1);
		}

		host=(char *)argv[1];
		port=atoi(argv[2]);
		socket=(char *)argv[3];
		user=(char *)argv[4];
		password=(char *)argv[5];
		if (argv[6]) {
			script=(char *)argv[6];
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

	// set up an environment
	environment	env;

	// process RC files
	systemRcFile(&sqlrcon,&sqlrcur,&env);
	userRcFile(&sqlrcon,&sqlrcur,&env);


	#ifdef HAVE_READLINE

		// handle the history file
		char	*filename;
		char	*home=getenv("HOME");
		if (home && home[0]) {
			filename=new char[strlen(home)+16+1];
			sprintf(filename,"%s/.sqlrsh_history",home);

			// create the history file if it doesn't exist now
			FILE	*historyfile=fopen(filename,"a");
			fclose(historyfile);

			read_history(filename);
		}
	#endif

	// if a script was specified, run it otherwise go into interactive mode
	if (script) {
		runScript(&sqlrcon,&sqlrcur,&env,script,1);
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

void sqlrsh::setColor(environment *env, int value) {
	if (env->color) {
		printf("\033[0;%dm",value);
	}
}

void sqlrsh::black(environment *env) {
	setColor(env,30);
}

void sqlrsh::red(environment *env) {
	setColor(env,31);
}

void sqlrsh::green(environment *env) {
	setColor(env,32);
}

void sqlrsh::yellow(environment *env) {
	setColor(env,33);
}

void sqlrsh::blue(environment *env) {
	setColor(env,34);
}

void sqlrsh::magenta(environment *env) {
	setColor(env,35);
}

void sqlrsh::cyan(environment *env) {
	setColor(env,36);
}

void sqlrsh::white(environment *env) {
	setColor(env,37);
}



int	main(int argc, const char **argv) {

	#include <version.h>

	sqlrsh	s;
	s.execute(argc,argv);
	exit(0);
}
