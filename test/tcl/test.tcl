#!/usr/bin/tclsh

load ../../src/api/tcl/sqlrelay.so sqlrelay

set con [sqlrcon -server localhost -port 8009 -user oracle8test -password oracle8test]
set cur [$con sqlrcur]

$cur sendQuery "select * from testtable"
