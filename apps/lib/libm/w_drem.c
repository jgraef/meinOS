/*
 * drem() wrapper for remainder().
 *
 * Written by J.T. Conklin, <jtc@wimsey.com>
 * Placed into the Public Domain, 1994.
 */

#include <math.h>

#include "math_private.h"

double drem(double x, double y)
{
	return remainder(x, y);
}
libm_hidden_def(drem)
