use SQLRelay::Connection;
use SQLRelay::Cursor;

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

# column names will be forced to upper case
$cur->upperCaseColumnNames();
$cur->sendQuery("select * from my_table");
$con->endSession();

for ($i=0; $i<$cur->colCount(); $i++) {
        print("Name:          ".cur->getColumnName(i)."\n");
}

# column names will be forced to lower case
$cur->lowerCaseColumnNames();
$cur->sendQuery("select * from my_table");
$con->endSession();

for ($i=0; $i<$cur->colCount(); $i++) {
        print("Name:          ".cur->getColumnName(i)."\n");
}

# column names will be the same as they are in the database
$cur->mixedCaseColumnNames();
$cur->sendQuery("select * from my_table");
$con->endSession();

for ($i=0; $i<$cur->colCount(); $i++) {
        print("Name:          ".cur->getColumnName(i)."\n");
}
