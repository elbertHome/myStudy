#!/usr/bin/perl
################################################################################
# ����            �ؿ�̾                              
#�ե�����̾ :   sldap_user_extract.pl         sldap�桼��������Хġ���
#������     :   ����μ
#������     :   2014/10/13
#�ѹ�����
################################################################################
use strict;
use warnings;
################################################################################
# �ؿ����
################################################################################
sub exUserExtractMain();
sub userExtractAvgCheck();
sub userExtractRstFileOpen($);
sub getLdapSearchCmd();
sub getBlockData($);
################################################################################

################################################################################
# ����������
my $g_LDAP_LOGIN_USER  = "\"uid=smadmin,ou=SysAccounts,dc=admin,dc=local\"";
my $g_LDAP_SRV_ADDRESS = "127.0.0.1:389";
################################################################################

#�¹ԥ᡼��ؿ���ƤӽФ�
&exUserExtractMain() or exit 1;

################################################################################
# �ؿ�̾                           �¹ԥ᡼��ؿ�
# �ؿ��ƽз���                     userExtractMain();
# �ؿ�����                         �桼��������н�����Ԥ�
#
#��ջ���                          �ʤ�
#
#�����ϥѥ�᡼��
#����            R/W
#-------------------------------------------------------------------------------
#�����
#
#��  ��
#
# �ѹ�����
#           1    2014/10/13    ������μ    ��������
################################################################################
sub exUserExtractMain() {
	#���¹Է��
	my $resultOK = 0;
	my $resultNG = 0;
	my $restoreOK = 0;
	my $restoreNG = 0;
	eval
	{	
	    #��ư�ѥ�᡼���θĿ�������å�
	    &userExtractAvgCheck();
		#�¹ԥ����ॢ���Ȥ�����
 		local $SIG{ALRM} = 
			sub { 
				die "sldap_user_extract_failed. ���顼����(ldapsearch timeout).\n";
				}; 
		alarm $ARGV[3];		

	    #��̥ե�����Υϥ�ɥ�ꥹ�Ȥ����
	    my @resFileHds = &userExtractRstFileOpen($ARGV[2]);
	    #ldapsearch���ޥ�ɤ����
	    my $cmd = &getLdapSearchCmd();
	
		my @cmdRtn;
		my @blockRtn;
		my $resultFileHd;
		my $restoreFileHd;
		# �ɤ߹���ʬ���������
		local $/ = "\n\n";
	    #ldapsearch���ޥ�ɤ�¹�
	    @cmdRtn = `$cmd`;
	    if ($?)
		{
			foreach(@cmdRtn)
			{
				print;
			}
			die "sldap_user_extract_failed. ���顼����(ldapsearch error).\n";
		}
		#���ޥ�ɷ�̤����
	    foreach (@cmdRtn)	
		{
			chomp;
			@blockRtn = &getBlockData($_);
			#Result�ե����������ͤ����
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
			#Restore�ե����������ͤ����
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
			#�ե�����˽񤭹���
			unless (print $resultFileHd ($blockRtn[1]))
			{
				die "sldap_user_extract_failed. ���顼����(result_file_write error).\n";
			}
			 unless (print $restoreFileHd ($blockRtn[3]))
			{
				die "sldap_user_extract_failed. ���顼����(restore_file_write error).\n";
			}
		}
		alarm 0;
	};

	if ($@)
	{
		#�¹Լ��Ԥξ��
		print $@;
		return 0;
	}
	else
	{
		#�¹������ξ��
		print "sldap_user_extract succeeded.\n";
		print "\tresult_file:  Success: " . $resultOK . ", Skip: " . $resultNG . "\n";
		print "\trestore_file: Success: " . $restoreOK . ", Skip: " . $restoreNG . "\n";
	}
	return 1;
}
################################################################################
# �ؿ�̾                           ��ư�ѥ�᡼�������å�
# �ؿ��ƽз���                     userExtractAvgCheck;
# �ؿ�����                         ��ư�ѥ�᡼���Υ����å�
#
#��ջ���                          �ʤ�
#
#�����ϥѥ�᡼��
#����            R/W
#-------------------------------------------------------------------------------
#�����
#
#��  ��
#
# �ѹ�����
#           1    2014/10/13    ������μ    ��������
################################################################################
sub userExtractAvgCheck() {
    my $usage = "usage : sldap_user_extract.sh ldap_passwd mailHost result_filepath timer(0-10000).\n";

    #�ѥ�᡼���Ŀ������å�
    if ( scalar(@ARGV) != 4 ) 
	{
        # ���顼��å�������Ф�
        die "��ư�ѥ�᡼���Ŀ��������Ǥ���\n$usage";
    }
	else
	{
		if ((($ARGV[3])=~m/[^0-9]/) || $ARGV[3] < 0 || $ARGV[3] > 10000)
		{
			die "ldapsearch�Υ����ॢ������[0-10000]�û��ꡣ\n$usage";
		}
	}
}

################################################################################
# �ؿ�̾                           �¹Է�̥ե�����򳫤�
# �ؿ��ƽз���                     userExtractRstFileOpen;
# �ؿ�����                         �¹Է�̥ե�����򳫤��ؿ�
#
#��ջ���                          �ʤ�
#
#�����ϥѥ�᡼��
#����            R/W
#-------------------------------------------------------------------------------
#�����                            �¹Է�̥ե�����Υϥ�ɥ�ꥹ�ȡ���������������������������
#
#��  �������������������������������¹Է�̥ե��������¸�ǥ��쥯�ȥ�
#
# �ѹ�����
#           1    2014/10/13    ������μ    ��������
################################################################################
sub userExtractRstFileOpen($) {

    # �ؿ������ʼ¹Է�̥ե��������¸�ǥ��쥯�ȥ�������
    my $rstPath = shift(@_);

	# �ǥ��쥯�ȥ꤬¸�ߤ��Ƥ��ʤ���硢��������
	unless (-d $rstPath)
	{	
		mkdir $rstPath , 0755 or return;
	}

    # ��̥ե�����ϥ�ɥ�����
    my $EXTRACT_RESULT_fh;
    my $EXTRACT_RESTORE_fh;
    my $EXTRACT_RESULT_SKIP_fh;
    my $EXTRACT_RESTORE_SKIP_fh;

      # �����ѥ桼���ꥹ�Ȥη�̥ե�����򳫤�
	unless ( open $EXTRACT_RESULT_fh, '>', $rstPath . "/sldap_user_extract_result.list" )
    {
       # ���顼��å�������Ф�
       die "sldap_user_extract failed. ���顼����(result_file open error). $!\n";
    }

    # �ꥹ�ȥ��ѥ桼���ꥹ�Ȥη�̥ե�����򳫤�
    unless ( open $EXTRACT_RESTORE_fh, '>', $rstPath . "/sldap_user_extract_restore.list" )
    {
       # ���顼��å�������Ф�
       die "sldap_user_extract failed. ���顼����(restore_file open error). $!\n";
    }

    # �����ѥ桼���Υ����åץꥹ�Ȥη�̥ե�����򳫤�
    unless ( open $EXTRACT_RESULT_SKIP_fh, '>', $rstPath . "/sldap_user_extract_skip.list" )
    {
       # ���顼��å�������Ф�
       die "sldap_user_extract failed. ���顼����(result_file open error). $!\n";
    }

    # �ꥹ�ȥ��ѥ桼���Υ����åץꥹ�Ȥη�̥ե�����򳫤�
    unless ( open $EXTRACT_RESTORE_SKIP_fh,'>', $rstPath . "/sldap_user_extract_restore_skip.list" )
    {
       # ���顼��å�������Ф�
       die "sldap_user_extract failed. ���顼����(restore_file open error) $!\n";
    }

    return (($EXTRACT_RESULT_fh,$EXTRACT_RESTORE_fh,$EXTRACT_RESULT_SKIP_fh, $EXTRACT_RESTORE_SKIP_fh));
}

################################################################################
# �ؿ�̾                           ldapsearch ���ޥ�ɥ饤������
# �ؿ��ƽз���                     getLdapSearchCmd;
# �ؿ�����                         ldapsearch ���ޥ�ɥ饤������
#
#��ջ���                          �ʤ�
#
#�����ϥѥ�᡼��
#����            R/W
#-------------------------------------------------------------------------------
#�����                            �ʤ�����������������������������
#
#��  �������������������������������ʤ�
#
# �ѹ�����
#           1    2014/10/13    ������μ    ��������
################################################################################
sub getLdapSearchCmd() {

    #��ư�ѥ�᡼�������
    my $ldpPWD     = shift(@ARGV);
    my $ldpMailHST = shift(@ARGV);
    my $rstPath    = shift(@ARGV);

    #���ޥ�ɥ饤����Ȥ߹�碌
    my $cmd .= "ldapsearch -h $g_LDAP_SRV_ADDRESS -x -D $g_LDAP_LOGIN_USER -w $ldpPWD -LLL -b \"ou=People,dc=ezweb,dc=ne,dc=jp\" ";
    unless ( $ldpMailHST =~ m/\ball\b/i ) 
	{
        $cmd .= "\"mailHost=$ldpMailHST\" ";
    }
    $cmd .= "sn cn postalCode mailHost modifyTimestamp 2>&1";
    return $cmd;
}

################################################################################
# �ؿ�̾                           �¹Է�̤����block�ǡ�����list�ΰ�Ԥ��Ѵ�
# �ؿ��ƽз���                     getBlockData;
# �ؿ�����                         �¹Է�̤����block�ǡ�����list�ΰ�Ԥ��Ѵ�
#
#��ջ���                          �ʤ�
#
#�����ϥѥ�᡼��
#����            R/W
#-------------------------------------------------------------------------------
#����͡������������������������� ��Result�ե���������ե饰
#							��������Result�ե���������ʸ����
#							��������Restore�ե���������ե饰
#							��������Restore�ե���������ʸ���󡡡�������������������������
#
#��  ��������������������������������ldapsearch���ޥ�ɤμ¹Է�̤�1 Block
#
# �ѹ�����
#           1    2014/10/13    ������μ    ��������
################################################################################
sub getBlockData($) 
{
	# ���֥�å��Υǡ�����split
	my @blockData = split "\n",$_[0];

	# ���ե饰 (default = 1)
	my $resultFlg = 1;
	my $restoreFlg = 1;
    # ���ʸ����
	my $result = '';
	my $restore = '';

	# ���Hash
	my %RES;

	# blockData��hash����¸
	foreach(@blockData)
	{
		chomp;
		if (m/: /)
		{
			#�����:�����ͤ򥭡��Ȥ��ơ���桧�����ͤ�Value�Ȥ��ơ�Hash����¸
			$RES{$`} = $';
		}
	}
	#��̥����å�
	#cn��¸�ߥ����å�
	if ( ! defined($RES{'cn'}))
	{
		# cn���ʤ���硢���åߤ�Ϳ���ޤ���
		$RES{'cn'} = '12345678901234_dummy';
	}
	#sn�η����ʸ�������ץ����å�
	if (! defined($RES{'sn'}))
	{
		# sn¸�ߤ��ʤ���硢����Ϳ����
		$RES{'sn'} = '';
		$resultFlg = 0;
		$restoreFlg = 0;
	}
	elsif( length($RES{'sn'}) != 14 || ($RES{'sn'} =~ m/[^a-zA-Z0-9]/ ))
	{
		# sn��������å����Ԥξ��
		$resultFlg = 0;
		$restoreFlg = 0;
	}
	#postalCode��¸�ߥ����å�
	if (! defined($RES{'postalCode'}))
	{
	    # postalCode���ʤ���硢�ꥹ�ȥ������åץե�����ϥ�ɥ��Ϳ����
		$RES{'postalCode'} = '';
		$restoreFlg = 0;
	}
	#mailHost��¸�ߥ����å�
	if (! defined($RES{'mailHost'}))
	{
	    # mailHost���ʤ���硢�ꥹ�ȥ������åץե�����ϥ�ɥ��Ϳ����
		$RES{'mailHost'} = '';
		$restoreFlg = 0;
	}
	# modifyTimestamp��¸�ߥ����å�
	if (! defined($RES{'modifyTimestamp'}))
	{
	    # modifyTimestamp���ʤ���硢���åߤ�Ϳ���ޤ���
		$RES{'modifyTimestamp'} = '20149999999999Z';
	}
		
	#��̥ե�����ΰ�Ԥ��Ȥ߹�碌
	$result = $RES{'cn'}.','.$RES{'sn'}.','.$RES{'modifyTimestamp'}."\n";
	$restore = $RES{'cn'}.','.$RES{'sn'}.','. $RES{'postalCode'}
	            .','. $RES{'mailHost'}.','. $RES{'modifyTimestamp'}."\n";
   return ($resultFlg,$result,$restoreFlg,$restore);
}

