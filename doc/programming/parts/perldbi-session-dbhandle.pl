my $dbh=DBI->connect("DBI:SQLRelay(AutoCommit=>0,PrintError=>0):host=sqlrserver;port=9000;socket=/tmp/test.socket;tries=0;retrytime=1;debug=0","testuser","testpassword"); 
