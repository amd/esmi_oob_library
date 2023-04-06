/*
 * University of Illinois/NCSA Open Source License
 *
 * Copyright (c) 2023, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Developed by:
 *
 *                 AMD Research and AMD Software Development
 *
 *                 Advanced Micro Devices, Inc.
 *
 *                 www.amd.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimers.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimers in
 *    the documentation and/or other materials provided with the distribution.
 *  - Neither the names of <Name of Development Group, Name of Institution>,
 *    nor the names of its contributors may be used to endorse or promote
 *    products derived from this Software without specific prior written
 *    permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 *
 */

 #ifndef INCLUDE_APML_COMMON_H_
 #define INCLUDE_APML_COMMON_H_

#include "apml.h"

#define BIT(N) ((uint32_t)1 << N)		//!< Perform left shift operation by N bits //
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0])) //!< Returns the array size //

/* Default data for input */
#define DEFAULT_DATA            0
#define BIT_LEN			1	//!< Bit length //
#define SEMI_NIBBLE_BITS	2	//!< Half nibble bits //
#define TRIBBLE_BITS		3	//!< 3 bits //
#define NIBBLE_BITS		4	//!< Nibble bits //
#define BYTE_BITS		8	//!< Byte bits //
#define WORD_BITS		16	//!< word bits //
#define D_WORD_BITS		32	//!< Double word bits //
#define LO_WORD_REG             0	//!< Low word register //
#define HI_WORD_REG             1	//!< High word register //
/* Legacy APML encodings count */
#define LEGACY_ENCODING_SIZE		8	//!< Legacy encoding size //
/* MI300A APML encoding count */
#define MI300A_ENCODING_SIZE		10	//!< MI300A encoding size //

/* MASKS */
#define NIBBLE_MASK		0xF
/* Mask for bmc control pcie rate */
#define GEN5_RATE_MASK          3
/* one byte mask for DDR bandwidth */
#define ONE_BYTE_MASK           0XFF
/* Two byte mask */
#define TWO_BYTE_MASK           0xFFFF
/* Three byte mask */
#define THREE_BYTE_MASK		0xFFFFFF
/* Four byte mask used in raplcoreenergy, raplpackageenergy */
#define FOUR_BYTE_MASK          0xFFFFFFFF
/* CPU index mask used in boost limit write */
#define CPU_INDEX_MASK          0xFFFF0000
/* TU Mask used in read bmc rapl units */
#define TU_MASK                 0xF
/* ESU Mask in read bmc rapl units */
#define ESU_MASK                0x1F
/* FCLK Mask used in read current df-pstate frequency */
#define FCLK_MASK               0xFFF
/* Bandwidth Mask used in reading ddr bandwidth */
#define BW_MASK                 0xFFF

/**
 * @brief APML link ID encodings for legacy platforms and MI300A.
 * Structure  contains the following members.
 * Link ID encoding value and its name.
 *  MI300A APML Link ID Encoding:
 *      00000011b: P2
 *      00000100b: P3
 *      00001000b: G0
 *      00001001b: G1
 *      00001010b: G2
 *      00001011b: G3
 *      00001100b: G4
 *      00001101b: G5
 *      00001110b: G6
 *      00001111b: G7
 *  For other platforms the APML Link ID Encoding:
 *      00000001b: P0
 *      00000010b: P1
 *      00000100b: P2
 *      00001000b: P3
 *      00010000b: G0
 *      00100000b: G1
 *      01000000b: G2
 *      10000000b: G3
 */
struct apml_encodings {
        uint8_t val;            //!< Link ID encoding value
        char    name[3];        //!< Link ID encoding name
};

/* Apml link ID encodings for legacy platforms */
extern const struct apml_encodings encodings[LEGACY_ENCODING_SIZE];             //!< Legacy platforms link ID encodings
/* Apml link ID encodings for MI300A encodings */
extern const struct apml_encodings mi300A_encodings[MI300A_ENCODING_SIZE];      //!< MI300A platforms link ID encodings

/**
 * @brief common utility functions
 */
static oob_status_t extract_n_bits(uint32_t num, uint8_t n_bits,
			    uint8_t pos, uint8_t *buffer)
{
	*buffer = (((1 << n_bits) - 1) & (num >> (pos - 1)));

	return OOB_SUCCESS;
}
#endif  // INCLUDE_APML_COMMON_H_
