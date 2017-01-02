/*
 * =====================================================================================
 *
 *       Filename:  prvGen.cpp
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>
#include <glib.h>

#include "ABE_FHE/ABE_FHE.h"
#include "ABE_FHE/abe_core.h"
#include "ABE_FHE/timing.h"
#include "serialize.h"
#include "common.h"
#include "policy_lang.h"

using namespace NTL;
using namespace std;
// Last Update:2015-05-22 10:17:52
/**
 * @file prvGen.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-23
 */

char* pub_file = 0;
char* msk_file = 0;
char* t_file = 0;
char* r_file = 0;
char** attrs = 0;
string out_file = "priv_key";
string eval_file = "eval_key";

gint comp_string( gconstpointer a, gconstpointer b)
{
    return strcmp((char*)a, (char*)b);
}

void parse_args( int argc, char** argv)
{
    int i;
    GSList* alist;
    GSList* ap;
    int n;

    alist = 0;
    for( i = 1; i < argc; i++)
    {
        if( !strcmp(argv[i], "-o") || !strcmp(argv[i], "--output") )
        {
            if( ++i >= argc )
                die(const_cast<char*>(string("error out_file").c_str()));
            else
            {
                out_file = argv[i];
                eval_file = argv[++i];
            }
        }
        else if( !pub_file )
        {
            pub_file = argv[i];
        }
        else if( !msk_file )
        {
            msk_file = argv[i];
        }
        else if( !t_file)
        {
            t_file = argv[i];
        }
        else if( !r_file)
        {
            r_file = argv[i];
        }
        else
            parse_attribute(&alist, argv[i]);
    }
    if(!alist)
        die(const_cast<char*>(string("parse attribute error!").c_str()));
    alist = g_slist_sort(alist, comp_string);
    n = g_slist_length(alist);
    attrs = (char**)malloc((n + 1) * sizeof(char*)); 

    i = 0;
    for( ap = alist; ap; ap = ap->next )
        attrs[i++] = (char*)ap->data;
    attrs[i] = 0;
}

int main( int argc, char** argv)
{
    //提取出属性
    parse_args(argc, argv);

    long n = 18;
    ZZ_p::init( conv<ZZ>(1 << n) );
    //long m = n * std::floor(log1(to_long(1<<n)));
    
    abe_pub_t* pub;
    abe_msk_t* msk;
    Vec<ZZ_p> t;
    Mat<ZZ_p> r;
    pub = abe_pub_unserialize(suck_file(pub_file), 1);
    msk = abe_msk_unserialize(pub, suck_file(msk_file), 1);
    GByteArray* b_t = suck_file(t_file);
    t = unserialize_vec_ZZ_p(b_t);
    GByteArray* b_r = suck_file(r_file);
    r = unserialize_mat_ZZ_p(b_r);
    
    //GByteArray* b_r = suck_file(r_file);
    setTimersOn();
    
    ABE_FHESecKey prvKey(n, pub, msk, t, r);  //ABE_FHESecKey的构造函数多了参数fhe_t
    prvKey.genSeckey(attrs);

    printAllTimers();
    setTimersOff();
    GByteArray* b_abePrv = abe_prv_serialize( prvKey.abe_prv );
    //GByteArray* b_fheSk;
    //GByteArray* b_fhet;
    GByteArray* b_fhev;
    //b_fheSk = g_byte_array_new();
    //b_fhet = g_byte_array_new();
    b_fhev = g_byte_array_new();
    //vec_ZZ_p_serialize(b_fheSk, prvKey.fhe_sk);
    //vec_ZZ_p_serialize(b_fhet, prvKey.fhe_t);
    vec_ZZ_p_serialize(b_fhev, prvKey.fhe_v);

    long bytesLen = b_abePrv->len + b_fhev->len; /*+ b_fheSk->len + b_fhet->len + b_fhev->len;*/
    long abeLen = b_abePrv->len;
    FILE* f;
    int i;
    f = fopen_write_or_die(const_cast<char*>(out_file.c_str()));
    for( i = 3; i >= 0; i--)
        fputc((bytesLen & 0xff<<(i*8))>>(i*8), f);
    for( i = 3; i >= 0; i--)
        fputc((abeLen & 0xff<<(i*8))>>(i*8), f);
    fwrite(b_abePrv->data, 1, b_abePrv->len, f);
    //fwrite(b_fheSk->data, 1, b_fheSk->len, f);
    //fwrite(b_fhet->data, 1, b_fhet->len, f);
    fwrite(b_fhev->data, 1, b_fhev->len, f);
    fclose(f);

    long evalLen = abeLen + b_r->len;
    FILE* eval_f;
    eval_f = fopen_write_or_die(const_cast<char*>(eval_file.c_str()));
    for( i = 3; i >= 0; i--)
        fputc((evalLen & 0xff<<(i*8))>>(i*8), eval_f);
    for( i = 3; i >= 0; i--)
        fputc((abeLen & 0xff<<(i*8))>>(i*8), eval_f);
    fwrite(b_abePrv->data, 1, abeLen, eval_f);
    fwrite(b_r->data, 1, b_r->len, eval_f);

    g_byte_array_free(b_abePrv, 1);
    //g_byte_array_free(b_fheSk, 1);
    //g_byte_array_free(b_fhet, 1);
    g_byte_array_free(b_fhev, 1);
    g_byte_array_free(b_t, 1);
    g_byte_array_free(b_r, 1);

    abe_msk_free(msk);
    abe_pub_free(pub);
    /*ABE_FHESecKey abefheSec(n, type_params);
    //初始化属性公钥和属性主密钥
    string pub_file = "pub_key";
    string msk_file = "master_key";
    spit_file(const_cast<char*>(pub_file.c_str()), abe_pub_serialize(abefheSec.mKey.abe_mpub), 1);
	spit_file(const_cast<char*>(msk_file.c_str()), abe_msk_serialize(abefheSec.mKey.abe_msec), 1);
    //根据属性生成密钥
    abefheSec.genSeckey(attrs);*/
    return 0;
}

