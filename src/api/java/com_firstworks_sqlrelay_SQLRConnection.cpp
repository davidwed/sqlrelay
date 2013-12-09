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

static sqlrconnection *getSqlrConnection(JNIEnv *env, jobject self) {
	return reinterpret_cast<sqlrconnection *>(
			env->GetLongField(self,
				env->GetFieldID(env->GetObjectClass(self),
				"connection","J")));
}

static char *conGetStringUTFChars(JNIEnv *env, jstring string, jboolean *modifier) {
	if (string) {
		return (char *)env->GetStringUTFChars(string,modifier);
	}
	return (char *)NULL;
}

static void conReleaseStringUTFChars(JNIEnv *env, jstring string, char *chararray) {
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

	sqlrconnection	*con=new sqlrconnection(hoststring,(uint16_t)port,
						socketstring,
						userstring,passwordstring,
						(int32_t)retrytime,
						(int32_t)tries,
						true);

	conReleaseStringUTFChars(env,host,hoststring);
	conReleaseStringUTFChars(env,socket,socketstring);
	conReleaseStringUTFChars(env,user,userstring);
	conReleaseStringUTFChars(env,password,passwordstring);
	return reinterpret_cast<jlong>(con);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    delete
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_delete
  (JNIEnv *env, jobject self) {
	delete getSqlrConnection(env,self);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    setConnectTimeout
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_setConnectTimeout
  (JNIEnv *env, jobject self, jint timeoutsec, jint timeoutusec) {
	getSqlrConnection(env,self)->
		setConnectTimeout((int32_t)timeoutsec,(int32_t)timeoutusec);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    setAuthenticationTimeout
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_setAuthenticationTimeout
  (JNIEnv *env, jobject self, jint timeoutsec, jint timeoutusec) {
	getSqlrConnection(env,self)->
		setAuthenticationTimeout((int32_t)timeoutsec,(int32_t)timeoutusec);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    setResponseTimeout
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_setResponseTimeout
  (JNIEnv *env, jobject self, jint timeoutsec, jint timeoutusec) {
	getSqlrConnection(env,self)->
		setResponseTimeout((int32_t)timeoutsec,(int32_t)timeoutusec);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    endSession
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_endSession
  (JNIEnv *env, jobject self) {
	getSqlrConnection(env,self)->endSession();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    suspendSession
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_suspendSession
  (JNIEnv *env, jobject self) {
	return (jboolean)getSqlrConnection(env,self)->suspendSession();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getConnectionPort
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getConnectionPort
  (JNIEnv *env, jobject self) {
	return (jshort)getSqlrConnection(env,self)->getConnectionPort();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getConnectionSocket
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getConnectionSocket
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(
			getSqlrConnection(env,self)->getConnectionSocket());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    resumeSession
 * Signature: (SLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_resumeSession
  (JNIEnv *env, jobject self, jshort port, jstring socket) {
	char	*socketstring=conGetStringUTFChars(env,socket,0);
	bool	retval=getSqlrConnection(env,self)->
				resumeSession((uint16_t)port,socketstring);
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
	return (jboolean)getSqlrConnection(env,self)->ping();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    selectDatabase
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_selectDatabase
  (JNIEnv *env, jobject self, jstring database) {
	char	*databasestring=conGetStringUTFChars(env,database,0);
	bool	retval=getSqlrConnection(env,self)->
				selectDatabase(databasestring);
	conReleaseStringUTFChars(env,database,databasestring);
	return (jboolean)retval;
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getCurrentDatabase
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getCurrentDatabase
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->getCurrentDatabase());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getLastInsertId
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getLastInsertId
  (JNIEnv *env, jobject self) {
	return (jlong)getSqlrConnection(env,self)->getLastInsertId();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    autoCommitOn
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_autoCommitOn
  (JNIEnv *env, jobject self) {
	return (jboolean)getSqlrConnection(env,self)->autoCommitOn();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    autoCommitOff
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_autoCommitOff
  (JNIEnv *env, jobject self) {
	return (jboolean)getSqlrConnection(env,self)->autoCommitOff();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    begin
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_begin
  (JNIEnv *env, jobject self) {
	return (jboolean)getSqlrConnection(env,self)->begin();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    commit
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_commit
  (JNIEnv *env, jobject self) {
	return (jboolean)getSqlrConnection(env,self)->commit();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    rollback
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_rollback
  (JNIEnv *env, jobject self) {
	return (jboolean)getSqlrConnection(env,self)->rollback();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    identify
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_identify
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->identify());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    dbVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_dbVersion
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->dbVersion());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    dbHostName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_dbHostName
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->dbHostName());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    dbIpAddress
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_dbIpAddress
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->dbIpAddress());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    serverVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_serverVersion
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->serverVersion());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    clientVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_clientVersion
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->clientVersion());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    bindFormat
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_bindFormat
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->bindFormat());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    errorMessage
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_errorMessage
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->errorMessage());
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    errorNumber
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_errorNumber
  (JNIEnv *env, jobject self) {
	return (jlong)getSqlrConnection(env,self)->errorNumber();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    debugOn
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_debugOn
  (JNIEnv *env, jobject self) {
	getSqlrConnection(env,self)->debugOn();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    debugOff
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_debugOff
  (JNIEnv *env, jobject self) {
	getSqlrConnection(env,self)->debugOff();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getDebug
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getDebug
  (JNIEnv *env, jobject self) {
	return (jboolean)getSqlrConnection(env,self)->getDebug();
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    setDebugFile
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_setDebugFile
  (JNIEnv *env, jobject self, jstring debugfile) {
	char	*debugfilestring=conGetStringUTFChars(env,debugfile,0);
	getSqlrConnection(env,self)->setDebugFile(debugfilestring);
	conReleaseStringUTFChars(env,debugfile,debugfilestring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    setClientInfo
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_setClientInfo
  (JNIEnv *env, jobject self, jstring clientinfo) {
	char	*clientinfostring=conGetStringUTFChars(env,clientinfo,0);
	getSqlrConnection(env,self)->setClientInfo(clientinfostring);
	conReleaseStringUTFChars(env,clientinfo,clientinfostring);
}

/*
 * Class:     com_firstworks_sqlrelay_SQLRConnection
 * Method:    getClientInfo
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_firstworks_sqlrelay_SQLRConnection_getClientInfo
  (JNIEnv *env, jobject self) {
	return env->NewStringUTF(getSqlrConnection(env,self)->getClientInfo());
}

#ifdef __cplusplus
}
#endif
