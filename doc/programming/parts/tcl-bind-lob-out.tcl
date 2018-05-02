load /usr/lib/sqlrelay/sqlrelay.so sqlrelay

set con [sqlrcon -server "host" -port 9000 -socket "/tmp/example.socket" -user "user" -password "password" -retrytime 0 -tries 1]
set cur [$con sqlrcur]

$cur prepareQuery "begin  select image into :image from images;  select description into :desc from images;  end;"
$cur defineOutputBindBlob "image"
$cur defineOutputBindClob "desc"
$cur executeQuery

set $image [$cur getOutputBindBlob "image"]
set $imagelength [$cur getOutputBindLength "image"]

set $desc [$cur getOutputBindClob "desc"]
set $desclength [$cur getOutputBindLength "desc"]

$con endSession

... do something with image and desc ...
