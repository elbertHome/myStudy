use strict;
use lib '/opt/zimbra/wk_ddc/lib/lib/perl5';
use Net::IMAP::Simple;
use Encode;
use Encode::IMAPUTF7;

use Data::Dumper;

binmode *STDOUT, ':utf8';

# the host address and port of MBS server
my $host = "15.210.145.138";
my $port = "7143";

# user and password
my $testUser = $ARGV[0];
my $password = 'password00';

my $hostPort = $host . ":" . "$port";

# Init Account(Delete & Create)
print "=========Delete & Create Account===============" . "\n";
system("zmprov da $testUser");
my $createUser = "zmprov ca $testUser\@ezweb.ne.jp $password ezwebSubscriberId ${testUser}_f3_1.ezweb.ne.jp ezwebUserPassword $password";
system($createUser) == 0 or die "system ca $testUser failed:$?";
`zmprov ma $testUser zimbraPrefIncludeSpamInSearch "TRUE"`;
print "Account:$testUser was created.\n";
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

tes';

my $Bigmail = 'Date: Fri, 10 Oct 2014 00:%MIN%:00 +0800 (CST)
From: txl <test1@atmail-zimbra.com>
To: mgr011@ezweb.ne.jp
Message-ID: <512474120.34.1412926661772.JavaMail.root@atmail-zimbra.com>
Subject: testBigCase%NUM%
MIME-Version: 1.0
Content-Type: text/plain; charset=utf-8
Content-Transfer-Encoding: 7bit
X-Originating-IP: [16.165.188.38]
X-Mailer: Zimbra 7.1.4_GA_2555 (ZimbraWebClient - IE7 (Win)/7.1.4_GA_2555)

woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
woshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguorwoshizhongguor
';


# Create the object
my $server = new Net::IMAP::Simple($hostPort, debug =>0);

# Log on
my $ret = $server->login($testUser, $password);

# create mailbox
$server->create_mailbox("Inbox/sub1") or print "create Inbox/sub1 ng\n";
$server->create_mailbox("sub5") or print "create sub5 ng\n";
$server->create_mailbox("sub6/sub7") or print "create sub6/sub7 ng\n";
$server->create_mailbox("sub8/sub9/sub10") or print "create sub8/sub9/sub10 ng\n";
$server->create_mailbox("sub8/sub9/sub11") or print "create sub8/sub9/sub11 ng\n";
$server->create_mailbox("sub8/sub9/sub10/sub12") or print "create sub8/sub9/sub10/sub12 ng\n";
$server->create_mailbox("sub13/sub14") or print "create sub13/sub14 ng\n";
$server->create_mailbox("sub15") or print "create sub15 ng\n";


# insert mail
my $min = 0;
for (my $i = 1; $i < 30; $i++,$min++)
{
    my $local_mail = $mail;
    my $m = $min % 60;
	$local_mail =~ s/%NUM%/$i/;
	$local_mail =~ s/%MIN%/$m/;
	$server->put('Inbox', $local_mail);
	$server->put('Sent', $local_mail);
	$server->put('CustomFolderCustomFolderCustomFolderCustomFolderCustomFolder12345', $local_mail);
	$server->put('sub5', $local_mail);
	$server->put('sub6/sub7', $local_mail);
	$server->put('sub8/sub9/sub10/sub12', $local_mail);
}

for (my $i = 1; $i <= 50; $i++,$min++)
{
    my $local_mail1 = $mail;
    my $local_mail2 = $Bigmail;
    my $m = $min % 60;
	$local_mail1 =~ s/%NUM%/$i/;
	$local_mail1 =~ s/%MIN%/$m/;
	$local_mail2 =~ s/%NUM%/$i/;
	$local_mail2 =~ s/%MIN%/$m/;

	$server->put('Inbox/sub1', $local_mail1);
	$server->put('sub13/sub14', $local_mail2);
	$server->put('sub8', $local_mail2);

}

for (my $i = 1; $i <= 50; $i++,$min++)
{
    my $local_mail1 = $mail;
    my $local_mail2 = $Bigmail;
    my $m = $min % 60;
	$local_mail1 =~ s/%NUM%/$i/;
	$local_mail1 =~ s/%MIN%/$m/;
	$local_mail2 =~ s/%NUM%/$i/;
	$local_mail2 =~ s/%MIN%/$m/;
	$server->put('Inbox', $local_mail1);
	$server->put('sub6/sub7', $local_mail2);

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
	print "$subFolder:" . $server->select($_) . "\n";
}


$server->quit();

