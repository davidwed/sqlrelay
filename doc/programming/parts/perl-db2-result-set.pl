$cur->sendQuery("select * from examplefunc() as (exampleint int, examplefloat float, examplechar char(40))");
my $field00=$cur->getField(0,0);
my $field01=$cur->getField(0,1);
my $field02=$cur->getField(0,2);
my $field10=$cur->getField(1,0);
my $field11=$cur->getField(1,1);
my $field12=$cur->getField(1,2);
