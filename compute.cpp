/*
 * =====================================================================================
 *
 *       Filename:  compute.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月30日 10时40分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <cstdlib>
#include <cstdlib>
#include <iostream>
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>
#include <glib.h>

#include "ABE_FHE/ABE_FHE.h"
#include "ABE_FHE/Ctxt.h"
#include "ABE_FHE/abe_core.h"
#include "ABE_FHE/matrix_fhe.h"
#include "ABE_FHE/timing.h"
#include "serialize.h"
#include "common.h"
#include "policy_lang.h"

using namespace std;
using namespace NTL;

char* mpub_file = 0;
//char* decKey_file = 0;
char* in_file1 = 0;
char* in_file2 = 0;
char* computeKey1_file = 0;
char* computeKey2_file = 0;
char* policy = 0;
char* out_add_file = 0;
char* out_mul_file = 0;
char usage[] = 
"Usage: abefhe-compute \n"
"input attr_mpub,computing files and compute_key \n"
"";
void parse_args(int argc, char** argv)
{
    int i;
    for(i = 1; i < argc; i++)
    
    {
        if(!mpub_file)
        {
            mpub_file = argv[i];
        }
        /*else if(!decKey_file)
        {
            decKey_file = argv[i];
        }*/
        else if(!in_file1)
        {
            in_file1 = argv[i];
        }
        else if(!in_file2)
        {
            in_file2 = argv[i];
        }
        else if(!computeKey1_file)
        {
            computeKey1_file = argv[i];
        }
        else if(!computeKey2_file)
        {
            computeKey2_file = argv[i];
        }
        else
        {
             policy = parse_policy_lang(argv[i]);
        }
    }
    if( !policy )
        policy = parse_policy_lang(suck_stdin());
    //if(!mpub_file || /*!decKey_file ||*/ !in_file1 || !in_file2 || !computeKey1_file || !computeKey2_file)
    //{
       //die(usage); 
    //}
}

int main(int argc, char** argv)
{
    parse_args(argc, argv);
    long n = 18;
    ZZ_p::init(conv<ZZ>(1 << n));
    GByteArray* cph_policy_buf1;
    GByteArray* cph_mat_buf1;
    GByteArray* cph_policy_buf2;
    GByteArray* cph_mat_buf2; 
    GByteArray* computeKey1_abe_buf;
    GByteArray* computeKey1_r_buf;
    GByteArray* computeKey2_abe_buf;
    GByteArray* computeKey2_r_buf;

    read_cph_file(in_file1, &cph_policy_buf1, &cph_mat_buf1);
    read_cph_file(in_file2, &cph_policy_buf2, &cph_mat_buf2);
    read_prv_file(computeKey1_file, &computeKey1_abe_buf, &computeKey1_r_buf);
    read_prv_file(computeKey2_file, &computeKey2_abe_buf, &computeKey2_r_buf);
    abe_pub_t* mpub = abe_pub_unserialize(suck_file(mpub_file), 1);  //属性主公钥    
    //第一个密文
    abe_cph_t* cph1_abe = abe_cph_unserialize(mpub, cph_policy_buf1, 1);
    Mat<ZZ_p> cph1_mat = unserialize_mat_ZZ_p(cph_mat_buf1);
    //第二个密文
    abe_cph_t* cph2_abe = abe_cph_unserialize(mpub, cph_policy_buf2, 1);
    Mat<ZZ_p> cph2_mat = unserialize_mat_ZZ_p(cph_mat_buf2);
    
    abe_prv_t* computeKey1_abe = abe_prv_unserialize(mpub, computeKey1_abe_buf, 1);
    abe_prv_t* computeKey2_abe = abe_prv_unserialize(mpub, computeKey2_abe_buf, 1);
    Mat<ZZ_p> computeKey_r = unserialize_mat_ZZ_p(computeKey1_r_buf);
    
    unsigned long r1,r2,r;
    bool isSatified1 = ABE_FHESecKey::dec_policy(r1, cph1_abe, mpub, computeKey1_abe);
    bool isSatified2 = ABE_FHESecKey::dec_policy(r2, cph2_abe, mpub, computeKey2_abe);
    if( isSatified1 && isSatified2 )
    {
        Mat<ZZ_p> mat_R1 = Flatten(r1 * computeKey_r);
        Mat<ZZ_p> mat_R2 = Flatten(r2 * computeKey_r);
        Ctxt c1(cph1_abe, cph1_mat - mat_R1), c2(cph2_abe, cph2_mat - mat_R2);
        abe_cph_t* result_abe_cph = ABE_FHEPubKey::sta_policy_enc( policy, mpub, r);
        setTimersOn(); 
        c1.addCtxt(c2);
        Mat<ZZ_p> mat_add = c1.A + Flatten(r * computeKey_r);

        c1.multiplyBy(c2);
        Mat<ZZ_p> mat_mul = c1.A + Flatten(r * computeKey_r);
        printAllTimers();
        setTimersOff();

        GByteArray* b_cph = abe_cph_serialize(result_abe_cph);
        GByteArray* b_add;
        GByteArray* b_mul;
        b_add = g_byte_array_new();
        b_mul = g_byte_array_new();
        mat_ZZ_p_serialize( b_add, mat_add );
        mat_ZZ_p_serialize( b_mul, mat_mul );

        FILE* f_add;
        FILE* f_mul;
        out_add_file = g_strdup_printf("add.abefhe");
        out_mul_file = g_strdup_printf("mul.abefhe");

        long cph_len = b_cph->len;
        long add_len = cph_len + b_add->len;
        long mul_len = cph_len + b_mul->len;

        f_add = fopen_write_or_die(out_add_file);
        f_mul = fopen_write_or_die(out_mul_file);
        int i;
        for( i = 3; i >= 0; i-- )
        {
            fputc((add_len & 0xff<<(i*8))>>(i*8), f_add);
        }
        for( i = 3; i >= 0; i-- )
        {
            fputc((cph_len & 0xff<<(i*8))>>(i*8), f_add); 
        }
        fwrite(b_cph->data, 1, cph_len, f_add);
        fwrite(b_add->data, 1, b_add->len, f_add);
        fclose(f_add);

        for( i = 3; i >= 0; i-- )
        {
            fputc((mul_len & 0xff<<(i*8))>>(i*8), f_mul);
        }
        for( i = 3; i >= 0; i-- )
        {
            fputc((cph_len & 0xff<<(i*8))>>(i*8), f_mul); 
        }
        fwrite(b_cph->data, 1, cph_len, f_mul);
        fwrite(b_mul->data, 1, b_mul->len, f_mul);
        fclose(f_mul);

        g_byte_array_free(b_cph, 1);
        g_byte_array_free(b_add, 1);
        g_byte_array_free(b_mul, 1);
    }
    else
    {
        printf("Your computeKey is not authoried to compute cypher %s and %s\n",in_file1,in_file2);
    }
 
    g_byte_array_free(cph_mat_buf1, 1);
    g_byte_array_free(cph_mat_buf2, 1);
    g_byte_array_free(computeKey1_r_buf, 1);
    g_byte_array_free(computeKey2_r_buf, 1);

    abe_cph_free(cph1_abe);
    abe_cph_free(cph2_abe);
    abe_prv_free(computeKey1_abe);
    abe_prv_free(computeKey2_abe);
    abe_pub_free(mpub);
    return 0;
}
// Last Update:2015-05-22 10:30:27
/**
 * @file compute.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-30
 */

