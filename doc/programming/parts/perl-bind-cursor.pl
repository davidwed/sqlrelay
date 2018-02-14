use SQLRelay::Connection;
use SQLRelay::Cursor;

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

$cur->prepareQuery("begin  :curs:=sp_mytable; end;");
$cur->defineOutputBindCursor("curs");
$cur->executeQuery();

my $bindcur=$cur->getOutputBindCursor("curs");
$bindcur->fetchFromBindCursor();

# print fields from table
for ($i=0; $i<$bindcur->rowCount(); $i++) {
        for ($j=0; $j<$bindcur->colCount(); $j++) {
                print($bindcur->getField($i,$j).", ");
        }
        print("\n");
}
