use strict;
use lib '/opt/zimbra/wk_ddc/lib/lib/perl5';
use Net::IMAP::Simple;
use Encode;
use Encode::IMAPUTF7;

use Data::Dumper;

binmode *STDOUT, ':utf8';

# the host address and port of NSIM server
my $host = "172.17.1.15";
my $port = "30143";

# user and password
my $testUser = $ARGV[0];
my $password = 'password00';

my $hostPort = $host . ":" . "$port";

my $server = new Net::IMAP::Simple($hostPort, debug =>0);
my $ret = $server->login($testUser, $password);

# the maillist before deleting
my @boxes1 = $server->mailboxes;
print "================Before deleting: ==============\n";
for(@boxes1)
{
	print "$_:" . $server->select($_) . "\n";
}
#print "Before deleting, mailboxes:@boxes1" . "\n";


#delete mail and mailbox 
for(my $i = $#boxes1; $i >= 0; $i--)
{	
	if($boxes1[$i] ne "INBOX")
	{
		#print "$boxes1[$i]\n";
		$server->delete_mailbox($boxes1[$i]);
	}
	else
	{
		$server->select($boxes1[$i]);
		
		#get the message number of the last message in INBOX
		my $message_number = $server->last;
		#print "message_number:$message_number\n";
		for(my $j = 0; $j <= $message_number; $j++)
		{
			$server->delete($j);
		}
		
		#delete the whole messages in INBOX
		my $expunged = $server->expunge_mailbox($boxes1[$i]) or die $server->errstr;
	}
}

# the maillist after deleting
my @boxes2 = $server->mailboxes;
print "================After deleting: ===============\n";
for(@boxes2)
{
	print "$_:" . $server->select($_) . "\n";
}


$server->quit();

