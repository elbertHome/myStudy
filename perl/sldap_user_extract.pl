#!/usr/bin/perl
################################################################################
# ¼ïÊÌ            ´Ø¿ôÌ¾                              
#¥Õ¥¡¥¤¥ëÌ¾ :   sldap_user_extract.pl         sldap¥æ¡¼¥¶¾ðÊóÃê½Ð¥Ä¡¼¥ë
#ºîÀ®¼Ô     :   ·ºÃæÎ¼
#ºîÀ®Æü     :   2014/10/13
#ÊÑ¹¹ÍúÎò
################################################################################
use strict;
use warnings;
################################################################################
# ´Ø¿ôÀë¸À
################################################################################
sub exUserExtractMain();
sub userExtractAvgCheck();
sub userExtractRstFileOpen($);
sub getLdapSearchCmd();
sub getBlockData($);
################################################################################

################################################################################
# Á´¶ÉÄê¿ôÄêµÁ
my $g_LDAP_LOGIN_USER  = "\"uid=smadmin,ou=SysAccounts,dc=admin,dc=local\"";
my $g_LDAP_SRV_ADDRESS = "127.0.0.1:389";
################################################################################

#¼Â¹Ô¥á¡¼¥ó´Ø¿ô¤ò¸Æ¤Ó½Ð¤¹
&exUserExtractMain() or exit 1;

################################################################################
# ´Ø¿ôÌ¾                           ¼Â¹Ô¥á¡¼¥ó´Ø¿ô
# ´Ø¿ô¸Æ½Ð·Á¼°                     userExtractMain();
# ´Ø¿ô³µÍ×                         ¥æ¡¼¥¶¾ðÊóÃê½Ð½èÍý¤ò¹Ô¤¦
#
#Ãí°Õ»ö¹à                          ¤Ê¤·
#
#Æþ½ÐÎÏ¥Ñ¥é¥á¡¼¥¿
#¼ïÊÌ            R/W
#-------------------------------------------------------------------------------
#Ìá¤êÃÍ
#
#°ú  ¿ô
#
# ÊÑ¹¹ÍúÎò
#           1    2014/10/13    ·º¡¡ÃæÎ¼    ¿·µ¬ºîÀ®
################################################################################
sub exUserExtractMain() {
	#¡¡¼Â¹Ô·ï¿ô
	my $resultOK = 0;
	my $resultNG = 0;
	my $restoreOK = 0;
	my $restoreNG = 0;
	eval
	{	
	    #µ¯Æ°¥Ñ¥é¥á¡¼¥¿¤Î¸Ä¿ô¤ò¥Á¥§¥Ã¥¯
	    &userExtractAvgCheck();
		#¼Â¹Ô¥¿¥¤¥à¥¢¥¦¥È¤òÀßÄê
 		local $SIG{ALRM} = 
			sub { 
				die "sldap_user_extract_failed. ¥¨¥é¡¼ÆâÍÆ(ldapsearch timeout).\n";
				}; 
		alarm $ARGV[3];		

	    #·ë²Ì¥Õ¥¡¥¤¥ë¤Î¥Ï¥ó¥É¥ë¥ê¥¹¥È¤ò¼èÆÀ
	    my @resFileHds = &userExtractRstFileOpen($ARGV[2]);
	    #ldapsearch¥³¥Þ¥ó¥É¤ò¼èÆÀ
	    my $cmd = &getLdapSearchCmd();
	
		my @cmdRtn;
		my @blockRtn;
		my $resultFileHd;
		my $restoreFileHd;
		# ÆÉ¤ß¹þ¤àÊ¬³äÉä¹æ¤ò»ØÄê
		local $/ = "\n\n";
	    #ldapsearch¥³¥Þ¥ó¥É¤ò¼Â¹Ô
	    @cmdRtn = `$cmd`;
	    if ($?)
		{
			foreach(@cmdRtn)
			{
				print;
			}
			die "sldap_user_extract_failed. ¥¨¥é¡¼ÆâÍÆ(ldapsearch error).\n";
		}
		#¥³¥Þ¥ó¥É·ë²Ì¤ò¼èÆÀ
	    foreach (@cmdRtn)	
		{
			chomp;
			@blockRtn = &getBlockData($_);
			#Result¥Õ¥¡¥¤¥ë¤ÎÌá¤êÃÍ¤ò¼èÆÀ
			if($blockRtn[0])
			{
				$resultFileHd = $resFileHds[0];
				$resultOK ++;
			}
			else
			{
				$resultFileHd = $resFileHds[2];
				$resultNG ++;
			}
			#Restore¥Õ¥¡¥¤¥ë¤ÎÌá¤êÃÍ¤ò¼èÆÀ
			if($blockRtn[2])
			{
				$restoreFileHd = $resFileHds[1];
				$restoreOK ++;
			}
			else
			{
				$restoreFileHd = $resFileHds[3];
				$restoreNG ++;
			}
			#¥Õ¥¡¥¤¥ë¤Ë½ñ¤­¹þ¤à
			unless (print $resultFileHd ($blockRtn[1]))
			{
				die "sldap_user_extract_failed. ¥¨¥é¡¼ÆâÍÆ(result_file_write error).\n";
			}
			 unless (print $restoreFileHd ($blockRtn[3]))
			{
				die "sldap_user_extract_failed. ¥¨¥é¡¼ÆâÍÆ(restore_file_write error).\n";
			}
		}
		alarm 0;
	};

	if ($@)
	{
		#¼Â¹Ô¼ºÇÔ¤Î¾ì¹ç
		print $@;
		return 0;
	}
	else
	{
		#¼Â¹ÔÀ®¸ù¤Î¾ì¹ç
		print "sldap_user_extract succeeded.\n";
		print "\tresult_file:  Success: " . $resultOK . ", Skip: " . $resultNG . "\n";
		print "\trestore_file: Success: " . $restoreOK . ", Skip: " . $restoreNG . "\n";
	}
	return 1;
}
################################################################################
# ´Ø¿ôÌ¾                           µ¯Æ°¥Ñ¥é¥á¡¼¥¿¥Á¥§¥Ã¥¯
# ´Ø¿ô¸Æ½Ð·Á¼°                     userExtractAvgCheck;
# ´Ø¿ô³µÍ×                         µ¯Æ°¥Ñ¥é¥á¡¼¥¿¤Î¥Á¥§¥Ã¥¯
#
#Ãí°Õ»ö¹à                          ¤Ê¤·
#
#Æþ½ÐÎÏ¥Ñ¥é¥á¡¼¥¿
#¼ïÊÌ            R/W
#-------------------------------------------------------------------------------
#Ìá¤êÃÍ
#
#°ú  ¿ô
#
# ÊÑ¹¹ÍúÎò
#           1    2014/10/13    ·º¡¡ÃæÎ¼    ¿·µ¬ºîÀ®
################################################################################
sub userExtractAvgCheck() {
    my $usage = "usage : sldap_user_extract.sh ldap_passwd mailHost result_filepath timer(0-10000).\n";

    #¥Ñ¥é¥á¡¼¥¿¸Ä¿ô¥Á¥§¥Ã¥¯
    if ( scalar(@ARGV) != 4 ) 
	{
        # ¥¨¥é¡¼¥á¥Ã¥»¡¼¥¸¤ò½Ð¤¹
        die "µ¯Æ°¥Ñ¥é¥á¡¼¥¿¸Ä¿ô¤¬ÉÔÀµ¤Ç¤¹¡£\n$usage";
    }
	else
	{
		if ((($ARGV[3])=~m/[^0-9]/) || $ARGV[3] < 0 || $ARGV[3] > 10000)
		{
			die "ldapsearch¤Î¥¿¥¤¥à¥¢¥¦¥ÈÃÍ[0-10000]ÉÃ»ØÄê¡£\n$usage";
		}
	}
}

################################################################################
# ´Ø¿ôÌ¾                           ¼Â¹Ô·ë²Ì¥Õ¥¡¥¤¥ë¤ò³«¤¯
# ´Ø¿ô¸Æ½Ð·Á¼°                     userExtractRstFileOpen;
# ´Ø¿ô³µÍ×                         ¼Â¹Ô·ë²Ì¥Õ¥¡¥¤¥ë¤ò³«¤¯´Ø¿ô
#
#Ãí°Õ»ö¹à                          ¤Ê¤·
#
#Æþ½ÐÎÏ¥Ñ¥é¥á¡¼¥¿
#¼ïÊÌ            R/W
#-------------------------------------------------------------------------------
#Ìá¤êÃÍ                            ¼Â¹Ô·ë²Ì¥Õ¥¡¥¤¥ë¤Î¥Ï¥ó¥É¥ë¥ê¥¹¥È¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡
#
#°ú  ¿ô¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¼Â¹Ô·ë²Ì¥Õ¥¡¥¤¥ë¤ÎÊÝÂ¸¥Ç¥£¥ì¥¯¥È¥ê
#
# ÊÑ¹¹ÍúÎò
#           1    2014/10/13    ·º¡¡ÃæÎ¼    ¿·µ¬ºîÀ®
################################################################################
sub userExtractRstFileOpen($) {

    # ´Ø¿ô°ú¿ô¡Ê¼Â¹Ô·ë²Ì¥Õ¥¡¥¤¥ë¤ÎÊÝÂ¸¥Ç¥£¥ì¥¯¥È¥ê¤ò¼èÆÀ¡Ë
    my $rstPath = shift(@_);

	# ¥Ç¥£¥ì¥¯¥È¥ê¤¬Â¸ºß¤·¤Æ¤¤¤Ê¤¤¾ì¹ç¡¢ºîÀ®¤¹¤ë
	unless (-d $rstPath)
	{	
		mkdir $rstPath , 0755 or return;
	}

    # ·ë²Ì¥Õ¥¡¥¤¥ë¥Ï¥ó¥É¥ë¤òÄêµÁ
    my $EXTRACT_RESULT_fh;
    my $EXTRACT_RESTORE_fh;
    my $EXTRACT_RESULT_SKIP_fh;
    my $EXTRACT_RESTORE_SKIP_fh;

      # ÀÚÂØÍÑ¥æ¡¼¥¶¥ê¥¹¥È¤Î·ë²Ì¥Õ¥¡¥¤¥ë¤ò³«¤¯
	unless ( open $EXTRACT_RESULT_fh, '>', $rstPath . "/sldap_user_extract_result.list" )
    {
       # ¥¨¥é¡¼¥á¥Ã¥»¡¼¥¸¤ò½Ð¤¹
       die "sldap_user_extract failed. ¥¨¥é¡¼ÆâÍÆ(result_file open error). $!\n";
    }

    # ¥ê¥¹¥È¥¢ÍÑ¥æ¡¼¥¶¥ê¥¹¥È¤Î·ë²Ì¥Õ¥¡¥¤¥ë¤ò³«¤¯
    unless ( open $EXTRACT_RESTORE_fh, '>', $rstPath . "/sldap_user_extract_restore.list" )
    {
       # ¥¨¥é¡¼¥á¥Ã¥»¡¼¥¸¤ò½Ð¤¹
       die "sldap_user_extract failed. ¥¨¥é¡¼ÆâÍÆ(restore_file open error). $!\n";
    }

    # ÀÚÂØÍÑ¥æ¡¼¥¶¤Î¥¹¥­¥Ã¥×¥ê¥¹¥È¤Î·ë²Ì¥Õ¥¡¥¤¥ë¤ò³«¤¯
    unless ( open $EXTRACT_RESULT_SKIP_fh, '>', $rstPath . "/sldap_user_extract_skip.list" )
    {
       # ¥¨¥é¡¼¥á¥Ã¥»¡¼¥¸¤ò½Ð¤¹
       die "sldap_user_extract failed. ¥¨¥é¡¼ÆâÍÆ(result_file open error). $!\n";
    }

    # ¥ê¥¹¥È¥¢ÍÑ¥æ¡¼¥¶¤Î¥¹¥­¥Ã¥×¥ê¥¹¥È¤Î·ë²Ì¥Õ¥¡¥¤¥ë¤ò³«¤¯
    unless ( open $EXTRACT_RESTORE_SKIP_fh,'>', $rstPath . "/sldap_user_extract_restore_skip.list" )
    {
       # ¥¨¥é¡¼¥á¥Ã¥»¡¼¥¸¤ò½Ð¤¹
       die "sldap_user_extract failed. ¥¨¥é¡¼ÆâÍÆ(restore_file open error) $!\n";
    }

    return (($EXTRACT_RESULT_fh,$EXTRACT_RESTORE_fh,$EXTRACT_RESULT_SKIP_fh, $EXTRACT_RESTORE_SKIP_fh));
}

################################################################################
# ´Ø¿ôÌ¾                           ldapsearch ¥³¥Þ¥ó¥É¥é¥¤¥ó¤òºîÀ®
# ´Ø¿ô¸Æ½Ð·Á¼°                     getLdapSearchCmd;
# ´Ø¿ô³µÍ×                         ldapsearch ¥³¥Þ¥ó¥É¥é¥¤¥ó¤òºîÀ®
#
#Ãí°Õ»ö¹à                          ¤Ê¤·
#
#Æþ½ÐÎÏ¥Ñ¥é¥á¡¼¥¿
#¼ïÊÌ            R/W
#-------------------------------------------------------------------------------
#Ìá¤êÃÍ                            ¤Ê¤·¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡
#
#°ú  ¿ô¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¤Ê¤·
#
# ÊÑ¹¹ÍúÎò
#           1    2014/10/13    ·º¡¡ÃæÎ¼    ¿·µ¬ºîÀ®
################################################################################
sub getLdapSearchCmd() {

    #µ¯Æ°¥Ñ¥é¥á¡¼¥¿¤ò¼èÆÀ
    my $ldpPWD     = shift(@ARGV);
    my $ldpMailHST = shift(@ARGV);
    my $rstPath    = shift(@ARGV);

    #¥³¥Þ¥ó¥É¥é¥¤¥ó¤òÁÈ¤ß¹ç¤ï¤»
    my $cmd .= "ldapsearch -h $g_LDAP_SRV_ADDRESS -x -D $g_LDAP_LOGIN_USER -w $ldpPWD -LLL -b \"ou=People,dc=ezweb,dc=ne,dc=jp\" ";
    unless ( $ldpMailHST =~ m/\ball\b/i ) 
	{
        $cmd .= "\"mailHost=$ldpMailHST\" ";
    }
    $cmd .= "sn cn postalCode mailHost modifyTimestamp 2>&1";
    return $cmd;
}

################################################################################
# ´Ø¿ôÌ¾                           ¼Â¹Ô·ë²Ì¤«¤é°ìblock¥Ç¡¼¥¿¤òlist¤Î°ì¹Ô¤ËÊÑ´¹
# ´Ø¿ô¸Æ½Ð·Á¼°                     getBlockData;
# ´Ø¿ô³µÍ×                         ¼Â¹Ô·ë²Ì¤«¤é°ìblock¥Ç¡¼¥¿¤òlist¤Î°ì¹Ô¤ËÊÑ´¹
#
#Ãí°Õ»ö¹à                          ¤Ê¤·
#
#Æþ½ÐÎÏ¥Ñ¥é¥á¡¼¥¿
#¼ïÊÌ            R/W
#-------------------------------------------------------------------------------
#Ìá¤êÃÍ¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡ ­¡Result¥Õ¥¡¥¤¥ë¤ÎÌá¤ë¥Õ¥é¥°
#							¡¡¡¡¡¡­¢Result¥Õ¥¡¥¤¥ë¤ÎÌá¤ëÊ¸»úÎó
#							¡¡¡¡¡¡­£Restore¥Õ¥¡¥¤¥ë¤ÎÌá¤ë¥Õ¥é¥°
#							¡¡¡¡¡¡­¤Restore¥Õ¥¡¥¤¥ë¤ÎÌá¤ëÊ¸»úÎó¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡
#
#°ú  ¿ô¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡­¡ldapsearch¥³¥Þ¥ó¥É¤Î¼Â¹Ô·ë²Ì¤Î1 Block
#
# ÊÑ¹¹ÍúÎò
#           1    2014/10/13    ·º¡¡ÃæÎ¼    ¿·µ¬ºîÀ®
################################################################################
sub getBlockData($) 
{
	# £±¥Ö¥í¥Ã¥¯¤Î¥Ç¡¼¥¿¤òsplit
	my @blockData = split "\n",$_[0];

	# Ìá¤ë¥Õ¥é¥° (default = 1)
	my $resultFlg = 1;
	my $restoreFlg = 1;
    # Ìá¤ëÊ¸»úÎó
	my $result = '';
	my $restore = '';

	# ·ë²ÌHash
	my %RES;

	# blockData¤òhash¤ËÊÝÂ¸
	foreach(@blockData)
	{
		chomp;
		if (m/: /)
		{
			#¡¡Éä¹æ:Á°¤ÎÃÍ¤ò¥­¡¼¤È¤·¤Æ¡¢Éä¹æ¡§¸å¤í¤ÎÃÍ¤òValue¤È¤·¤Æ¡¢Hash¤ËÊÝÂ¸
			$RES{$`} = $';
		}
	}
	#·ë²Ì¥Á¥§¥Ã¥¯
	#cn¤ÎÂ¸ºß¥Á¥§¥Ã¥¯
	if ( ! defined($RES{'cn'}))
	{
		# cn¤¬¤Ê¤¤¾ì¹ç¡¢¥À¥Ã¥ß¤òÍ¿¤¨¤Þ¤¹¡£
		$RES{'cn'} = '12345678901234_dummy';
	}
	#sn¤Î·å¿ô¤ÈÊ¸»ú¥¿¥¤¥×¥Á¥§¥Ã¥¯
	if (! defined($RES{'sn'}))
	{
		# snÂ¸ºß¤·¤Ê¤¤¾ì¹ç¡¢¶õ¤òÍ¿¤¨¤ë
		$RES{'sn'} = '';
		$resultFlg = 0;
		$restoreFlg = 0;
	}
	elsif( length($RES{'sn'}) != 14 || ($RES{'sn'} =~ m/[^a-zA-Z0-9]/ ))
	{
		# sn·å¿ô¥Á¥§¥Ã¥¯¼ºÇÔ¤Î¾ì¹ç
		$resultFlg = 0;
		$restoreFlg = 0;
	}
	#postalCode¤ÎÂ¸ºß¥Á¥§¥Ã¥¯
	if (! defined($RES{'postalCode'}))
	{
	    # postalCode¤¬¤Ê¤¤¾ì¹ç¡¢¥ê¥¹¥È¥¢¥¹¥­¥Ã¥×¥Õ¥¡¥¤¥ë¥Ï¥ó¥É¥ë¤òÍ¿¤¨¤ë
		$RES{'postalCode'} = '';
		$restoreFlg = 0;
	}
	#mailHost¤ÎÂ¸ºß¥Á¥§¥Ã¥¯
	if (! defined($RES{'mailHost'}))
	{
	    # mailHost¤¬¤Ê¤¤¾ì¹ç¡¢¥ê¥¹¥È¥¢¥¹¥­¥Ã¥×¥Õ¥¡¥¤¥ë¥Ï¥ó¥É¥ë¤òÍ¿¤¨¤ë
		$RES{'mailHost'} = '';
		$restoreFlg = 0;
	}
	# modifyTimestamp¤ÎÂ¸ºß¥Á¥§¥Ã¥¯
	if (! defined($RES{'modifyTimestamp'}))
	{
	    # modifyTimestamp¤¬¤Ê¤¤¾ì¹ç¡¢¥À¥Ã¥ß¤òÍ¿¤¨¤Þ¤¹¡£
		$RES{'modifyTimestamp'} = '20149999999999Z';
	}
		
	#·ë²Ì¥Õ¥¡¥¤¥ë¤Î°ì¹Ô¤òÁÈ¤ß¹ç¤ï¤»
	$result = $RES{'cn'}.','.$RES{'sn'}.','.$RES{'modifyTimestamp'}."\n";
	$restore = $RES{'cn'}.','.$RES{'sn'}.','. $RES{'postalCode'}
	            .','. $RES{'mailHost'}.','. $RES{'modifyTimestamp'}."\n";
   return ($resultFlg,$result,$restoreFlg,$restore);
}

