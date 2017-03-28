/**
 * \file
 *
 * \brief SAM Power manager
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
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
 *    and/or other materials provided with the distribution.
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
 *
 */

#include <hpl_pm_base.h>
#include <hpl_pm_config.h>
#include <hpl_reset.h>
#include <hpl_sleep.h>
#include <utils_assert.h>
/**
 * \brief Retrieve AHB index
 *
 * \param[in] module Module to get index for
 *
 * \return index of the given module if succeeds, ERR_INVALID_ARG otherwise
 */
static int32_t _pm_get_ahb_index(const void *const module)
{
	if ((uint32_t)module == (uint32_t)PM_BUS_APBA) {
		return 0;
	} else if ((uint32_t)module == (uint32_t)PM_BUS_APBB) {
		return 1;
	} else if ((uint32_t)module == (uint32_t)PM_BUS_APBC) {
		return 2;
	}
	if ((uint32_t)module == (uint32_t)DSU) {
		return 3;
	} else if ((uint32_t)module == (uint32_t)NVMCTRL) {
		return 4;
	} else if ((uint32_t)module == (uint32_t)DMAC) {
		return 5;
	}

	return ERR_INVALID_ARG;
}

/**
 * \brief Retrieve APBB index
 *
 * \param[in] module Module to get index for
 *
 * \return index of the given module if succeeds, ERR_INVALID_ARG otherwise
 */
static int32_t _pm_get_apbb_index(const void *const module)
{
	if ((uint32_t)module == (uint32_t)PAC1) {
		return 0;
	} else if ((uint32_t)module == (uint32_t)DSU) {
		return 1;
	} else if ((uint32_t)module == (uint32_t)NVMCTRL) {
		return 2;
	}
	if ((uint32_t)module == (uint32_t)PORT) {
		return 3;
	} else if ((uint32_t)module == (uint32_t)DMAC) {
		return 4;
	}

	return ERR_INVALID_ARG;
}
/**
 * \brief Retrieve the reset reason
 */
enum reset_reason _get_reset_reason(void)
{
	return (enum reset_reason)hri_pm_read_RCAUSE_reg(PM);
}

/**
 * \brief Set the sleep mode for the device
 */
int32_t _set_sleep_mode(const uint8_t mode)
{
	switch (mode) {
	case 0:
	case 1:
	case 2:
		SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
		PM->SLEEP.reg = mode;
		return ERR_NONE;
	case 3:
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
		return ERR_NONE;
	default:
		return ERR_INVALID_ARG;
	}
}

/**
 * \brief Enable clock on the given bus for the given hardware module
 */
void _pm_enable_bus_clock(const enum _pm_bus bus, const void *const module)
{
	uint32_t peripheral = ((uint32_t)module & 0x0000ff00) >> 10;

	switch (bus) {
	case PM_BUS_AHB:
		if (_pm_get_ahb_index(module) >= 0) {
			peripheral = (uint32_t)_pm_get_ahb_index(module);
			PM->AHBMASK.reg |= 1 << peripheral;
		}
		break;
	case PM_BUS_APBA:
		PM->APBAMASK.reg |= 1 << peripheral;
		break;
	case PM_BUS_APBB:
		if (_pm_get_apbb_index(module) >= 0) {
			peripheral = (uint32_t)_pm_get_apbb_index(module);
			PM->APBBMASK.reg |= 1 << peripheral;
		}
		break;
	case PM_BUS_APBC:
		PM->APBCMASK.reg |= 1 << peripheral;
		break;
	default:
		Dummy_Handler();
		break;
	}
}

/**
 * \brief Disable clock on the given bus for the given hardware module
 */
void _pm_disable_bus_clock(const enum _pm_bus bus, const void *const module)
{
	uint32_t peripheral = ((uint32_t)module & 0x0000ff00) >> 10;

	switch (bus) {
	case PM_BUS_AHB:
		if (_pm_get_ahb_index(module) >= 0) {
			peripheral = (uint32_t)_pm_get_ahb_index(module);
			PM->AHBMASK.reg &= ~(1 << peripheral);
		}
		break;
	case PM_BUS_APBA:
		PM->APBAMASK.reg &= ~(1 << peripheral);
		break;
	case PM_BUS_APBB:
		if (_pm_get_apbb_index(module) >= 0) {
			peripheral = (uint32_t)_pm_get_apbb_index(module);
			PM->APBBMASK.reg &= ~(1 << peripheral);
		}
		break;
	case PM_BUS_APBC:
		PM->APBCMASK.reg &= ~(1 << peripheral);
		break;
	default:
		Dummy_Handler();
		break;
	}
}

/**
 * \brief Power Manager Init
 */
void _pm_init(void)
{
	hri_pm_set_CPUSEL_CPUDIV_bf(PM, CONF_CPU_DIV);
	hri_pm_set_APBASEL_APBADIV_bf(PM, CONF_APBA_DIV);
	hri_pm_set_APBBSEL_APBBDIV_bf(PM, CONF_APBB_DIV);
	hri_pm_set_APBCSEL_APBCDIV_bf(PM, CONF_APBC_DIV);
}
