/**
 * \file
 *
 * \brief SAM Power manager
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distributionn.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 */

#ifndef _HPL_PM_BASE_H_INCLUDED
#define _HPL_PM_BASE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//#include <utils_assert.h>

/**
 * \addtogroup pm_group PM Low Level Driver Helpers
 *
 * \section pm_helpers_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

/**
 * \brief Power Manager buses
 */
enum _pm_bus { PM_BUS_AHB, PM_BUS_APBA, PM_BUS_APBB, PM_BUS_APBC };

/**
 * \name HPL functions
 */
//@{

/**
 * \brief Enable clock on the given bus for the given hardware module
 *
 * This function enables clock on the given bus for the given hardware module.
 * For an overview of available buses and hardware modules see datasheet.
 *
 * \param[in] bus A bus to enable clock on
 * \param[in] module A hardware module to enable clock for
 */
void _pm_enable_bus_clock(const enum _pm_bus bus, const void *const module);

/**
 * \brief Disable clock on the given bus for the given hardware module
 *
 * This function disables clock on the given bus for the given hardware module.
 * For an overview of available buses and hardware modules see datasheet.
 *
 * \param[in] bus A bus to disable clock on
 * \param[in] module A hardware module to disable clock for
 */
void _pm_disable_bus_clock(const enum _pm_bus bus, const void *const module);

/**@}*/

#ifdef __cplusplus
}
#endif
#endif /* _HPL_PM_BASE_H_INCLUDED */
