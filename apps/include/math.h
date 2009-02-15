/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _MATH_H_
#define _MATH_H_

# define M_E		2.7182818284590452354	/* e */
# define M_LOG2E	1.4426950408889634074	/* log_2 e */
# define M_LOG10E	0.43429448190325182765	/* log_10 e */
# define M_LN2		0.69314718055994530942	/* log_e 2 */
# define M_LN10		2.30258509299404568402	/* log_e 10 */
# define M_PI		3.14159265358979323846	/* pi */
# define M_PI_2		1.57079632679489661923	/* pi/2 */
# define M_PI_4		0.78539816339744830962	/* pi/4 */
# define M_1_PI		0.31830988618379067154	/* 1/pi */
# define M_2_PI		0.63661977236758134308	/* 2/pi */
# define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
# define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
# define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

#define FLT_EVAL_METHOD 0

#define MAXFLOAT  ((float)3.40282346638528860e+38)
#define	HUGE      MAXFLOAT
#define HUGE_VAL  (__builtin_huge_val())
#define HUGE_VALF (__builtin_huge_valf())
#define HUGE_VALL (__builtin_huge_vall())
#define NAN       (__builtin_nanf(""))

# define FP_NAN       1
# define FP_INFINITE  2
# define FP_ZERO      3
# define FP_SUBNORMAL 4
# define FP_NORMAL    5

#define X_TLOSS 1.41484755040568800000e+16

// Types of exceptions in the `type' field.
#define DOMAIN    1
#define SING      2
#define OVERFLOW  3
#define UNDERFLOW 4
#define TLOSS     5
#define PLOSS     6

#define fpclassify(x) (sizeof(x)==sizeof(float)?__fpclassifyf(x):(sizeof(x)==sizeof(double)?__fpclassify(x):__fpclassifyl(x)))
#define isfinite(x)   (sizeof(x)==sizeof(float)?__isfinitef(x):(sizeof(x)==sizeof(double)?__isfinite(x):__isfinitel(x)))
#define isinf(x)      (sizeof(x)==sizeof(float)?__isinff(x):(sizeof(x)==sizeof(double)?__isinf(x):__isinfl(x)))
#define isnan(x)      (sizeof(x)==sizeof(float)?__isnanf(x):(sizeof(x)==sizeof(double)?__isnan(x):__isnanl(x)))
#define isnormal(x)   (sizeof(x)==sizeof(float)?__isnormalf(x):(sizeof(x)==sizeof(double)?__isnormal(x):__isnormall(x)))
#define signbit(x)    (sizeof(x)==sizeof(float)?__signbitf(x):(sizeof(x)==sizeof(double)?__signbit(x):__signbitl(x)))

#define isgreater(x, y)		__builtin_isgreater(x, y)
#define isgreaterequal(x, y)	__builtin_isgreaterequal(x, y)
#define isless(x, y)		__builtin_isless(x, y)
#define islessequal(x, y)	__builtin_islessequal(x, y)
#define islessgreater(x, y)	__builtin_islessgreater(x, y)
#define isunordered(u, v)	__builtin_isunordered(u, v)

typedef double double_t;
typedef float float_t;

# ifdef __cplusplus
struct __exception
# else
struct exception
# endif
{
  int type;
  char *name;
  double arg1;
  double arg2;
  double retval;
};

# ifdef __cplusplus
extern int matherr(struct __exception *__exc) throw ();
# else
extern int matherr(struct exception *__exc);
# endif

int __isfinite(double x);
int __isnan(double x);
int __isinf(double x);

int __isfinitef(float x);
int __isnanf(float x);
int __isinff(float x);

#endif /* _MATH_H_ */
