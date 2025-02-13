//  ---------------------------------------------------------------------------
//  This file is part of reSID, a MOS6581 SID emulator engine.
//  Copyright (C) 1999  Dag Lem <resid@nimrod.no>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  ---------------------------------------------------------------------------

#ifndef __SIDDEFS_FP_H__
#define __SIDDEFS_FP_H__

#ifndef M_PI
#define M_PI    3.14159265358979323846
#define M_PI_f  3.14159265358979323846f
#else
#define M_PI_f  ((float) M_PI)
#endif

#ifndef M_LN2
#define M_LN2   0.69314718055994530942
#define M_LN2_f 0.69314718055994530942f
#else
#define M_LN2_f ((float) M_LN2)
#endif

// Define bool, true, and false for C++ compilers that lack these keywords.
#define RESID_HAVE_BOOL 1

#if !RESID_HAVE_BOOL
typedef int bool;
const bool true = 1;
const bool false = 0;
#endif

// We could have used the smallest possible data type for each SID register,
// however this would give a slower engine because of data type conversions.
// An int is assumed to be at least 32 bits (necessary in the types reg24,
// cycle_count, and sound_sample). GNU does not support 16-bit machines
// (GNU Coding Standards: Portability between CPUs), so this should be
// a valid assumption.

typedef unsigned int reg4;
typedef unsigned int reg8;
typedef unsigned int reg12;
typedef unsigned int reg16;
typedef unsigned int reg24;

typedef int cycle_count;

enum chip_model { MOS6581FP=1, MOS8580FP };

enum sampling_method { SAMPLE_INTERPOLATE=1, SAMPLE_RESAMPLE_INTERPOLATE };

// Inlining on/off.
#define RESID_INLINE inline

#define HAVE_LOGF
#define HAVE_EXPF
#define HAVE_LOGF_PROTOTYPE
#define HAVE_EXPF_PROTOTYPE

#endif // not __SIDDEFS_H__
