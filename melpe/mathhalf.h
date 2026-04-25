/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/*

2.4 kbps MELP Proposed Federal Standard speech coder

Fixed-point C code, version 1.0

Copyright (c) 1998, Texas Instruments, Inc.

Texas Instruments has intellectual property rights on the MELP
algorithm.	The Texas Instruments contact for licensing issues for
commercial and non-government use is William Gordon, Director,
Government Contracts, Texas Instruments Incorporated, Semiconductor
Group (phone 972 480 7442).

The fixed-point version of the voice codec Mixed Excitation Linear
Prediction (MELP) is based on specifications on the C-language software
simulation contained in GSM 06.06 which is protected by copyright and
is the property of the European Telecommunications Standards Institute
(ETSI). This standard is available from the ETSI publication office
tel. +33 (0)4 92 94 42 58. ETSI has granted a license to United States
Department of Defense to use the C-language software simulation contained
in GSM 06.06 for the purposes of the development of a fixed-point
version of the voice codec Mixed Excitation Linear Prediction (MELP).
Requests for authorization to make other use of the GSM 06.06 or
otherwise distribute or modify them need to be addressed to the ETSI
Secretariat fax: +33 493 65 47 16.

*/

#ifndef _MATHHALF_H_
#define _MATHHALF_H_

#if defined(_MSC_VER)
#define MELPE_ALWAYS_INLINE __forceinline
#define MELPE_ATTR_ALWAYS_INLINE
#elif defined(__GNUC__) || defined(__clang__)
#define MELPE_ALWAYS_INLINE inline __attribute__((always_inline))
#define MELPE_ATTR_ALWAYS_INLINE __attribute__((always_inline))
#else
#define MELPE_ALWAYS_INLINE inline
#define MELPE_ATTR_ALWAYS_INLINE
#endif

/* addition */

static MELPE_ALWAYS_INLINE int16_t melpe_add(int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

static MELPE_ALWAYS_INLINE int16_t melpe_sub(int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

static MELPE_ALWAYS_INLINE int32_t melpe_L_add(int32_t L_var1, int32_t L_var2) MELPE_ATTR_ALWAYS_INLINE;	/* 2 ops */

static MELPE_ALWAYS_INLINE int32_t melpe_L_sub(int32_t L_var1, int32_t L_var2) MELPE_ATTR_ALWAYS_INLINE;	/* 2 ops */

/* multiplication */

static MELPE_ALWAYS_INLINE int16_t melpe_mult(int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

static MELPE_ALWAYS_INLINE int32_t melpe_L_mult(int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

/* arithmetic shifts */

static inline int16_t melpe_shr(int16_t var1, int16_t var2);	/* 1 ops */

static MELPE_ALWAYS_INLINE int16_t melpe_shl(int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

static inline int32_t melpe_L_shr(int32_t L_var1, int16_t var2);	/* 2 ops */

static MELPE_ALWAYS_INLINE int32_t melpe_L_shl(int32_t L_var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 2 ops */

static MELPE_ALWAYS_INLINE int16_t melpe_shift_r(int16_t var, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 2 ops */

static MELPE_ALWAYS_INLINE int32_t melpe_L_shift_r(int32_t L_var, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 3 ops */

/* absolute value  */

static MELPE_ALWAYS_INLINE int16_t melpe_abs_s(int16_t var1) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

static MELPE_ALWAYS_INLINE int32_t melpe_L_abs(int32_t var1) MELPE_ATTR_ALWAYS_INLINE;	/* 3 ops */

/* multiply accumulate	*/

static MELPE_ALWAYS_INLINE int32_t melpe_L_mac(int32_t L_var3, int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 1 op */

static MELPE_ALWAYS_INLINE int32_t melpe_L_msu(int32_t L_var3, int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 1 op */

static MELPE_ALWAYS_INLINE int16_t melpe_msu_r(int32_t L_var3, int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 2 op */

/* negation  */

static MELPE_ALWAYS_INLINE int16_t melpe_negate(int16_t var1) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

static MELPE_ALWAYS_INLINE int32_t melpe_L_negate(int32_t L_var1) MELPE_ATTR_ALWAYS_INLINE;	/* 2 ops */

/* Accumulator manipulation */

static MELPE_ALWAYS_INLINE int32_t melpe_L_deposit_l(int16_t var1) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

static MELPE_ALWAYS_INLINE int32_t melpe_L_deposit_h(int16_t var1) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

static MELPE_ALWAYS_INLINE int16_t melpe_extract_l(int32_t L_var1) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

static MELPE_ALWAYS_INLINE int16_t melpe_extract_h(int32_t L_var1) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

/* r_ound */

static MELPE_ALWAYS_INLINE int16_t melpe_r_ound(int32_t L_var1) MELPE_ATTR_ALWAYS_INLINE;	/* 1 ops */

/* Normalization */

static MELPE_ALWAYS_INLINE int16_t melpe_norm_l(int32_t L_var1) MELPE_ATTR_ALWAYS_INLINE;	/* 30 ops */

static MELPE_ALWAYS_INLINE int16_t melpe_norm_s(int16_t var1) MELPE_ATTR_ALWAYS_INLINE;	/* 15 ops */

/* Division */

static MELPE_ALWAYS_INLINE int16_t melpe_divide_s(int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;	/* 18 ops */

/* -------------------------------------------------------------------------- */
/* 40-Bit Routines....added by Andre 11/23/99 */

/* new 40 bits basic operators */

static MELPE_ALWAYS_INLINE int64_t melpe_L40_add(int64_t acc, int32_t L_var1) MELPE_ATTR_ALWAYS_INLINE;

static MELPE_ALWAYS_INLINE int64_t melpe_L40_sub(int64_t acc, int32_t L_var1) MELPE_ATTR_ALWAYS_INLINE;

static MELPE_ALWAYS_INLINE int64_t melpe_L40_mac(int64_t acc, int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;

static MELPE_ALWAYS_INLINE int64_t melpe_L40_msu(int64_t acc, int16_t var1, int16_t var2) MELPE_ATTR_ALWAYS_INLINE;

static inline int64_t melpe_L40_shl(int64_t acc, int16_t var1);

static MELPE_ALWAYS_INLINE int64_t melpe_L40_shr(int64_t acc, int16_t var1) MELPE_ATTR_ALWAYS_INLINE;

static MELPE_ALWAYS_INLINE int64_t melpe_L40_negate(int64_t acc) MELPE_ATTR_ALWAYS_INLINE;

static MELPE_ALWAYS_INLINE int16_t melpe_norm32(int64_t acc) MELPE_ATTR_ALWAYS_INLINE;
static MELPE_ALWAYS_INLINE int32_t melpe_L_sat32(int64_t acc) MELPE_ATTR_ALWAYS_INLINE;

#include "mathhalf_i.h"

#endif
