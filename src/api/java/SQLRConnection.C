// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

#include <sqlrelay/sqlrclient.h>
#include <SQLRConnection.h>

//#ifdef __cplusplus
extern "C" {
//#endif

char	*conGetStringUTFChars(JNIEnv *env, jstring string, jboolean *modifier) {
	if (string) {
		return (char *)env->GetStringUTFChars(string,modifier);
	}
	return (char *)NULL;
}

void	conReleaseStringUTFChars(JNIEnv *env, jstring string, char *chararray) {
	if (string) {
		env->ReleaseStringUTFChars(string,chararray);
	}
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    alloc
 * Signature: (Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;II)V
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_alloc(JNIEnv *env, 
			jobject self, 
			jstring host, jint port, jstring socket, 
			jstring user, jstring password, 
			jint retrytime, jint tries) {

	char	*hoststring=conGetStringUTFChars(env,host,0);
	char	*socketstring=conGetStringUTFChars(env,socket,0);
	char	*userstring=conGetStringUTFChars(env,user,0);
	char	*passwordstring=conGetStringUTFChars(env,password,0);

	sqlrconnection	*con=(sqlrconnection *)
				new sqlrconnection(hoststring,(int)port,
						socketstring,
						userstring,passwordstring,
						(int)retrytime,(int)tries);
	con->copyReferences();

	conReleaseStringUTFChars(env,host,hoststring);
	conReleaseStringUTFChars(env,socket,socketstring);
	conReleaseStringUTFChars(env,user,userstring);
	conReleaseStringUTFChars(env,password,passwordstring);
	return (jint)con;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    delete
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_delete
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	delete con;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    endSession
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_endSession
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return (jint)con->endSession();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    suspendSession
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_suspendSession
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return (jint)con->suspendSession();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getConnectionPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getConnectionPort
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return (jint)con->getConnectionPort();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getConnectionSocket
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getConnectionSocket
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return env->NewStringUTF(con->getConnectionSocket());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    resumeSession
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_resumeSession
  (JNIEnv *env, jobject self, jint port, jstring socket) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	char	*socketstring=conGetStringUTFChars(env,socket,0);
	int	retval=con->resumeSession((int)port,socketstring);
	conReleaseStringUTFChars(env,socket,socketstring);
	return (jint)retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    ping
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_ping
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return (jint)cur->ping();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    autoCommitOn
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_autoCommitOn
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return (jint)cur->autoCommitOn();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    autoCommitOff
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_autoCommitOff
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return (jint)cur->autoCommitOff();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    commit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_commit
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return (jint)cur->commit();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    rollback
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_rollback
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return (jint)cur->rollback();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    identify
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_identify
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return env->NewStringUTF(cur->identify());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    debugOn
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_debugOn
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	con->debugOn();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    debugOff
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_debugOff
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	con->debugOff();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getDebug
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getDebug
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","I"));
	return (jint)con->getDebug();
}

//#ifdef __cplusplus
}
//#endif
