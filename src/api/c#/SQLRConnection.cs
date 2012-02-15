using System.Runtime.InteropServices;

public class SQLRConnection
{
    [DllImport("libsqlrclientwrapper.dll")]
    static public extern IntPtr sqlrcon_alloc(string server,
                                        ushort port, string socket,
                                        string user, string password, 
                                        int retrytime, int tries);

    static void Main(string[] args)
    {
        IntPtr a=sqlrcon_alloc("fedora",9000,null,"test","test",0,1);
    }
}
