#! /usr/bin/env perl

# Copyright (c) 2001  David Muse
# See the file COPYING for more information.


use DBI;

sub checkUndef {

	$value=shift(@_);

	if (!defined($value)) {
		print("success ");
	} else {
		print("failure ");
		exit;
	}
}

sub checkSuccess {

	$value=shift(@_);
	$success=shift(@_);

	if ($value==$success) {
		print("success ");
	} else {
		print("$value != $success ");
		print("failure ");
		exit;
	}
}

sub checkSuccessString {

	$value=shift(@_);
	$success=shift(@_);

	if ($value eq $success) {
		print("success ");
	} else {
		print("$value != $success ");
		print("failure ");
		exit;
	}
}


# instantiation
#my $dbh=DBI->connect("DBI:SQLRelay:host=localhost;port=9000;socket=/tmp/test.socket;debug=1","test","test",{AutoCommit=>0}) or die DBI->errstr;
my $dbh=DBI->connect("DBI:SQLRelay:host=localhost;port=9000;socket=/tmp/test.socket;","test","test",{AutoCommit=>0}) or die DBI->errstr;


# ping
print("PING: \n");
checkSuccess($dbh->ping(),1);
print("\n");

# drop existing table
$dbh->do("drop table testtable");

print("CREATE TEMPTABLE: \n");
checkSuccessString($dbh->do("create table testtable (testnumber number, testchar char(40), testvarchar varchar2(40), testdate date)"),"0E0");
print("\n");

print("INSERT and AFFECTED ROWS: \n");
checkSuccess($dbh->do("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001')"),1);
print("\n");

print("EXECUTE WITH BIND VALUES: \n");
my $sth=$dbh->prepare("insert into testtable values (:var1,:var2,:var3,:var4)");
checkSuccess($sth->execute(2,"testchar2","testvarchar2","01-JAN-2002"),1);
print("\n");

print("AFFECTED ROWS: \n");
checkSuccess($sth->rows(),1);
print("\n");

print("BIND PARAM BY POSITION: \n");
$sth->bind_param("1",3);
$sth->bind_param("2","testchar3");
$sth->bind_param("3","testvarchar3");
$sth->bind_param("4","01-JAN-2003");
checkSuccess($sth->execute(),1);
print("\n");

print("PARAM COUNT: \n");
checkSuccess($sth->{NUM_OF_PARAMS},4);
print("\n");

#print("EXECUTE ARRAY: \n");
#@var1s=(4,5,6);
#@var2s=("testchar4","testchar5","testchar6");
#@var3s=("testvarchar4","testvarchar5","testvarchar6");
#@var4s=("01-JAN-2004","01-JAN-2005","01-JAN-2006");
#checkSuccess($sth->execute_array({ ArrayTupleStatus => \my @tuple_status },\@var1s,\@var2s,\@var3s,\@var4s),3);
#for (my $index=0; $index<4; $index++) {
	#checkSuccess(@tuple_status[$index]->[0],1);
#}
#print("\n");

#print("BIND PARAM ARRAY: \n");

print("BIND BY NAME: \n");
$sth->bind_param("var1",4);
$sth->bind_param("var2","testchar4");
$sth->bind_param("var3","testvarchar4");
$sth->bind_param("var4","01-JAN-2004");
checkSuccess($sth->execute(),1);
print("\n");

print("BIND BY NAME: \n");
$sth->bind_param("var1",5);
$sth->bind_param("var2","testchar5");
$sth->bind_param("var3","testvarchar5");
$sth->bind_param("var4","01-JAN-2005");
checkSuccess($sth->execute(),1);
print("\n");

print("BIND BY NAME: \n");
$sth->bind_param("var1",6);
$sth->bind_param("var2","testchar6");
$sth->bind_param("var3","testvarchar6");
$sth->bind_param("var4","01-JAN-2006");
checkSuccess($sth->execute(),1);
print("\n");

print("BIND BY NAME: \n");
$sth->bind_param("var1",7);
$sth->bind_param("var2","testchar7");
$sth->bind_param("var3","testvarchar7");
$sth->bind_param("var4","01-JAN-2007");
checkSuccess($sth->execute(),1);
print("\n");

print("OUTPUT BIND BY NAME: \n");
$sth=$dbh->prepare("begin  :numvar:=1; :stringvar:='hello'; :floatvar:=2.5; end;");
$sth->bind_param_inout("numvar",\$numvar,10);
$sth->bind_param_inout("stringvar",\$stringvar,10);
$sth->bind_param_inout("floatvar",\$floatvar,10);
checkSuccess($sth->execute(),1);
checkSuccessString($numvar,'1');
checkSuccessString($stringvar,'hello');
checkSuccessString($floatvar,'2.5');
print("\n");

print("OUTPUT BIND BY POSITION: \n");
$sth->bind_param_inout("1",\$numvar,10);
$sth->bind_param_inout("2",\$stringvar,10);
$sth->bind_param_inout("3",\$floatvar,10);
checkSuccess($sth->execute(),1);
checkSuccessString($numvar,'1');
checkSuccessString($stringvar,'hello');
checkSuccessString($floatvar,'2.5');
print("\n");

print("SELECT: \n");
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
print("\n");

print("COLUMN COUNT: \n");
checkSuccess($sth->{NUM_OF_FIELDS},4);
print("\n");

print("COLUMN NAMES: \n");
checkSuccessString($sth->{NAME}->[0],"TESTNUMBER");
checkSuccessString($sth->{NAME}->[1],"TESTCHAR");
checkSuccessString($sth->{NAME}->[2],"TESTVARCHAR");
checkSuccessString($sth->{NAME}->[3],"TESTDATE");
print("\n");

print("COLUMN NAMES (lc): \n");
checkSuccessString($sth->{NAME_lc}->[0],"testnumber");
checkSuccessString($sth->{NAME_lc}->[1],"testchar");
checkSuccessString($sth->{NAME_lc}->[2],"testvarchar");
checkSuccessString($sth->{NAME_lc}->[3],"testdate");
print("\n");

print("COLUMN NAMES (uc): \n");
checkSuccessString($sth->{NAME_uc}->[0],"TESTNUMBER");
checkSuccessString($sth->{NAME_uc}->[1],"TESTCHAR");
checkSuccessString($sth->{NAME_uc}->[2],"TESTVARCHAR");
checkSuccessString($sth->{NAME_uc}->[3],"TESTDATE");
print("\n");

print("COLUMN TYPES: \n");
checkSuccessString($sth->{TYPE}->[0],"NUMBER");
checkSuccessString($sth->{TYPE}->[1],"CHAR");
checkSuccessString($sth->{TYPE}->[2],"VARCHAR2");
checkSuccessString($sth->{TYPE}->[3],"DATE");
print("\n");

print("COLUMN INDICES FROM NAME_hash: \n");
checkSuccessString($sth->{NAME_hash}->{TESTNUMBER},0);
checkSuccessString($sth->{NAME_hash}->{TESTCHAR},1);
checkSuccessString($sth->{NAME_hash}->{TESTVARCHAR},2);
checkSuccessString($sth->{NAME_hash}->{TESTDATE},3);
print("\n");

print("COLUMN INDICES FROM NAME_lc_hash: \n");
checkSuccessString($sth->{NAME_lc_hash}->{testnumber},0);
checkSuccessString($sth->{NAME_lc_hash}->{testchar},1);
checkSuccessString($sth->{NAME_lc_hash}->{testvarchar},2);
checkSuccessString($sth->{NAME_lc_hash}->{testdate},3);
print("\n");

print("COLUMN INDICES FROM NAME_uc_hash: \n");
checkSuccessString($sth->{NAME_uc_hash}->{TESTNUMBER},0);
checkSuccessString($sth->{NAME_uc_hash}->{TESTCHAR},1);
checkSuccessString($sth->{NAME_uc_hash}->{TESTVARCHAR},2);
checkSuccessString($sth->{NAME_uc_hash}->{TESTDATE},3);
print("\n");

#print("TYPE INFO ALL: \n");
#print("\n");

#print("TYPE INFO: \n");
#print("\n");

#print("FIELDS BY INDEX: \n");
#checkSuccessString($cur->getField(0,0),"1");
#checkSuccessString($cur->getField(0,1),"testchar1                               ");
#checkSuccessString($cur->getField(0,2),"testvarchar1");
#checkSuccessString($cur->getField(0,3),"01-JAN-01");
#print("\n");
#checkSuccessString($cur->getField(7,0),"8");
#checkSuccessString($cur->getField(7,1),"testchar8                               ");
#checkSuccessString($cur->getField(7,2),"testvarchar8");
#checkSuccessString($cur->getField(7,3),"01-JAN-08");
#print("\n");

#print("FIELDS BY NAME: \n");
#checkSuccessString($cur->getField(0,"testnumber"),"1");
#checkSuccessString($cur->getField(0,"testchar"),"testchar1                               ");
#checkSuccessString($cur->getField(0,"testvarchar"),"testvarchar1");
#checkSuccessString($cur->getField(0,"testdate"),"01-JAN-01");
#print("\n");
#checkSuccessString($cur->getField(7,"testnumber"),"8");
#checkSuccessString($cur->getField(7,"testchar"),"testchar8                               ");
#checkSuccessString($cur->getField(7,"testvarchar"),"testvarchar8");
#checkSuccessString($cur->getField(7,"testdate"),"01-JAN-08");
#print("\n");

print("FIELDS BY ARRAYREF: \n");
$fieldsref=$sth->fetchrow_arrayref;
checkSuccess($$fieldsref[0],1);
checkSuccessString($$fieldsref[1],"testchar1                               ");
checkSuccessString($$fieldsref[2],"testvarchar1");
checkSuccessString($$fieldsref[3],"01-JAN-01");
print("\n");

print("FIELDS BY ARRAY: \n");
@fields=$sth->fetchrow_array;
checkSuccess($fields[0],2);
checkSuccessString($fields[1],"testchar2                               ");
checkSuccessString($fields[2],"testvarchar2");
checkSuccessString($fields[3],"01-JAN-02");
print("\n");

print("FIELDS BY HASH: \n");
$fieldshashref=$sth->fetchrow_hashref;
checkSuccess($$fieldshashref{"TESTNUMBER"},3);
checkSuccessString($$fieldshashref{"TESTCHAR"},"testchar3                               ");
checkSuccessString($$fieldshashref{"TESTVARCHAR"},"testvarchar3");
checkSuccessString($$fieldshashref{"TESTDATE"},"01-JAN-03");
print("\n");

print("SELECTROW_ARRAY: \n");
@row=$dbh->selectrow_array("select * from testtable order by testnumber");
checkSuccess($row[0],1);
checkSuccessString($row[1],"testchar1                               ");
checkSuccessString($row[2],"testvarchar1");
checkSuccessString($row[3],"01-JAN-01");
print("\n");

print("SELECTROW_ARRAYREF: \n");
$row=$dbh->selectrow_arrayref("select * from testtable order by testnumber");
checkSuccess($$row[0],1);
checkSuccessString($$row[1],"testchar1                               ");
checkSuccessString($$row[2],"testvarchar1");
checkSuccessString($$row[3],"01-JAN-01");
print("\n");

print("SELECTROW_HASHREF: \n");
$row=$dbh->selectrow_hashref("select * from testtable order by testnumber");
checkSuccess($$row{TESTNUMBER},1);
checkSuccessString($$row{TESTCHAR},"testchar1                               ");
checkSuccessString($$row{TESTVARCHAR},"testvarchar1");
checkSuccessString($$row{TESTDATE},"01-JAN-01");
print("\n");

print("FETCHALL_ARRAYREF: \n");
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
$rows=$sth->fetchall_arrayref();
checkSuccess($$rows[0][0],1);
checkSuccessString($$rows[0][1],"testchar1                               ");
checkSuccessString($$rows[0][2],"testvarchar1");
checkSuccessString($$rows[0][3],"01-JAN-01");
checkSuccess($$rows[6][0],7);
checkSuccessString($$rows[6][1],"testchar7                               ");
checkSuccessString($$rows[6][2],"testvarchar7");
checkSuccessString($$rows[6][3],"01-JAN-07");
print("\n");

print("SELECTALL_ARRAYREF: \n");
$rows=$dbh->selectall_arrayref("select * from testtable order by testnumber");
checkSuccess($$rows[0][0],1);
checkSuccessString($$rows[0][1],"testchar1                               ");
checkSuccessString($$rows[0][2],"testvarchar1");
checkSuccessString($$rows[0][3],"01-JAN-01");
checkSuccess($$rows[6][0],7);
checkSuccessString($$rows[6][1],"testchar7                               ");
checkSuccessString($$rows[6][2],"testvarchar7");
checkSuccessString($$rows[6][3],"01-JAN-07");
print("\n");

print("FETCHALL_HASHREF: \n");
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
$rows=$sth->fetchall_hashref("TESTNUMBER");
checkSuccessString($$rows{1}->{TESTCHAR},"testchar1                               ");
checkSuccessString($$rows{1}->{TESTVARCHAR},"testvarchar1");
checkSuccessString($$rows{1}->{TESTDATE},"01-JAN-01");
checkSuccessString($$rows{7}->{TESTCHAR},"testchar7                               ");
checkSuccessString($$rows{7}->{TESTVARCHAR},"testvarchar7");
checkSuccessString($$rows{7}->{TESTDATE},"01-JAN-07");
print("\n");
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
$rows=$sth->fetchall_hashref(1);
checkSuccessString($$rows{1}->{TESTCHAR},"testchar1                               ");
checkSuccessString($$rows{1}->{TESTVARCHAR},"testvarchar1");
checkSuccessString($$rows{1}->{TESTDATE},"01-JAN-01");
checkSuccessString($$rows{7}->{TESTCHAR},"testchar7                               ");
checkSuccessString($$rows{7}->{TESTVARCHAR},"testvarchar7");
checkSuccessString($$rows{7}->{TESTDATE},"01-JAN-07");
print("\n");

print("SELECTALL_HASHREF: \n");
$rows=$dbh->selectall_hashref("select * from testtable order by testnumber","TESTNUMBER");
checkSuccessString($$rows{1}->{TESTCHAR},"testchar1                               ");
checkSuccessString($$rows{1}->{TESTVARCHAR},"testvarchar1");
checkSuccessString($$rows{1}->{TESTDATE},"01-JAN-01");
checkSuccessString($$rows{7}->{TESTCHAR},"testchar7                               ");
checkSuccessString($$rows{7}->{TESTVARCHAR},"testvarchar7");
checkSuccessString($$rows{7}->{TESTDATE},"01-JAN-07");
print("\n");
$rows=$dbh->selectall_hashref("select * from testtable order by testnumber",1);
checkSuccessString($$rows{1}->{TESTCHAR},"testchar1                               ");
checkSuccessString($$rows{1}->{TESTVARCHAR},"testvarchar1");
checkSuccessString($$rows{1}->{TESTDATE},"01-JAN-01");
checkSuccessString($$rows{7}->{TESTCHAR},"testchar7                               ");
checkSuccessString($$rows{7}->{TESTVARCHAR},"testvarchar7");
checkSuccessString($$rows{7}->{TESTDATE},"01-JAN-07");
print("\n");

print("SELECTCOL_ARRAYREF: \n");
$cols=$dbh->selectcol_arrayref("select * from testtable order by testnumber");
checkSuccess($$cols[0],1);
checkSuccess($$cols[1],2);
checkSuccess($$cols[2],3);
checkSuccess($$cols[3],4);
checkSuccess($$cols[4],5);
checkSuccess($$cols[5],6);
checkSuccess($$cols[6],7);
print("\n");

print("COMMIT AND ROLLBACK: \n");
#my $dbh2=DBI->connect("DBI:SQLRelay:host=localhost;port=9000;socket=/tmp/test.socket;debug=1","test","test",{AutoCommit=>0}) or die DBI->errstr;
my $dbh2=DBI->connect("DBI:SQLRelay:host=localhost;port=9000;socket=/tmp/test.socket;","test","test",{AutoCommit=>0}) or die DBI->errstr;
my @row=$dbh2->selectrow_array("select count(*) from testtable");
checkSuccess($row[0],0);
checkSuccess($dbh->commit(),1);
@row=$dbh2->selectrow_array("select count(*) from testtable");
checkSuccess($row[0],7);
$dbh->{AutoCommit}=1;
checkSuccess($dbh->do("insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010')"),1);
my @row=$dbh2->selectrow_array("select count(*) from testtable");
checkSuccess($row[0],8);
$dbh->{AutoCommit}=0;
print("\n");

# lots of rows
print("LOTS OF ROWS: \n");
$dbh->do("delete from testtable");
for ($i=0; $i<200; $i++) {
	$dbh->do("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001')");
}
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
for ($i=0; $i<200; $i++) {
	@fields=$sth->fetchrow_array;
	if ($fields[0]!=1) {
		break;
	}
}
checkSuccess($i,200);
print("\n");

# null binds
print("NULL BINDS: \n");
$dbh->do("delete from testtable");
$sth=$dbh->prepare("insert into testtable values (:var1,:var2,:var3,:var4)");
$sth->bind_param("1",undef);
$sth->bind_param("2",undef);
$sth->bind_param("3",undef);
$sth->bind_param("4",undef);
checkSuccess($sth->execute(),1);
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
@fields=$sth->fetchrow_array;
checkUndef($fields[0]);
checkUndef($fields[1]);
checkUndef($fields[2]);
checkUndef($fields[3]);
print("\n");

# drop existing table
$dbh->do("drop table testtable");

# CLOB/BLOB binds
print("CLOB/BLOB BINDS: \n");
$dbh->do("drop table testtable");
checkSuccessString($dbh->do("create table testtable (testclob clob, testblob blob)"),"0E0");
my $sth=$dbh->prepare("insert into testtable values (:var1,:var2)");
$sth->bind_param("var1","testclob",DBD::SQLRelay::SQL_CLOB);
$sth->bind_param("var2","testblob",{type=>DBD::SQLRelay::SQL_BLOB,length=>8});
checkSuccess($sth->execute(),1);
$dbh->do("drop table testtable");
print("\n");

# invalid queries...
print("INVALID QUERIES: \n");
checkSuccess($dbh->do("select * from testtable order by testnumber"),0);
checkSuccess($dbh->do("select * from testtable order by testnumber"),0);
checkSuccess($dbh->do("select * from testtable order by testnumber"),0);
checkSuccess($dbh->do("select * from testtable order by testnumber"),0);
print("\n");
checkSuccess($dbh->do("insert into testtable values (1,2,3,4)"),0);
checkSuccess($dbh->do("insert into testtable values (1,2,3,4)"),0);
checkSuccess($dbh->do("insert into testtable values (1,2,3,4)"),0);
checkSuccess($dbh->do("insert into testtable values (1,2,3,4)"),0);
print("\n");
checkSuccess($dbh->do("create table testtable"),0);
checkSuccess($dbh->do("create table testtable"),0);
checkSuccess($dbh->do("create table testtable"),0);
checkSuccess($dbh->do("create table testtable"),0);
print("\n");
