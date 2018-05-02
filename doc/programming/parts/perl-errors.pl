use SQLRelay::Connection;
use SQLRelay::Cursor;

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

if (!$cur->sendQuery("select * from my_nonexistant_table")) {
        printf("%s\n",$cur->errorMessage());
}
