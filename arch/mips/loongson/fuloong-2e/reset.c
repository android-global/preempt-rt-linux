/*
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * Copyright (C) 2007 Lemote, Inc. & Institute of Computing Technology
 * Author: Fuxin Zhang, zhangfx@lemote.com
 */
#include <linux/pm.h>

#include <asm/reboot.h>

static void loongson_restart(char *command)
{
#ifdef CONFIG_32BIT
	*(unsigned long *)0xbfe00104 &= ~(1 << 2);
	*(unsigned long *)0xbfe00104 |= (1 << 2);
#else
	*(unsigned long *)0xffffffffbfe00104 &= ~(1 << 2);
	*(unsigned long *)0xffffffffbfe00104 |= (1 << 2);
#endif
	__asm__ __volatile__("jr\t%0" : : "r"(0xbfc00000));
}

static void loongson_halt(void)
{
	while (1)
		;
}

void loongson_reboot_setup(void)
{
	_machine_restart = loongson_restart;
	_machine_halt = loongson_halt;
	pm_power_off = loongson_halt;
}
