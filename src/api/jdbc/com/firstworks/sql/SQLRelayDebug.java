package com.firstworks.sql;

public class SQLRelayDebug {

	public static final boolean	debug=true;

	public void debugFunction() {
		if (!debug) {
			return;
		}
		System.out.println("\n"+this.getClass(). getSimpleName()+"."+
					new Throwable().
						getStackTrace()[1].
						getMethodName()+"...");
	}

	public void debugPrint(String str) {
		if (!debug) {
			return;
		}
		System.out.print(str);
	}

	public void debugPrintln(String str) {
		if (!debug) {
			return;
		}
		System.out.println(str);
	}

	private static final char[] hexarray="0123456789ABCDEF".toCharArray();
	public void debugPrint(byte[] bytes) {
		if (!debug) {
			return;
		}
		for (int i=0; i<bytes.length; i++) {
			if (bytes[i]>=' ' && bytes[i]<='~') {
				System.out.print((char)bytes[i]);
			} else {
				int	b=bytes[i]&0xff;
				System.out.print(hexarray[b>>>4]);
				System.out.print(hexarray[b&0x0f]);
			}
		}
	}
}
