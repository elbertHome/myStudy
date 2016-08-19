use strict;
use lib '/opt/zimbra/wk_ddc/lib/lib/perl5';
use Net::IMAP::Simple;
use Encode;
use Encode::IMAPUTF7;

use Data::Dumper;

binmode *STDOUT, ':utf8';

# the host address and port of NSIM server
my $host = "15.210.145.138";
my $port = "7143";

# user and password
my $testUser = $ARGV[0];
my $password = 'password00';

my $hostPort = $host . ":" . "$port";

# Create the object
my $server = new Net::IMAP::Simple($hostPort, debug =>0);
# Log on
my $ret = $server->login($testUser, $password);

# print mail list
print "====================List mail list start====================\n";
foreach ($server->mailboxes)
{
    my $count = ($_ =~ s#/#/#g);
	for(my $i = 0; $i < $count; $i++)
	{
		print "\t";
	}
	my $lastCur = rindex($_,"/");
	my $subFolder = substr($_, $lastCur+1);
	print "$subFolder:" . $server->select($_) . "\n";
}
print "====================List mail list end====================\n";

$server->quit();

