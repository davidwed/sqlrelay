// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

using System;
using Npgsql;
using System.Data;
using System.IO;
using System.Drawing;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace NpgsqlClientTest
{
    class NpgsqlAdapterTest
    {

        public static void Main(String[] args)
        {

		using (var conn=new NpgsqlConnection(
				"Host=localhost;Username=testuser;" +
				"Password=testpassword;Database=testdb;" +
				"Pooling=false")) {
			conn.Open();
			using (var cmd=new NpgsqlCommand()) {
				cmd.Connection=conn;
				cmd.CommandText="select $1";
				cmd.Parameters.AddWithValue("1","1");
				using (var reader=cmd.ExecuteReader()) {
					while (reader.Read()) {
						Console.WriteLine(
							reader.GetString(0));
					}
				}
			}
		}
        }
    }
}
