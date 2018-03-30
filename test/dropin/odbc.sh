rm -f odbc.class
/home/dmuse/software/jdk1.7.0_80/bin/javac odbc.java

LD_PRELOAD=/usr/lib64/libodbc.so LD_LIBRARY_PATH=/home/dmuse/software/jdk1.7.0_80/jre/lib/amd64 /home/dmuse/software/jdk1.7.0_80/bin/java -cp ./ odbc
