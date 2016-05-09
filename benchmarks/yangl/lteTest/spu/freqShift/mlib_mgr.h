/*
 * mlib_mgr.h
 *
 *  Created on: Feb 13, 2014
 *      Author: wangl
 */

#ifndef MLIB_MGR_H_
#define MLIB_MGR_H_

#include "mlib.h"

#define MAX_MC_NUM 16

void load_mcode(int id);

#ifndef MLOAD
#define MLOAD(name)
#endif

#endif /* APCLIB_MGR_H_ */
