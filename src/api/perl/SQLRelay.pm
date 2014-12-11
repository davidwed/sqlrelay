package DBD::SQLRelay;

use strict;
use vars qw($err $errstr $sqlstate $drh);
use SQLRelay::Connection;
use SQLRelay::Cursor;
use Data::Dumper;

use DBI qw(:sql_types);

$err=0;			# holds error code for DBI::err
$errstr='';		# holds error string for DBI::err
$sqlstate='';		# holds SQL state for DBI::state

$drh=undef;		# holds driver handle

sub driver {

	# return the driver handle if it's already 
	# defined to prevent multiple driver instances
	return $drh if $drh;	

	# get parameters
	my ($class,$attr)=@_;

	# append ::dr to the class name
	$class .='::dr';

	# create the driver handle
	$drh=DBI::_new_drh($class, {
		'Name'		=>	'SQLRelay',
		'Version'	=>	0,
		'Err'		=>	\$DBD::SQLRelay::err,
		'Errstr'	=>	\$DBD::SQLRelay::errstr,
		'State'		=>	\$DBD::SQLRelay::state,
		'Attribution'	=>	'DBD::SQLRelay by Dmitry Ovsyanko',
	});
	return $drh
}


# driver class
package DBD::SQLRelay::dr;

$DBD::SQLRelay::dr::imp_data_size=0;

sub connect {

	# get parameters
	my ($drh, $dbname, $user, $password, $attr)=@_;

	local $ENV{DBI_AUTOPROXY}='' if $ENV{DBI_AUTOPROXY} && $ENV{DBI_AUTOPROXY} =~ /^dbi:SQLRelay/i;

	# create a blank database handle
	my $dbh=DBI::_new_dbh($drh, {
		'Name'		=>	$dbname,
		'USER'		=>	$user,
		'CURRENT_USER'	=>	$user,
	});

	# set some defaults
	my %dsn;
	$dsn{'host'}='localhost';
	$dsn{'port'}=9000;
	$dsn{'socket'}='';
	$dsn{'retrytime'}=0;
	$dsn{'tries'}=1;
	$dsn{'debug'}=0;

	# split the dsn
	my $var;
	my $val;
	foreach $var (split(/;/,$dbname)) {
		if ($var=~/(.*?)=(.*)/) {
			$var=$1;
			$val=$2;
			$dsn{$var}=$val;
			$dbh->STORE($var,$val);
		}
	}
	
	# create an Connection
	my $connection=SQLRelay::Connection->new($dsn{'host'},
							$dsn{'port'},
							$dsn{'socket'},
							$user,
							$password,
							$dsn{'retrytime'},
							$dsn{'tries'});

	# turn on debugging if debugging was specified in the dsn
	$connection->debugOn() if $dsn{'debug'};

	# store some references in the database handle
	$dbh->STORE('driver_database_handle',$drh);
	$dbh->STORE('driver_connection',$connection);

	# store a 1 for this database handle in the 'database handles' hash 
	# in the driver handle, indicating that this database handle exists
	# and can be disconnected
	$drh->{'dbhs'}->{$dbh}=1;

	# mark this connection Active
	$dbh->STORE('Active',1);

	return $dbh;
}

sub disconnect_all {

	# get parameters
	my ($drh)=@_;

	# run through the hash of database handles, disconnecting each
	foreach (keys %{$drh->{'dbhs'}}) {
		my $dbh=$drh->{'dbhs'}->{$_};
		next unless ref $dbh;
		$dbh->disconnect();
	}

	return 1;
}


# database class
package DBD::SQLRelay::db;

$DBD::SQLRelay::db::imp_data_size=0;

sub _new_statement {

	# get parameters
	my ($dbh, $statement)=@_;

	# create a blank statement handle
	my $sth=DBI::_new_sth($dbh,{'Statement'=>$statement});

	# create an Cursor
	my $cursor=SQLRelay::Cursor->new($dbh->FETCH('driver_connection'));

	# set result set buffer size
	my $rowcachesize=$dbh->FETCH('RowCacheSize');
	if (!defined($rowcachesize) || $rowcachesize==0) {
		$cursor->setResultSetBufferSize(100);
	} elsif ($rowcachesize<0) {
		$cursor->setResultSetBufferSize(0);
	} else {
		$cursor->setResultSetBufferSize($rowcachesize);
	}

	# store statement-specific data in the statement handle
	$sth->STORE('driver_database_handle',$dbh);
	$sth->STORE('driver_is_select',($statement=~/^\s*select/i));
	$sth->STORE('driver_cursor',$cursor);

 
 	for (grep /^ext_SQLR/, keys %$dbh) {
 		$sth->STORE($_, $dbh->FETCH($_));
 	}

	# clear any binds still hanging around from
	# the last time this cursor was used
	$cursor->clearBinds();
 
 	$cursor->getNullsAsUndefined();

	return $sth;
}

sub prepare {

	# get parameters
	my ($dbh, $statement, @attribs)=@_;

	# create a statement
	my $sth=_new_statement($dbh,$statement);

	# get the cursor from the statement
	my $cursor=$sth->FETCH('driver_cursor');

	# prepare the query
	$cursor->prepareQuery($statement);

	# count bind vars
	$sth->STORE('NUM_OF_PARAMS',$cursor->countBindVariables());
	return $sth;
}

sub disconnect {

	# get parameters
	my ($dbh)=@_;

	# end the session
	$dbh->FETCH('driver_connection')->endSession();

	# remove references to this database handle from the driver handle
	delete $dbh->FETCH('driver_database_handle')->{$dbh};
	delete $dbh->FETCH('driver_database_handle')->{'dbhs'}->{$dbh};

	# mark this connection not Active
	$dbh->STORE('Active',0);
}

sub begin_work {

	# get parameters
	my ($dbh)=@_;

	# handle autocommit
	if ($dbh->FETCH('driver_AutoCommit')) {
		if ($dbh->FETCH('Warn')) {
			warn('Commit ineffective while AutoCommit is on');
		}
	}
	
	# execute a begin
	return $dbh->FETCH('driver_connection')->begin();
}

sub commit {

	# get parameters
	my ($dbh)=@_;

	# handle autocommit
	if ($dbh->FETCH('driver_AutoCommit')) {
		if ($dbh->FETCH('Warn')) {
			warn('Commit ineffective while AutoCommit is on');
		}
	}
	
	# execute a commit
	return $dbh->FETCH('driver_connection')->commit();
}

sub rollback {

	# get parameters
	my ($dbh)=@_;

	# handle autocommit
	if ($dbh->FETCH('driver_AutoCommit')) {
		if ($dbh->FETCH('Warn')) {
			warn('Commit ineffective while AutoCommit is on');
		}
	}
	
	# execute a rollback
	return $dbh->FETCH('driver_connection')->rollback();
}

sub get_info {

	my ($dbh,$index)=@_;

	# see GetInfoType for where these numbers come from

	if ($index==2) {
		# data source name
		return $dbh->FETCH('driver_connection')->getCurrentDatabase();
	} elsif ($index==17) {
		# dbms name
		if ($dbh->FETCH('driver_dbmsname') eq '') {
			$dbh->STORE('driver_dbmsname',
				$dbh->FETCH('driver_connection')->identify());
		}
		return $dbh->FETCH('driver_dbmsname');
	} elsif ($index==18) {
		# dbms version
		return $dbh->FETCH('driver_connection')->dbVersion();
	} elsif ($index==13) {
		# server name
		return $dbh->FETCH('driver_connection')->dbHostName();
	} elsif ($index==47) {
		# user name
		return $dbh->FETCH('USER');
	} elsif ($index==29) {
		# identifier quote character
		my $identity=$dbh->get_info(17);
		if ($identity eq 'mysql') {
			return '`';
		}
		return '"';
	} elsif ($index==41) {
		# catalog name separator
		my $identity=$dbh->get_info(17);
		if ($identity eq 'oracle8') {
			return '@';
		}
		return '.';
	} elsif ($index==114) {
		# catalog location
		my $identity=$dbh->get_info(17);
		if ($identity eq 'oracle8') {
			return 2;
		}
		return 1;
	}

	return undef;
}

sub STORE {

	# get parameters
	my ($dbh,$attr,$val)=@_;

	# special cases...
	if ($attr eq 'AutoCommit') {
		$dbh->{'driver_AutoCommit'}=$val;
		my $connection=$dbh->FETCH('driver_connection');
		if ($val) {
			$connection->autoCommitOn();
		} else {
			$connection->autoCommitOff();
		}
		return 1;
	} elsif ($attr eq 'RowCacheSize') {
		$dbh->{'driver_RowCacheSize'}=$val;
		return 1;
	} elsif ($attr =~ /^ext_SQLR_Debug$/) {
		my $connection=$dbh->FETCH('driver_connection');
		if ($val==1) {
			$connection->debugOn();
		} elsif ($val==2) {
			$connection->debugOff();
		} else {
			$connection->setDebugFile($val);
		}
		return 1;
	}

	# handle all other cases
	if ($attr =~ /^(?:driver|ext_SQLR)_/) {
		$dbh->{$attr}=$val;
		return 1;
	}

	# if the attribute didn't start with 'driver_' 
	# then pass it up to the parent class
	return $dbh->SUPER::STORE($attr,$val);
}

sub FETCH {

	# get parameters
	my ($dbh,$attr)=@_;

	# special cases...
	if ($attr eq 'AutoCommit') {
		return $dbh->{'driver_AutoCommit'};
	}
	elsif ($attr eq 'RowCacheSize') {
		return $dbh->{'driver_RowCacheSize'};
	}

	# handle all other cases
	if ($attr =~ /^(?:driver|ext_SQLR)_/) {
		return $dbh->{$attr};
	}

	# if the attribute didn't start with 'driver_' 
	# then pass it up to the parent class
	$dbh->SUPER::FETCH($attr);
}

sub ping {

	# get parameters
	my ($dbh,$attr)=@_;

	# execute a ping
	return $dbh->FETCH('driver_connection')->ping();
}

sub last_insert_id {

	# get parameters
	my ($dbh)=@_;

	# get the last insert id
	return $dbh->FETCH('driver_connection')->getLastInsertId();
}

# statement class
package DBD::SQLRelay::st;

$DBD::SQLRelay::st::imp_data_size=0;

sub bind_param {

	# get parameters
	my ($sth,$param,$val,$attr)=@_;

	# determine type, length, precision, scale...
	my $type;
	my $length;
	my $precision;
	my $scale;
	if ($attr) {
		if (!ref($attr)) {
			$type=$attr;
		} elsif (ref($attr) eq 'HASH') {
			$type=$attr->{type} || $attr->{Type} || $attr->{TYPE};
			$length=$attr->{length};
			$precision=$attr->{precision};
			$scale=$attr->{scale};
		}
	}
	if (!defined($length)) {
		$length=length($val);
	}

	# bind the parameter
	my $cursor=$sth->FETCH('driver_cursor');
	if ($type eq 'DBD::SQLRelay::SQL_CLOB') {
		$cursor->inputBindClob($param,$val,$length);
	} elsif ($type eq 'DBD::SQLRelay::SQL_BLOB') {
		$cursor->inputBindBlob($param,$val,$length);
	} elsif (defined($precision) && defined($scale)) {
		$cursor->inputBind($param,$val,$precision,$scale);
	} else {
		$cursor->inputBind($param,$val,$length);
	}

	# update ParamValues, ParamTypes
	if (!$sth->FETCH('ParamValues')) {
		$sth->STORE('ParamValues',{});
	}
	$sth->FETCH('ParamValues')->{$param}=$val;
	if (!defined($type)) {
		$type='SQL_VARCHAR';
	}
	if (!$sth->FETCH('ParamTypes')) {
		$sth->STORE('ParamTypes',{});
	}
	$sth->FETCH('ParamTypes')->{$param}=$type;
	return 1;
}

sub bind_param_inout {

	# get parameters
	my ($sth,$param,$variable,$maxlen,$attr)=@_;

	# determine type, length, precision, scale...
	my $type;
	if ($attr) {
		if (!ref($attr)) {
			$type=$attr;
		} elsif (ref($attr) eq 'HASH') {
			$type=$attr->{type} || $attr->{Type} || $attr->{TYPE};
		}
	}

	# bind the parameter
	my $cursor=$sth->FETCH('driver_cursor');
	if ($type eq 'DBD::SQLRelay::SQL_CLOB') {
		$cursor->defineOutputBindClob($param);
	} elsif ($type eq 'DBD::SQLRelay::SQL_BLOB') {
		$cursor->defineOutputBindBlob($param);
	} else {
		$cursor->defineOutputBindString($param,$maxlen);
	}

	# store the parameter name in the list of inout parameters
	my $param_inout_list=$sth->FETCH('driver_param_inout_list');
	$param_inout_list=$param_inout_list.' '.$param;
	$sth->STORE('driver_param_inout_list',$param_inout_list);

	# store the variable so data can be fetched into it later
	$sth->STORE('driver_param_inout_'.$param,$variable);

	# store the variable type
	$sth->STORE('driver_param_inout_type_'.$param,$type);

	return 1;
}

sub execute {

	# get parameters
	my ($sth,@bind_values)=@_;
	my $dbh=$sth->{'Database'};

	# handle binds
	my $cursor=$sth->FETCH('driver_cursor');

	# Clear and reset binds if they are being passed to execute()
	if (scalar(@bind_values)) {
		if (@bind_values!=$sth->FETCH('NUM_OF_PARAMS')) {
			return $dbh->set_err(1,'Expected '.
					$sth->FETCH('NUM_OF_PARAMS').
					' bind values but was given '.
					@bind_values);
		}

		my $index=1;
		my $bind_value;
		foreach $bind_value (@bind_values) {
			$sth->bind_param($index,$bind_value) or return;
			$index=$index+1;
		}
	}

	# send the query
	if (not $cursor->executeQuery()) {
		$sth->STORE('driver_NUM_OF_ROWS',0);
		if (!$sth->FETCH('NUM_OF_FIELDS')) {
			$sth->STORE('NUM_OF_FIELDS',0);
		}
		$sth->STORE('driver_FETCHED_ROWS',0);
		$sth->STORE('driver_RowsInCache',0);
		return $dbh->DBI::set_err(1,$cursor->errorMessage());
	}

	# get some result set info
	my $colcount=$cursor->colCount();
	my $rowcount=$cursor->rowCount();
	my @colnames=map {$cursor->getColumnName($_)} (0..$colcount-1);
	my @coltypes=map {$cursor->getColumnType($_)} (0..$colcount-1);
	my @colprecision=map {$cursor->getColumnPrecision($_)} (0..$colcount-1);
	my @colscale=map {$cursor->getColumnScale($_)} (0..$colcount-1);
	my @colnullable=map {$cursor->getColumnIsNullable($_)} (0..$colcount-1);
 	if (!$sth->FETCH('NUM_OF_FIELDS')) {
 		$sth->STORE('NUM_OF_FIELDS',$colcount);
 	}
	$sth->{NAME}=\@colnames;
	$sth->{TYPE}=\@coltypes;
	$sth->{PRECISION}=\@colprecision;
	$sth->{SCALE}=\@colscale;
	$sth->{NULLABLE}=\@colnullable;
	$sth->STORE('driver_FETCHED_ROWS',0);
	$sth->STORE('driver_RowsInCache',$cursor->rowCount());

	# get the list of output bind variables and turn it into an array
	my $param_inout_list=$sth->FETCH('driver_param_inout_list');
 	my @param_inout_array=split(' ',$param_inout_list || '');

	# loop through the array of parameters, for each, get the appropriate
	# variable and store the output bind data in the variable
	my $param;
	foreach $param(@param_inout_array) {
		my $variable=$sth->FETCH('driver_param_inout_'.$param);
		my $type=$sth->FETCH('driver_param_inout_type_'.$param);
		if ($type eq 'DBD::SQLRelay::SQL_CLOB') {
			$$variable=$cursor->getOutputBindClob($param);
		} elsif ($type eq 'DBD::SQLRelay::SQL_BLOB') {
			$$variable=$cursor->getOutputBindBlob($param);
		} else {
			$$variable=$cursor->getOutputBindString($param);
		}
	}

	# mark this statement Active
	$sth->STORE('Active',1);

	my $rows=$sth->rows();
	if ($rows==0) {
		return '0E0';
	}
	return $sth->rows;
}

sub fetchrow_arrayref {

	# get parameters
	my ($sth)=@_;

	# get the number of rows fetched so far
	my $fetched_rows=$sth->FETCH('driver_FETCHED_ROWS');

	# get a row
	my @row=$sth->FETCH('driver_cursor')->getRow($fetched_rows);
	if (scalar(@row)==0) {
		$sth->STORE('driver_RowsInCache',0);
		$sth->finish();
		return undef;
	}

	# increment the fetched row count
	$sth->STORE('driver_FETCHED_ROWS',$fetched_rows+1);

	# update rows in cache
	my $rowsincache=$sth->FETCH('driver_RowsInCache');
	if ($rowsincache==0) {
		my $rowcachesize=$sth->{'Database'}->FETCH('RowCacheSize');
		if ($rowcachesize>0) {
			$rowsincache=$rowcachesize;
		}
	}
	if ($rowsincache>0) {
		$rowsincache--;
	}
	$sth->STORE('driver_RowsInCache',$rowsincache);

	# chop blanks, if that's set
	if ($sth->FETCH('ChopBlanks')) {
		map { $_=~s/\s+$//; } @row;
	}

	return $sth->_set_fbav(\@row);
}


# required alias for fetchrow_arrayref
*fetch=\&fetchrow_arrayref;

sub rows {

	# get parameters
	my ($sth)=@_;

	# return the number of affected rows
	return $sth->FETCH('driver_cursor')->affectedRows();
}

sub finish {
	
	# get parameters
	my ($sth)=@_;

	# mark this statement not Active
	# (older DBI's don't do this in their finish methods)
	$sth->STORE('Active',0);

	# call finish from the parent class
	$sth->SUPER::finish();
}

sub DESTROY {
	
	# get parameters
	my ($sth)=@_;

	# mark this statement not Active
	# (older DBI's don't do this in their DESTROY methods)
	$sth->STORE('Active',0);

	# call finish from the parent class
	$sth->SUPER::DESTROY();
}

sub STORE {

	# get parameters
	my ($sth,$attr,$val)=@_;

	# special cases...
	if ($attr =~ /^ext_SQLR_BufferSize$/) {
		my $cursor=$sth->FETCH('driver_cursor');
		$cursor->setResultSetBufferSize($val);
		return 1;
	} elsif ($attr eq 'RowsInCache') {
		$sth->{'driver_RowsInCache'}=$val;
		return 1;
	} elsif ($attr eq 'ParamValues') {
		$sth->{'driver_ParamValues'}=$val;
		return 1;
	} elsif ($attr eq 'ParamTypes') {
		$sth->{'driver_ParamTypes'}=$val;
		return 1;
	}

	# handle all other cases
	if ($attr =~ /^driver_/) {
		$sth->{$attr}=$val;
		return 1;
	}

	# if the attribute didn't start with 'driver_' 
	# then pass it up to the parent class
	return $sth->SUPER::STORE($attr,$val);
}

sub FETCH {

	# get parameters
	my ($sth,$attr)=@_;

	# special cases...
	if ($attr =~ /^ext_SQLR_BufferSize$/) {
		my $cursor=$sth->FETCH('driver_cursor');
		return $cursor->getResultSetBufferSize();
	} elsif ($attr eq 'RowsInCache') {
		return $sth->{'driver_RowsInCache'};
	} elsif ($attr eq 'ParamValues') {
		return $sth->{'driver_ParamValues'};
	} elsif ($attr eq 'ParamTypes') {
		return $sth->{'driver_ParamTypes'};
	}

	# handle all other cases
	if ($attr =~ /^driver_/) {
		return $sth->{$attr};
	}

	# if the attribute didn't start with 'driver_' 
	# then pass it up to the parent class
	$sth->SUPER::FETCH($attr);
}

1;
__END__
#

=head1 NAME

DBD::SQLRelay - perl DBI driver for SQL Relay 

=head1 SYNOPSIS

use DBD::SQLRelay;

my $dbh = DBI -> connect ('dbi:SQLRelay:$dsn', $login, $password);

=head1 DESCRIPTION

This module is a pure-Perl DBI binding to SQL Relay's native API. 
Connection string consists of following parts:

=over

=item B<host=...>      default: I<localhost> --- hostname of SQL Relay server;

=item B<port=...>      default: I<9000>      --- port number that SQL Relay server listens on;

=item B<tries=...>     default: I<1>         --- how much times do we try to connect;

=item B<retrytime=...> default: I<0>         --- time (in seconds) between connect attempts;

=item B<debug=...>     default: I<0>         --- set it to 1 if you want to get some debug messages in stdout;

=back

=head1 USAGE

Once connected, DB handler works as usual (see L<DBI>). 

Don't ever try to share one SQLRelay connect by multiple scripts, for example, if you use 
Apache mod_perl. Every $dbh holds one of server connections, so call disconnect() directly
at the end of every script and don't use Apache::DBI or SQLRelay will be deadlocked.

=head2 Note for HTML::Mason Users

If you use L<HTML::Mason>, your handler.pl sould look like this:

  ...

     {
       package HTML::Mason::Commands;
       use DBI;
       use vars qw($db);  
     }
 
  ...

     sub handler {
       
       $HTML::Mason::Commands::dbh = DBI -> connect (...);
       
       my $status = $ah -> handle_request (...);
     
       $HTML::Mason::Commands::dbh -> disconnect;
       
       return $status;
              
     }
     

=head1 AUTHOR

D. E. Ovsyanko, do@mobile.ru

Contributions by:

Erik Hollensbe <erik@hollensbe.org>

Tony Fleisher <tfleisher@musiciansfriend.com>

=head1 SEE ALSO

http://www.firstworks.com

=cut
