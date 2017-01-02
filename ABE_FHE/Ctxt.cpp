/*
 * =====================================================================================
 *
 *       Filename:  Ctxt.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年04月23日 09时36分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <glib.h>
#include <pbc/pbc.h>
#include <NTL/mat_ZZ_p.h>

#include "abe_core.h"
#include "Ctxt.h"
#include "matrix_fhe.h"
#include "timing.h"

NTL_CLIENT
// Last Update:2015-05-16 16:03:04
/**
 * @file Ctxt.cpp
 * @brief 
 * @author youdao5657@163.com
 * @version 0.1.00
 * @date 2015-04-23
 */
Ctxt::Ctxt(abe_cph_t* policy_cp, Mat<ZZ_p> cipher)
{
    //pCp = (abe_cph_t*)malloc(sizeof(abe_cph_t));
    //element_init_same_as(pCp->c, policy_cp->c);
    //element_init_same_as(pCp->cs, policy_cp->cs);
    //element_set(pCp->c, policy_cp->c);
    //element_set(pCp->cs, policy_cp->cs);
    //pCp->p = policy_cp->p;
    pCp = policy_cp;
    A = cipher;
}

Ctxt& Ctxt::operator=(const Ctxt& other)
{
    A = other.A;
    pCp = other.pCp;
    return *this;
}

void Ctxt::addCtxt(const Ctxt& other)
{
    FHE_TIMER_START;
    A += Flatten(other.A);
    FHE_TIMER_STOP;
}

void Ctxt::multiplyBy(const Ctxt& other)
{
    FHE_TIMER_START;
    
    A = Flatten(A * other.A);

    FHE_TIMER_STOP;
}
