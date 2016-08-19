use strict;
use lib '/opt/zimbra/wk_ddc/lib/lib/perl5';
use Net::IMAP::Simple;
use Encode;
use Encode::IMAPUTF7;

use Data::Dumper;

binmode *STDOUT, ':utf8';

# the host address and port of MBS server
my $host = "172.17.1.15";
my $port = "30143";

# user and password
my $testUser = $ARGV[0];
my $password = 'password00';

my $hostPort = $host . ":" . "$port";

# the content of mail
my $mail = 'Date: Fri, 10 Oct 2014 00:%MIN%:00 +0800 (CST)
From: txl <test1@atmail-zimbra.com>
To: mgr011@ezweb.ne.jp
Message-ID: <512474120.34.1412926661772.JavaMail.root@atmail-zimbra.com>
Subject: testCase%NUM%
MIME-Version: 1.0
Content-Type: text/plain; charset=utf-8
Content-Transfer-Encoding: 7bit
X-Originating-IP: [16.165.188.38]
X-Mailer: Zimbra 7.1.4_GA_2555 (ZimbraWebClient - IE7 (Win)/7.1.4_GA_2555)

testtesttest011';

# Create the object
my $server = new Net::IMAP::Simple($hostPort, debug =>0);

# Log on
my $ret = $server->login($testUser, $password);


my $folders4 = "folder4";
#$folders4 = decode('UTF-8', $folders4);
#$folders4 = encode('IMAP-UTF-7', $folders4);

my $folders5 = "folder5";
#$folders5 = decode('UTF-8', $folders5);
#$folders5 = encode('IMAP-UTF-7', $folders5);

my $folders6 = "folder6";
#$folders6 = decode('UTF-8', $folders6);
#$folders6 = encode('IMAP-UTF-7', $folders6);

my $folders7 = "folder7";
#$folders7 = decode('UTF-8', $folders7);
#$folders7 = encode('IMAP-UTF-7', $folders7);

my $folders8 = "folder8";
#$folders8 = decode('UTF-8', $folders8);
#$folders8 = encode('IMAP-UTF-7', $folders8);

my $folders9 = "folder9";
#$folders9 = decode('UTF-8', $folders9);
#$folders9 = encode('IMAP-UTF-7', $folders9);

# create mailbox
$server->create_mailbox($folders4) or print "create ng\n";
$server->create_mailbox($folders5) or print "create ng\n";
$server->create_mailbox($folders6) or print "create ng\n";
$server->create_mailbox($folders7) or print "create ng\n";
$server->create_mailbox($folders8) or print "create ng\n";
$server->create_mailbox($folders9) or print "create ng\n";


# insert mail
my $min = 0;
for (my $i = 1; $i < 3; $i++,$min++)
{
    my $local_mail = $mail;
    my $m = $min % 60;
	$local_mail =~ s/%NUM%/$i/;
	$local_mail =~ s/%MIN%/$m/;
	$server->put('Inbox', $local_mail);
	$server->put($folders4, $local_mail);
	$server->put($folders5, $local_mail);
    $server->put($folders6, $local_mail);
	$server->put($folders7, $local_mail);
	$server->put($folders8, $local_mail);
	$server->put($folders9, $local_mail);
}

# print mail list
print "========After Create mailboxes & mails:================" . "\n";
foreach ($server->mailboxes)
{
	my $count = ($_ =~ s#/#/#g);
	for(my $i = 0; $i < $count; $i++)
	{
		print "\t";
	}
	my $lastCur = rindex($_,"/");
	my $subFolder = substr($_, $lastCur+1);
	$subFolder = decode('IMAP-UTF-7', $subFolder);
	print "$subFolder:" . $server->select($_) . "\n";
}

$server->quit();

