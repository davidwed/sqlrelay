// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

#ifdef __CYGWIN__
	#include <windows.h>
#endif

#include <sqlrelay/sqlrclient.h>
#include <com_firstworks_sqlrelay_SQLRConnection.h>

#ifdef __cplusplus
extern "C" {
#endif

char	*conGetStringUTFChars(JNIEnv *env, jstring string, jboolean *modifier) {
	if (string) {
		return (char *)env->GetStringUTFChars(string,modifier);
	}
	return (char *)NULL;
}

void	conReleaseStringUTFChars(JNIEnv *env, jstring string, char *chararray) {
	if (string && chararray) {
		env->ReleaseStringUTFChars(string,chararray);
	}
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    alloc
 * Signature: (Ljava/lang/String;SLjava/lang/String;Ljava/lang/String;Ljava/lang/String;II)V
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_alloc(JNIEnv *env, 
			jobject self, 
			jstring host, jshort port, jstring socket, 
			jstring user, jstring password, 
			jint retrytime, jint tries) {

	char	*hoststring=conGetStringUTFChars(env,host,0);
	char	*socketstring=conGetStringUTFChars(env,socket,0);
	char	*userstring=conGetStringUTFChars(env,user,0);
	char	*passwordstring=conGetStringUTFChars(env,password,0);

	sqlrconnection	*con=(sqlrconnection *)
				new sqlrconnection(hoststring,(uint16_t)port,
						socketstring,
						userstring,passwordstring,
						(int32_t)retrytime,
						(int32_t)tries);
	con->copyReferences();

	conReleaseStringUTFChars(env,host,hoststring);
	conReleaseStringUTFChars(env,socket,socketstring);
	conReleaseStringUTFChars(env,user,userstring);
	conReleaseStringUTFChars(env,password,passwordstring);
	return (jlong)con;
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
				env->GetFieldID(cls,"connection","J"));
	delete con;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    endSession
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_endSession
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	con->endSession();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    suspendSession
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_suspendSession
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return (jboolean)con->suspendSession();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getConnectionPort
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getConnectionPort
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return (jshort)con->getConnectionPort();
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
				env->GetFieldID(cls,"connection","J"));
	return env->NewStringUTF(con->getConnectionSocket());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    resumeSession
 * Signature: (SLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_resumeSession
  (JNIEnv *env, jobject self, jshort port, jstring socket) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	char	*socketstring=conGetStringUTFChars(env,socket,0);
	bool	retval=con->resumeSession((uint16_t)port,socketstring);
	conReleaseStringUTFChars(env,socket,socketstring);
	return (jboolean)retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    ping
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_ping
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return (jboolean)cur->ping();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    autoCommitOn
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_autoCommitOn
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return (jboolean)cur->autoCommitOn();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    autoCommitOff
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_autoCommitOff
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return (jboolean)cur->autoCommitOff();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    commit
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_commit
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return (jboolean)cur->commit();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    rollback
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_rollback
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return (jboolean)cur->rollback();
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
				env->GetFieldID(cls,"connection","J"));
	return env->NewStringUTF(cur->identify());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    dbVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_dbVersion
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return env->NewStringUTF(cur->dbVersion());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    serverVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_serverVersion
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return env->NewStringUTF(cur->serverVersion());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    clientVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_clientVersion
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return env->NewStringUTF(cur->clientVersion());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    bindFormat
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_bindFormat
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection	*cur=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return env->NewStringUTF(cur->bindFormat());
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
				env->GetFieldID(cls,"connection","J"));
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
				env->GetFieldID(cls,"connection","J"));
	con->debugOff();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getDebug
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getDebug
  (JNIEnv *env, jobject self) {
	jclass		cls=env->GetObjectClass(self);
	sqlrconnection 	*con=(sqlrconnection *)env->GetIntField(self,
				env->GetFieldID(cls,"connection","J"));
	return (jboolean)con->getDebug();
}

#ifdef __cplusplus
}
#endif
