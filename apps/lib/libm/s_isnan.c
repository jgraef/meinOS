/* @(#)s_isnan.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * isnan(x) returns 1 is x is nan, else 0;
 * no branching!
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	int __isnan(double x)
#else
	int __isnan(x)
	double x;
#endif
{
	int32_t hx,lx;
	EXTRACT_WORDS(hx,lx,x);
	hx &= 0x7fffffff;
	hx |= (u_int32_t)(lx|(-lx))>>31;
	hx = 0x7ff00000 - hx;
	return (int)(((u_int32_t)hx)>>31);
}
libm_hidden_def(__isnan)
weak_alias(__isnan,isnan)
libm_hidden_weak(isnan)
