/*
 * =====================================================================================
 *
 *       Filename:  setup.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月23日 12时02分41秒
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
#include <unistd.h>
//#include <glib.h>
#include <pbc/pbc.h>
#include <pbc/pbc_random.h>

#include <string>
#include "ABE_FHE/ABE_mkey.h"
//#include "ABE_FHE/abe_core.h"
#include "common.h"
#include "serialize.h"

using namespace std;

char usage[] =
"Usage: cpabe-setup [OPTION ...]\n"
"\n"
"Generate system parameters, a public key, and a master secret key\n"
"for use with cpabe-keygen, cpabe-enc, and cpabe-dec.\n"
"\n"
"Output will be written to the files \"pub_key\" and \"master_key\"\n"
"unless the --output-public-key or --output-master-key options are\n"
"used.\n"
"\n"
"Mandatory arguments to long options are mandatory for short options too.\n\n"
" -h, --help                    print this message\n\n"
" -v, --version                 print version information\n\n"
" -p, --output-public-key FILE  write public key to FILE\n\n"
" -m, --output-master-key FILE  write master secret key to FILE\n\n"
" -d, --deterministic           use deterministic \"random\" numbers\n"
"                               (only for debugging)\n\n"
"";

//char const *pub_file = "pub_key";
//char const *msk_file = "master_key";
string pub_file = "pub_key";
string msk_file = "master_key";
string t_file = "t_key";
string r_file = "r_key";

void
parse_args( int argc, char** argv )
{
	int i;

	for( i = 1; i < argc; i++ )
		if(      !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") )
		{
			printf("%s", usage);
			exit(0);
		}
		/*else if( !strcmp(argv[i], "-v") || !strcmp(argv[i], "--version") )
		{
			printf(CPABE_VERSION, "-setup");
			exit(0);
		}*/
		else if( !strcmp(argv[i], "-p") || !strcmp(argv[i], "--output-public-key") )
		{
			if( ++i >= argc )
				die(usage);
			else
				pub_file = argv[i];
		}
		else if( !strcmp(argv[i], "-m") || !strcmp(argv[i], "--output-master-key") )
		{
			if( ++i >= argc )
				die(usage);
			else
				msk_file = argv[i];
		}
        else if ( !strcmp(argv[i], "-t") || !strcmp(argv[i], "--outut-t-key"))
        {
            if( ++i >= argc)
                die(usage);
            else
                t_file = argv[i];
        }
        else if ( !strcmp(argv[i], "-r") || !strcmp(argv[i], "--output-r-key"))
        {
            if(++i >= argc)
                die(usage);
            else
                r_file = argv[i];
        }
		else if( !strcmp(argv[i], "-d") || !strcmp(argv[i], "--deterministic") )
		{
			pbc_random_set_deterministic(0);
		}
		else
			die(usage);
}

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

int
main( int argc, char** argv )
{
	//abe_pub_t* pub;
	//abe_msk_t* msk;    
    //pub = (abe_pub_t*)malloc(sizeof(abe_pub_t));
    //msk = (abe_msk_t*)malloc(sizeof(abe_msk_t));
	//abe_setup(&pub, &msk, type_params);
    long n = 18;
	parse_args(argc, argv);
    ABE_mkey mKey(type_params, n);
	spit_file(const_cast<char*>(pub_file.c_str()), abe_pub_serialize(mKey.abe_mpub), 1);
	spit_file(const_cast<char*>(msk_file.c_str()), abe_msk_serialize(mKey.abe_msec), 1);
    GByteArray* b;
    b = g_byte_array_new();
    vec_ZZ_p_serialize(b, mKey.t);
    spit_file(const_cast<char*>(t_file.c_str()), b, 1);

    GByteArray* b_r;
    b_r = g_byte_array_new();
    mat_ZZ_p_serialize(b_r, mKey.mR);
    spit_file(const_cast<char*>(r_file.c_str()), b_r, 1);
    //g_byte_array_free(b, 1);

	return 0;
}
// Last Update:2015-05-14 11:43:05
/**
 * @file setup.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-23
 */

