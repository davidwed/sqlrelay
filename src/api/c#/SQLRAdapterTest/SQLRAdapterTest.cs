using System;
using SQLRClient;

namespace SQLRClientTest
{
    class SQLRAdapterTest
    {
        public static void Main(string[] args)
        {
            SQLRelayConnection sqlrcon = new SQLRelayConnection("host=fedora;port=9000;socket=;user=test;password=test;retrytime=0;tries=1");

            sqlrcon.Open();

            SQLRelayCommand sqlrcom = new SQLRelayCommand("select 1 from dual", sqlrcon);
            sqlrcom.ExecuteNonQuery();

            sqlrcon.Close();
        }
    }
}