#include <common.h>
#include <command.h>

static int do_hello(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	printf("hello u-boot! %d\n", argc);
	for (i=0; i<argc; i++)
	{
		printf("argv[%d]:%s\n",i,argv[i]);
	}
	return 0;
}

U_BOOT_CMD(
	hello,	CONFIG_SYS_MAXARGS,	1,	do_hello,
	"Just for test.",
	"long help...........................\n"
);
