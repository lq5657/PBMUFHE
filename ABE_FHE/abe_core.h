/*
 * =====================================================================================
 *
 *       Filename:  abe_core.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月21日 12时05分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
// Last Update:2015-05-14 22:32:05
/**
 * @file abe_core.h
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-21
 */

#ifndef ABE_CORE_H
#define ABE_CORE_H
#include <glib.h>

#include "abe_t.h"

#if defined (__cplusplus)
extern "C" {
#endif

/*
  A public key.
*/
typedef struct abe_pub_s abe_pub_t;

/*
  A master secret key.
*/
typedef struct abe_msk_s abe_msk_t;

/*
  A private key.
*/
typedef struct abe_prv_s abe_prv_t;

/*
  A ciphertext. Note that this library only handles encrypting a
  single group element, so if you want to encrypt something bigger,
  you will have to use that group element as a symmetric key for
  hybrid encryption (which you do yourself).
*/
typedef struct abe_cph_s abe_cph_t;

void raise_error(char* fmt, ...);

void element_from_string(element_t h, char* s);

/* 生成主公钥和主密钥 */
void abe_setup( abe_pub_t** pub, abe_msk_t** msk, const char* params );

/*
  Exactly what it seems.
*/
GByteArray* abe_pub_serialize( abe_pub_t* pub );
GByteArray* abe_msk_serialize( abe_msk_t* msk );
GByteArray* abe_prv_serialize( abe_prv_t* prv );
GByteArray* abe_cph_serialize( abe_cph_t* cph );

/*void serialize_policy( GByteArray* b, abe_policy_t* p );
void serialize_unit32( GByteArray* b, uint32_t k );
void serialize_element( GByteArray* b, element_t e );
void serialize_string( GByteArray* b, char* s);*/


/*
  Also exactly what it seems. If free is true, the GByteArray passed
  in will be free'd after it is read.
*/
abe_pub_t* abe_pub_unserialize( GByteArray* b, int free );
abe_msk_t* abe_msk_unserialize( abe_pub_t* pub, GByteArray* b, int free );
abe_prv_t* abe_prv_unserialize( abe_pub_t* pub, GByteArray* b, int free );
abe_cph_t* abe_cph_unserialize( abe_pub_t* pub, GByteArray* b, int free );

/*abe_policy_t* unserialize_policy( abe_pub_t* pub, GByteArray* b, int* offset );
uint32_t unserizlize_uint32( GByteArray* b, int* offset );
void unserialize_element( GByteArray* b, int* offset, element_t e );
char* unserialize_string( GByteArray* b, int* offset );*/

/*
  Again, exactly what it seems.
*/
void abe_pub_free( abe_pub_t* pub );
void abe_msk_free( abe_msk_t* msk );
void abe_prv_free( abe_prv_t* prv );
void abe_cph_free( abe_cph_t* cph );

/*
  Return a description of the last error that occured. Call this after
  bswabe_enc or bswabe_dec returns 0. The returned string does not
  need to be free'd.
*/
char* abe_error();

/* 根据策略创建策略访问树 */
abe_policy_t* parse_policy_postfix( char* s);
/* 加密策略访问树 */
void enc_policy( abe_policy_t* p, abe_pub_t* pub, element_t e);

/* 检测属性是否满足密文策略 */
void check_sat( abe_policy_t* p, abe_prv_t* prv);

void pick_sat_min_leaves( abe_policy_t* p, abe_prv_t* prv);
void dec_flatten( element_t r, abe_policy_t* p, abe_prv_t* prv, abe_pub_t* pub);

#if defined (__cplusplus)
} // extern "C"
#endif

#endif  /*ABE_CORE_H*/
