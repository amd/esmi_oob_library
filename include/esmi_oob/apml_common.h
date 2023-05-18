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

#include <esmi_oob/apml.h>

/* Default data for input */
#define DEFAULT_DATA            0
#define BIT_LEN			1	//!< Bit length //
#define SEMI_NIBBLE_BITS	2	//!< Half nibble bits //
#define NIBBLE_BITS		4	//!< Nibble bits //
#define BYTE_BITS		8	//!< Byte bits //
#define WORD_BITS		16	//!< word bits //
#define D_WORD_BITS		32	//!< Double word bits //
#define LO_WORD_REG             0	//!< Low word register //
#define HI_WORD_REG             1	//!< High word register //

/* MASKS */
#define NIBBLE_MASK		0xF
/* Mask for bmc control pcie rate */
#define GEN5_RATE_MASK          3
/* one byte mask for DDR bandwidth */
#define ONE_BYTE_MASK           0XFF
/* Teo byte mask */
#define TWO_BYTE_MASK           0xFFFF
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
 * @brief common utility functions
 */
static oob_status_t extract_n_bits(uint32_t num, uint8_t n_bits,
			    uint8_t pos, uint8_t *buffer)
{
	*buffer = (((1 << n_bits) - 1) & (num >> (pos - 1)));

	return OOB_SUCCESS;
}
#endif  // INCLUDE_APML_COMMON_H_
