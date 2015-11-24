   zreladdr-y	+= 0x60008000
params_phys-y	:= 0x60000100
initrd_phys-y	:= 0x60800000

dtb-$(CONFIG_MAPU_CHIP)	+= mapu_a8.dtb
dtb-$(CONFIG_MAPU_SIM)	+= mapu_sim.dtb
