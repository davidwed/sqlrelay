package com.firstworks.sql;

public class SQLRelayDebug {

	public static final boolean	debug=true;

	public void debugFunction() {
		if (debug) {
			System.out.println("\n"+this.getClass().
							getSimpleName()+"."+
						new Throwable().
							getStackTrace()[1].
							getMethodName()+"...");
		}
	}

	public void debugPrintln(String str) {
		if (debug) {
			System.out.println(str);
		}
	}
}
