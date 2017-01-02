/*
 * =====================================================================================
 *
 *       Filename:  dec.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月28日 19时52分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <cstdlib>
#include <iostream>
#include <NTL/ZZ_p.h>
#include <NTL/matrix.h>
#include <glib.h>

#include "ABE_FHE/ABE_FHE.h"
#include "ABE_FHE/timing.h"
#include "ABE_FHE/Ctxt.h"
#include "ABE_FHE/abe_core.h"
#include "serialize.h"
#include "common.h"
#include "policy_lang.h"

using namespace std;
using namespace NTL;

char* mpub_file = 0;
char* r_file = 0;
char* prv_file = 0;
char* in_file = 0;

void parse_args(int argc, char** argv)
{
    int i;
    for( i = 1; i < argc; i++ )
    {
        if( !mpub_file )
        {
            mpub_file = argv[i];
        }
        else if( !r_file )
        {
            r_file = argv[i];
        }
        else if( !prv_file )
        {
            prv_file = argv[i];
        }
        else if( !in_file )
        {
            in_file = argv[i];
        }
    }
}

int main(int argc, char** argv)
{
    long n = 18;

    ZZ_p::init(conv<ZZ>(1 << n));
    parse_args(argc, argv);
    GByteArray* cph_policy_buf;
    GByteArray* prv_abe_buf;
    GByteArray* cph_mat_buf;
    GByteArray* prv_v_buf;
    GByteArray* m_r_buf;
    read_cph_file(in_file, &cph_policy_buf, &cph_mat_buf);
    read_prv_file(prv_file, &prv_abe_buf, &prv_v_buf);
    
    m_r_buf = suck_file(r_file);
    Mat<ZZ_p> r_mat = unserialize_mat_ZZ_p(m_r_buf);

    abe_pub_t* mpub = abe_pub_unserialize(suck_file(mpub_file), 1);
    abe_prv_t* prv_abe = abe_prv_unserialize(mpub, prv_abe_buf, 1);
    abe_cph_t* cph_abe = abe_cph_unserialize(mpub, cph_policy_buf, 1);
    Mat<ZZ_p> cph_mat = unserialize_mat_ZZ_p(cph_mat_buf);
    Vec<ZZ_p> prv_v = unserialize_vec_ZZ_p(prv_v_buf);

    setTimersOn();
    ZZ_p data = ABE_FHESecKey::decrypt(cph_abe, cph_mat, mpub, prv_abe, prv_v, r_mat);
    cout << "解密数据："<< conv<long>(data) << endl;
    printAllTimers();
    setTimersOff();

    g_byte_array_free(m_r_buf, 1);
    g_byte_array_free(cph_mat_buf, 1);
    g_byte_array_free(prv_v_buf, 1);
    
    abe_prv_free(prv_abe);
    abe_cph_free(cph_abe);
    abe_pub_free(mpub);

    return 0;
}
// Last Update:2015-05-22 10:20:00
/**
 * @file dec.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-28
 */

