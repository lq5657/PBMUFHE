/*
 * =====================================================================================
 *
 *       Filename:  test_abefhe.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月26日 14时12分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <cstdlib>
#include <cstring>
#include <string>
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>
#include <glib.h>

#include "ABE_FHE/ABE_FHE.h"
#include "ABE_FHE/Ctxt.h"
#include "ABE_FHE/ABE_mkey.h"
#include "ABE_FHE/abe_core.h"
#include "serialize.h"
#include "common.h"
#include "policy_lang.h"

using namespace NTL;
using namespace std;

const char* type_params= 
"type a\n" \
"q 87807107996633125224377819847540498158068831994142082" \
"1102865339926647563088022295707862517942266222142315585" \
"8769582317459277713367317481324925129998224791\n" \
"h 12016012264891146079388821366740534204802954401251311" \
"822919615131047207289359704531102844802183906537786776\n" \
"r 730750818665451621361119245571504901405976559617\n" \
"exp2 159\n" \
"exp1 107\n" \
"sign1 1\n" \
"sign0 1\n";

void parse_strings(string strs[], int len, char** attrs)
{
    int i;
    GSList* alist;
    GSList* ap;
    int n;

    alist = 0;
    for( i = 0; i < len; i++)
    {
        char* str = const_cast<char*>(strs[i].c_str());
        parse_attribute(&alist, str);
    }
    if(!alist)
        die(const_cast<char*>(string("parse attribute error!").c_str()));
    alist = g_slist_sort(alist, comp_string);
    n = g_slist_length(alist);
    //attrs = (char**)malloc((n + 1) * sizeof(char*)); 

    i = 0;
    for( ap = alist; ap; ap = ap->next )
        attrs[i++] = (char*)ap->data;
    attrs[i] = 0;
}

char* policy = 0;
char** attrs = 0;
void parse_args( int argc, char** argv )
{
    int i;
    for( i = 1; i < argc; i++ )
    {
        policy = parse_policy_lang(argv[i]);
        /*  if( !strcmp(argv[i], "--mypolicy") )
        {
            cout << "请输入你的访问策略！" << endl;
        }
        else
        {
            if( !policy )
                policy = parse_policy_lang(argv[i]);
        }*/
    }
    if( !policy )
        policy = parse_policy_lang(suck_stdin());
}

int main( int argc, char** argv )
{
    int i;
    long n = 16;
    long m = n * std::floor(log2(to_long(1<<n)));
    cout << "请输入你的加密策略" << endl;
    parse_args(argc, argv);

    cout << "生成主密钥" << endl;
    ABE_mkey mKey(type_params);
    //生成ABE_FHE公钥
    cout << "生成公钥" << endl;
    ABE_FHEPubKey abefhePubKey(n, m, mKey);
    //加密
    cout << "加密" << endl;
   
    ZZ_p a_ZZ_p = conv<ZZ_p>(5);
    Ctxt ctxt = abefhePubKey.Encrypt(a_ZZ_p, policy);
    free(policy);
    cout << "加密成功！" << endl;
    //生成ABE_FHE私密钥
    cout << "请输入你的解密属性！" << endl;
    string str;
    //char mych[100] = {"\0"};
    GSList* alist;
    GSList* ap;
    int attrsLen;
    alist = 0;
    while(getline(cin, str,'\n'))
    {
        char* chs = const_cast<char*>(str.c_str());
       // cout << chs << " ";
        parse_attribute(&alist, chs);
        //memset(mych, '\0', sizeof(mych));
        str.clear();
    }
    cout << endl;
    alist = g_slist_sort(alist, comp_string);
    attrsLen = g_slist_length(alist);
    attrs = (char**)malloc((attrsLen + 1) * sizeof(char*));
    i = 0;
    for( ap = alist; ap; ap = ap->next )
    {
        cout << (char*)ap->data << " "; 
        attrs[i++] = (char*)ap->data;
    }
    cout << endl;
    attrs[i] = 0;
    
    abe_pub_t* pub;
    pub = mKey.abe_mpub;
    abe_msk_t* msk; 
    msk = mKey.abe_msec;
    ABE_FHESecKey prvKey(n, abefhePubKey.fhe_t, pub, msk);
    prvKey.genSeckey(attrs);
    cout << "生成密钥成功！" << endl;
    //解密
    ZZ_p plain_a_ZZ_p = prvKey.Decrypt(ctxt);
    cout << "解密后数据为：" << plain_a_ZZ_p << endl;

    for( i = 0; i < 6; i++)
    {
        free(attrs[i]);
    }
    free(attrs);

    return 0;
}
// Last Update:2015-04-28 12:19:00
/**
 * @file test_abefhe.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-26
 */

