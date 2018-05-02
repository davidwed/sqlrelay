load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

$cur executeQuery "create table images (image blob, description clob)"

... read an image from a file into imagedata and the length of the
        file into imagelength ...

... read a description from a file into description and the length of
        the file into desclength ...

$cur prepareQuery "insert into images values (:image,:desc)"
$cur inputBindBlob "image" $imagedata $imagelength
$cur inputBindClob "desc" $description $desclength
$cur executeQuery
