from SQLRelay import PySQLRClient

con=PySQLRClient.sqlrconnection('sqlrserver',9000,'/tmp/example.socket','user','password',0,1)
cur=PySQLRClient.sqlrcursor(con)

imagedata=none
imagelength=none

... read an image from a file into imagedata and the length of the
        file into imagelength ...

description=none
desclength=none

... read a description from a file into description and the length of
        the file into desclength ...

cur.prepareQuery('insert into images values (:image,:desc)')
cur.inputBindBlob('image',imagedata,imagelength)
cur.inputBindClob('desc',description,desclength)
cur.executeQuery()
