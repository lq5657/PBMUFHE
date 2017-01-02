/*
 * =====================================================================================
 *
 *       Filename:  testPrvGen.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月23日 22时20分23秒
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
#include "common.h"
#include "policy_lang.h"

using namespace NTL;
using namespace std;
// Last Update:2015-04-28 16:24:02
/**
 * @file testPrvGen.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-23
 */

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

char** attrs = 0;

gint comp_string( gconstpointer a, gconstpointer b)
{
    return strcmp(a, b);
}

void parse_args( int argc, char** argv)
{
    int i;
    GSlist* alist;
    GSlist* ap;
    int n;

    alist = 0;
    for( i = 1; i < argc; i++)
    {
        parse_attribute(&alist, argv[i]);
    }
    if(!alist)
        die("parse attribute error!");
    alist = g_slist_sort(alist, comp_string);
    n = g_slist_length(alist);
    attrs = (char**)malloc((n + 1) * sizeof(char*)); 

    i = 0;
    for( ap = alist; ap; ap = ap->next )
        attrs[i++] = ap->data;
    attrs[i] = 0;
}

int main( int argc, char** argv)
{
    //提取出属性
    parse_args(argc, argv);

    long n = 16;
    long m = n * std::floor(log2(to_long(1<<n)));
    ABE_FHESecKey abefheSec(n, type_params);
    
    //初始化属性公钥和属性主密钥
    string pub_file = "pub_key";
    string msk_file = "master_key";
    spit_file(const_cast<char*>(pub_file.c_str()), abe_pub_serialize(abefheSec.mKey.abe_mpub), 1);
	spit_file(const_cast<char*>(msk_file.c_str()), abe_msk_serialize(abefheSec.mKey.abe_msec), 1);
    
    //根据属性生成密钥
    abefheSec.genSeckey(attrs);


    return 0;
}
