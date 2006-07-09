// Copyright (c) 2000  David Muse
// See the file COPYING for more information.

#ifndef GTKFE_H
#define GTKFE_H
#include <gtk/gtk.h>
#include <configfile.h>

#if GTK_VERSION!=2
	#define gtk_toggle_button_set_active gtk_toggle_button_set_state
#endif

class gtkfe {
	public:
				gtkfe();
				~gtkfe();
		static	void	run(int argc, char **argv);
	private:

		// interface building methods
		static	void	buildInterface();
		static	void	buildMainWindow();
		static	void	buildMenuBar();
		static	void	buildInstanceList();
		static	void	buildNotebookFrame();
		static	void	buildInstancePage();
		static	void	buildConnectionsPage();
		static	void	buildUsersPage();
		static	void	showMainWindow();

		static	void	populateInstanceList();
		static	void	populateInstanceParameters(instance *inst);
		static	void	clearInstanceParameters();
		static	void	defaultInstanceParameters();
		static	void	populateUserList(instance *inst);
		static	void	populateUserParameters(user *usr);
		static	void	clearUserParameters();
		static	void	populateConnectionList(instance *inst);
		static	void	populateConnectionParameters(connection *conn);
		static	void	clearConnectionParameters();
		static	void	defaultConnectionParameters();

		static	void	okDialog(const char *message);

		// callbacks
		static	void	newFile(GtkWidget *widget, 
						gpointer data);
		static	void	openFile(GtkWidget *widget, 
						gpointer data);
		static	void	handleFile(const char *filename);
		static	void	closeFile(GtkWidget *widget, 
						gpointer data);
		static	void	saveFile(GtkWidget *widget, 
						gpointer data);
		static	void	saveFileAs(GtkWidget *widget, 
						gpointer data);
		static	void	quit(GtkWidget *widget, gpointer data);

		static	void	openFileOk(GtkWidget *widget, 
						gpointer data);
		static	void	saveFileAsOk(GtkWidget *widget, 
						gpointer data);
		static	void	fileCancel(GtkWidget *widget, 
						gpointer data);
		static	void	instanceListSelect(GtkWidget *widget,
						gint row, gint column,
						GdkEventButton *event,
						gpointer data);
		static	void	userListSelect(GtkWidget *widget,
						gint row, gint column,
						GdkEventButton *event,
						gpointer data);
		static	void	connectionListSelect(GtkWidget *widget,
						gint row, gint column,
						GdkEventButton *event,
						gpointer data);

		static	void	instanceParametersSave(GtkWidget *widget,
						gpointer data);
		static	void	instanceParametersCancel(GtkWidget *widget,
						gpointer data);
		static	void	clearAllInstanceData();
		static	void	connectionParametersSave(GtkWidget *widget,
						gpointer data);
		static	void	connectionParametersCancel(GtkWidget *widget,
						gpointer data);
		static	void	clearAllConnectionData();
		static	void	userParametersSave(GtkWidget *widget,
						gpointer data);
		static	void	userParametersCancel(GtkWidget *widget,
						gpointer data);
		static	void	clearAllUserData();
		static	void	dialogOk(GtkWidget *widget,
						gpointer data);


		static	void	newInstance(GtkWidget *widget,
						gpointer data);
		static	void	deleteInstance(GtkWidget *widget,
						gpointer data);
		static	void	newConnection(GtkWidget *widget,
						gpointer data);
		static	void	deleteConnection(GtkWidget *widget,
						gpointer data);
		static	void	newUser(GtkWidget *widget,
						gpointer data);
		static	void	deleteUser(GtkWidget *widget,
						gpointer data);

		static	void	toggleCommit(GtkWidget *widget,
						gpointer data);


		// configfile
		static	configfile	*conffile;

		static	instance	*currentinstance;
		static	connection	*currentconnection;
		static	user		*currentuser;
		static	int		currentinstanceindex;
		static	int		currentconnectionindex;
		static	int		currentuserindex;

		// widgets
		static	GtkWidget	*window;

		static	GtkWidget	*vbox;
		static	GtkWidget	*hbox;

		static	GtkWidget	*filemenu;
		static	GtkWidget	*newitem;
		static	GtkWidget	*openitem;
		static	GtkWidget	*closeitem;
		static	GtkWidget	*saveitem;
		static	GtkWidget	*saveasitem;
		static	GtkWidget	*quititem;

		static	GtkWidget	*menubar;
		static	GtkWidget	*fileitem;

		static	GtkWidget	*instancelistvbox;
		static	GtkWidget	*instancelistscroll;
		static	GtkWidget	*instancelist;
		static	GtkWidget	*newinstancebutton;
		static	GtkWidget	*deleteinstancebutton;

		static	GtkWidget	*instanceframevbox;
		static	GtkWidget	*notebook;
		static	GtkWidget	*instancebuttonhbox;
		static	GtkWidget	*saveinstancebutton;
		static	GtkWidget	*cancelinstancebutton;

		static	GtkWidget	*propertiestab;
		static	GtkWidget	*propertiesframe;
		static	GtkWidget	*propertiesvbox;
		static	GtkWidget	*propertiestable;

		static	GtkWidget	*idlabel;
		static	GtkWidget	*portlabel;
		static	GtkWidget	*unixportlabel;
		static	GtkWidget	*dbaselabel;
		static	GtkWidget	*connectionslabel;
		static	GtkWidget	*maxconnectionslabel;
		static	GtkWidget	*maxqueuelengthlabel;
		static	GtkWidget	*growbylabel;
		static	GtkWidget	*ttllabel;
		static	GtkWidget	*endofsessionlabel;
		static	GtkWidget	*sessiontimeoutlabel;
		static	GtkWidget	*runasuserlabel;
		static	GtkWidget	*runasgrouplabel;
		static	GtkWidget	*cursorslabel;
		static	GtkWidget	*authtierlabel;
		static	GtkWidget	*handofflabel;
		static	GtkWidget	*deniedipslabel;
		static	GtkWidget	*allowedipslabel;
		static	GtkWidget	*debuglabel;
		static	GtkWidget	*maxquerysizelabel;
		static	GtkWidget	*maxstringbindvaluelengthlabel;
		static	GtkWidget	*maxlobbindvaluelengthlabel;
		static	GtkWidget	*idleclienttimeoutlabel;
		static	GtkWidget	*maxlistenerslabel;
		static	GtkWidget	*listenertimeoutlabel;

		static	GtkWidget	*identry;
		static	GtkWidget	*portentry;
		static	GtkWidget	*unixportentry;
		static	GtkWidget	*dbasecombo;
		static	GList		*dblist;
		static	GtkWidget	*connectionsentry;
		static	GtkWidget	*maxconnectionsentry;
		static	GtkWidget	*maxqueuelengthentry;
		static	GtkWidget	*growbyentry;
		static	GtkWidget	*ttlentry;
		static	GtkWidget	*commitbutton;
		static	GtkWidget	*rollbackbutton;
		static	int		commit;
		static	GSList		*endofsessiongroup;
		static	GtkWidget	*sessiontimeoutentry;
		static	GtkWidget	*runasuserentry;
		static	GtkWidget	*runasgroupentry;
		static	GtkWidget	*cursorsentry;
		static	GtkWidget	*authtiercombo;
		static	GList		*authlist;
		static	GtkWidget	*handoffcombo;
		static	GList		*handofflist;
		static	GtkWidget	*deniedipsentry;
		static	GtkWidget	*allowedipsentry;
		static	GtkWidget	*debugcombo;
		static	GList		*debuglist;
		static	GtkWidget	*maxquerysizeentry;
		static	GtkWidget	*maxstringbindvaluelengthentry;
		static	GtkWidget	*maxlobbindvaluelengthentry;
		static	GtkWidget	*idleclienttimeoutentry;
		static	GtkWidget	*maxlistenersentry;
		static	GtkWidget	*listenertimeoutentry;

		static	GtkWidget	*userstab;
		static	GtkWidget	*usersframe;
		static	GtkWidget	*usershbox;
		static	GtkWidget	*userlistvbox;
		static	GtkWidget	*userlistscroll;
		static	GtkWidget	*userlist;
		static	GtkWidget	*newuserbutton;
		static	GtkWidget	*deleteuserbutton;
		static	GtkWidget	*usersvbox;
		static	GtkWidget	*userstable;
		static	GtkWidget	*userlabel;
		static	GtkWidget	*passwordlabel;
		static	GtkWidget	*userentry;
		static	GtkWidget	*passwordentry;

		static	GtkWidget	*userbuttonhbox;
		static	GtkWidget	*saveuserbutton;
		static	GtkWidget	*canceluserbutton;

		static	GtkWidget	*connectionstab;
		static	GtkWidget	*connectionsframe;
		static	GtkWidget	*connectionshbox;
		static	GtkWidget	*connectionlistvbox;
		static	GtkWidget	*connectionlistscroll;
		static	GtkWidget	*connectionlist;
		static	GtkWidget	*newconnectionbutton;
		static	GtkWidget	*deleteconnectionbutton;
		static	GtkWidget	*connectionsvbox;
		static	GtkWidget	*connectionstable;
		static	GtkWidget	*connectionidlabel;
		static	GtkWidget	*stringlabel;
		static	GtkWidget	*metriclabel;
		static	GtkWidget	*behindloadbalancerlabel;
		static	GtkWidget	*connectionidentry;
		static	GtkWidget	*stringentry;
		static	GtkWidget	*metricentry;
		static	GtkWidget	*behindloadbalancercombo;
		static	GList		*behindloadbalancerlist;

		static	GtkWidget	*connectionbuttonhbox;
		static	GtkWidget	*saveconnectionbutton;
		static	GtkWidget	*cancelconnectionbutton;

		static	GtkWidget	*fileselector;

		static	GtkWidget	*okdialog;
		static	GtkWidget	*okdialoglabel;
		static	GtkWidget	*okdialogbutton;

		// flags
		static	int		newinstance;
		static	int		newconnection;
		static	int		newuser;

		// clist arrays
		static	char		*clisttext[1][1];
};

#endif
