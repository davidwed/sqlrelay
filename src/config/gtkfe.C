// Copyright (c) 2000  David Muse
// See the file COPYING for more information

#include <config.h>

#include <gtkfe.h>
#include <dblist.h>

#include <stdio.h>
#include <stdlib.h>

#include <defaults.h>

// configfile
configfile	*gtkfe::conffile;

instance	*gtkfe::currentinstance;
int		gtkfe::currentinstanceindex;
connection	*gtkfe::currentconnection;
int		gtkfe::currentconnectionindex;
user		*gtkfe::currentuser;
int		gtkfe::currentuserindex;

// widgets
GtkWidget	*gtkfe::window;

GtkWidget	*gtkfe::vbox;
GtkWidget	*gtkfe::hbox;

GtkWidget	*gtkfe::filemenu;
GtkWidget	*gtkfe::newitem;
GtkWidget	*gtkfe::openitem;
GtkWidget	*gtkfe::closeitem;
GtkWidget	*gtkfe::saveitem;
GtkWidget	*gtkfe::saveasitem;
GtkWidget	*gtkfe::quititem;

GtkWidget	*gtkfe::menubar;
GtkWidget	*gtkfe::fileitem;

GtkWidget	*gtkfe::instancelistvbox;
GtkWidget	*gtkfe::instancelistscroll;
GtkWidget	*gtkfe::instancelist;
GtkWidget	*gtkfe::newinstancebutton;
GtkWidget	*gtkfe::deleteinstancebutton;

GtkWidget	*gtkfe::instanceframevbox;
GtkWidget	*gtkfe::notebook;
GtkWidget	*gtkfe::instancebuttonhbox;
GtkWidget	*gtkfe::saveinstancebutton;
GtkWidget	*gtkfe::cancelinstancebutton;

GtkWidget	*gtkfe::propertiestab;
GtkWidget	*gtkfe::propertiesframe;
GtkWidget	*gtkfe::propertiesvbox;
GtkWidget	*gtkfe::propertiestable;

GtkWidget	*gtkfe::idlabel;
GtkWidget	*gtkfe::portlabel;
GtkWidget	*gtkfe::unixportlabel;
GtkWidget	*gtkfe::dbaselabel;
GtkWidget	*gtkfe::connectionslabel;
GtkWidget	*gtkfe::maxconnectionslabel;
GtkWidget	*gtkfe::maxqueuelengthlabel;
GtkWidget	*gtkfe::growbylabel;
GtkWidget	*gtkfe::ttllabel;
GtkWidget	*gtkfe::endofsessionlabel;
GtkWidget	*gtkfe::sessiontimeoutlabel;
GtkWidget	*gtkfe::runasuserlabel;
GtkWidget	*gtkfe::runasgrouplabel;
GtkWidget	*gtkfe::cursorslabel;
GtkWidget	*gtkfe::authtierlabel;
GtkWidget	*gtkfe::handofflabel;
GtkWidget	*gtkfe::deniedipslabel;
GtkWidget	*gtkfe::allowedipslabel;
GtkWidget	*gtkfe::debuglabel;

GtkWidget	*gtkfe::identry;
GtkWidget	*gtkfe::portentry;
GtkWidget	*gtkfe::unixportentry;
GtkWidget	*gtkfe::dbasecombo;
GList		*gtkfe::dblist;
GtkWidget	*gtkfe::connectionsentry;
GtkWidget	*gtkfe::maxconnectionsentry;
GtkWidget	*gtkfe::maxqueuelengthentry;
GtkWidget	*gtkfe::growbyentry;
GtkWidget	*gtkfe::ttlentry;
GtkWidget	*gtkfe::commitbutton;
GtkWidget	*gtkfe::rollbackbutton;
int		gtkfe::commit;
GSList		*gtkfe::endofsessiongroup;
GtkWidget	*gtkfe::sessiontimeoutentry;
GtkWidget	*gtkfe::runasuserentry;
GtkWidget	*gtkfe::runasgroupentry;
GtkWidget	*gtkfe::cursorsentry;
GtkWidget	*gtkfe::authtiercombo;
GList		*gtkfe::authlist;
GtkWidget	*gtkfe::handoffcombo;
GList		*gtkfe::handofflist;
GtkWidget	*gtkfe::deniedipsentry;
GtkWidget	*gtkfe::allowedipsentry;
GtkWidget	*gtkfe::debugcombo;
GList		*gtkfe::debuglist;

GtkWidget	*gtkfe::userstab;
GtkWidget	*gtkfe::usersframe;
GtkWidget	*gtkfe::usershbox;
GtkWidget	*gtkfe::userlistvbox;
GtkWidget	*gtkfe::userlistscroll;
GtkWidget	*gtkfe::userlist;
GtkWidget	*gtkfe::newuserbutton;
GtkWidget	*gtkfe::deleteuserbutton;
GtkWidget	*gtkfe::usersvbox;
GtkWidget	*gtkfe::userstable;
GtkWidget	*gtkfe::userlabel;
GtkWidget	*gtkfe::passwordlabel;
GtkWidget	*gtkfe::userentry;
GtkWidget	*gtkfe::passwordentry;

GtkWidget	*gtkfe::userbuttonhbox;
GtkWidget	*gtkfe::saveuserbutton;
GtkWidget	*gtkfe::canceluserbutton;

GtkWidget	*gtkfe::connectionstab;
GtkWidget	*gtkfe::connectionsframe;
GtkWidget	*gtkfe::connectionshbox;
GtkWidget	*gtkfe::connectionlistvbox;
GtkWidget	*gtkfe::connectionlistscroll;
GtkWidget	*gtkfe::connectionlist;
GtkWidget	*gtkfe::newconnectionbutton;
GtkWidget	*gtkfe::deleteconnectionbutton;
GtkWidget	*gtkfe::connectionsvbox;
GtkWidget	*gtkfe::connectionstable;
GtkWidget	*gtkfe::connectionidlabel;
GtkWidget	*gtkfe::stringlabel;
GtkWidget	*gtkfe::metriclabel;
GtkWidget	*gtkfe::connectionidentry;
GtkWidget	*gtkfe::stringentry;
GtkWidget	*gtkfe::metricentry;

GtkWidget	*gtkfe::connectionbuttonhbox;
GtkWidget	*gtkfe::saveconnectionbutton;
GtkWidget	*gtkfe::cancelconnectionbutton;

GtkWidget	*gtkfe::fileselector;

GtkWidget	*gtkfe::okdialog;
GtkWidget	*gtkfe::okdialoglabel;
GtkWidget	*gtkfe::okdialogbutton;

char		*gtkfe::clisttext[1][1];

gtkfe::gtkfe() {
	conffile=(configfile *)NULL;
}

gtkfe::~gtkfe() {
	if (conffile) {
		delete conffile;
	}
}

void gtkfe::run(int argc, char **argv) {

	// init gtk and build the interface
	gtk_init(&argc, &argv);
	buildInterface();

	// if the user passed in a filename on the command line, use it
	if (argc>1) {
		handleFile(argv[1]);
	}

	// main loop
	gtk_main();
}

void gtkfe::buildInterface() {

	buildMainWindow();
	buildMenuBar();
	buildInstanceList();
	buildNotebookFrame();
	buildInstancePage();
	buildConnectionsPage();
	buildUsersPage();
	showMainWindow();
}


void gtkfe::buildMainWindow() {

	// main window
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_usize(GTK_WIDGET(window),600,650);
	gtk_window_set_title(GTK_WINDOW(window),"SQL Relay");

	gtk_signal_connect(GTK_OBJECT(window),"delete_event",
				GTK_SIGNAL_FUNC(quit),
				(gpointer)NULL);
	gtk_signal_connect(GTK_OBJECT(window),"destroy",
				GTK_SIGNAL_FUNC(quit),
				(gpointer)NULL);


	// vertical box
	vbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	gtk_widget_show(vbox);

	// horizontal box
	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_end(GTK_BOX(vbox),hbox,TRUE,TRUE,5);
	gtk_widget_show(hbox);
}


void gtkfe::buildMenuBar() {

	// file menu
	filemenu=gtk_menu_new();
	newitem=gtk_menu_item_new_with_label("New");
	gtk_signal_connect(GTK_OBJECT(newitem),"activate",
				GTK_SIGNAL_FUNC(newFile),
				(gpointer)NULL);
	openitem=gtk_menu_item_new_with_label("Open...");
	gtk_signal_connect(GTK_OBJECT(openitem),"activate",
				GTK_SIGNAL_FUNC(openFile),
				(gpointer)NULL);
	closeitem=gtk_menu_item_new_with_label("Close");
	gtk_widget_set_sensitive(closeitem,FALSE);
	gtk_signal_connect(GTK_OBJECT(closeitem),"activate",
				GTK_SIGNAL_FUNC(closeFile),
				(gpointer)NULL);
	saveitem=gtk_menu_item_new_with_label("Save");
	gtk_widget_set_sensitive(saveitem,FALSE);
	gtk_signal_connect(GTK_OBJECT(saveitem),"activate",
				GTK_SIGNAL_FUNC(saveFile),
				(gpointer)NULL);
	saveasitem=gtk_menu_item_new_with_label("Save As...");
	gtk_widget_set_sensitive(saveasitem,FALSE);
	gtk_signal_connect(GTK_OBJECT(saveasitem),"activate",
				GTK_SIGNAL_FUNC(saveFileAs),
				(gpointer)NULL);
	quititem=gtk_menu_item_new_with_label("Quit");
	gtk_signal_connect(GTK_OBJECT(quititem),"activate",
				GTK_SIGNAL_FUNC(quit),
				(gpointer)NULL);

	gtk_menu_append(GTK_MENU(filemenu),newitem);
	gtk_menu_append(GTK_MENU(filemenu),openitem);
	gtk_menu_append(GTK_MENU(filemenu),closeitem);
	gtk_menu_append(GTK_MENU(filemenu),saveitem);
	gtk_menu_append(GTK_MENU(filemenu),saveasitem);
	gtk_menu_append(GTK_MENU(filemenu),quititem);

	gtk_widget_show(newitem);
	gtk_widget_show(openitem);
	gtk_widget_show(closeitem);
	gtk_widget_show(saveitem);
	gtk_widget_show(saveasitem);
	gtk_widget_show(quititem);


	// menubar
	menubar=gtk_menu_bar_new();

	fileitem=gtk_menu_item_new_with_label("File");
	gtk_widget_show(fileitem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileitem),filemenu);
	
	gtk_menu_bar_append(GTK_MENU_BAR(menubar),fileitem);

	gtk_box_pack_start(GTK_BOX(vbox),menubar,FALSE,FALSE,0);
	gtk_widget_show(menubar);
}


void gtkfe::buildInstanceList() {

	// instance list vertical box
	instancelistvbox=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbox),instancelistvbox,TRUE,TRUE,5);
	gtk_widget_set_sensitive(instancelistvbox,FALSE);
	gtk_widget_show(instancelistvbox);

	// instance list scrolling window
	instancelistscroll=gtk_scrolled_window_new((GtkAdjustment *)NULL,
							(GtkAdjustment *)NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(instancelistscroll),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(instancelistvbox),
				instancelistscroll,TRUE,TRUE,0);
	gtk_widget_show(instancelistscroll);

	// instance list
	instancelist=gtk_clist_new(1);
	gtk_signal_connect(GTK_OBJECT(instancelist),"select_row",
				GTK_SIGNAL_FUNC(instanceListSelect),
				(gpointer)NULL);
	gtk_clist_set_column_title(GTK_CLIST(instancelist),0,"Instances");
	gtk_clist_column_titles_passive(GTK_CLIST(instancelist));
	gtk_clist_column_titles_show(GTK_CLIST(instancelist));
	gtk_clist_set_selection_mode(GTK_CLIST(instancelist),
					GTK_SELECTION_SINGLE);
	gtk_clist_set_column_width(GTK_CLIST(instancelist),0,75);
	gtk_container_add(GTK_CONTAINER(instancelistscroll),instancelist);
	gtk_widget_show(instancelist);

	// new, delete buttons
	newinstancebutton=gtk_button_new_with_label("New");
	gtk_box_pack_start(GTK_BOX(instancelistvbox),
				newinstancebutton,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(newinstancebutton),"clicked",
				GTK_SIGNAL_FUNC(newInstance),
				(gpointer)NULL);
	gtk_widget_show(newinstancebutton);

	deleteinstancebutton=gtk_button_new_with_label("Delete");
	gtk_box_pack_start(GTK_BOX(instancelistvbox),
				deleteinstancebutton,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(deleteinstancebutton),"clicked",
				GTK_SIGNAL_FUNC(deleteInstance),
				(gpointer)NULL);
	gtk_widget_set_sensitive(deleteinstancebutton,FALSE);
	gtk_widget_show(deleteinstancebutton);
}


void gtkfe::buildNotebookFrame() {

	// instance list vertical box
	instanceframevbox=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbox),instanceframevbox,TRUE,TRUE,5);

	// notebook
	notebook=gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook),GTK_POS_TOP);
	gtk_box_pack_start(GTK_BOX(instanceframevbox),notebook,TRUE,TRUE,0);
	gtk_widget_show(notebook);

	// instance button hbox
	instancebuttonhbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(instanceframevbox),
					instancebuttonhbox,FALSE,FALSE,10);
	gtk_widget_show(instancebuttonhbox);

	// instance buttons
	saveinstancebutton=gtk_button_new_with_label("Save");
	gtk_signal_connect(GTK_OBJECT(saveinstancebutton),"clicked",
				GTK_SIGNAL_FUNC(instanceParametersSave),
				(gpointer)NULL);
	gtk_widget_show(saveinstancebutton);
	gtk_box_pack_start(GTK_BOX(instancebuttonhbox),
				saveinstancebutton,TRUE,FALSE,0);

	cancelinstancebutton=gtk_button_new_with_label("Cancel");
	gtk_signal_connect(GTK_OBJECT(cancelinstancebutton),"clicked",
				GTK_SIGNAL_FUNC(instanceParametersCancel),
				(gpointer)NULL);
	gtk_widget_show(cancelinstancebutton);
	gtk_box_pack_start(GTK_BOX(instancebuttonhbox),
				cancelinstancebutton,TRUE,FALSE,0);

	gtk_widget_set_sensitive(instanceframevbox,FALSE);
	gtk_widget_show(instanceframevbox);
}

void gtkfe::buildInstancePage() {

	// properties tab
	propertiestab=gtk_label_new("Properties");
	gtk_widget_show(propertiestab);

	// properties frame
	propertiesframe=gtk_frame_new((gchar *)NULL);
	gtk_widget_show(propertiesframe);

	// properties vertical box
	propertiesvbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(propertiesframe),propertiesvbox);
	gtk_widget_show(propertiesvbox);

	// properties table
	propertiestable=gtk_table_new(3,19,FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(propertiestable),1);
	gtk_table_set_col_spacings(GTK_TABLE(propertiestable),4);
	gtk_box_pack_start(GTK_BOX(propertiesvbox),
				propertiestable,TRUE,FALSE,0);
	gtk_widget_show(propertiestable);

	// properties property labels
	idlabel=gtk_label_new("Id");
	gtk_misc_set_alignment(GTK_MISC(idlabel),1.0,0.5);
	gtk_widget_show(idlabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),idlabel,
					0,1,0,1);

	portlabel=gtk_label_new("Port");
	gtk_misc_set_alignment(GTK_MISC(portlabel),1.0,0.5);
	gtk_widget_show(portlabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),portlabel,
					0,1,1,2);

	unixportlabel=gtk_label_new("Socket");
	gtk_misc_set_alignment(GTK_MISC(unixportlabel),1.0,0.5);
	gtk_widget_show(unixportlabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),unixportlabel,
					0,1,2,3);

	dbaselabel=gtk_label_new("Database Type");
	gtk_misc_set_alignment(GTK_MISC(dbaselabel),1.0,0.5);
	gtk_widget_show(dbaselabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),dbaselabel,
					0,1,3,4);

	connectionslabel=gtk_label_new("Connections");
	gtk_misc_set_alignment(GTK_MISC(connectionslabel),1.0,0.5);
	gtk_widget_show(connectionslabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),connectionslabel,
					0,1,4,5);

	maxconnectionslabel=gtk_label_new("Max Connections");
	gtk_misc_set_alignment(GTK_MISC(maxconnectionslabel),1.0,0.5);
	gtk_widget_show(maxconnectionslabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					maxconnectionslabel,
					0,1,5,6);

	maxqueuelengthlabel=gtk_label_new("Max Queue Length");
	gtk_misc_set_alignment(GTK_MISC(maxqueuelengthlabel),1.0,0.5);
	gtk_widget_show(maxqueuelengthlabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					maxqueuelengthlabel,
					0,1,6,7);

	growbylabel=gtk_label_new("Grow By");
	gtk_misc_set_alignment(GTK_MISC(growbylabel),1.0,0.5);
	gtk_widget_show(growbylabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),growbylabel,
					0,1,7,8);

	ttllabel=gtk_label_new("Time To Live");
	gtk_misc_set_alignment(GTK_MISC(ttllabel),1.0,0.5);
	gtk_widget_show(ttllabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),ttllabel,
					0,1,8,9);

	endofsessionlabel=gtk_label_new("End Of Session");
	gtk_misc_set_alignment(GTK_MISC(endofsessionlabel),1.0,0.5);
	gtk_widget_show(endofsessionlabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					endofsessionlabel,
					0,1,9,10);

	sessiontimeoutlabel=gtk_label_new("Session Timeout");
	gtk_misc_set_alignment(GTK_MISC(sessiontimeoutlabel),1.0,0.5);
	gtk_widget_show(sessiontimeoutlabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					sessiontimeoutlabel,
					0,1,10,11);

	runasuserlabel=gtk_label_new("User To Run As");
	gtk_misc_set_alignment(GTK_MISC(runasuserlabel),1.0,0.5);
	gtk_widget_show(runasuserlabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					runasuserlabel,
					0,1,11,12);

	runasgrouplabel=gtk_label_new("Group To Run As");
	gtk_misc_set_alignment(GTK_MISC(runasgrouplabel),1.0,0.5);
	gtk_widget_show(runasgrouplabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					runasgrouplabel,
					0,1,12,13);

	cursorslabel=gtk_label_new("Cursors To Open");
	gtk_misc_set_alignment(GTK_MISC(cursorslabel),1.0,0.5);
	gtk_widget_show(cursorslabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					cursorslabel,
					0,1,13,14);

	authtierlabel=gtk_label_new("Authentication Tier");
	gtk_misc_set_alignment(GTK_MISC(authtierlabel),1.0,0.5);
	gtk_widget_show(authtierlabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					authtierlabel,
					0,1,14,15);

	handofflabel=gtk_label_new("Handoff");
	gtk_misc_set_alignment(GTK_MISC(handofflabel),1.0,0.5);
	gtk_widget_show(handofflabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					handofflabel,
					0,1,15,16);

	deniedipslabel=gtk_label_new("Denied IP's");
	gtk_misc_set_alignment(GTK_MISC(deniedipslabel),1.0,0.5);
	gtk_widget_show(deniedipslabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					deniedipslabel,
					0,1,16,17);

	allowedipslabel=gtk_label_new("Allowed IP's");
	gtk_misc_set_alignment(GTK_MISC(allowedipslabel),1.0,0.5);
	gtk_widget_show(allowedipslabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					allowedipslabel,
					0,1,17,18);

	debuglabel=gtk_label_new("Debug");
	gtk_misc_set_alignment(GTK_MISC(debuglabel),1.0,0.5);
	gtk_widget_show(debuglabel);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					debuglabel,
					0,1,18,19);


	// properties property inputs
	identry=gtk_entry_new();
	gtk_widget_show(identry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),identry,
					1,3,0,1);

	portentry=gtk_entry_new_with_max_length(5);
	gtk_widget_show(portentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),portentry,
					1,3,1,2);

	unixportentry=gtk_entry_new();
	gtk_widget_show(unixportentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),unixportentry,
					1,3,2,3);

	dbasecombo=gtk_combo_new();
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(dbasecombo)->entry),FALSE);
	dblist=(GList *)NULL;
	int	index=0;
	while (databaselist[index]) {
		dblist=g_list_append(dblist,databaselist[index]);
		index++;
	}
	gtk_combo_set_popdown_strings(GTK_COMBO(dbasecombo),dblist);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(dbasecombo)->entry),"");
	gtk_widget_show(dbasecombo);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),dbasecombo,
					1,3,3,4);

	connectionsentry=gtk_entry_new_with_max_length(5);
	gtk_widget_show(connectionsentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),connectionsentry,
					1,3,4,5);

	maxconnectionsentry=gtk_entry_new_with_max_length(5);
	gtk_widget_show(maxconnectionsentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					maxconnectionsentry,
					1,3,5,6);

	maxqueuelengthentry=gtk_entry_new_with_max_length(5);
	gtk_widget_show(maxqueuelengthentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					maxqueuelengthentry,
					1,3,6,7);

	growbyentry=gtk_entry_new_with_max_length(5);
	gtk_widget_show(growbyentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),growbyentry,
					1,3,7,8);

	ttlentry=gtk_entry_new_with_max_length(5);
	gtk_widget_show(ttlentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),ttlentry,
					1,3,8,9);

	commitbutton=gtk_radio_button_new_with_label((GSList *)NULL,
							"commit");
	gtk_signal_connect(GTK_OBJECT(commitbutton),"clicked",
				GTK_SIGNAL_FUNC(toggleCommit),
				(gpointer)NULL);
	gtk_widget_show(commitbutton);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),commitbutton,
					1,2,9,10);

	endofsessiongroup=gtk_radio_button_group(
					GTK_RADIO_BUTTON(commitbutton));

	rollbackbutton=gtk_radio_button_new_with_label(
					endofsessiongroup,"rollback");
	gtk_widget_show(rollbackbutton);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),rollbackbutton,
					2,3,9,10);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(commitbutton),TRUE);

	sessiontimeoutentry=gtk_entry_new_with_max_length(5);
	gtk_widget_show(sessiontimeoutentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					sessiontimeoutentry,
					1,3,10,11);

	runasuserentry=gtk_entry_new_with_max_length(32);
	gtk_widget_show(runasuserentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					runasuserentry,
					1,3,11,12);

	runasgroupentry=gtk_entry_new_with_max_length(32);
	gtk_widget_show(runasgroupentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					runasgroupentry,
					1,3,12,13);

	cursorsentry=gtk_entry_new_with_max_length(5);
	gtk_widget_show(cursorsentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					cursorsentry,
					1,3,13,14);

	authtiercombo=gtk_combo_new();
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(authtiercombo)->entry),
				FALSE);
	authlist=(GList *)NULL;
	authlist=g_list_append(authlist,(void *)"none");
	authlist=g_list_append(authlist,(void *)"listener");
	authlist=g_list_append(authlist,(void *)"connection");
	authlist=g_list_append(authlist,(void *)"listener_and_connection");
	authlist=g_list_append(authlist,(void *)"database");
	gtk_combo_set_popdown_strings(GTK_COMBO(authtiercombo),authlist);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(authtiercombo)->entry),"");
	gtk_widget_show(authtiercombo);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					authtiercombo,
					1,3,14,15);

	handoffcombo=gtk_combo_new();
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(handoffcombo)->entry),
				FALSE);
	handofflist=(GList *)NULL;
	handofflist=g_list_append(handofflist,(void *)"reconnect");
	handofflist=g_list_append(handofflist,(void *)"pass");
	gtk_combo_set_popdown_strings(GTK_COMBO(handoffcombo),handofflist);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(handoffcombo)->entry),"");
	gtk_widget_show(handoffcombo);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					handoffcombo,
					1,3,15,16);

	deniedipsentry=gtk_entry_new_with_max_length(1024);
	gtk_widget_show(deniedipsentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					deniedipsentry,
					1,3,16,17);

	allowedipsentry=gtk_entry_new_with_max_length(1024);
	gtk_widget_show(allowedipsentry);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),
					allowedipsentry,
					1,3,17,18);

	debugcombo=gtk_combo_new();
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(debugcombo)->entry),FALSE);
	debuglist=(GList *)NULL;
	debuglist=g_list_append(debuglist,(void *)"none");
	debuglist=g_list_append(debuglist,(void *)"listener only");
	debuglist=g_list_append(debuglist,(void *)"connections only");
	debuglist=g_list_append(debuglist,(void *)"listener_and_connections");
	gtk_combo_set_popdown_strings(GTK_COMBO(debugcombo),debuglist);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(debugcombo)->entry),"");
	gtk_widget_show(debugcombo);
	gtk_table_attach_defaults(GTK_TABLE(propertiestable),debugcombo,
					1,3,18,19);

	// properties notebook page
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
					propertiesframe,propertiestab);
}


void gtkfe::buildConnectionsPage() {

	// connections tab
	connectionstab=gtk_label_new("Connections");
	gtk_widget_show(connectionstab);

	// connections frame
	connectionsframe=gtk_frame_new((gchar *)NULL);
	gtk_widget_show(connectionsframe);

	// connections horizontal box
	connectionshbox=gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(connectionsframe),connectionshbox);
	gtk_widget_show(connectionshbox);

	// connection list vertical box
	connectionlistvbox=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(connectionshbox),
				connectionlistvbox,TRUE,TRUE,0);
	gtk_widget_show(connectionlistvbox);

	// connection list scrolling window
	connectionlistscroll=gtk_scrolled_window_new((GtkAdjustment *)NULL,
							(GtkAdjustment *)NULL);
	gtk_scrolled_window_set_policy(
				GTK_SCROLLED_WINDOW(connectionlistscroll),
				GTK_POLICY_AUTOMATIC,
				GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(connectionlistvbox),
				connectionlistscroll,TRUE,TRUE,0);
	gtk_widget_show(connectionlistscroll);

	// connection list
	connectionlist=gtk_clist_new(1);
	gtk_signal_connect(GTK_OBJECT(connectionlist),"select_row",
				GTK_SIGNAL_FUNC(connectionListSelect),
				(gpointer)NULL);
	gtk_clist_set_column_title(GTK_CLIST(connectionlist),0,"Connections");
	gtk_clist_column_titles_passive(GTK_CLIST(connectionlist));
	gtk_clist_column_titles_show(GTK_CLIST(connectionlist));
	gtk_clist_set_selection_mode(GTK_CLIST(connectionlist),
					GTK_SELECTION_SINGLE);
	gtk_clist_set_column_width(GTK_CLIST(connectionlist),0,50);
	gtk_container_add(GTK_CONTAINER(connectionlistscroll),connectionlist);
	gtk_widget_show(connectionlist);

	// new, delete buttons
	newconnectionbutton=gtk_button_new_with_label("New");
	gtk_box_pack_start(GTK_BOX(connectionlistvbox),
				newconnectionbutton,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(newconnectionbutton),"clicked",
				GTK_SIGNAL_FUNC(newConnection),
				(gpointer)NULL);
	gtk_widget_show(newconnectionbutton);

	deleteconnectionbutton=gtk_button_new_with_label("Delete");
	gtk_box_pack_start(GTK_BOX(connectionlistvbox),
				deleteconnectionbutton,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(deleteconnectionbutton),"clicked",
				GTK_SIGNAL_FUNC(deleteConnection),
				(gpointer)NULL);
	gtk_widget_set_sensitive(deleteconnectionbutton,FALSE);
	gtk_widget_show(deleteconnectionbutton);

	// connections vertical box
	connectionsvbox=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(connectionshbox),
				connectionsvbox,TRUE,FALSE,0);
	gtk_widget_set_sensitive(connectionsvbox,FALSE);
	gtk_widget_show(connectionsvbox);

	// connections table
	connectionstable=gtk_table_new(2,4,FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(connectionstable),1);
	gtk_table_set_col_spacings(GTK_TABLE(connectionstable),4);
	gtk_widget_show(connectionstable);
	gtk_box_pack_start(GTK_BOX(connectionsvbox),
				connectionstable,TRUE,FALSE,0);

	// connections labels
	connectionidlabel=gtk_label_new("Connection Id");
	gtk_misc_set_alignment(GTK_MISC(connectionidlabel),1.0,0.5);
	gtk_widget_show(connectionidlabel);
	gtk_table_attach_defaults(GTK_TABLE(connectionstable),connectionidlabel,
					0,1,0,1);

	stringlabel=gtk_label_new("Connect String");
	gtk_misc_set_alignment(GTK_MISC(stringlabel),1.0,0.5);
	gtk_widget_show(stringlabel);
	gtk_table_attach_defaults(GTK_TABLE(connectionstable),stringlabel,
					0,1,1,2);

	metriclabel=gtk_label_new("Metric");
	gtk_misc_set_alignment(GTK_MISC(metriclabel),1.0,0.5);
	gtk_widget_show(metriclabel);
	gtk_table_attach_defaults(GTK_TABLE(connectionstable),metriclabel,
					0,1,2,3);

	// connections entries
	connectionidentry=gtk_entry_new();
	gtk_widget_show(connectionidentry);
	gtk_table_attach_defaults(GTK_TABLE(connectionstable),connectionidentry,
					1,2,0,1);

	stringentry=gtk_entry_new();
	gtk_widget_show(stringentry);
	gtk_table_attach_defaults(GTK_TABLE(connectionstable),stringentry,
					1,2,1,2);

	metricentry=gtk_entry_new_with_max_length(5);
	gtk_widget_show(metricentry);
	gtk_table_attach_defaults(GTK_TABLE(connectionstable),metricentry,
					1,2,2,3);

	// connection button hbox
	connectionbuttonhbox=gtk_hbox_new(FALSE,0);
	gtk_table_attach(GTK_TABLE(connectionstable),connectionbuttonhbox,
					1,2,3,4,
					(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),
					(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),
					0,10);
	gtk_widget_show(connectionbuttonhbox);

	// connection buttons
	saveconnectionbutton=gtk_button_new_with_label("Save");
	gtk_signal_connect(GTK_OBJECT(saveconnectionbutton),"clicked",
			GTK_SIGNAL_FUNC(connectionParametersSave),
			(gpointer)NULL);
	gtk_widget_show(saveconnectionbutton);
	gtk_box_pack_start(GTK_BOX(connectionbuttonhbox),
				saveconnectionbutton,TRUE,FALSE,0);
	cancelconnectionbutton=gtk_button_new_with_label("Cancel");
	gtk_signal_connect(GTK_OBJECT(cancelconnectionbutton),"clicked",
			GTK_SIGNAL_FUNC(connectionParametersCancel),
			(gpointer)NULL);
	gtk_widget_show(cancelconnectionbutton);
	gtk_box_pack_start(GTK_BOX(connectionbuttonhbox),
				cancelconnectionbutton,TRUE,FALSE,0);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
					connectionsframe,connectionstab);
}


void gtkfe::buildUsersPage() {

	// users tab
	userstab=gtk_label_new("Users");
	gtk_widget_show(userstab);

	// users frame
	usersframe=gtk_frame_new((gchar *)NULL);
	gtk_widget_show(usersframe);

	// users horizontal box
	usershbox=gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(usersframe),usershbox);
	gtk_widget_show(usershbox);

	// user list vertical box
	userlistvbox=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(usershbox),
				userlistvbox,TRUE,TRUE,0);
	gtk_widget_show(userlistvbox);

	// user list scrolling window
	userlistscroll=gtk_scrolled_window_new((GtkAdjustment *)NULL,
							(GtkAdjustment *)NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(userlistscroll),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(userlistvbox),userlistscroll,TRUE,TRUE,0);
	gtk_widget_show(userlistscroll);

	// user list
	userlist=gtk_clist_new(1);
	gtk_signal_connect(GTK_OBJECT(userlist),"select_row",
				GTK_SIGNAL_FUNC(userListSelect),
				(gpointer)NULL);
	gtk_clist_set_column_title(GTK_CLIST(userlist),0,"Users");
	gtk_clist_column_titles_passive(GTK_CLIST(userlist));
	gtk_clist_column_titles_show(GTK_CLIST(userlist));
	gtk_clist_set_selection_mode(GTK_CLIST(userlist),
					GTK_SELECTION_SINGLE);
	gtk_clist_set_column_width(GTK_CLIST(userlist),0,50);
	gtk_container_add(GTK_CONTAINER(userlistscroll),userlist);
	gtk_widget_show(userlist);

	// new, delete buttons
	newuserbutton=gtk_button_new_with_label("New");
	gtk_box_pack_start(GTK_BOX(userlistvbox),
				newuserbutton,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(newuserbutton),"clicked",
				GTK_SIGNAL_FUNC(newUser),
				(gpointer)NULL);
	gtk_widget_show(newuserbutton);

	deleteuserbutton=gtk_button_new_with_label("Delete");
	gtk_box_pack_start(GTK_BOX(userlistvbox),
				deleteuserbutton,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(deleteuserbutton),"clicked",
				GTK_SIGNAL_FUNC(deleteUser),
				(gpointer)NULL);
	gtk_widget_set_sensitive(deleteuserbutton,FALSE);
	gtk_widget_show(deleteuserbutton);


	// users vertical box
	usersvbox=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(usershbox),
				usersvbox,TRUE,FALSE,0);
	gtk_widget_set_sensitive(usersvbox,FALSE);
	gtk_widget_show(usersvbox);

	// users table
	userstable=gtk_table_new(2,3,FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(userstable),1);
	gtk_table_set_col_spacings(GTK_TABLE(userstable),4);
	gtk_widget_show(userstable);
	gtk_box_pack_start(GTK_BOX(usersvbox),userstable,TRUE,FALSE,0);

	// users labels
	userlabel=gtk_label_new("User");
	gtk_misc_set_alignment(GTK_MISC(userlabel),1.0,0.5);
	gtk_widget_show(userlabel);
	gtk_table_attach_defaults(GTK_TABLE(userstable),userlabel,
					0,1,0,1);

	passwordlabel=gtk_label_new("Password");
	gtk_misc_set_alignment(GTK_MISC(passwordlabel),1.0,0.5);
	gtk_widget_show(passwordlabel);
	gtk_table_attach_defaults(GTK_TABLE(userstable),passwordlabel,
					0,1,1,2);

	// users entries
	userentry=gtk_entry_new();
	gtk_widget_show(userentry);
	gtk_table_attach_defaults(GTK_TABLE(userstable),userentry,
					1,2,0,1);

	passwordentry=gtk_entry_new();
	gtk_widget_show(passwordentry);
	gtk_table_attach_defaults(GTK_TABLE(userstable),passwordentry,
					1,2,1,2);

	// user button hbox
	userbuttonhbox=gtk_hbox_new(FALSE,0);
	gtk_table_attach(GTK_TABLE(userstable),userbuttonhbox,
					1,2,2,3,
					(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),
					(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),
					0,10);
	gtk_widget_show(userbuttonhbox);

	// user buttons
	saveuserbutton=gtk_button_new_with_label("Save");
	gtk_signal_connect(GTK_OBJECT(saveuserbutton),"clicked",
				GTK_SIGNAL_FUNC(userParametersSave),
				(gpointer)NULL);
	gtk_widget_show(saveuserbutton);
	gtk_box_pack_start(GTK_BOX(userbuttonhbox),
				saveuserbutton,TRUE,FALSE,0);
	canceluserbutton=gtk_button_new_with_label("Cancel");
	gtk_signal_connect(GTK_OBJECT(canceluserbutton),"clicked",
				GTK_SIGNAL_FUNC(userParametersCancel),
				(gpointer)NULL);
	gtk_widget_show(canceluserbutton);
	gtk_box_pack_start(GTK_BOX(userbuttonhbox),
				canceluserbutton,TRUE,FALSE,0);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
					usersframe,userstab);
}


void gtkfe::showMainWindow() {

	// show the main window
	gtk_widget_show(window);
}


void gtkfe::populateInstanceList() {

	// freeze the list
	gtk_clist_freeze(GTK_CLIST(instancelist));

	// clear the list for good measure
	gtk_clist_clear(GTK_CLIST(instancelist));

	// add the instance names to the list
	instance	*inst=conffile->firstInstance();
	int		i=0;
	while (inst) {
		clisttext[0][0]=const_cast<char *>(inst->getId());
		gtk_clist_append(GTK_CLIST(instancelist),(gchar **)clisttext);
		gtk_clist_set_row_data(GTK_CLIST(instancelist),i,
					(gpointer)inst);
		inst=inst->nextInstance();
		i++;
	}

	// thaw the list
	gtk_clist_thaw(GTK_CLIST(instancelist));
}

void gtkfe::populateInstanceParameters(instance *inst) {

	// set the entries' text to the value from the DOM tree
	gtk_entry_set_text(GTK_ENTRY(identry),inst->getId());
	gtk_entry_set_text(GTK_ENTRY(portentry),inst->getPort());
	gtk_entry_set_text(GTK_ENTRY(unixportentry),inst->getUnixPort());
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(dbasecombo)->entry),
				inst->getDbase());
	gtk_entry_set_text(GTK_ENTRY(connectionsentry),
				inst->getConnections());
	gtk_entry_set_text(GTK_ENTRY(maxconnectionsentry),
				inst->getMaxConnections());
	gtk_entry_set_text(GTK_ENTRY(maxqueuelengthentry),
				inst->getMaxQueueLength());
	gtk_entry_set_text(GTK_ENTRY(growbyentry),inst->getGrowby());
	gtk_entry_set_text(GTK_ENTRY(ttlentry),inst->getTtl());
	if (!charstring::compare(inst->getEndOfSession(),"commit")) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(commitbutton),
						TRUE);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rollbackbutton),
						TRUE);
	}
	gtk_entry_set_text(GTK_ENTRY(sessiontimeoutentry),
				inst->getSessionTimeout());
	gtk_entry_set_text(GTK_ENTRY(runasuserentry),inst->getRunAsUser());
	gtk_entry_set_text(GTK_ENTRY(runasgroupentry),inst->getRunAsGroup());
	gtk_entry_set_text(GTK_ENTRY(cursorsentry),inst->getCursors());
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(authtiercombo)->entry),
				inst->getAuthTier());
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(handoffcombo)->entry),
				inst->getHandoff());
	gtk_entry_set_text(GTK_ENTRY(deniedipsentry),inst->getDeniedIps());
	gtk_entry_set_text(GTK_ENTRY(allowedipsentry),inst->getAllowedIps());
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(debugcombo)->entry),
				inst->getDebug());
}

void gtkfe::clearInstanceParameters() {

	// set the entries' text to ""
	gtk_entry_set_text(GTK_ENTRY(identry),"");
	gtk_entry_set_text(GTK_ENTRY(portentry),"");
	gtk_entry_set_text(GTK_ENTRY(unixportentry),"");
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(dbasecombo)->entry),"");
	gtk_entry_set_text(GTK_ENTRY(connectionsentry),"");
	gtk_entry_set_text(GTK_ENTRY(maxconnectionsentry),"");
	gtk_entry_set_text(GTK_ENTRY(maxqueuelengthentry),"");
	gtk_entry_set_text(GTK_ENTRY(growbyentry),"");
	gtk_entry_set_text(GTK_ENTRY(ttlentry),"");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(commitbutton),TRUE);
	gtk_entry_set_text(GTK_ENTRY(sessiontimeoutentry),"");
	gtk_entry_set_text(GTK_ENTRY(runasuserentry),"");
	gtk_entry_set_text(GTK_ENTRY(runasgroupentry),"");
	gtk_entry_set_text(GTK_ENTRY(cursorsentry),"");
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(authtiercombo)->entry),"");
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(handoffcombo)->entry),"");
	gtk_entry_set_text(GTK_ENTRY(deniedipsentry),"");
	gtk_entry_set_text(GTK_ENTRY(allowedipsentry),"");
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(debugcombo)->entry),"");
}

void gtkfe::defaultInstanceParameters() {

	// set the entries' text to default values
	gtk_entry_set_text(GTK_ENTRY(identry),"");
	gtk_entry_set_text(GTK_ENTRY(portentry),DEFAULT_PORT);
	gtk_entry_set_text(GTK_ENTRY(unixportentry),DEFAULT_SOCKET);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(dbasecombo)->entry),
							DEFAULT_DBASE);
	gtk_entry_set_text(GTK_ENTRY(connectionsentry),DEFAULT_CONNECTIONS);
	gtk_entry_set_text(GTK_ENTRY(maxconnectionsentry),
					DEFAULT_MAXCONNECTIONS);
	gtk_entry_set_text(GTK_ENTRY(maxqueuelengthentry),
					DEFAULT_MAXQUEUELENGTH);
	gtk_entry_set_text(GTK_ENTRY(growbyentry),DEFAULT_GROWBY);
	gtk_entry_set_text(GTK_ENTRY(ttlentry),DEFAULT_TTL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(commitbutton),TRUE);
	gtk_entry_set_text(GTK_ENTRY(sessiontimeoutentry),
					DEFAULT_SESSIONTIMEOUT);
	gtk_entry_set_text(GTK_ENTRY(runasuserentry),DEFAULT_RUNASUSER);
	gtk_entry_set_text(GTK_ENTRY(runasgroupentry),DEFAULT_RUNASGROUP);
	gtk_entry_set_text(GTK_ENTRY(cursorsentry),DEFAULT_CURSORS);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(authtiercombo)->entry),
							DEFAULT_AUTHTIER);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(handoffcombo)->entry),
							DEFAULT_HANDOFF);
	gtk_entry_set_text(GTK_ENTRY(deniedipsentry),DEFAULT_DENIEDIPS);
	gtk_entry_set_text(GTK_ENTRY(allowedipsentry),DEFAULT_ALLOWEDIPS);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(debugcombo)->entry),
							DEFAULT_DEBUG);
}

void gtkfe::populateUserList(instance *inst) {

	// freeze the list
	gtk_clist_freeze(GTK_CLIST(userlist));

	// clear the list for good measure
	gtk_clist_clear(GTK_CLIST(userlist));

	// add the user names to the list
	user	*usr=inst->firstUser();
	int	i=0;
	while (usr) {
		clisttext[0][0]=const_cast<char *>(usr->getUser());
		gtk_clist_append(GTK_CLIST(userlist),(gchar **)clisttext);
		gtk_clist_set_row_data(GTK_CLIST(userlist),i,
					(gpointer)usr);
		usr=usr->nextUser();
		i++;
	}

	// thaw the list
	gtk_clist_thaw(GTK_CLIST(userlist));
}

void gtkfe::populateUserParameters(user *usr) {

	// set the entries' text to the value from the DOM tree
	gtk_entry_set_text(GTK_ENTRY(userentry),usr->getUser());
	gtk_entry_set_text(GTK_ENTRY(passwordentry),usr->getPassword());
}

void gtkfe::clearUserParameters() {

	// set the entries' text to ""
	gtk_entry_set_text(GTK_ENTRY(userentry),"");
	gtk_entry_set_text(GTK_ENTRY(passwordentry),"");
}

void gtkfe::populateConnectionList(instance *inst) {

	// freeze the list
	gtk_clist_freeze(GTK_CLIST(connectionlist));

	// clear the list for good measure
	gtk_clist_clear(GTK_CLIST(connectionlist));

	// add the connection names to the list
	connection	*conn=inst->firstConnection();
	int		i=0;
	while (conn) {
		clisttext[0][0]=const_cast<char *>(conn->getConnectionId());
		gtk_clist_append(GTK_CLIST(connectionlist),(gchar **)clisttext);
		gtk_clist_set_row_data(GTK_CLIST(connectionlist),i,
					(gpointer)conn);
		conn=conn->nextConnection();
		i++;
	}

	// thaw the list
	gtk_clist_thaw(GTK_CLIST(connectionlist));
}

void gtkfe::populateConnectionParameters(connection *conn) {

	// set the entries' text to the value from the DOM tree
	gtk_entry_set_text(GTK_ENTRY(connectionidentry),
						conn->getConnectionId());
	gtk_entry_set_text(GTK_ENTRY(stringentry),conn->getString());
	gtk_entry_set_text(GTK_ENTRY(metricentry),conn->getMetric());
}

void gtkfe::clearConnectionParameters() {

	// set the entries' text to ""
	gtk_entry_set_text(GTK_ENTRY(connectionidentry),"");
	gtk_entry_set_text(GTK_ENTRY(stringentry),"");
	gtk_entry_set_text(GTK_ENTRY(metricentry),"");
}

void gtkfe::defaultConnectionParameters() {

	// set the entries' text to ""
	gtk_entry_set_text(GTK_ENTRY(connectionidentry),"");
	gtk_entry_set_text(GTK_ENTRY(stringentry),"");
	gtk_entry_set_text(GTK_ENTRY(metricentry),DEFAULT_METRIC);
}

void gtkfe::newFile(GtkWidget *widget, gpointer data) {

	// create a blank conffile
	if (conffile) {
		delete conffile;
	}
	conffile=new configfile();
	conffile->blank();

	// clear the entries and lists
	clearInstanceParameters();
	gtk_clist_clear(GTK_CLIST(instancelist));
	clearConnectionParameters();
	gtk_clist_clear(GTK_CLIST(connectionlist));
	clearUserParameters();
	gtk_clist_clear(GTK_CLIST(userlist));

	// set almost everything insensitive
	gtk_widget_set_sensitive(instanceframevbox,FALSE);
	gtk_widget_set_sensitive(deleteinstancebutton,FALSE);
	gtk_widget_set_sensitive(deleteuserbutton,FALSE);
	gtk_widget_set_sensitive(deleteconnectionbutton,FALSE);

	// make the instance list and new button sensitive
	gtk_widget_set_sensitive(instancelistvbox,TRUE);

	// set the close, save buttons sensitive
	gtk_widget_set_sensitive(saveitem,TRUE);
	gtk_widget_set_sensitive(saveasitem,TRUE);
	gtk_widget_set_sensitive(closeitem,TRUE);
}

void gtkfe::openFile(GtkWidget *widget, gpointer data) {

	// create a file dialog, attach callbacks to it's buttons
	fileselector=gtk_file_selection_new("Open");
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileselector)->
				ok_button),"clicked",
				GTK_SIGNAL_FUNC(openFileOk),
				(gpointer)NULL);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileselector)->
				cancel_button),"clicked",
				GTK_SIGNAL_FUNC(fileCancel),
				(gpointer)NULL);

	// make the dialog modal
	gtk_grab_add(fileselector);

	// show the dialog
	gtk_widget_show(fileselector);
}

void gtkfe::closeFile(GtkWidget *widget, gpointer data) {

	// close the conffile
	if (conffile) {
		delete conffile;
		conffile=(configfile *)NULL;
	}

	// clear the entries and lists
	clearInstanceParameters();
	gtk_clist_clear(GTK_CLIST(instancelist));
	clearConnectionParameters();
	gtk_clist_clear(GTK_CLIST(connectionlist));
	clearUserParameters();
	gtk_clist_clear(GTK_CLIST(userlist));

	// set everything insensitive
	gtk_widget_set_sensitive(instanceframevbox,FALSE);
	gtk_widget_set_sensitive(instancelistvbox,FALSE);
	gtk_widget_set_sensitive(deleteinstancebutton,FALSE);
	gtk_widget_set_sensitive(deleteuserbutton,FALSE);
	gtk_widget_set_sensitive(deleteconnectionbutton,FALSE);

	// set the close, save buttons insensitive
	gtk_widget_set_sensitive(saveitem,FALSE);
	gtk_widget_set_sensitive(saveasitem,FALSE);
	gtk_widget_set_sensitive(closeitem,FALSE);
}

void gtkfe::saveFile(GtkWidget *widget, gpointer data) {

	if (conffile->currentFile()) {
		// write the current DOM tree to the currently open file
		if (!conffile->write()) {
			okDialog("Could not save file.");
		}
	} else {
		// if no file is currently open, do the saveFileAs stuff
		saveFileAs(widget,data);
	}
}

void gtkfe::saveFileAs(GtkWidget *widget, gpointer data) {

	// create a file dialog, attach callbacks to it's buttons
	fileselector=gtk_file_selection_new("Save As...");
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileselector)->
				ok_button),"clicked",
				GTK_SIGNAL_FUNC(saveFileAsOk),
				(gpointer)NULL);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileselector)->
				cancel_button),"clicked",
				GTK_SIGNAL_FUNC(fileCancel),
				(gpointer)NULL);

	// make the dialog modal
	gtk_grab_add(fileselector);

	// show the dialog
	gtk_widget_show(fileselector);
}

void gtkfe::quit(GtkWidget *widget, gpointer data) {

	// clean up
	if (conffile) {
		delete conffile;
		conffile=(configfile *)NULL;
	}

	// quit
	gtk_main_quit();
}

void gtkfe::openFileOk(GtkWidget *widget, gpointer data) {

	// get the filename from the dialog
	handleFile(gtk_file_selection_get_filename(
				GTK_FILE_SELECTION(fileselector)));

	// de-modalize and destroy the dialog
	gtk_grab_remove(fileselector);
	gtk_widget_destroy(fileselector);
}

void gtkfe::handleFile(const char *filename) {

	// create an instance of configfile
	if (conffile) {
		delete conffile;
	}
	conffile=new configfile();

	// parse the file
	if (conffile->parse(filename)) {

		// if the parse succeeded...

		// build the instance list
		populateInstanceList();

		// make the instance list and new button sensitive
		gtk_widget_set_sensitive(instancelistvbox,TRUE);

	} else {

		// if the parse failed...

		// show a "bad file" dialog
		size_t	msglen=charstring::length(filename)+1+31+1;
		char	*message=new char[msglen];
		snprintf(message,msglen,
			"%s\nis not an SQL Relay config file.",filename);
		okDialog(message);
		delete[] message;
	}

	// set the close, save buttons sensitive
	gtk_widget_set_sensitive(saveitem,TRUE);
	gtk_widget_set_sensitive(saveasitem,TRUE);
	gtk_widget_set_sensitive(closeitem,TRUE);
}

void gtkfe::saveFileAsOk(GtkWidget *widget, gpointer data) {

	// write the file to the file name from the dialog
	if (conffile->write(gtk_file_selection_get_filename(
				GTK_FILE_SELECTION(fileselector)))) {
		// de-modalize and destroy the dialog
		gtk_grab_remove(fileselector);
		gtk_widget_destroy(fileselector);
	} else {
		// error message
		okDialog("Could not save file.");
	}
}

void gtkfe::fileCancel(GtkWidget *widget, gpointer data) {

	// de-modalize and destroy the dialog
	gtk_grab_remove(fileselector);
	gtk_widget_destroy(fileselector);
}

void gtkfe::instanceListSelect(GtkWidget *widget, gint row, gint column,
				GdkEventButton *event, gpointer data) {

	// if we were adding a new one already, delete it
	if (currentinstanceindex==-1) {
		conffile->deleteInstance(currentinstance);
	}

	// clear the entries and lists
	clearInstanceParameters();
	clearConnectionParameters();
	gtk_clist_clear(GTK_CLIST(connectionlist));
	clearUserParameters();
	gtk_clist_clear(GTK_CLIST(userlist));

	// get which instance was picked
	currentinstance=(instance *)gtk_clist_get_row_data(
					GTK_CLIST(instancelist),row);
	currentinstanceindex=row;

	// populate the entries and lists
	populateInstanceParameters(currentinstance);
	populateConnectionList(currentinstance);
	populateUserList(currentinstance);

	// sensitize the interface
	gtk_widget_set_sensitive(deleteinstancebutton,TRUE);
	gtk_widget_set_sensitive(instanceframevbox,TRUE);
}

void gtkfe::userListSelect(GtkWidget *widget, gint row, gint column,
				GdkEventButton *event, gpointer data) {

	// if we were adding a new one already, delete it
	if (currentuserindex==-1) {
		currentinstance->deleteUser(currentuser);
	}

	// clear the entries
	clearUserParameters();

	// get which user was picked
	currentuser=(user *)gtk_clist_get_row_data(GTK_CLIST(userlist),row);
	currentuserindex=row;

	// populate the entries
	populateUserParameters(currentuser);

	// sensitize the interface
	gtk_widget_set_sensitive(deleteuserbutton,TRUE);
	gtk_widget_set_sensitive(usersvbox,TRUE);
}

void gtkfe::connectionListSelect(GtkWidget *widget, gint row, gint column,
					GdkEventButton *event, gpointer data) {

	// if we were adding a new one already, delete it
	if (currentconnection && currentconnectionindex==-1) {
		currentinstance->deleteConnection(currentconnection);
	}

	// clear the entries
	clearConnectionParameters();

	// get which connection was picked
	currentconnection=(connection *)gtk_clist_get_row_data(
					GTK_CLIST(connectionlist),row);
	currentconnectionindex=row;

	// populate the entries
	populateConnectionParameters(currentconnection);

	// sensitize the interface
	gtk_widget_set_sensitive(deleteconnectionbutton,TRUE);
	gtk_widget_set_sensitive(connectionsvbox,TRUE);
}

void gtkfe::instanceParametersSave(GtkWidget *widget, gpointer data) {

	char	*id=gtk_entry_get_text(GTK_ENTRY(identry));

	if (!id[0]) {
		okDialog("The Id field cannot be blank.");
		return;
	}

	if (currentinstanceindex==-1 && conffile->findInstance(id)) {
		okDialog("The Id field must be unique.");
		return;
	}

	// set the DOM tree values to the values of the entries
	currentinstance->setId(id);
	currentinstance->setPort(
			gtk_entry_get_text(GTK_ENTRY(portentry)));
	currentinstance->setUnixPort(
			gtk_entry_get_text(GTK_ENTRY(unixportentry)));
	currentinstance->setDbase(
			gtk_entry_get_text(
			GTK_ENTRY(GTK_COMBO(dbasecombo)->entry)));
	currentinstance->setConnections(
			gtk_entry_get_text(
			GTK_ENTRY(connectionsentry)));
	currentinstance->setMaxConnections(
			gtk_entry_get_text(
			GTK_ENTRY(maxconnectionsentry)));
	currentinstance->setMaxQueueLength(
			gtk_entry_get_text(
			GTK_ENTRY(maxqueuelengthentry)));
	currentinstance->setGrowby(
			gtk_entry_get_text(GTK_ENTRY(growbyentry)));
	currentinstance->setTtl(
			gtk_entry_get_text(GTK_ENTRY(ttlentry)));
	if (commit) {
		currentinstance->setEndOfSession("commit");
	} else {
		currentinstance->setEndOfSession("rollback");
	}
	currentinstance->setSessionTimeout(
			gtk_entry_get_text(
			GTK_ENTRY(sessiontimeoutentry)));
	currentinstance->setRunAsUser(
			gtk_entry_get_text(GTK_ENTRY(runasuserentry)));
	currentinstance->setRunAsGroup(
			gtk_entry_get_text(GTK_ENTRY(runasgroupentry)));
	currentinstance->setCursors(
			gtk_entry_get_text(GTK_ENTRY(cursorsentry)));
	currentinstance->setAuthTier(
			gtk_entry_get_text(
			GTK_ENTRY(GTK_COMBO(authtiercombo)->entry)));
	currentinstance->setHandoff(
			gtk_entry_get_text(
			GTK_ENTRY(GTK_COMBO(handoffcombo)->entry)));
	currentinstance->setDeniedIps(
			gtk_entry_get_text(GTK_ENTRY(deniedipsentry)));
	currentinstance->setAllowedIps(
			gtk_entry_get_text(GTK_ENTRY(allowedipsentry)));
	currentinstance->setDebug(
			gtk_entry_get_text(
			GTK_ENTRY(GTK_COMBO(debugcombo)->entry)));

	// rebuild the instance list in case we were adding a new instance or
	// changing the id of an old one
	populateInstanceList();

	// if it was not a new instance unselect the instance 
	// in the list
	if (currentinstanceindex>-1) {
		gtk_clist_unselect_row(GTK_CLIST(instancelist),
					currentinstanceindex,0);
	}

	clearAllInstanceData();
}

void gtkfe::instanceParametersCancel(GtkWidget *widget, gpointer data) {

	if (currentinstanceindex==-1) {
		// if it was a new instance, delete it
		conffile->deleteInstance(currentinstance);
	} else {
		// if it was not a new instance unselect the instance 
		// in the list
		gtk_clist_unselect_row(GTK_CLIST(instancelist),
					currentinstanceindex,0);
	}

	clearAllInstanceData();
}

void gtkfe::clearAllInstanceData() {

	// clear all instance-related data
	clearInstanceParameters();
	currentinstance=(instance *)NULL;
	currentinstanceindex=-2;

	// clear all connection-related data
	clearConnectionParameters();
	currentconnection=(connection *)NULL;
	currentconnectionindex=-2;
	gtk_clist_clear(GTK_CLIST(connectionlist));

	// clear all user-related data
	clearUserParameters();
	currentuser=(user *)NULL;
	currentuserindex=-2;
	gtk_clist_clear(GTK_CLIST(userlist));

	// flip to the instance parameters page of the notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook),0);

	// desensitize the interface
	gtk_widget_set_sensitive(deleteinstancebutton,FALSE);
	gtk_widget_set_sensitive(instanceframevbox,FALSE);
}

void gtkfe::connectionParametersSave(GtkWidget *widget, gpointer data) {

	char	*conn=gtk_entry_get_text(GTK_ENTRY(connectionidentry));

	if (!conn[0]) {
		okDialog("The Connection Id field cannot be blank.");
		return;
	}

	if (currentconnectionindex==-1 && 
				currentinstance->findConnection(conn)) {
		okDialog("The Connection Id field must be unique.");
		return;
	}

	// set the DOM tree values to the values of the entries
	currentconnection->setConnectionId(conn);
	currentconnection->setString(
			gtk_entry_get_text(GTK_ENTRY(stringentry)));
	currentconnection->setMetric(
			gtk_entry_get_text(GTK_ENTRY(metricentry)));

	// rebuild the connection list in case we were adding a new 
	// connection or changing the id of an old one
	populateConnectionList(currentinstance);

	// if it was not a new instance unselect the instance 
	// in the list
	if (currentconnectionindex>-1) {
		gtk_clist_unselect_row(GTK_CLIST(connectionlist),
					currentconnectionindex,0);
	}

	clearAllConnectionData();
}

void gtkfe::connectionParametersCancel(GtkWidget *widget, gpointer data) {

	if (currentconnectionindex==-1) {
		// if it was a new connection, delete it
		currentinstance->deleteConnection(currentconnection);
	} else {
		// if it was not a new connection unselect the connection 
		// in the list
		gtk_clist_unselect_row(GTK_CLIST(connectionlist),
						currentconnectionindex,0);
	}

	clearAllConnectionData();
}

void gtkfe::clearAllConnectionData() {

	// clear all connection-related data
	clearConnectionParameters();
	currentconnection=(connection *)NULL;
	currentconnectionindex=-2;

	// desensitize the interface
	gtk_widget_set_sensitive(deleteconnectionbutton,FALSE);
	gtk_widget_set_sensitive(connectionsvbox,FALSE);
}

void gtkfe::userParametersSave(GtkWidget *widget, gpointer data) {

	char	*usr=gtk_entry_get_text(GTK_ENTRY(userentry));

	if (!usr[0]) {
		okDialog("The User field cannot be blank.");
		return;
	}

	if (currentuserindex==-1 && currentinstance->findUser(usr)) {
		okDialog("The User field must be unique.");
		return;
	}

	// set the DOM tree values to the values of the entries
	currentuser->setUser(usr);
	currentuser->setPassword(gtk_entry_get_text(GTK_ENTRY(passwordentry)));

	// rebuild the connection list in case we were adding a new 
	// connection or changing the id of an old one
	populateUserList(currentinstance);

	// if it was not a new user unselect the user in the list
	if (currentuserindex>-1) {
		gtk_clist_unselect_row(GTK_CLIST(userlist),
					currentuserindex,0);
	}

	clearAllUserData();
}

void gtkfe::userParametersCancel(GtkWidget *widget, gpointer data) {

	if (currentuserindex==-1) {
		// if it was a new user, delete it
		currentinstance->deleteUser(currentuser);
	} else {
		// if it was not a new user unselect the user in the list
		gtk_clist_unselect_row(GTK_CLIST(userlist),
						currentuserindex,0);
	}

	clearAllUserData();
}

void gtkfe::clearAllUserData() {

	// clear all user-related data
	clearUserParameters();
	currentuser=(user *)NULL;
	currentuserindex=-2;

	// desensitize the interface
	gtk_widget_set_sensitive(deleteuserbutton,FALSE);
	gtk_widget_set_sensitive(usersvbox,FALSE);
}

void gtkfe::okDialog(const char *message) {

	// create a dialog
	okdialog=gtk_dialog_new();


	// put a label on the dialog
	okdialoglabel=gtk_label_new(message);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(okdialog)->vbox),
				okdialoglabel,TRUE,TRUE,0);
	gtk_widget_show(okdialoglabel);


	// put an ok button on the dialog
	okdialogbutton=gtk_button_new_with_label("Ok");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(okdialog)->action_area),
				okdialogbutton,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT(okdialogbutton),"clicked",
					GTK_SIGNAL_FUNC(dialogOk),
					(gpointer)NULL);
	gtk_widget_show(okdialogbutton);


	// make it modal
	gtk_grab_add(okdialog);

	// show the dialog
	gtk_widget_show(okdialog);
}

void gtkfe::dialogOk(GtkWidget *widget, gpointer data) {

	// de-modalize and destroy the dialog
	gtk_grab_remove(okdialog);
	gtk_widget_destroy(okdialog);
}

void gtkfe::newInstance(GtkWidget *widget, gpointer data) {

	// if we were adding a new one already, delete it
	if (currentinstance && currentinstanceindex==-1) {
		conffile->deleteInstance(currentinstance);
	}

	// unselect the selected instance
	if (currentinstanceindex>-1) {
		gtk_clist_unselect_row(GTK_CLIST(instancelist),
					currentinstanceindex,0);
	}

	// clear all instance-related data
	clearAllInstanceData();
	currentinstanceindex=-1;

	// create a blank instance
	currentinstance=conffile->addInstance("",
					DEFAULT_PORT,
					DEFAULT_SOCKET,
					DEFAULT_DBASE,
					DEFAULT_CONNECTIONS,
					DEFAULT_MAXCONNECTIONS,
					DEFAULT_MAXQUEUELENGTH,
					DEFAULT_GROWBY,
					DEFAULT_TTL,
					DEFAULT_ENDOFSESSION,
					DEFAULT_SESSIONTIMEOUT,
					DEFAULT_RUNASUSER,
					DEFAULT_RUNASGROUP,
					DEFAULT_CURSORS,
					DEFAULT_AUTHTIER,
					DEFAULT_HANDOFF,
					DEFAULT_DENIEDIPS,
					DEFAULT_ALLOWEDIPS,
					DEFAULT_DEBUG);

	defaultInstanceParameters();

	// sensitize the interface
	gtk_widget_set_sensitive(deleteinstancebutton,TRUE);
	gtk_widget_set_sensitive(instanceframevbox,TRUE);
}

void gtkfe::deleteInstance(GtkWidget *widget, gpointer data) {

	// unselect the selected instance
	gtk_clist_unselect_row(GTK_CLIST(instancelist),currentinstanceindex,0);

	// remove the instance from the list
	gtk_clist_remove(GTK_CLIST(instancelist),currentinstanceindex);

	// clear all instance-related data
	conffile->deleteInstance(currentinstance);
	clearAllInstanceData();
}

void gtkfe::newConnection(GtkWidget *widget, gpointer data) {

	// if we were adding a new one already, delete it
	if (currentconnectionindex==-1) {
		currentinstance->deleteConnection(currentconnection);
	}

	// unselect the selected user
	if (currentconnectionindex>-1) {
		gtk_clist_unselect_row(GTK_CLIST(connectionlist),
					currentconnectionindex,0);
	}

	// clear all connection-related data
	clearAllConnectionData();
	currentconnectionindex=-1;

	// create a blank connection
	currentconnection=currentinstance->addConnection("","",DEFAULT_METRIC);

	defaultConnectionParameters();

	// sensitize the interface
	gtk_widget_set_sensitive(deleteconnectionbutton,TRUE);
	gtk_widget_set_sensitive(connectionsvbox,TRUE);
}

void gtkfe::deleteConnection(GtkWidget *widget, gpointer data) {

	// unselect the selected instance
	gtk_clist_unselect_row(GTK_CLIST(connectionlist),
					currentconnectionindex,0);

	// remove the instance from the list
	gtk_clist_remove(GTK_CLIST(connectionlist),currentconnectionindex);

	// clear all connection-related data
	currentinstance->deleteConnection(currentconnection);
	clearAllConnectionData();
}

void gtkfe::newUser(GtkWidget *widget, gpointer data) {

	// if we were adding a new one already, delete it
	if (currentuser && currentuserindex==-1) {
		currentinstance->deleteUser(currentuser);
	}

	// unselect the selected user
	if (currentuserindex>-1) {
		gtk_clist_unselect_row(GTK_CLIST(userlist),
					currentuserindex,0);
	}

	// clear all user-related data
	clearAllUserData();
	currentuserindex=-1;

	// create a blank user
	currentuser=currentinstance->addUser("","");

	// sensitize the interface
	gtk_widget_set_sensitive(deleteuserbutton,TRUE);
	gtk_widget_set_sensitive(usersvbox,TRUE);
}

void gtkfe::deleteUser(GtkWidget *widget, gpointer data) {

	// unselect the selected instance
	gtk_clist_unselect_row(GTK_CLIST(userlist),currentuserindex,0);

	// remove the instance from the list
	gtk_clist_remove(GTK_CLIST(userlist),currentuserindex);

	// clear all user-related data
	currentinstance->deleteUser(currentuser);
	clearAllUserData();
}

void gtkfe::toggleCommit(GtkWidget *widget, gpointer data) {
	if (GTK_TOGGLE_BUTTON(widget)->active) {
		commit=1;
	} else {
		commit=0;
	}
}



int	main(int argc, char **argv) {

	#include <version.h>

	gtkfe	g;
	g.run(argc, argv);
}
