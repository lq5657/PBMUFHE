/*
 * =====================================================================================
 *
 *       Filename:  abe_core.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月22日 12时02分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
// Last Update:2015-05-15 18:09:32
/**
 * @file abe_core.c
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-22
 */
#include <stdlib.h>
#include <string.h>
#ifndef ABE_DEBUG
#define NDEBUG
#endif
#include <assert.h>

#include <openssl/sha.h>
#include <glib.h>
#include <pbc/pbc.h>

#include "abe_core.h"

char exist_error[256];
char* abe_error()
{
    return exist_error;
}
void raise_error(char * fmt, ... )
{
    va_list args;

#ifdef ABE_DEBUF
    va_start(args, fmt);
    vfsprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
#else
    va_start(args, fmt);
    vsnprintf(exist_error, 256, fmt, args);
    va_end(args);
#endif
}

void element_from_string( element_t h, char* s)
{
    unsigned char* r;
    r = (unsigned char*)malloc(SHA_DIGEST_LENGTH);
    SHA1((unsigned char*) s, strlen(s), r);
    element_from_hash(h, r, SHA_DIGEST_LENGTH);
    free(r);
}

abe_policy_t* base_node( int k, char* s)
{
    abe_policy_t* p;

    p = (abe_policy_t*)malloc(sizeof(abe_policy_t));
    p->k = k;
    p->attr = s ? strdup(s) : 0;
    p->children = g_ptr_array_new();
    p->q = 0;

    return p;
}

abe_policy_t* parse_policy_postfix( char* s)
{
    char** toks;
    char** cur_toks;
    char* tok;
    GPtrArray* stack;
    abe_policy_t* root;

    toks = g_strsplit(s, " ", 0);
    cur_toks = toks;
    stack = g_ptr_array_new();

    while( *cur_toks )
	{
		int i, k, n;

		tok = *(cur_toks++);

		if( !*tok )
			continue;

		if( sscanf(tok, "%dof%d", &k, &n) != 2 )
			/* push leaf token */
			g_ptr_array_add(stack, base_node(1, tok));
		else
		{
			abe_policy_t* node;

			/* parse "kofn" operator */

			if( k < 1 )
			{
				raise_error("error parsing \"%s\": trivially satisfied operator \"%s\"\n", s, tok);
				return 0;
			}
			else if( k > n )
			{
				raise_error("error parsing \"%s\": unsatisfiable operator \"%s\"\n", s, tok);
				return 0;
			}
			else if( n == 1 )
			{
				raise_error("error parsing \"%s\": identity operator \"%s\"\n", s, tok);
				return 0;
			}
			else if( n > stack->len )
			{
				raise_error("error parsing \"%s\": stack underflow at \"%s\"\n", s, tok);
				return 0;
			}
			
			/* pop n things and fill in children */
			node = base_node(k, 0);
			g_ptr_array_set_size(node->children, n);
			for( i = n - 1; i >= 0; i-- )
				node->children->pdata[i] = g_ptr_array_remove_index(stack, stack->len - 1);
			
			/* push result */
			g_ptr_array_add(stack, node);
		}
	}

	if( stack->len > 1 )
	{
		raise_error("error parsing \"%s\": extra tokens left on stack\n", s);
		return 0;
	}
	else if( stack->len < 1 )
	{
		raise_error("error parsing \"%s\": empty policy\n", s);
		return 0;
	}

	root = (abe_policy_t*)g_ptr_array_index(stack, 0);

 	g_strfreev(toks);
 	g_ptr_array_free(stack, 0);

	return root;
}

abe_polynomial_t* rand_poly( int deg, element_t zero_val)
{
	int i;
	abe_polynomial_t* q;

	q = (abe_polynomial_t*) malloc(sizeof(abe_polynomial_t));
	q->deg = deg;
	q->coef = (element_t*) malloc(sizeof(element_t) * (deg + 1));

	for( i = 0; i < q->deg + 1; i++ )
		element_init_same_as(q->coef[i], zero_val);

	element_set(q->coef[0], zero_val);

	for( i = 1; i < q->deg + 1; i++ )
 		element_random(q->coef[i]);

	return q;
}

void eval_poly( element_t r, abe_polynomial_t* q, element_t x)
{
    int i;
	element_t s, t;

	element_init_same_as(s, r);
	element_init_same_as(t, r);

	element_set0(r);
	element_set1(t);

	for( i = 0; i < q->deg + 1; i++ )
	{
		/* r += q->coef[i] * t */
		element_mul(s, q->coef[i], t);
		element_add(r, r, s);

		/* t *= x */
		element_mul(t, t, x);
	}

	element_clear(s);
	element_clear(t);
}

void enc_policy( abe_policy_t* policy, abe_pub_t* pub, element_t e)
{
    int i;
    element_t r;
    element_t t;
    element_t h;
    element_init_Zr(r, pub->p);
    element_init_Zr(r, pub->p);
	element_init_Zr(t, pub->p);
	element_init_G2(h, pub->p);

	policy->q = rand_poly(policy->k - 1, e);

	if( policy->children->len == 0 )
	{
		element_init_G1(policy->c,  pub->p);
		element_init_G2(policy->cp, pub->p);

		element_from_string(h, policy->attr);
		element_pow_zn(policy->c,  pub->g, policy->q->coef[0]);
		element_pow_zn(policy->cp, h,      policy->q->coef[0]);
	}
	else
		for( i = 0; i < policy->children->len; i++ )
		{
			element_set_si(r, i + 1);
			eval_poly(t, policy->q, r);
			enc_policy(g_ptr_array_index(policy->children, i), pub, t);
		}

	element_clear(r);
	element_clear(t);
	element_clear(h);
}

void check_sat( abe_policy_t* p, abe_prv_t* prv)
{
	int i, l;

	p->satisfiable = 0;
	if( p->children->len == 0 )
	{
		for( i = 0; i < prv->cmps->len; i++ )
			if( !strcmp(g_array_index(prv->cmps, abe_prv_comp_t, i).attr,
									p->attr) )
			{
                //printf("%s\n",p->attr);
				p->satisfiable = 1;
				p->attri = i;
				break;
			}
	}
	else
	{
		for( i = 0; i < p->children->len; i++ )
			check_sat(g_ptr_array_index(p->children, i), prv);

		l = 0;
		for( i = 0; i < p->children->len; i++ )
			if( ((abe_policy_t*) g_ptr_array_index(p->children, i))->satisfiable )
				l++;

		if( l >= p->k )
			p->satisfiable = 1;
	}
}

void abe_setup( abe_pub_t** pub, abe_msk_t** msk, const char* params )
{
    element_t alpha;
    (*pub)->pairing_desc = strdup(params);
    pairing_init_set_buf((*pub)->p, (*pub)->pairing_desc, strlen((*pub)->pairing_desc));
    
   	element_init_G1((*pub)->g,           (*pub)->p);
	element_init_G1((*pub)->h,           (*pub)->p);
	element_init_G2((*pub)->gp,          (*pub)->p);
	element_init_GT((*pub)->g_hat_alpha, (*pub)->p);              //e(g,g)^alpha
	element_init_Zr(alpha,               (*pub)->p);
	element_init_Zr((*msk)->beta,        (*pub)->p);
	element_init_G2((*msk)->g_alpha,     (*pub)->p);

	/* compute */

 	element_random(alpha);
 	element_random((*msk)->beta);
	element_random((*pub)->g);
	element_random((*pub)->gp);

	element_pow_zn((*msk)->g_alpha, (*pub)->gp, alpha);
	element_pow_zn((*pub)->h, (*pub)->g, (*msk)->beta);
  pairing_apply((*pub)->g_hat_alpha, (*pub)->g, (*msk)->g_alpha, (*pub)->p);
}

void
serialize_uint32( GByteArray* b, uint32_t k )
{
	int i;
	guint8 byte;

	for( i = 3; i >= 0; i-- )
	{
		byte = (k & 0xff<<(i*8))>>(i*8);
		g_byte_array_append(b, &byte, 1);
	}
}

uint32_t
unserialize_uint32( GByteArray* b, int* offset )
{
	int i;
	uint32_t r;

	r = 0;
	for( i = 3; i >= 0; i-- )
		r |= (b->data[(*offset)++])<<(i*8);

	return r;
}

void
serialize_element( GByteArray* b, element_t e )
{
	uint32_t len;
	unsigned char* buf;

	len = element_length_in_bytes(e);
	serialize_uint32(b, len);

	buf = (unsigned char*) malloc(len);
	element_to_bytes(buf, e);
	g_byte_array_append(b, buf, len);
	free(buf);
}

void
unserialize_element( GByteArray* b, int* offset, element_t e )
{
	uint32_t len;
	unsigned char* buf;

	len = unserialize_uint32(b, offset);

	buf = (unsigned char*) malloc(len);
	memcpy(buf, b->data + *offset, len);
	*offset += len;

	element_from_bytes(e, buf);
	free(buf);
}

void
serialize_string( GByteArray* b, char* s )
{
	g_byte_array_append(b, (unsigned char*) s, strlen(s) + 1);
}

char*
unserialize_string( GByteArray* b, int* offset )
{
	GString* s;
	char* r;
	char c;

	s = g_string_sized_new(32);
	while( 1 )
	{
		c = b->data[(*offset)++];
		if( c && c != EOF )
			g_string_append_c(s, c);
		else
			break;
	}

	r = s->str;
	g_string_free(s, 0);

	return r;
}

GByteArray* abe_pub_serialize( abe_pub_t* pub )
{
    GByteArray* b;

	b = g_byte_array_new();
	serialize_string(b,  pub->pairing_desc);
	serialize_element(b, pub->g);
	serialize_element(b, pub->h);
	serialize_element(b, pub->gp);
	serialize_element(b, pub->g_hat_alpha);

	return b;
}

GByteArray* abe_msk_serialize( abe_msk_t* msk )
{
    GByteArray* b;

	b = g_byte_array_new();
	serialize_element(b, msk->beta);
	serialize_element(b, msk->g_alpha);

	return b;
}

GByteArray* abe_prv_serialize( abe_prv_t* prv )
{
	GByteArray* b;
	int i;

	b = g_byte_array_new();

	serialize_element(b, prv->d);
	serialize_uint32( b, prv->cmps->len);

	for( i = 0; i < prv->cmps->len; i++ )
	{
		serialize_string( b, g_array_index(prv->cmps, abe_prv_comp_t, i).attr);
		serialize_element(b, g_array_index(prv->cmps, abe_prv_comp_t, i).d);
		serialize_element(b, g_array_index(prv->cmps, abe_prv_comp_t, i).dp);
	}

	return b;
}

void serialize_policy( GByteArray* b, abe_policy_t* p )
{
    int i;

	serialize_uint32(b, (uint32_t) p->k);

	serialize_uint32(b, (uint32_t) p->children->len);
	if( p->children->len == 0 )
	{
		serialize_string( b, p->attr);
		serialize_element(b, p->c);
		serialize_element(b, p->cp);
	}
	else
		for( i = 0; i < p->children->len; i++ )
			serialize_policy(b, g_ptr_array_index(p->children, i));
}

GByteArray* abe_cph_serialize( abe_cph_t* cph )
{
    GByteArray* b;

	b = g_byte_array_new();
	serialize_element(b, cph->cs);
	serialize_element(b, cph->c);
	serialize_policy( b, cph->p);

	return b;
}

abe_pub_t* abe_pub_unserialize( GByteArray* b, int free )
{
    abe_pub_t* pub;
    int offset;
    pub = (abe_pub_t*) malloc(sizeof(abe_pub_t));
    offset = 0;

    pub->pairing_desc = unserialize_string(b, &offset);
	pairing_init_set_buf(pub->p, pub->pairing_desc, strlen(pub->pairing_desc));

	element_init_G1(pub->g,           pub->p);
	element_init_G1(pub->h,           pub->p);
	element_init_G2(pub->gp,          pub->p);
	element_init_GT(pub->g_hat_alpha, pub->p);

	unserialize_element(b, &offset, pub->g);
	unserialize_element(b, &offset, pub->h);
	unserialize_element(b, &offset, pub->gp);
	unserialize_element(b, &offset, pub->g_hat_alpha);

	if( free )
		g_byte_array_free(b, 1);

	return pub;
}

abe_msk_t* abe_msk_unserialize( abe_pub_t* pub, GByteArray* b, int free )
{
	abe_msk_t* msk;
	int offset;

	msk = (abe_msk_t*) malloc(sizeof(abe_msk_t));
	offset = 0;

	element_init_Zr(msk->beta, pub->p);
	element_init_G2(msk->g_alpha, pub->p);

	unserialize_element(b, &offset, msk->beta);
	unserialize_element(b, &offset, msk->g_alpha);

	if( free )
		g_byte_array_free(b, 1);

	return msk;
}

abe_prv_t* abe_prv_unserialize( abe_pub_t* pub, GByteArray* b, int free )
{
	abe_prv_t* prv;
	int i;
	int len;
	int offset;

	prv = (abe_prv_t*) malloc(sizeof(abe_prv_t));
	offset = 0;

	element_init_G2(prv->d, pub->p);
	unserialize_element(b, &offset, prv->d);

	prv->cmps = g_array_new(0, 1, sizeof(abe_prv_comp_t));
	len = unserialize_uint32(b, &offset);

	for( i = 0; i < len; i++ )
	{
		abe_prv_comp_t c;

		c.attr = unserialize_string(b, &offset);

		element_init_G2(c.d,  pub->p);
		element_init_G2(c.dp, pub->p);

		unserialize_element(b, &offset, c.d);
		unserialize_element(b, &offset, c.dp);

		g_array_append_val(prv->cmps, c);
	}

	if( free )
		g_byte_array_free(b, 1);

	return prv;
}

abe_policy_t* unserialize_policy( abe_pub_t* pub, GByteArray* b, int* offset )
{
    int i;
	int n;
	abe_policy_t* p;

	p = (abe_policy_t*) malloc(sizeof(abe_policy_t));

	p->k = (int) unserialize_uint32(b, offset);
	p->attr = 0;
	p->children = g_ptr_array_new();

	n = unserialize_uint32(b, offset);
	if( n == 0 )
	{
		p->attr = unserialize_string(b, offset);
		element_init_G1(p->c,  pub->p);
		element_init_G1(p->cp, pub->p);
		unserialize_element(b, offset, p->c);
		unserialize_element(b, offset, p->cp);
	}
	else
		for( i = 0; i < n; i++ )
			g_ptr_array_add(p->children, unserialize_policy(pub, b, offset));

	return p;
}

abe_cph_t* abe_cph_unserialize( abe_pub_t* pub, GByteArray* b, int free )
{
    abe_cph_t* cph;
	int offset;

	cph = (abe_cph_t*) malloc(sizeof(abe_cph_t));
	offset = 0;

	element_init_GT(cph->cs, pub->p);
	element_init_G1(cph->c,  pub->p);
	unserialize_element(b, &offset, cph->cs);
	unserialize_element(b, &offset, cph->c);
	cph->p = unserialize_policy(pub, b, &offset);

	if( free )
		g_byte_array_free(b, 1);

	return cph;
}

void abe_pub_free( abe_pub_t* pub )
{
    element_clear(pub->g);
	element_clear(pub->h);
	element_clear(pub->gp);
	element_clear(pub->g_hat_alpha);
	pairing_clear(pub->p);
	free(pub->pairing_desc);
	free(pub);
}
void abe_msk_free( abe_msk_t* msk )
{
	element_clear(msk->beta);
	element_clear(msk->g_alpha);
	free(msk);
}
void abe_prv_free( abe_prv_t* prv )
{
    int i;
	
	element_clear(prv->d);

	for( i = 0; i < prv->cmps->len; i++ )
	{
		abe_prv_comp_t c;

		c = g_array_index(prv->cmps, abe_prv_comp_t, i);
		free(c.attr);
		element_clear(c.d);
		element_clear(c.dp);
	}

	g_array_free(prv->cmps, 1);

	free(prv);
}

void abe_policy_free( abe_policy_t* p)
{
	int i;

	if( p->attr )
	{
		free(p->attr);
		element_clear(p->c);
		element_clear(p->cp);
	}

	for( i = 0; i < p->children->len; i++ )
		abe_policy_free(g_ptr_array_index(p->children, i));

	g_ptr_array_free(p->children, 1);

	free(p);

}
void abe_cph_free( abe_cph_t* cph )
{
    element_clear(cph->cs);
	element_clear(cph->c);
	abe_policy_free(cph->p);

}

void
lagrange_coef( element_t r, GArray* s, int i )
{
	int j, k;
	element_t t;

	element_init_same_as(t, r);

	element_set1(r);
	for( k = 0; k < s->len; k++ )
	{
		j = g_array_index(s, int, k);
		if( j == i )
			continue;
		element_set_si(t, - j);
		element_mul(r, r, t); /* num_muls++; */
		element_set_si(t, i - j);
		element_invert(t, t);
		element_mul(r, r, t); /* num_muls++; */
	}

	element_clear(t);
}

abe_policy_t* cur_comp_pol;
int
cmp_int( const void* a, const void* b )
{
	int k, l;
	
	k = ((abe_policy_t*) g_ptr_array_index(cur_comp_pol->children, *((int*)a)))->min_leaves;
	l = ((abe_policy_t*) g_ptr_array_index(cur_comp_pol->children, *((int*)b)))->min_leaves;

	return
		k <  l ? -1 :
		k == l ?  0 : 1;
}

void pick_sat_min_leaves( abe_policy_t* p, abe_prv_t* prv)
{
    int i, k, l;
	int* c;

	assert(p->satisfiable == 1);

	if( p->children->len == 0 )
		p->min_leaves = 1;
	else
	{
		for( i = 0; i < p->children->len; i++ )
			if( ((abe_policy_t*) g_ptr_array_index(p->children, i))->satisfiable )
				pick_sat_min_leaves(g_ptr_array_index(p->children, i), prv);

		c = alloca(sizeof(int) * p->children->len);
		for( i = 0; i < p->children->len; i++ )
			c[i] = i;
	    cur_comp_pol = p;
		qsort(c, p->children->len, sizeof(int), cmp_int);

		p->satl = g_array_new(0, 0, sizeof(int));
		p->min_leaves = 0;
		l = 0;

		for( i = 0; i < p->children->len && l < p->k; i++ )
			if( ((abe_policy_t*) g_ptr_array_index(p->children, c[i]))->satisfiable )
			{
				l++;
				p->min_leaves += ((abe_policy_t*) g_ptr_array_index(p->children, c[i]))->min_leaves;
				k = c[i] + 1;
				g_array_append_val(p->satl, k);
			}
		assert(l == p->k);
	}
}

void
dec_leaf_flatten( element_t r, element_t exp,
					abe_policy_t* p, abe_prv_t* prv, abe_pub_t* pub )
{
	abe_prv_comp_t* c;
	element_t s;
	element_t t;

	c = &(g_array_index(prv->cmps, abe_prv_comp_t, p->attri));

	element_init_GT(s, pub->p);
	element_init_GT(t, pub->p);

	pairing_apply(s, p->c,  c->d,  pub->p); /* num_pairings++; */
	pairing_apply(t, p->cp, c->dp, pub->p); /* num_pairings++; */
	element_invert(t, t);
	element_mul(s, s, t); /* num_muls++; */
	element_pow_zn(s, s, exp); /* num_exps++; */

	element_mul(r, r, s); /* num_muls++; */

	element_clear(s);
	element_clear(t);
}

void
dec_node_flatten( element_t r, element_t exp,
									abe_policy_t* p, abe_prv_t* prv, 
                                    abe_pub_t* pub );

void
dec_internal_flatten( element_t r, element_t exp,
					abe_policy_t* p, abe_prv_t* prv, abe_pub_t* pub )
{
	int i;
	element_t t;
	element_t expnew;

	element_init_Zr(t, pub->p);
	element_init_Zr(expnew, pub->p);

	for( i = 0; i < p->satl->len; i++ )
	{
 		lagrange_coef(t, p->satl, g_array_index(p->satl, int, i));
		element_mul(expnew, exp, t); /* num_muls++; */
		dec_node_flatten(r, expnew, g_ptr_array_index
										 (p->children, g_array_index(p->satl, int, i) - 1), prv, pub);
	}

	element_clear(t);
	element_clear(expnew);
}

void
dec_node_flatten( element_t r, element_t exp,
									abe_policy_t* p, abe_prv_t* prv, 
                                    abe_pub_t* pub )
{
	assert(p->satisfiable);
	if( p->children->len == 0 )
		dec_leaf_flatten(r, exp, p, prv, pub);
	else
		dec_internal_flatten(r, exp, p, prv, pub);
}

void dec_flatten( element_t r, abe_policy_t* p, abe_prv_t* prv, abe_pub_t* pub)
{
    element_t one;

	element_init_Zr(one, pub->p);

	element_set1(one);
	element_set1(r);

	dec_node_flatten(r, one, p, prv, pub);

	element_clear(one);
}
