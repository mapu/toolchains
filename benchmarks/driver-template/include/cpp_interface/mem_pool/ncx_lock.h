#ifndef _NCX_LOCK_H_
#define _NCX_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define ncx_shmtx_lock(x)   { /*void*/ }
#define ncx_shmtx_unlock(x) { /*void*/ }

typedef struct {

  ncx_uint_t spin;

} ncx_shmtx_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
