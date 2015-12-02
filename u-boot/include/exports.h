#ifndef __EXPORTS_H__
#define __EXPORTS_H__

#ifndef __ASSEMBLY__

/* These are declarations of exported functions available in C code */
unsigned long get_version(void);
int  getc(void);
int  tstc(void);
void putc(const char);
void puts(const char*);
int printf(const char* fmt, ...);
void install_hdlr(int, void (*interrupt_handler_t)(void *), void*);
void free_hdlr(int);
void *malloc(size_t);
void free(void*);
void __udelay(unsigned long);
unsigned long get_timer(unsigned long);
int vprintf(const char *, va_list);
unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base);
int strict_strtoul(const char *cp, unsigned int base, unsigned long *res);
char *getenv (const char *name);
int setenv (const char *varname, const char *varvalue);
long simple_strtol(const char *cp,char **endp,unsigned int base);
int strcmp(const char * cs,const char * ct);
unsigned long ustrtoul(const char *cp, char **endp, unsigned int base);
unsigned long long ustrtoull(const char *cp, char **endp, unsigned int base);
#if defined(CONFIG_CMD_I2C)
int i2c_write (uchar, uint, int , uchar* , int);
int i2c_read (uchar, uint, int , uchar* , int);
#endif

void app_startup(char * const *);

#endif    /* ifndef __ASSEMBLY__ */

enum {
#define EXPORT_FUNC(x) XF_ ## x ,
#include <_exports.h>
#undef EXPORT_FUNC

	XF_MAX
};

#define XF_VERSION	6

#if defined(CONFIG_X86)
extern gd_t *global_data;
#endif

/* apc */
/* ape.h */
int32_t ape_dma_request(struct dma_if *dma_conf, uint32_t cpuid);
int32_t ape_dma_wait(uint32_t group, uint32_t cpuid);
int32_t ape_dma_status(uint32_t cpuid);
int32_t ape_mail_status(uint32_t cpuid);
int32_t ape_status(uint32_t cpuid);
int32_t ape_stop(uint32_t cpuid);
int32_t ape_start(uint32_t cpuid);
int32_t ape_wait(uint32_t cpuid);
void apc_init(void);

/* loader.h */
int32_t apcapi_core(const char *api_name, void *param, uint32_t cpuid);
int32_t load_spu(const char *funcname, void *param, uint32_t cpuid);
int32_t set_spu_section(uint32_t dst, uint32_t value, uint32_t size, uint32_t cpuid);
int32_t load_spu_section(uint32_t dst, uint32_t src, uint32_t size, uint32_t cpuid);
int32_t load_mpu_section(uint32_t src, uint32_t size, uint32_t cpuid);
int32_t send_vector(uint32_t dst, uint32_t src, uint32_t len, uint32_t size,
                   uint32_t cpuid);
int32_t send_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t height,
                    uint32_t size, uint32_t cpuid);
int32_t fetch_vector(uint32_t dst, uint32_t src, uint32_t len, uint32_t size,
                     uint32_t cpuid);
int32_t fetch_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t height,
                     uint32_t size, uint32_t cpuid);

#endif	/* __EXPORTS_H__ */
