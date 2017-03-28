/*
 * ExternalInterrupt.c
 *
 * Created: 06.02.2017 12:54:16
 *  Author: M43734
 */ 

 void EXTERNAL_IRQ_0_init(void)
 {
#if 0
	 NVIC_DisableIRQ(EIC_IRQn);

	 /* Set pin direction to input */
	 gpio_set_pin_direction(PA25, GPIO_DIRECTION_IN);
	 gpio_set_pin_pull_mode(PA25, GPIO_PULL_UP);
	 gpio_set_pin_function(PA25, PINMUX_PA25A_EIC_EXTINT5);

	 /* Enable clocks and reset EIC */
	 hri_gclk_write_CLKCTRL_reg(GCLK, GCLK_CLKCTRL_ID(EIC_GCLK_ID) | GCLK_CLKCTRL_GEN(CONF_GCLK_EIC_SRC) | GCLK_CLKCTRL_CLKEN);
	 hri_eic_set_CTRL_SWRST_bit(EIC);
	 hri_eic_wait_for_sync(EIC);

	 /* Enable INT5 */
	 hri_eic_set_INTEN_reg(EIC, EIC_INTENSET_EXTINT5);

	 /* Optional: Enable Filter */
	 hri_eic_write_CONFIG_reg(EIC, 0, EIC_CONFIG_SENSE5_LOW /*| EIC_CONFIG_FILTEN5 */);

	 /* Enable EIC */
	 hri_eic_set_CTRL_ENABLE_bit(EIC);

	 /* Clear interrupt flags */
	 hri_eic_clear_INTFLAG_EXTINT5_bit(EIC);
	 NVIC_ClearPendingIRQ(EIC_IRQn);
	 NVIC_EnableIRQ(EIC_IRQn);
	 #endif
 }

 void EIC_Handler(void)
 {
#if 0
	 hri_eic_clear_INTFLAG_EXTINT5_bit(EIC);
#endif
 }