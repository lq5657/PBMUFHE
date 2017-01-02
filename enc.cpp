/*
 * =====================================================================================
 *
 *       Filename:  enc.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月28日 17时13分28秒
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
#include "ABE_FHE/timing.h"
#include "serialize.h"
#include "common.h"
#include "policy_lang.h"

using namespace NTL;
using namespace std;

char* pub_file = 0;
char* t_file = 0;
char* policy = 0;
char* encryNumber = 0;
char* out_file = 0;
char* r_file = 0;
char* pubKey_file = 0;

void parse_args( int argc, char** argv)
{
    int i;
    for( i = 1; i < argc; ++i)
    {
        if( !pub_file ) 
        {
            pub_file = argv[i];
        }
        else if( !r_file )
        {
            r_file = argv[i];
        }
        else if( !t_file )
        {
            t_file = argv[i];
        }
        else if( !encryNumber )
        {
            encryNumber = argv[i];
        }
        else
        {
            policy = parse_policy_lang(argv[i]);
        }
    }
    if( !policy )
       policy = parse_policy_lang(suck_stdin());
}

int main(int argc, char** argv)
{
    setTimersOn();

    parse_args(argc, argv);

    int i;
    long n = 18;
    long m = n * std::floor(log2(to_long(1<<n)));
    ZZ_p::init(conv<ZZ>(1 << n));

    abe_pub_t* pub;
    Vec<ZZ_p> t;
    Mat<ZZ_p> mr;
    GByteArray* b_mpub = suck_file(pub_file);
    GByteArray* b_t = suck_file(t_file);
    GByteArray* b_r = suck_file(r_file);
    pub = abe_pub_unserialize(b_mpub, 0);
    t = unserialize_vec_ZZ_p(b_t);
    mr = unserialize_mat_ZZ_p(b_r);
    ABE_FHEPubKey abefhePubKey(n, m, pub, t, mr);
    GByteArray* b_fhePKey_A;
    b_fhePKey_A = g_byte_array_new();
    mat_ZZ_p_serialize(b_fhePKey_A, abefhePubKey.A);
    long pubkey_len = b_mpub->len + b_fhePKey_A->len + b_r->len;

    int data;
    sscanf(encryNumber, "%d", &data);
    ZZ_p a_ZZ_p = conv<ZZ_p>(data);
    Ctxt ctxt = abefhePubKey.Encrypt(a_ZZ_p, policy);

    printAllTimers();
    setTimersOff();

    FILE* pubKey_f;
    pubKey_file = g_strdup_printf("%d.pubKey",data);
    pubKey_f = fopen_write_or_die(pubKey_file);
    for( i = 3; i >= 0; i-- )
    {
        fputc((pubkey_len & 0xff<<(i*8))>>(i*8), pubKey_f);
    }
    for( i = 3; i >= 0; i-- )
    {
       fputc((b_mpub->len & 0xff<<(i*8))>>(i*8), pubKey_f); 
    }
    fwrite(b_mpub->data, 1, b_mpub->len, pubKey_f);
    fwrite(b_fhePKey_A->data, 1, b_fhePKey_A->len, pubKey_f);
    fwrite(b_r->data, 1, b_r->len, pubKey_f);
    fclose(pubKey_f);
    
    GByteArray* b_cph = abe_cph_serialize(ctxt.pCp);
    GByteArray* b_A;
    b_A = g_byte_array_new();
    mat_ZZ_p_serialize( b_A, ctxt.A);
    FILE* f;
    out_file = g_strdup_printf("%d.abefhe", data);
    long len = b_cph->len + b_A->len;
    long cph_len = b_cph->len;
    f = fopen_write_or_die(out_file);
    for( i = 3; i >= 0; i-- )
    {
        fputc((len & 0xff<<(i*8))>>(i*8), f);
    }
    for( i = 3; i >= 0; i-- )
    {
       fputc((cph_len & 0xff<<(i*8))>>(i*8), f); 
    }
    fwrite(b_cph->data, 1, b_cph->len, f);
    fwrite(b_A->data, 1, b_A->len, f);
    fclose(f);

    free(policy);
    abe_cph_free(ctxt.pCp);
    abe_pub_free(pub);
    //g_byte_array_free(b_R, 1);
    g_byte_array_free(b_cph, 1);
    g_byte_array_free(b_A, 1);
    g_byte_array_free(b_mpub, 1);
    g_byte_array_free(b_t, 1);
    g_byte_array_free(b_r, 1);
    g_byte_array_free(b_fhePKey_A, 1);
 
    return 0;
}
// Last Update:2015-05-22 12:06:13
/**
 * @file enc.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-28
 */

