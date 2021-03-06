#!/usr/bin/perl
################################################################################
# 種別            関数名                              
#ファイル名 :   sldap_user_extract.pl         sldapユーザ情報抽出ツール
#作成者     :   刑中亮
#作成日     :   2014/10/13
#変更履歴
################################################################################
use strict;
use warnings;
################################################################################
# 関数宣言
################################################################################
sub exUserExtractMain();
sub userExtractAvgCheck();
sub userExtractRstFileOpen($);
sub getLdapSearchCmd();
sub getBlockData($);
################################################################################

################################################################################
# 全局定数定義
my $g_LDAP_LOGIN_USER  = "\"uid=smadmin,ou=SysAccounts,dc=admin,dc=local\"";
my $g_LDAP_SRV_ADDRESS = "127.0.0.1:389";
################################################################################

#実行メーン関数を呼び出す
&exUserExtractMain() or exit 1;

################################################################################
# 関数名                           実行メーン関数
# 関数呼出形式                     userExtractMain();
# 関数概要                         ユーザ情報抽出処理を行う
#
#注意事項                          なし
#
#入出力パラメータ
#種別            R/W
#-------------------------------------------------------------------------------
#戻り値
#
#引  数
#
# 変更履歴
#           1    2014/10/13    刑　中亮    新規作成
################################################################################
sub exUserExtractMain() {
	#　実行件数
	my $resultOK = 0;
	my $resultNG = 0;
	my $restoreOK = 0;
	my $restoreNG = 0;
	eval
	{	
	    #起動パラメータの個数をチェック
	    &userExtractAvgCheck();
		#実行タイムアウトを設定
 		local $SIG{ALRM} = 
			sub { 
				die "sldap_user_extract_failed. エラー内容(ldapsearch timeout).\n";
				}; 
		alarm $ARGV[3];		

	    #結果ファイルのハンドルリストを取得
	    my @resFileHds = &userExtractRstFileOpen($ARGV[2]);
	    #ldapsearchコマンドを取得
	    my $cmd = &getLdapSearchCmd();
	
		my @cmdRtn;
		my @blockRtn;
		my $resultFileHd;
		my $restoreFileHd;
		# 読み込む分割符号を指定
		local $/ = "\n\n";
	    #ldapsearchコマンドを実行
	    @cmdRtn = `$cmd`;
	    if ($?)
		{
			foreach(@cmdRtn)
			{
				print;
			}
			die "sldap_user_extract_failed. エラー内容(ldapsearch error).\n";
		}
		#コマンド結果を取得
	    foreach (@cmdRtn)	
		{
			chomp;
			@blockRtn = &getBlockData($_);
			#Resultファイルの戻り値を取得
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
			#Restoreファイルの戻り値を取得
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
			#ファイルに書き込む
			unless (print $resultFileHd ($blockRtn[1]))
			{
				die "sldap_user_extract_failed. エラー内容(result_file_write error).\n";
			}
			 unless (print $restoreFileHd ($blockRtn[3]))
			{
				die "sldap_user_extract_failed. エラー内容(restore_file_write error).\n";
			}
		}
		alarm 0;
	};

	if ($@)
	{
		#実行失敗の場合
		print $@;
		return 0;
	}
	else
	{
		#実行成功の場合
		print "sldap_user_extract succeeded.\n";
		print "\tresult_file:  Success: " . $resultOK . ", Skip: " . $resultNG . "\n";
		print "\trestore_file: Success: " . $restoreOK . ", Skip: " . $restoreNG . "\n";
	}
	return 1;
}
################################################################################
# 関数名                           起動パラメータチェック
# 関数呼出形式                     userExtractAvgCheck;
# 関数概要                         起動パラメータのチェック
#
#注意事項                          なし
#
#入出力パラメータ
#種別            R/W
#-------------------------------------------------------------------------------
#戻り値
#
#引  数
#
# 変更履歴
#           1    2014/10/13    刑　中亮    新規作成
################################################################################
sub userExtractAvgCheck() {
    my $usage = "usage : sldap_user_extract.sh ldap_passwd mailHost result_filepath timer(0-10000).\n";

    #パラメータ個数チェック
    if ( scalar(@ARGV) != 4 ) 
	{
        # エラーメッセージを出す
        die "起動パラメータ個数が不正です。\n$usage";
    }
	else
	{
		if ((($ARGV[3])=~m/[^0-9]/) || $ARGV[3] < 0 || $ARGV[3] > 10000)
		{
			die "ldapsearchのタイムアウト値[0-10000]秒指定。\n$usage";
		}
	}
}

################################################################################
# 関数名                           実行結果ファイルを開く
# 関数呼出形式                     userExtractRstFileOpen;
# 関数概要                         実行結果ファイルを開く関数
#
#注意事項                          なし
#
#入出力パラメータ
#種別            R/W
#-------------------------------------------------------------------------------
#戻り値                            実行結果ファイルのハンドルリスト　　　　　　　　　　　　　　
#
#引  数　　　　　　　　　　　　　　実行結果ファイルの保存ディレクトリ
#
# 変更履歴
#           1    2014/10/13    刑　中亮    新規作成
################################################################################
sub userExtractRstFileOpen($) {

    # 関数引数（実行結果ファイルの保存ディレクトリを取得）
    my $rstPath = shift(@_);

	# ディレクトリが存在していない場合、作成する
	unless (-d $rstPath)
	{	
		mkdir $rstPath , 0755 or return;
	}

    # 結果ファイルハンドルを定義
    my $EXTRACT_RESULT_fh;
    my $EXTRACT_RESTORE_fh;
    my $EXTRACT_RESULT_SKIP_fh;
    my $EXTRACT_RESTORE_SKIP_fh;

      # 切替用ユーザリストの結果ファイルを開く
	unless ( open $EXTRACT_RESULT_fh, '>', $rstPath . "/sldap_user_extract_result.list" )
    {
       # エラーメッセージを出す
       die "sldap_user_extract failed. エラー内容(result_file open error). $!\n";
    }

    # リストア用ユーザリストの結果ファイルを開く
    unless ( open $EXTRACT_RESTORE_fh, '>', $rstPath . "/sldap_user_extract_restore.list" )
    {
       # エラーメッセージを出す
       die "sldap_user_extract failed. エラー内容(restore_file open error). $!\n";
    }

    # 切替用ユーザのスキップリストの結果ファイルを開く
    unless ( open $EXTRACT_RESULT_SKIP_fh, '>', $rstPath . "/sldap_user_extract_skip.list" )
    {
       # エラーメッセージを出す
       die "sldap_user_extract failed. エラー内容(result_file open error). $!\n";
    }

    # リストア用ユーザのスキップリストの結果ファイルを開く
    unless ( open $EXTRACT_RESTORE_SKIP_fh,'>', $rstPath . "/sldap_user_extract_restore_skip.list" )
    {
       # エラーメッセージを出す
       die "sldap_user_extract failed. エラー内容(restore_file open error) $!\n";
    }

    return (($EXTRACT_RESULT_fh,$EXTRACT_RESTORE_fh,$EXTRACT_RESULT_SKIP_fh, $EXTRACT_RESTORE_SKIP_fh));
}

################################################################################
# 関数名                           ldapsearch コマンドラインを作成
# 関数呼出形式                     getLdapSearchCmd;
# 関数概要                         ldapsearch コマンドラインを作成
#
#注意事項                          なし
#
#入出力パラメータ
#種別            R/W
#-------------------------------------------------------------------------------
#戻り値                            なし　　　　　　　　　　　　　　
#
#引  数　　　　　　　　　　　　　　なし
#
# 変更履歴
#           1    2014/10/13    刑　中亮    新規作成
################################################################################
sub getLdapSearchCmd() {

    #起動パラメータを取得
    my $ldpPWD     = shift(@ARGV);
    my $ldpMailHST = shift(@ARGV);
    my $rstPath    = shift(@ARGV);

    #コマンドラインを組み合わせ
    my $cmd .= "ldapsearch -h $g_LDAP_SRV_ADDRESS -x -D $g_LDAP_LOGIN_USER -w $ldpPWD -LLL -b \"ou=People,dc=ezweb,dc=ne,dc=jp\" ";
    unless ( $ldpMailHST =~ m/\ball\b/i ) 
	{
        $cmd .= "\"mailHost=$ldpMailHST\" ";
    }
    $cmd .= "sn cn postalCode mailHost modifyTimestamp 2>&1";
    return $cmd;
}

################################################################################
# 関数名                           実行結果から一blockデータをlistの一行に変換
# 関数呼出形式                     getBlockData;
# 関数概要                         実行結果から一blockデータをlistの一行に変換
#
#注意事項                          なし
#
#入出力パラメータ
#種別            R/W
#-------------------------------------------------------------------------------
#戻り値　　　　　　　　　　　　　 ��Resultファイルの戻るフラグ
#							　　　��Resultファイルの戻る文字列
#							　　　��Restoreファイルの戻るフラグ
#							　　　��Restoreファイルの戻る文字列　　　　　　　　　　　　　　
#
#引  数　　　　　　　　　　　　　　��ldapsearchコマンドの実行結果の1 Block
#
# 変更履歴
#           1    2014/10/13    刑　中亮    新規作成
################################################################################
sub getBlockData($) 
{
	# １ブロックのデータをsplit
	my @blockData = split "\n",$_[0];

	# 戻るフラグ (default = 1)
	my $resultFlg = 1;
	my $restoreFlg = 1;
    # 戻る文字列
	my $result = '';
	my $restore = '';

	# 結果Hash
	my %RES;

	# blockDataをhashに保存
	foreach(@blockData)
	{
		chomp;
		if (m/: /)
		{
			#　符号:前の値をキーとして、符号：後ろの値をValueとして、Hashに保存
			$RES{$`} = $';
		}
	}
	#結果チェック
	#cnの存在チェック
	if ( ! defined($RES{'cn'}))
	{
		# cnがない場合、ダッミを与えます。
		$RES{'cn'} = '12345678901234_dummy';
	}
	#snの桁数と文字タイプチェック
	if (! defined($RES{'sn'}))
	{
		# sn存在しない場合、空を与える
		$RES{'sn'} = '';
		$resultFlg = 0;
		$restoreFlg = 0;
	}
	elsif( length($RES{'sn'}) != 14 || ($RES{'sn'} =~ m/[^a-zA-Z0-9]/ ))
	{
		# sn桁数チェック失敗の場合
		$resultFlg = 0;
		$restoreFlg = 0;
	}
	#postalCodeの存在チェック
	if (! defined($RES{'postalCode'}))
	{
	    # postalCodeがない場合、リストアスキップファイルハンドルを与える
		$RES{'postalCode'} = '';
		$restoreFlg = 0;
	}
	#mailHostの存在チェック
	if (! defined($RES{'mailHost'}))
	{
	    # mailHostがない場合、リストアスキップファイルハンドルを与える
		$RES{'mailHost'} = '';
		$restoreFlg = 0;
	}
	# modifyTimestampの存在チェック
	if (! defined($RES{'modifyTimestamp'}))
	{
	    # modifyTimestampがない場合、ダッミを与えます。
		$RES{'modifyTimestamp'} = '20149999999999Z';
	}
		
	#結果ファイルの一行を組み合わせ
	$result = $RES{'cn'}.','.$RES{'sn'}.','.$RES{'modifyTimestamp'}."\n";
	$restore = $RES{'cn'}.','.$RES{'sn'}.','. $RES{'postalCode'}
	            .','. $RES{'mailHost'}.','. $RES{'modifyTimestamp'}."\n";
   return ($resultFlg,$result,$restoreFlg,$restore);
}

