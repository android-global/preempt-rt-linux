/* Board-specific reboot/shutdown routines
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

#include <linux/types.h>
#include <asm/bootinfo.h>

#include <loongson.h>
#include <machine.h>

/*
 * The following registers are determined by the EC index configuration.
 * 1, fill the PORT_HIGH as EC register high part.
 * 2, fill the PORT_LOW as EC register low part.
 * 3, fill the PORT_DATA as EC register write data or get the data from it.
 */
#define	EC_RESET_IO_PORT_HIGH	0x0381
#define	EC_RESET_IO_PORT_LOW	0x0382
#define	EC_RESET_IO_PORT_DATA	0x0383
#define	REG_RESET_HIGH	0xF4	/* reset the machine auto-clear : rd/wr */
#define REG_RESET_LOW	0xEC
#define	BIT_RESET_ON	(1 << 0)

/* 7inch yeeloong have the different shutdown hardware logic from 8.9inch */
#define	EC_SHUTDOWN_IO_PORT_HIGH	0xff2d
#define	EC_SHUTDOWN_IO_PORT_LOW		0xff2e
#define	EC_SHUTDOWN_IO_PORT_DATA	0xff2f
#define	REG_SHUTDOWN_HIGH	0xFC
#define REG_SHUTDOWN_LOW	0x29
#define	BIT_SHUTDOWN_ON	(1 << 1)

void mach_prepare_reboot(void)
{
	/*
	 * reset cpu to full speed, this is needed when enabling cpu frequency
	 * scalling
	 */
	LOONGSON_CHIPCFG0 |= 0x7;

	/* sending an reset signal to EC(embedded controller) */
	writeb(REG_RESET_HIGH,
	       (u8 *) (mips_io_port_base + EC_RESET_IO_PORT_HIGH));
	writeb(REG_RESET_LOW,
	       (u8 *) (mips_io_port_base + EC_RESET_IO_PORT_LOW));
	mmiowb();
	writeb(BIT_RESET_ON,
	       (u8 *) (mips_io_port_base + EC_RESET_IO_PORT_DATA));
	mmiowb();
}

void mach_prepare_shutdown(void)
{
	char *system_type = get_system_type();

	if (strstr(system_type, "8.9inch") != NULL) {
		/* cpu-gpio0 output low */
		LOONGSON_GPIODATA &= ~0x00000001;
		/* cpu-gpio0 as output */
		LOONGSON_GPIOIE &= ~0x00000001;
	} else if (strstr(system_type, "7inch") != NULL) {
		u8 val;
		u64 i;

		writeb(REG_SHUTDOWN_HIGH,
		       (u8 *) (mips_io_port_base + EC_SHUTDOWN_IO_PORT_HIGH));
		writeb(REG_SHUTDOWN_LOW,
		       (u8 *) (mips_io_port_base + EC_SHUTDOWN_IO_PORT_LOW));
		mmiowb();
		val =
		    readb((u8 *) (mips_io_port_base +
				  EC_SHUTDOWN_IO_PORT_DATA));
		writeb(val & (~BIT_SHUTDOWN_ON),
		       (u8 *) (mips_io_port_base + EC_SHUTDOWN_IO_PORT_DATA));
		mmiowb();
		/* need enough wait here... how many microseconds needs? */
		for (i = 0; i < 0x10000; i++)
			delay();
		writeb(val | BIT_SHUTDOWN_ON,
		       (u8 *) (mips_io_port_base + EC_SHUTDOWN_IO_PORT_DATA));
		mmiowb();
	} else
		printk(KERN_INFO "you can shutdown the power safely now!\n");
}
