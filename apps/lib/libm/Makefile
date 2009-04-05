-include ../../../Makefile.config

# Possible modes are:
#  -D_IEEE_MODE  - IEEE (recommended)
#  -D_XOPEN_MODE - X/OPEN
#  -D_POSIX_MODE - POSIX/ANSI
#  -D_SVID3_MODE - SVID

CC = $(LIBS_CC)
CFLAGS = $(LIBS_CFLAGS) -D_IEEE_MODE

OBJFILES = \
carg.o              i386/fegetround.o    s_fpclassify.o   s_trunc.o \
e_acos.o            i386/feholdexcpt.o   s_fpclassifyf.o  w_acos.o \
e_acosh.o           i386/fesetenv.o      s_frexp.o        w_acosh.o \
e_asin.o            i386/fesetround.o    s_ilogb.o        w_asin.o \
e_atan2.o           i386/feupdateenv.o   s_isinf.o        w_atan2.o \
e_atanh.o           i386/fgetexcptflg.o  s_isinff.o       w_atanh.o \
e_cosh.o            i386/fraiseexcpt.o   s_isnan.o        w_cabs.o \
e_exp.o             i386/fsetexcptflg.o  s_isnanf.o       w_cosh.o \
e_fmod.o            i386/ftestexcept.o   s_ldexp.o        w_drem.o \
e_gamma.o           k_cos.o              s_lib_version.o  w_exp2.o \
e_gamma_r.o         k_rem_pio2.o         s_llrint.o       w_exp.o \
e_hypot.o           k_sin.o              s_llround.o      w_fmod.o \
e_j0.o              k_standard.o         s_log1p.o        w_gamma.o \
e_j1.o              k_tan.o              s_logb.o         w_gamma_r.o \
e_jn.o              ldouble_wrappers.o   s_lrint.o        w_hypot.o \
e_lgamma.o          nan.o                s_lround.o       w_j0.o \
e_lgamma_r.o        s_asinh.o            s_matherr.o      w_j1.o \
e_log10.o           s_atan.o             s_modf.o         w_jn.o \
e_log2.o            s_cbrt.o             s_nearbyint.o    w_lgamma.o \
e_log.o             s_ceil.o             s_nextafter.o    w_lgamma_r.o \
e_pow.o             s_copysign.o         s_remquo.o       w_log10.o \
e_remainder.o       s_cos.o              s_rint.o         w_log2.o \
e_rem_pio2.o        s_erf.o              s_round.o        w_log.o \
e_scalb.o           s_expm1.o            s_scalbln.o      w_pow.o \
e_sinh.o            s_fabs.o             s_scalbn.o       w_remainder.o \
e_sqrt.o            s_fdim.o             s_signbit.o      w_scalb.o \
float_wrappers.o    s_finite.o           s_signbitf.o     w_sinh.o \
i386/fclrexcpt.o    s_finitef.o          s_signgam.o      w_sqrt.o \
i386/fedisblxcpt.o  s_floor.o            s_significand.o  w_tgamma.o \
i386/feenablxcpt.o  s_fma.o              s_sin.o \
i386/fegetenv.o     s_fmax.o             s_tan.o \
i386/fegetexcept.o  s_fmin.o             s_tanh.o

all: ../libm.a

../libm.a : $(OBJFILES)
	$(LIBS_AR) cru $@ $^
	$(LIBS_RANLIB) $@

clean: 
	rm -f $(OBJFILES) ../libm.a
