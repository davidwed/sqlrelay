#! /usr/bin/env perl

# Copyright (c) 1999-2018 David Muse
# See the file COPYING for more information.

use DBI;
use DBI::Const::GetInfoType;
use Data::Dumper;

sub checkUndef {

	$value=shift(@_);

	if (!defined($value)) {
		print("success ");
	} else {
		print("failure ");
		exit 1;
	}
}

sub checkDefined {

	$value=shift(@_);

	if (defined($value)) {
		print("success ");
	} else {
		print("failure ");
		exit 1;
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
		exit 1;
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
		exit 1;
	}
}

$tlscert="/usr/local/firstworks/etc/sqlrelay.conf.d/client.pem";
$tlsca="/usr/local/firstworks/etc/sqlrelay.conf.d/ca.pem";
if ($^O eq "MSWin32") {
	$tlscert="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\client.pfx";
	$tlsca="C:\\Program Files\\Firstworks\\etc\\sqlrelay.conf.d\\ca.pfx";
}


# instantiation
my $prefix="DBI:SQLRelay(AutoCommit=>0,PrintError=>0):";
my $connectstring="host=sqlrelay;port=9000;socket=/tmp/test.socket;debug=0;tls=yes;tlscert=$tlscert;tlsvalidate=ca;tlsca=$tlsca";
my $dsn=$prefix.$connectstring;

# parse dsn
if ($DBI::VERSION>=1.43) {
	print("PARSE DSN: \n");
	my ($scheme,$driver,$attr_string,$attr_hash,$driver_dsn)=DBI->parse_dsn($dsn);
	checkSuccessString($scheme,"dbi");
	checkSuccessString($driver,"SQLRelay");
	checkSuccessString($attr_string,"AutoCommit=>0,PrintError=>0");
	checkSuccess($attr_hash->{AutoCommit},0);
	checkSuccess($attr_hash->{PrintError},0);
	checkSuccessString($driver_dsn,$connectstring);
	print("\n");
}

# connect
print("CONNECT: \n");
my $dbh=DBI->connect($dsn,"","") or die DBI->errstr;
checkSuccessString($dbh->{Type},"db");
if ($DBI::VERSION>=1.40) {
	checkSuccessString($dbh->{Username},"");
}
checkDefined($dbh);
$dbh->disconnect();
$ENV{"DBI_DSN"}=$dsn;
my $dbh=DBI->connect(undef,"","",{AutoCommit=>0,PrintError=>0}) or die DBI->errstr;
checkDefined($dbh);
checkSuccessString($dbh->{Name},$connectstring);
print("\n");


# ping
print("PING: \n");
checkSuccess($dbh->ping(),1);
print("\n");

# drop existing table
$dbh->do("drop table testtable");

print("CREATE TEMPTABLE: \n");
if ($DBI::VERSION>=1.41) {
	$dbh->{Executed}=0;
}
my $stmt="create table testtable (testnumber number not null, testchar char(40), testvarchar varchar2(40), testdate date)";
checkSuccessString($dbh->do($stmt),"0E0");
if ($DBI::VERSION>=1.41) {
	checkSuccess($dbh->{Executed},1);
}
checkSuccessString($dbh->{Statement},$stmt);
print("\n");

print("INSERT and AFFECTED ROWS: \n");
checkSuccess($dbh->do("insert into testtable values (1,'testchar1','testvarchar1','01-JAN-2001')"),1);
print("\n");

print("DO WITH BIND VALUES: \n");
checkSuccess($dbh->do("insert into testtable values (:var1,:var2,:var3,:var4)",undef,(2,"testchar2","testvarchar2","01-JAN-2002")),1);
print("\n");

print("EXECUTE WITH BIND VALUES: \n");
if ($DBI::VERSION>=1.41) {
	$dbh->{Executed}=0;
}
$stmt="insert into testtable values (:var1,:var2,:var3,:var4)";
my $sth=$dbh->prepare($stmt);
checkSuccessString($sth->{Type},"st");
checkSuccessString($sth->{Statement},$stmt);
checkSuccess($dbh->{Kids},1);
checkSuccess($dbh->{ActiveKids},0);
checkSuccess($sth->{Active},0);
checkSuccess($sth->execute(3,"testchar3","testvarchar3","01-JAN-2003"),1);
checkSuccess($sth->{Active},1);
checkSuccess($dbh->{ActiveKids},1);
if ($DBI::VERSION>1.41) {
	checkSuccess($sth->{Executed},1);
	checkSuccess($dbh->{Executed},1);
}
print("\n");

print("AFFECTED ROWS: \n");
checkSuccess($sth->rows(),1);
print("\n");

print("BIND PARAM BY POSITION: \n");
$sth->bind_param(1,4,SQL_INTEGER);
$sth->bind_param(2,"testchar4",SQL_CHAR);
$sth->bind_param(3,"testvarchar4",{type=>SQL_VARCHAR,length=>12});
$sth->bind_param(4,"01-JAN-2004",{type=>SQL_DATETIME});
checkSuccess($sth->{ParamValues}->{1},4);
checkSuccess($sth->{ParamValues}->{2},"testchar4");
checkSuccess($sth->{ParamValues}->{3},"testvarchar4");
checkSuccess($sth->{ParamValues}->{4},"01-JAN-2004");
checkSuccess($sth->{ParamTypes}->{"var1"},"SQL_INTEGER");
checkSuccess($sth->{ParamTypes}->{"var2"},"SQL_CHAR");
checkSuccess($sth->{ParamTypes}->{"var3"},"SQL_VARCHAR");
checkSuccess($sth->{ParamTypes}->{"var4"},"SQL_DATETIME");
checkSuccess($sth->execute(),1);
print("\n");

print("PARAM COUNT: \n");
checkSuccess($sth->{NUM_OF_PARAMS},4);
print("\n");

if ($DBI::VERSION>=1.22) {
	print("EXECUTE ARRAY: \n");
	@var1s=(5,6);
	@var2s=("testchar5","testchar6");
	@var3s=("testvarchar5","testvarchar6");
	@var4s=("01-JAN-2005","01-JAN-2006");
	if ($DBI::VERSION>=1.41) {
		$dbh->{Executed}=0;
	}
	my ($tuples,$rows)=$sth->execute_array({ ArrayTupleStatus=>\my @tuple_status },\@var1s,\@var2s,\@var3s,\@var4s);
	if ($DBI::VERSION>=1.41) {
		checkSuccess($sth->{Executed},1);
		checkSuccess($dbh->{Executed},1);
	}
	checkSuccess($tuples,2);
	if ($DBI::VERSION>=1.60) {
		checkSuccess($rows,2);
	}
	for (my $index=0; $index<2; $index++) {
		checkSuccess(@tuple_status[$index],1);
	}
	checkSuccess($sth->{ParamArrays}->{1}->[0],5);
	checkSuccess($sth->{ParamArrays}->{1}->[1],6);
	checkSuccess($sth->{ParamArrays}->{2}->[0],"testchar5");
	checkSuccess($sth->{ParamArrays}->{2}->[1],"testchar6");
	checkSuccess($sth->{ParamArrays}->{3}->[0],"testvarchar5");
	checkSuccess($sth->{ParamArrays}->{3}->[1],"testvarchar6");
	checkSuccess($sth->{ParamArrays}->{4}->[0],"01-JAN-2005");
	checkSuccess($sth->{ParamArrays}->{4}->[1],"01-JAN-2006");
	print("\n");

	print("BIND PARAM ARRAY: \n");
	$sth->bind_param_array(1,[7,8]);
	$sth->bind_param_array(2,["testchar7","testchar8"]);
	$sth->bind_param_array(3,["testvarchar7","testvarchar8"]);
	$sth->bind_param_array(4,["01-JAN-2007","01-JAN-2008"]);
	my ($tuples,$rows)=$sth->execute_array({ ArrayTupleStatus=>\my @tuple_status });
	checkSuccess($tuples,2);
	if ($DBI::VERSION>=1.60) {
		checkSuccess($rows,2);
	}
	for (my $index=0; $index<2; $index++) {
		checkSuccess(@tuple_status[$index],1);
	}
	print("\n");
} else {
	$dbh->do("insert into testtable values (5,'testchar5','testvarchar5','01-JAN-2005')");
	$dbh->do("insert into testtable values (6,'testchar6','testvarchar6','01-JAN-2006')");
	$dbh->do("insert into testtable values (7,'testchar7','testvarchar7','01-JAN-2007')");
	$dbh->do("insert into testtable values (8,'testchar8','testvarchar8','01-JAN-2008')");
}

print("BIND BY NAME: \n");
$sth->bind_param("var1",9);
$sth->bind_param("var2","testchar9");
$sth->bind_param("var3","testvarchar9");
$sth->bind_param("var4","01-JAN-2009");
checkSuccess($sth->{ParamValues}->{"var1"},9);
checkSuccess($sth->{ParamValues}->{"var2"},"testchar9");
checkSuccess($sth->{ParamValues}->{"var3"},"testvarchar9");
checkSuccess($sth->{ParamValues}->{"var4"},"01-JAN-2009");
checkSuccess($sth->{ParamTypes}->{"var1"},"SQL_VARCHAR");
checkSuccess($sth->{ParamTypes}->{"var2"},"SQL_VARCHAR");
checkSuccess($sth->{ParamTypes}->{"var3"},"SQL_VARCHAR");
checkSuccess($sth->{ParamTypes}->{"var4"},"SQL_VARCHAR");
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
$sth->bind_param_inout(1,\$numvar,10);
$sth->bind_param_inout(2,\$stringvar,10);
$sth->bind_param_inout(3,\$floatvar,10);
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

print("PRECISION: \n");
checkSuccessString($sth->{PRECISION}->[0],0);
checkSuccessString($sth->{PRECISION}->[1],0);
checkSuccessString($sth->{PRECISION}->[2],0);
checkSuccessString($sth->{PRECISION}->[3],0);
print("\n");

print("SCALE: \n");
checkSuccessString($sth->{SCALE}->[0],129);
checkSuccessString($sth->{SCALE}->[1],0);
checkSuccessString($sth->{SCALE}->[2],0);
checkSuccessString($sth->{SCALE}->[3],0);
print("\n");

print("NULLABLE: \n");
checkSuccessString($sth->{NULLABLE}->[0],0);
checkSuccessString($sth->{NULLABLE}->[1],1);
checkSuccessString($sth->{NULLABLE}->[2],1);
checkSuccessString($sth->{NULLABLE}->[3],1);
print("\n");

#print("TYPE INFO ALL: \n");
#print("\n");

#print("TYPE INFO: \n");
#print("\n");

print("FETCH: \n");
$fieldsref=$sth->fetch;
checkSuccess($$fieldsref[0],1);
checkSuccessString($$fieldsref[1],"testchar1                               ");
checkSuccessString($$fieldsref[2],"testvarchar1");
checkSuccessString($$fieldsref[3],"01-JAN-01");
print("\n");

print("FIELDS BY ARRAYREF: \n");
$fieldsref=$sth->fetchrow_arrayref;
checkSuccess($$fieldsref[0],2);
checkSuccessString($$fieldsref[1],"testchar2                               ");
checkSuccessString($$fieldsref[2],"testvarchar2");
checkSuccessString($$fieldsref[3],"01-JAN-02");
print("\n");

print("FIELDS BY ARRAY: \n");
@fields=$sth->fetchrow_array;
checkSuccess($fields[0],3);
checkSuccessString($fields[1],"testchar3                               ");
checkSuccessString($fields[2],"testvarchar3");
checkSuccessString($fields[3],"01-JAN-03");
print("\n");

print("FIELDS BY HASH: \n");
$fieldshashref=$sth->fetchrow_hashref;
checkSuccess($$fieldshashref{"TESTNUMBER"},4);
checkSuccessString($$fieldshashref{"TESTCHAR"},"testchar4                               ");
checkSuccessString($$fieldshashref{"TESTVARCHAR"},"testvarchar4");
checkSuccessString($$fieldshashref{"TESTDATE"},"01-JAN-04");
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
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
$rows=$sth->fetchall_arrayref([2,3]);
checkSuccessString($$rows[0][0],"testvarchar1");
checkSuccessString($$rows[0][1],"01-JAN-01");
checkSuccessString($$rows[6][0],"testvarchar7");
checkSuccessString($$rows[6][1],"01-JAN-07");
print("\n");
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
$rows=$sth->fetchall_arrayref([2,3],1);
checkSuccessString($$rows[0][0],"testvarchar1");
checkSuccessString($$rows[0][1],"01-JAN-01");
checkUndef($$rows[1][0]);
checkUndef($$rows[1][1]);
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
$sth=$dbh->prepare("select * from testtable order by testnumber");
$rows=$dbh->selectall_arrayref($sth);
checkSuccess($$rows[0][0],1);
checkSuccessString($$rows[0][1],"testchar1                               ");
checkSuccessString($$rows[0][2],"testvarchar1");
checkSuccessString($$rows[0][3],"01-JAN-01");
checkSuccess($$rows[6][0],7);
checkSuccessString($$rows[6][1],"testchar7                               ");
checkSuccessString($$rows[6][2],"testvarchar7");
checkSuccessString($$rows[6][3],"01-JAN-07");
print("\n");
$rows=$dbh->selectall_arrayref($sth,{Slice=>[2,3]});
checkSuccessString($$rows[0][0],"testvarchar1");
checkSuccessString($$rows[0][1],"01-JAN-01");
checkSuccessString($$rows[6][0],"testvarchar7");
checkSuccessString($$rows[6][1],"01-JAN-07");
print("\n");
$rows=$dbh->selectall_arrayref($sth,{Slice=>[2,3],MaxRows=>1});
checkSuccessString($$rows[0][0],"testvarchar1");
checkSuccessString($$rows[0][1],"01-JAN-01");
checkUndef($$rows[6][0]);
checkUndef($$rows[6][1]);
print("\n");
$rows=$dbh->selectall_arrayref($sth,{Slice=>{}});
checkSuccessString($$rows[0]{TESTVARCHAR},"testvarchar1");
checkSuccessString($$rows[0]{TESTDATE},"01-JAN-01");
checkSuccessString($$rows[6]{TESTVARCHAR},"testvarchar7");
checkSuccessString($$rows[6]{TESTDATE},"01-JAN-07");
@rows=@{$dbh->selectall_arrayref($sth)};
checkSuccessString($rows[0][0],"1");
checkSuccessString($rows[0][1],"testchar1                               ");
checkSuccessString($rows[6][0],"7");
checkSuccessString($rows[6][1],"testchar7                               ");
print("\n");
$rows=$dbh->selectall_arrayref("select * from testtable where testnumber=:var1 or testnumber=:var2 order by testnumber",undef,('1','2'));
checkSuccessString($rows[0][0],"1");
checkSuccessString($rows[0][1],"testchar1                               ");
checkSuccessString($rows[1][0],"2");
checkSuccessString($rows[1][1],"testchar2                               ");
checkUndef($$rows[6][0]);
checkUndef($$rows[6][1]);
$rows=$dbh->selectall_arrayref("select * from testtable where testnumber=:var1 or testnumber=:var2 order by testnumber",undef,'1','2');
checkSuccessString($rows[0][0],"1");
checkSuccessString($rows[0][1],"testchar1                               ");
checkSuccessString($rows[1][0],"2");
checkSuccessString($rows[1][1],"testchar2                               ");
checkUndef($$rows[6][0]);
checkUndef($$rows[6][1]);
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
$sth=$dbh->prepare("select * from testtable order by testnumber");
$rows=$dbh->selectall_hashref($sth,1);
checkSuccessString($$rows{1}->{TESTCHAR},"testchar1                               ");
checkSuccessString($$rows{1}->{TESTVARCHAR},"testvarchar1");
checkSuccessString($$rows{1}->{TESTDATE},"01-JAN-01");
checkSuccessString($$rows{7}->{TESTCHAR},"testchar7                               ");
checkSuccessString($$rows{7}->{TESTVARCHAR},"testvarchar7");
checkSuccessString($$rows{7}->{TESTDATE},"01-JAN-07");
print("\n");
$rows=$dbh->selectall_hashref("select * from testtable where testnumber=:var1 or testnumber=:var2 order by testnumber",1,undef,('1','2'));
checkSuccessString($$rows{1}->{TESTCHAR},"testchar1                               ");
checkSuccessString($$rows{1}->{TESTVARCHAR},"testvarchar1");
checkSuccessString($$rows{1}->{TESTDATE},"01-JAN-01");
checkSuccessString($$rows{2}->{TESTCHAR},"testchar2                               ");
checkSuccessString($$rows{2}->{TESTVARCHAR},"testvarchar2");
checkSuccessString($$rows{2}->{TESTDATE},"01-JAN-02");
checkUndef($$rows{7}->{TESTCHAR});
checkUndef($$rows{7}->{TESTVARCHAR});
checkUndef($$rows{7}->{TESTDATE});
print("\n");
$rows=$dbh->selectall_hashref("select * from testtable where testnumber=:var1 or testnumber=:var2 order by testnumber",1,undef,'1','2');
checkSuccessString($$rows{1}->{TESTCHAR},"testchar1                               ");
checkSuccessString($$rows{1}->{TESTVARCHAR},"testvarchar1");
checkSuccessString($$rows{1}->{TESTDATE},"01-JAN-01");
checkSuccessString($$rows{2}->{TESTCHAR},"testchar2                               ");
checkSuccessString($$rows{2}->{TESTVARCHAR},"testvarchar2");
checkSuccessString($$rows{2}->{TESTDATE},"01-JAN-02");
checkUndef($$rows{7}->{TESTCHAR});
checkUndef($$rows{7}->{TESTVARCHAR});
checkUndef($$rows{7}->{TESTDATE});
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
$cols=$dbh->selectcol_arrayref("select * from testtable where testnumber=:var1 or testnumber=:var2 order by testnumber",undef,(1,2));
checkSuccess($$cols[0],1);
checkSuccess($$cols[1],2);
checkUndef($$cols[2]);
checkUndef($$cols[3]);
checkUndef($$cols[4]);
checkUndef($$cols[5]);
checkUndef($$cols[6]);
print("\n");

print("CHOP BLANKS: \n");
$dbh->{ChopBlanks}=1;
$rows=$dbh->selectall_arrayref("select * from testtable order by testnumber");
checkSuccessString($$rows[0][1],"testchar1");
checkSuccessString($$rows[6][1],"testchar7");
$dbh->{ChopBlanks}=0;
$rows=$dbh->selectall_arrayref("select * from testtable order by testnumber");
checkSuccessString($$rows[0][1],"testchar1                               ");
checkSuccessString($$rows[6][1],"testchar7                               ");
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
$sth->{ChopBlanks}=1;
$rows=$sth->fetchall_arrayref();
checkSuccessString($$rows[0][1],"testchar1");
checkSuccessString($$rows[6][1],"testchar7");
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
$sth->{ChopBlanks}=0;
$rows=$sth->fetchall_arrayref();
checkSuccessString($$rows[0][1],"testchar1                               ");
checkSuccessString($$rows[6][1],"testchar7                               ");
print("\n");

print("COMMIT AND ROLLBACK: \n");
my $dbh2=DBI->connect($dsn,"","",{AutoCommit=>0}) or die DBI->errstr;
my @row=$dbh2->selectrow_array("select count(*) from testtable");
checkSuccess($row[0],0);
if ($DBI::VERSION>=1.41) {
	checkSuccess($dbh->{Executed},1);
}
checkSuccess($dbh->commit(),1);
if ($DBI::VERSION>=1.41) {
	checkSuccess($dbh->{Executed},0);
}
@row=$dbh2->selectrow_array("select count(*) from testtable");
checkSuccess($row[0],9);
$dbh->{AutoCommit}=1;
checkSuccess($dbh->do("insert into testtable values (10,'testchar10','testvarchar10','01-JAN-2010')"),1);
my @row=$dbh2->selectrow_array("select count(*) from testtable");
checkSuccess($row[0],10);
$dbh2->disconnect();
$dbh->{AutoCommit}=0;
checkSuccess($dbh->do("insert into testtable values (11,'testchar11','testvarchar11','01-JAN-2011')"),1);
my @row=$dbh->selectrow_array("select count(*) from testtable");
checkSuccess($row[0],11);
my @row=$dbh2->selectrow_array("select count(*) from testtable");
checkSuccess($row[0],10);
if ($DBI::VERSION>=1.41) {
	checkSuccess($dbh->{Executed},1);
}
checkSuccess($dbh->rollback(),1);
if ($DBI::VERSION>=1.41) {
	checkSuccess($dbh->{Executed},0);
}
my @row=$dbh2->selectrow_array("select count(*) from testtable");
checkSuccess($row[0],10);
print("\n");

# row cache size
print("ROW CACHE SIZE: \n");
checkSuccess($dbh->{RowCacheSize},0);
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
checkSuccess($sth->{RowsInCache},10);
for (my $i=10; $i>0; $i--) {
	@row=$sth->fetchrow_array();
	checkSuccess($sth->{RowsInCache},$i-1);
}
print("\n");
$dbh->{RowCacheSize}=0;
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
checkSuccess($sth->{RowsInCache},10);
for (my $i=10; $i>0; $i--) {
	@row=$sth->fetchrow_array();
	checkSuccess($sth->{RowsInCache},$i-1);
}
print("\n");
$dbh->{RowCacheSize}=-1;
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
checkSuccess($sth->{RowsInCache},10);
for (my $i=10; $i>0; $i--) {
	@row=$sth->fetchrow_array();
	checkSuccess($sth->{RowsInCache},$i-1);
}
print("\n");
$dbh->{RowCacheSize}=1;
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
checkSuccess($sth->{RowsInCache},1);
for (my $i=10; $i>0; $i--) {
	@row=$sth->fetchrow_array();
	checkSuccess($sth->{RowsInCache},0);
}
print("\n");
$dbh->{RowCacheSize}=10;
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
@rows=@{$sth->fetchall_arrayref()};
checkSuccess($#rows+1,10);
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
$sth=$dbh->prepare("insert into testtable values (1,:var2,:var3,:var4)");
$sth->bind_param(1,undef);
$sth->bind_param(2,undef);
$sth->bind_param(3,undef);
checkSuccess($sth->execute(),1);
$sth=$dbh->prepare("select * from testtable order by testnumber");
checkSuccessString($sth->execute(),"0E0");
@fields=$sth->fetchrow_array;
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
$sth=$dbh->prepare("begin select testclob into :clobvar from testtable;  select testblob into :blobvar from testtable; end;");
$sth->bind_param_inout("clobvar",\$testclob,undef,DBD::SQLRelay::SQL_CLOB);
$sth->bind_param_inout("blobvar",\$testblob,undef,DBD::SQLRelay::SQL_BLOB);
checkSuccess($sth->execute(),1);
checkSuccessString($testclob,"testclob");
checkSuccessString($testblob,"testblob");
$dbh->do("drop table testtable");
print("\n");

# prepare_cached
print("PREPARE CACHED: \n");
$sth=$dbh->prepare_cached("select 1 from dual");
my $sth1=$dbh->prepare_cached("select 1 from dual");
my $sth2=$dbh->prepare_cached("select 2 from dual");
checkSuccess($sth,$sth1);
$success="false";
if ($sth2==$sth) {
	$success="true"
}
checkSuccessString($success,"false");
print("\n");

# get info
print("GET INFO: \n");
checkSuccessString($dbh->get_info($GetInfoType{SQL_DATA_SOURCE_NAME}),"TESTUSER");
checkSuccessString($dbh->get_info($GetInfoType{SQL_DBMS_NAME}),"oracle");
checkSuccessString($dbh->get_info($GetInfoType{SQL_DBMS_VER}),"Oracle Database 12c Enterprise Edition Release 12.2.0.1.0 - 64bit Production");
checkSuccessString($dbh->get_info($GetInfoType{SQL_USER_NAME}),"");
checkSuccessString($dbh->get_info($GetInfoType{SQL_IDENTIFIER_QUOTE_CHAR}),"\"");
checkSuccessString($dbh->get_info($GetInfoType{SQL_CATALOG_NAME_SEPARATOR}),"@");
checkSuccessString($dbh->get_info($GetInfoType{SQL_CATALOG_LOCATION}),2);
print("\n");

# quote
print("QUOTE: \n");
checkSuccessString($dbh->quote("don't"),"'don''t'");
checkSuccessString($dbh->quote("don't",SQL_CHAR),"'don''t'");
checkSuccessString($dbh->quote("don't",SQL_VARCHAR),"'don''t'");
checkSuccessString($dbh->quote("123",SQL_INTEGER),"'123'");
print("\n");
checkSuccessString($dbh->quote_identifier("mytable"),"\"mytable\"");
checkSuccessString($dbh->quote_identifier("mycatalog","myschema","mytable"),"\"myschema\".\"mytable\"\@\"mycatalog\"");
print("\n");

print("NON-LAZY CONNECT: \n");
$dsn = $prefix."sqlrelay:host=invalidhost;port=0;socket=/invalidsocket;tries=1;retrytime=1;debug=0;lazyconnect=0;tls=yes;tlscert=$tlscert;tlsvalidate=ca;tlsca=$tlsca";
checkUndef(DBI->connect($dsn,"",""));
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


$dbh->disconnect();
