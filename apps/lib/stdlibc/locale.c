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

#include <locale.h>

static struct lconv locale_posix = {
  .currency_symbol = "$",
  .decimal_point = ".",
  .frac_digits = 2,
  .grouping = "3",
  .thousands_sep = ",",

  .int_curr_symbol = "USD",
  .int_frac_digits = 2,
  .int_n_cs_precedes = 1,
  .int_n_sep_by_space = 0,
  .int_n_sign_posn = 1,
  .int_p_cs_precedes = 1,
  .int_p_sep_by_space = 0,
  .int_p_sign_posn =1 ,

  .mon_decimal_point = ".",
  .mon_grouping = "3",
  .mon_thousands_sep = ",",

  .negative_sign = "-",
  .n_cs_precedes = 1,
  .n_sep_by_space = 0,
  .n_sign_posn = 1,
  .positive_sign = "+",
  .p_cs_precedes = 1,
  .p_sep_by_space = 0,
  .p_sign_posn = 1
};
static struct lconv locale_current;
static char *locale_current_name;

/// @todo Implement
static void locale_copy(struct lconv *dest,struct lconv *src,int category) {
  if (category==LC_ALL) memcpy(dest,src,sizeof(struct lconv));
  else if (category==LC_COLLATE) {

  }
  else if (category==LC_CTYPE) {

  }
  else if (category==LC_MESSAGES) {

  }
  else if (category==LC_MONETARY) {

  }
  else if (category==LC_NUMERIC) {

  }
  else if (category==LC_TIME) {

  }
}

struct lconv *localeconv(void) {
  return &locale_current;
}

/// @todo Environment variable
char *setlocale(int category,const char *locale) {
  if (locale==NULL) return locale_current_name;
  else if (strcmp(locale,"POSIX")==0 || strcmp(locale,"C")==0 || locale[0]==0) {
    locale_copy(&locale_current,&locale_posix,category);
    locale_current_name = locale;
    return locale;
  }
  else return NULL;
}
