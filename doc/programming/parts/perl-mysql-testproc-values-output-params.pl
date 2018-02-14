$cur->sendQuery("set \@out1=0, \@out2=0.0, \@out3=''");
$cur->sendQuery("call exampleproc(\@out1,\@out2,\@out3)");
$cur->sendQuery("select \@out1,\@out2,\@out3");
my $out1=$cur->getField(0,0);
my $out2=$cur->getField(0,1);
my $out3=$cur->getField(0,2);
