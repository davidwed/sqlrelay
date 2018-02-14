use SQLRelay::Connection;
use SQLRelay::Cursor;

$con=SQLRelay::Connection->new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1);
$cur=SQLRelay::Cursor->new($con);

my $imagedata;
my $imagelength;

... read an image from a file into imagedata and the length of the
        file into imagelength ...

my $description;
my $desclength;

... read a description from a file into description and the length of
        the file into desclength ...

$cur->prepareQuery("insert into images values (:image,:desc)");
$cur->inputBindBlob("image",$imagedata,$imagelength);
$cur->inputBindClob("desc",$description,$desclength);
$cur->executeQuery();
