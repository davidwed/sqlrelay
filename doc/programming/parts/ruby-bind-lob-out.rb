require('sqlrelay')

con=SQLRConnection.new("sqlrserver",9000,"/tmp/example.socket","user","password",0,1)
cur=SQLRCursor.new(con)

cur->prepareQuery("begin  select image into :image from images;  select description into :desc from images;  end;")
cur->defineOutputBindBlob("image")
cur->defineOutputBindClob("desc")
cur->executeQuery()

image=cur->getOutputBindBlob("image")
imagelength=cur->getOutputBindLength("image")

desc=cur->getOutputBindClob("desc")
desclength=cur->getOutputBindLength("desc")

con->endSession()

... do something with image and desc ...
