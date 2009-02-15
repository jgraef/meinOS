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

#ifndef _FENV_H_
#define _FENV_H_

#define FE_DIVBYZERO 0x01
#define FE_INEXACT   0x02
#define FE_INVALID   0x04
#define FE_OVERFLOW  0x08
#define FE_UNDERFLOW 0x10

#define FE_TONEAREST  0x000
#define FE_DOWNWARD   0x400
#define FE_UPWARD     0x800
#define FE_TOWARDZERO 0xC00

#define FE_ALL_EXCEPT (FE_DIVBYZERO|FE_INEXACT|FE_INVALID|FE_OVERFLOW|FE_UNDERFLOW)

//#define FE_DFL_ENV __fe_dfl_env;
#define FE_DFL_ENV    ((const fenv_t*)-1)
#define FE_NOMASK_ENV ((const fenv_t*)-2)

typedef struct {
  unsigned short int __control_word;
  unsigned short int __unused1;
  unsigned short int __status_word;
  unsigned short int __unused2;
  unsigned short int __tags;
  unsigned short int __unused3;
  unsigned int __eip;
  unsigned short int __cs_selector;
  unsigned int __opcode:11;
  unsigned int __unused4:5;
  unsigned int __data_offset;
  unsigned short int __data_selector;
  unsigned short int __unused5;
} fenv_t;

typedef unsigned short int fexcept_t;

/*const fenv_t __fe_dfl_env_data = {
  /// @todo TODO
};

const fenv_t *__fe_dfl_env = &__fe_dfl_env_data;*/

int feclearexcept(int);
int fegetexceptflag(fexcept_t *, int);
int feraiseexcept(int);
int fesetexceptflag(const fexcept_t *, int);
int fetestexcept(int);
int fegetround(void);
int fesetround(int);
int fegetenv(fenv_t *);
int feholdexcept(fenv_t *);
int fesetenv(const fenv_t *);
int feupdateenv(const fenv_t *);

#endif /* _FENV_H_ */
