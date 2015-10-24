/*
 * ARM MaPU emulation.
 *
 * Copyright (c) 2010 - 2011 B Labs Ltd.
 * Copyright (c) 2011 Linaro Limited
 * Written by Bahadir Balban, Amit Mahajan, Peter Maydell
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 *  Contributions after 2012-01-13 are licensed under the terms of the
 *  GNU GPL, version 2 or (at your option) any later version.
 */

#include "hw/sysbus.h"
#include "hw/arm/arm.h"
#include "hw/arm/primecell.h"
#include "hw/devices.h"
#include "net/net.h"
#include "sysemu/sysemu.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "exec/address-spaces.h"
#include "sysemu/block-backend.h"
#include "hw/block/flash.h"
#include "sysemu/device_tree.h"
#include "qemu/error-report.h"
#include <libfdt.h>

#include "hw/intc/arm_gic.h"
#include "hw/char/serial.h"

#include "sys/shm.h"

#define MaPU_BOARD_ID 0x8e0
#define MaPU_FLASH_SIZE (512 * 1024 * 1024)
#define MaPU_FLASH_SECT_SIZE (256 * 1024)
#define MaPU_LOADER_START_ADDR 0x60000000

enum
{
	MaPU_NOR_FLASH,
	MaPU_SRAM,
	MaPU_SHAREMEM,
	MaPU_APC_REG,
	MaPU_DMA,
	MaPU_TIMER,
	MaPU_UART0,
	MaPU_UART1,
	MaPU_UART2,
	MaPU_GICCPU,
	MaPU_GICDIS,
	MaPU_SDRAM
};

static hwaddr MaPUboard_map[] =
{ [MaPU_NOR_FLASH]= 0, [MaPU_SRAM] = 0x20000000, [MaPU_SHAREMEM] = 0x40400000,
		[MaPU_APC_REG] = 0x41000000,
		[MaPU_DMA] = 0x50000000, [MaPU_TIMER] = 0x50400000,
		[MaPU_UART0] = 0x50900000, [MaPU_UART1] = 0x50910000, [MaPU_UART2
				] = 0x50920000, [MaPU_GICCPU] = 0x547f0000, [MaPU_GICDIS
				] = 0x547f1000, [MaPU_SDRAM] = 0x60000000};

static struct arm_boot_info mapu_binfo;

static void mapu_init(MachineState *mms)
{
	ObjectClass *cpu_oc;
	Object *cpuobj;
	ARMCPU *cpu;
	DriveInfo *dinfo;
	qemu_irq pic[160];
	DeviceState *gicdev;
	SysBusDevice *busdev;

	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *sram = g_new(MemoryRegion, 1);
	MemoryRegion *sdram = g_new(MemoryRegion, 1);
	MemoryRegion *share_mem = g_new(MemoryRegion, 1);
	MemoryRegion *ddr3_sdram = g_new(MemoryRegion, 1);

	int shmFlg;
	/*
	 * Create CPU
	 */
	cpu_oc = cpu_class_by_name(TYPE_ARM_CPU, "cortex-a8");
	if (!cpu_oc)
	{
		fprintf(stderr, "Unable to find CPU definition\n");
		exit(1);
	}
	cpuobj = object_new(object_class_get_name(cpu_oc));

	/*
	 * Disable EL3 support
	 */
	Error *err = NULL;
	if (object_property_find(cpuobj, "has_el3", NULL))
	{
		object_property_set_bool(cpuobj, false, "has_el3", &err);
		if (err)
		{
			error_report_err(err);
			exit(1);
		}
	}

	object_property_set_bool(cpuobj, true, "realized", &err);
	if (err)
	{
		error_report_err(err);
		exit(1);
	}
	cpu = ARM_CPU(cpuobj);

	/* Memory */

	if (mms->ram_size > 0x20000000)
	{
		/* 512MB is the maximum the address space permits */
		fprintf(stderr, "mapu_cortext-a8: cannot model more than 512MB RAM\n");
		exit(1);
	}

	fprintf(stderr, "ram size = %x\n", (unsigned int) mms->ram_size);

	memory_region_allocate_system_memory(sram, NULL, "mapu.sram", mms->ram_size);

	memory_region_add_subregion(sysmem, MaPUboard_map[MaPU_SRAM], sram);

	isShared = 1;

	memory_region_allocate_system_memory(sdram, NULL, "mapu.sdram", 0x21000000);

	isShared = 0;

    memory_region_init_alias(share_mem, NULL, "share_memory", sdram, 0x20000000, 0x1000000);
    memory_region_init_alias(ddr3_sdram, NULL, "DDR3_sdram", sdram, 0, 0x20000000);

    memory_region_add_subregion(sysmem, MaPUboard_map[MaPU_SHAREMEM], share_mem);

	memory_region_add_subregion(sysmem, MaPUboard_map[MaPU_SDRAM], ddr3_sdram);


	/* irq controller
	 * arm_gic
	 * cpu interface: 0x547f0000
	 * distributor interface: 0x547f1000
	 * number of IRQ: 96
	 */
	gicdev = qdev_create(NULL, "arm_gic");
	qdev_prop_set_uint32(DEVICE(gicdev), "num-irq", 160);
	qdev_prop_set_uint32(DEVICE(gicdev), "revision", 2);
	qdev_prop_set_uint32(DEVICE(gicdev), "num-cpu", 1);
	qdev_init_nofail(gicdev);
	busdev = SYS_BUS_DEVICE(gicdev);
	sysbus_mmio_map(busdev, 0, MaPUboard_map[MaPU_GICDIS]);
	sysbus_mmio_map(busdev, 1, MaPUboard_map[MaPU_GICCPU]);

	int n;

	for (n = 0; n < 160; n++)
	{
		pic[n] = qdev_get_gpio_in(gicdev, n);
	}

	/* connect the CPU to GIC   */
	sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(DEVICE(cpu), 0));
	sysbus_connect_irq(busdev, 1, qdev_get_gpio_in(DEVICE(cpu), 1));

	/* NOR flash
	 * Size: 64 MB
	 * Address: 0x00000000
	 */
	dinfo = drive_get(IF_PFLASH, 0, 0);

	if (!pflash_cfi01_register(MaPUboard_map[MaPU_NOR_FLASH], NULL,
			"MaPU.flash",
			MaPU_FLASH_SIZE, dinfo ? blk_by_legacy_dinfo(dinfo) : NULL,
			MaPU_FLASH_SECT_SIZE,
			MaPU_FLASH_SIZE / MaPU_FLASH_SECT_SIZE, 4, 0x0089, 0x0018, 0x0000,
			0x0, 0))
	{
		fprintf(stderr, "qemu: Error registering flash memory.\n");
	}

	fprintf(stderr, "pflash init done!\n");
	/* UART 0
	 * Address: 0x5090 0000
	 *  */

	if (serial_hds[0])
	{
		serial_mm_init(get_system_memory(), MaPUboard_map[MaPU_UART0], 2, pic[2], 115200, serial_hds[0], DEVICE_NATIVE_ENDIAN);
		fprintf(stderr, "mapu uart0 init done!\n");
	}
	if (serial_hds[1])
	{
		serial_mm_init(get_system_memory(), MaPUboard_map[MaPU_UART1], 2, pic[3], 115200, serial_hds[1], DEVICE_NATIVE_ENDIAN);
		fprintf(stderr, "mapu uart1 init done!\n");
	}
	if (serial_hds[2])
	{
		serial_mm_init(get_system_memory(), MaPUboard_map[MaPU_UART2], 2, pic[4], 115200, serial_hds[2], DEVICE_NATIVE_ENDIAN);
		fprintf(stderr, "mapu uart2 init done!\n");
	}

    sysbus_create_varargs("dw_apb_timer", MaPUboard_map[MaPU_TIMER],
                          pic[8],
                          pic[9],
						  pic[10],
						  pic[11],
                          pic[12],
                          pic[13],
						  pic[14],
						  pic[15],
                          NULL);
    fprintf(stderr, "mapu timer init done!\n");

    sysbus_create_varargs("apc_if", MaPUboard_map[MaPU_APC_REG], NULL);

    fprintf(stderr, "mapu apc reg init done!\n");

	mapu_binfo.ram_size = mms->ram_size;
	mapu_binfo.kernel_filename = mms->kernel_filename;
	mapu_binfo.kernel_cmdline = mms->kernel_cmdline;
	mapu_binfo.initrd_filename = mms->initrd_filename;
	mapu_binfo.board_id = MaPU_BOARD_ID;
	mapu_binfo.loader_start = MaPU_LOADER_START_ADDR;
	mapu_binfo.firmware_loaded = bios_name || drive_get(IF_PFLASH, 0, 0);
	arm_load_kernel(cpu, &mapu_binfo);

	fprintf(stderr, "mapu board init done!\n");
}

static QEMUMachine mapu_machine =
{ .name = "mapu", .desc = "ARM MaPU (Cortex-A8)", .init = mapu_init,
		.block_default_type = IF_SCSI, };

static void mapu_machine_init(void)
{
	qemu_register_machine(&mapu_machine);
}

machine_init(mapu_machine_init);
