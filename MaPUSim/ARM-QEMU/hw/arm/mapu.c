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
	MaPU_WDT,
	MaPU_RTC,
	MaPU_VIF,
	MaPU_UART0,
	MaPU_UART1,
	MaPU_UART2,
	MaPU_KEYBOARD,
	MaPU_MOUSE,
	MaPU_GMAC,
	MaPU_SDC,
	MaPU_GICCPU,
	MaPU_GICDIS,
	MaPU_DDR3REG,
	MaPU_SDRAM
};

static hwaddr MaPUboard_map[] =
{ [MaPU_NOR_FLASH]= 0, [MaPU_SRAM] = 0x20000000, [MaPU_SHAREMEM] = 0x40400000,
		[MaPU_APC_REG] = 0x41000000, [MaPU_DMA] = 0x50000000,
		[MaPU_TIMER] = 0x50400000, [MaPU_WDT] = 0x50440000,[MaPU_RTC] = 0x50480000,
		[MaPU_VIF] = 0x50500000,
		[MaPU_UART0] = 0x50920000, /*Match MaPU chip*/
		[MaPU_UART1] = 0x50910000,
		[MaPU_UART2] = 0x50920000, [MaPU_KEYBOARD] = 0x50a20000,
		[MaPU_MOUSE] = 0x50a30000, [MaPU_GMAC] = 0x50c00000,
		[MaPU_SDC] = 0x50d00000,
		[MaPU_GICCPU] = 0x547f0000, [MaPU_GICDIS] = 0x547f1000,
		[MaPU_DDR3REG] = 0x54800000, [MaPU_SDRAM] = 0x60000000};

static struct arm_boot_info mapu_binfo;

/*
 * support for MaPU APC
 * luoxq
 */
int shmId = 0;
int enAPC = 1;
FILE *infoout = NULL;



static void mapu_init(MachineState *mms)
{
	ObjectClass *cpu_oc;
	Object *cpuobj;
	ARMCPU *cpu;
	DriveInfo *dinfo;
	qemu_irq pic[160];
	DeviceState *gicdev;
	SysBusDevice *busdev;
	int key;
	void *ptr;

	MemoryRegion *sysmem = get_system_memory();
	MemoryRegion *sram = g_new(MemoryRegion, 1);
	MemoryRegion *share_mem = g_new(MemoryRegion, 1);
	MemoryRegion *ddr3_sdram = g_new(MemoryRegion, 1);
	MemoryRegion *ddr3_reg = g_new(MemoryRegion, 1);

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

	memory_region_allocate_system_memory(sram, NULL, "mapu.sram", 0x20000000);
	memory_region_add_subregion(sysmem, MaPUboard_map[MaPU_SRAM], sram);
	fprintf(stderr, "\tmapu sram init done!\n");
	/*
	 * Add shared memory
	 */
	if (enAPC == 1)
	{
	  infoout = fopen( "info.out", "w+");

	  if(infoout == NULL)
	  {
	    fprintf(stderr, "\ncreate info.out for APC fail!\n\n");
	    exit(1);
	  }

    if (shmId == 0)
    {
      key = 9000;
      while ((shmId = shmget(key, 0x21000000, IPC_CREAT | IPC_EXCL | 0666)) < 0)
        key++;
        fprintf(infoout, "\nShare memory key is %d\n", key);
        fprintf(stderr, "\nShare memory key is %d\n", key);
        //fclose( infoout);
    }
    ptr = (uint8_t *)shmat(shmId, NULL, 0);
    assert(ptr != -1);
    memory_region_init_ram_ptr(share_mem, NULL, "share_memory", 0x1000000, ptr);
    vmstate_register_ram_global(share_mem);
    memory_region_init_ram_ptr(ddr3_sdram, NULL, "DDR3_sdram", 0x20000000, ptr+0x1000000);
    vmstate_register_ram_global(ddr3_sdram);
	}
	else
	{
	  memory_region_allocate_system_memory(share_mem, NULL, "share_memory", 0x1000000);
	  memory_region_allocate_system_memory(ddr3_sdram, NULL, "DDR3_sdram", 0x20000000);
	}

  memory_region_add_subregion(sysmem, MaPUboard_map[MaPU_SHAREMEM], share_mem);
  memory_region_add_subregion(sysmem, MaPUboard_map[MaPU_SDRAM], ddr3_sdram);
  fprintf(stderr, "\tmapu shared memory (%s) init done!\n", enAPC ? "with shared memory" : "with normal memory");

  /*
   * Add DDR3 control registers
   * no use in simulation, but avoid unpredictable problems of writing to space without declaration
   */

  memory_region_allocate_system_memory(ddr3_reg, NULL, "DDR3_reg", MaPUboard_map[MaPU_SDRAM] - MaPUboard_map[MaPU_DDR3REG]);
  memory_region_add_subregion(sysmem, MaPUboard_map[MaPU_DDR3REG], ddr3_reg);
  fprintf(stderr, "\tmapu DDR3 register init done!\n");

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
  fprintf(stderr, "\tmapu gic init done!\n");
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
	fprintf(stderr, "\tmapu pflash init done!\n");
	/* UART 0
	 * Address: 0x5090 0000
	 *  */

	if (serial_hds[0])
	{
		serial_mm_init(get_system_memory(), MaPUboard_map[MaPU_UART0], 2, pic[2], 115200, serial_hds[0], DEVICE_NATIVE_ENDIAN);
		fprintf(stderr, "\tmapu uart0 init done!\n");
	}
//	if (serial_hds[1])
//	{
//		serial_mm_init(get_system_memory(), MaPUboard_map[MaPU_UART1], 2, pic[3], 115200, serial_hds[1], DEVICE_NATIVE_ENDIAN);
//		fprintf(stderr, "\tmapu uart1 init done!\n");
//	}
//	if (serial_hds[2])
//	{
//		serial_mm_init(get_system_memory(), MaPUboard_map[MaPU_UART2], 2, pic[4], 115200, serial_hds[2], DEVICE_NATIVE_ENDIAN);
//		fprintf(stderr, "\tmapu uart2 init done!\n");
//	}

  sysbus_create_varargs("dw_apb_timer", MaPUboard_map[MaPU_TIMER],
      pic[8], pic[9], pic[10], pic[11],
      pic[12], pic[13], pic[14], pic[15],
                        NULL);
  fprintf(stderr, "\tmapu timer init done!\n");

  sysbus_create_varargs("dw_apb_rtc", MaPUboard_map[MaPU_RTC], pic[1], NULL);
  fprintf(stderr, "\tmapu dw_apb_rtc init done!\n");
  sysbus_create_varargs("dw_apb_wdt", MaPUboard_map[MaPU_WDT],
                  pic[30], NULL);
          fprintf(stderr, "\tmapu dw_apb_wdt init done!\n");
  if (enAPC == 1)
  {
    sysbus_create_varargs("apc_if", MaPUboard_map[MaPU_APC_REG],
        pic[42], pic[43], pic[44], pic[45],
        pic[46], pic[47], pic[48], pic[49],
        NULL);

    fprintf(stderr, "\tmapu apc if init done!\n");
    if(infoout != NULL)
      fclose(infoout);
  }

  sysbus_create_varargs("dw_apb_dmac", MaPUboard_map[MaPU_DMA],
          pic[4], NULL);
  fprintf(stderr, "\tmapu dw_apb_dmac init done!\n");

  sysbus_create_simple("pl110", MaPUboard_map[MaPU_VIF], pic[37]);
  fprintf(stderr, "\tmapu pl110 init done!\n");

  sysbus_create_simple("pl050_keyboard", MaPUboard_map[MaPU_KEYBOARD], pic[26]);
  fprintf(stderr, "\tmapu keyboard init done!\n");
  sysbus_create_simple("pl050_mouse", MaPUboard_map[MaPU_MOUSE], pic[27]);

  busdev = qdev_create(NULL, "generic-sdhci");
  qdev_init_nofail(busdev);
  sysbus_mmio_map(SYS_BUS_DEVICE(busdev), 0, MaPUboard_map[MaPU_SDC]);
  sysbus_connect_irq(SYS_BUS_DEVICE(busdev), 0, pic[31]);
  fprintf(stderr, "\tmapu sdhci init done!\n");

  if(nd_table[0].used)
  {
    smc91c111_init(&nd_table[0], MaPUboard_map[MaPU_GMAC], pic[0]);
    fprintf(stderr, "\tmapu network interface init done!\n");
  }
	mapu_binfo.ram_size = mms->ram_size;
	mapu_binfo.kernel_filename = mms->kernel_filename;
	mapu_binfo.kernel_cmdline = mms->kernel_cmdline;
	mapu_binfo.initrd_filename = mms->initrd_filename;
	mapu_binfo.board_id = MaPU_BOARD_ID;
	mapu_binfo.loader_start = MaPU_LOADER_START_ADDR;
	mapu_binfo.firmware_loaded = bios_name || drive_get(IF_PFLASH, 0, 0);
	arm_load_kernel(cpu, &mapu_binfo);

	fprintf(stderr, "\tmapu board init done!\n");
}

static QEMUMachine mapu_machine =
{ .name = "mapu", .desc = "ARM MaPU (Cortex-A8)", .init = mapu_init,
		.block_default_type = IF_SCSI, };

static void mapu_machine_init(void)
{
	qemu_register_machine(&mapu_machine);
}

machine_init(mapu_machine_init);
