$cur->sendQuery("select testproc()");
my $result=$cur->getField(0,0);
