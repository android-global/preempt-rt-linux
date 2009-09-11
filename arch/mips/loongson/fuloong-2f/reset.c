/* Board-specific reboot/shutdown routines
 *
 * Copyright (c) 2009 Philippe Vachon <philippe@cowpig.ca>
 *
 * Copyright (C) 2009 Lemote Inc. & Insititute of Computing Technology
 * Author: Wu Zhangjin, wuzj@lemote.com
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/io.h>
#include <linux/delay.h>
#include <linux/types.h>

#include <loongson.h>

/* cs5536 is the south bridge used by fuloong2f mini PC */
#include <cs5536/cs5536.h>

void mach_prepare_reboot(void)
{
	/*
	 * reset cpu to full speed, this is needed when enabling cpu frequency
	 * scalling
	 */
	LOONGSON_CHIPCFG0 |= 0x7;

	/* send a reset signal to south bridge.
	 *
	 * NOTE: if enable "Power Management" in kernel, rtl8169 will not reset
	 * normally with this reset operation and it will not work in PMON, but
	 * you can type halt command and then reboot, seems the hardware reset
	 * logic not work normally.
	 */
#if 1
	{
		u32 hi, lo;
		_rdmsr(DIVIL_MSR_REG(DIVIL_SOFT_RESET), &hi, &lo);
		lo |= 0x00000001;
		_wrmsr(DIVIL_MSR_REG(DIVIL_SOFT_RESET), hi, lo);
	}
#else
	{
		u32 hi, lo;
		_rdmsr(GLCP_MSR_REG(GLCP_SYS_RST), &hi, &lo);
		lo |= 0x00000001;
		_wrmsr(GLCP_MSR_REG(GLCP_SYS_RST), hi, lo);
	}
#endif
}

void mach_prepare_shutdown(void)
{
	u32 hi, lo, val;
	phys_addr_t gpio_base;

	_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_GPIO), &hi, &lo);

	gpio_base = mips_io_port_base | (lo & 0xff00);

	/* make cs5536 gpio13 output enable */
	val = (readl((u32 *) (gpio_base + GPIOL_OUT_EN)) & ~(1 << (16 + 13)))
	    | (1 << 13);
	writel(val, (u32 *) (gpio_base + GPIOL_OUT_EN));
	mmiowb();
	/* make cs5536 gpio13 output low level voltage. */
	val = (readl((u32 *) (gpio_base + GPIOL_OUT_VAL)) & ~(1 << (13)))
	    | (1 << (16 + 13));
	writel(val, (u32 *) (gpio_base + GPIOL_OUT_VAL));
	mmiowb();
}
