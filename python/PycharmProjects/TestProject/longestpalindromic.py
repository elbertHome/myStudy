#!/usr/local/bin/python
# -*- coding: UTF-8 -*-


def longest_palindromic(text):

    newText = "#"
    for eachChar in text:
        newText += eachChar + "#"
    text = newText

    # check start position
    pos = len(text) // 2
    # check start len
    chkStartLen = pos
    # checked length of max palindromic substring
    maxLen = 0
    maxPos = 1
    # positvie and negitive array
    pn = [-1, 1]
    # index
    index = 0

    while pos >= 0:
        maxPos, maxLen = getPalindrmoicMaxLen(text, pos, chkStartLen, maxPos, maxLen)
        index += 1
        offset = int(pn[index % 2] * round(index / 2.0))
        pos = int(len(text) // 2 + offset)
        chkStartLen = int(len(text) // 2 - round(index / 2.0))
        if chkStartLen < maxLen:
            break

    newText = text[maxPos - maxLen:maxPos + maxLen + 1]
    text = ""
    for eachChar in newText:
        if eachChar == '#':
            continue
        else:
            text += eachChar
    return text


def getPalindrmoicMaxLen(text, pos, chkStartLen, maxPos, maxLen):

    while chkStartLen >= maxLen:
        if text[pos - chkStartLen:pos] == text[pos + chkStartLen:pos:-1]:
            maxPos = pos
            maxLen = chkStartLen
            break
        else:
            chkStartLen -= 1

    return maxPos, maxLen


def longest_palindromic2(text):

    palindrome=text[0]

    for i in range(len(text)):

        tmp = list(text[i:])

        while len(tmp)>1:

            if tmp == tmp[-1::-1] and len(tmp)>len(palindrome):

                palindrome =tmp

                break

            else:

                tmp.pop(-1)

    return ''.join(palindrome)


if __name__ == '__main__':
    import time
    strTest = """fjkhekljhkljnaqdbajncbvjkhqwyethjashfkljnkznvaflk;dsf;elktjh;lkajsf;lkj;askdjlf;erlkjt;ja;lkdjsfeiasfhjeklhrjklshafklhe4lkjroi4utoi3475890gvxxvg0985743iotu4iorjkl4ehlkfahslkjrhlkejhfsaiefsjdkl;aj;tjklre;fljdksa;jkl;fsjakl;hjtkle;fsd;klfavnzknjlkfhsanvmshtjikrle
    hqtasdfhzxchytalfhaklutpuqujopirktjg90w8439rjlkajnvahdsf
    erasdfkshgtrensANcvkjsahfhermasajcvhhg;fkljh';lskfklhasjkfghkjeyriuhaskjfhkawmlascdhk asjhfaswhfasfhnlkgatrlkfasdhfljkhsafglkerjht;lsajhvnavkaljsjhf
    askfhje;lkhfjk;ashdf;lkajf;lekjrf;khdsf;lkjeyruifyhasnzxnckjhaswfklehfrZAKSfhakljhfs;lwkajefr;lkjas;fkljaw;klerf;lakkjsfas;kfja;lkdjsflkera;jfdas;kjfask
    askfhje;ljrfansfdlkhaeifjhnsakn jfas;kjfl;kejhfasflkafhe4q5r72198ruwoqjfa';fksak;ugoiruytaufanvfnzbmxghvwlqaiher49thasf;afja;skjfd;lke4utroajfld;sajf;as
    askdjhf;lehr;lajl;kdsfjeouiyutp43qkalrsfjnlknsaklahyiaoht;3o4q4jtlkawsjf;lsajfl;aifyq8y7t61295j32l4;k509qwau90tq237ru5092578rawskjf;askfhjqwlhjerha;wlkrfj
    a;lkdf;elkjhr;lqwajerfa;ljsf;lkawjrf;ewlkrjfa;kljfa;lkjfal;kjf;le3jf;elwakj;askjfa;lkdjewipoqurfaqw;jrfi;oajsfdk;alfjsepoirup32q0jrkaswjf;lkjsaf;asdkjf;asjfd
    askdjf;lqwejk;lakdsjf;lakjsfd;lkajf;ldkjaf;lkdfjew[qporufj;alkds;lkajs;alkjs;lakjfe3opiurfawopurfq2w0p8urawsjf;lkjsaf;askjfa;ljsf;laksjfd;lekjf;alkjfas;kdjla;ldjfe;lkjfasf
    has;dkj;elkjrp0q2wrufjo4e3jtlkjmalfmfsa;kljfha;lkjsfejtl;aksjfdaskdfj;lewk4tu43u56asjfalksjf'a;fke';ljtlqkf;jajht   2o231r4ujfsj;sakjltrfe;4ljfafk;ajfasjf4[pitkgfafgajsja'jfweqlhrfasfh
    as;djfe;lkjf;akjsfeioprufjaskfjla;lkjfdas;kjfe;lkjfasfj;lewjkf;lkdjsqwejflsadfjn;lakjf;kdjslf;lekjf;asdkjfe;ujf;lajsfka';ljf;kldjsaf;lke3;lfjeswaf;kljasd;kfljasjdfk
    asd;kfle;lkjfa;kjsf;kjsfa;lkjr;lkdsfaklasjkask;flejwlrtu4i3utijkjklsafhjlkew;qlkjfasjfjhg';qwjfadsjfa;lkfa;aasjlkd;f';asfj';ladsj;lkajfelkjfas;kdfja;lkjfe4;oighjgha;lhdfjlkjhsafhlksafdh
    as;dfklask;djfla;lkds;lke4utr9483275trfaswi;ofhj;lkdsafjaksurf4e8utrahfjah4otrfa;lodjfadsfjal;kdje;lfask;dfjel;kjfasdk;je;lwkju;kfjsad;kflaskja;kljfasfje;lkjfsadjfk"faskjhfek
    fkjqwryiou4y5qhafnczhvbasbfkjlherwqalkfylvhsahflkjyu;lwj;glkjhas;flhlakjfhlkhjadfle4yh5touiyqpoyrfoasjhfd;lakjsdf;lkjerj;qlw3u4rpoqwaufpoasjhfdajfd;klj;le3jr;flajsfd;lkdjsfa;lkjdas;lkjfeoiu49875awfashjlyjk7;'8i-0wsfe9ahkjhfelkjhrelkjhfaskdjfhe3ur7y938ujfaskdjh;lkjaasdfhekjhriuyfasdfajhdfhajskdfjklehflkhaskdhlekjhrflkjehflkhjsafjklhelkhs;dkjfe;lkreerkl;efjkd;shklehlkjfasjhklfhejklfrhjkelhdksahklfhelkjfdksjahfdhjafdsafyuirhjkehfdsaajkl;hjdksafju839y7ru3ehfjdksafhjklerhjklefhjkha9efsw0-i8';7kjyljhsafwa57894uioefjkl;sadjkl;afsjdkl;dfsjalf;rj3el;jlk;dfjadfhjsaopfuawqopr4u3wlq;jrejkl;fdsjkal;dfhjsaofryopqyiuot5hy4elfdajhklhfjkalhlf;sahjklg;jwl;uyjklfhashvlyfklaqwrehljkfbsabvhzcnfahq5y4uoiyrwqjkf
    alkdjs;lekjur3oiuroawjfdlksajfdh;ahsdfjlkhe4riuyawuo9ifyasdhfklhjsadflkhjsadkfleyoiruyfloisahfdlahsfklk as fhakshflekyrhoiuwqyrqiohrfahfklash askjfhalkjhfe4ur5y832765awhflkdhjsafasf
    fsafasjhdklfhwa567238y5ru4efhjklahfjksa hsalkfhafrhoiqryqwuiohrykelfhskahf sa klkfshaldfhasiolfyurioyelfkdasjhklfdasjhlkfhdsayfi9ouwayuir4ehkljfdsha;hdfjaskldfjwaoruio3rujkel;sjdkla
    askdfhelkrjh;lakjsfdl;kajs;dkj;elkjrpoajfsdk;lj;alkdjsfal;kdj;lksajdflk;ejr;ejr;lkajsdf;lkej;klarfk;lerj;lkae;lkja;ldkjf;lekjf;lejf;askdjf;l"""


    t0 = time.time()
    print longest_palindromic2(strTest)
    print time.time() - t0
    t0 = time.time()
    print longest_palindromic(strTest)
    print time.time() - t0

    # assert longest_palindromic("abcdefghik") == "a", "The Longest"
    # assert longest_palindromic("abacada") == "aba", "The First"
    # assert longest_palindromic("aaaa") == "aaaa", "The A"