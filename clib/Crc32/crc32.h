//
//  crc32.h
//
//  Created by Miroslav Kudrnac on 9/11/12.
//  Copyright (c) 2012 Miroslav Kudrnac. All rights reserved.
//

#ifndef CRC32_H
#define CRC32_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../CDefines.h"

/** Must be called before use of crc32 function
 */
void crc32_init(void);

/** computes crc32
 */
uint32 crc32_compute(const BYTE* buf, SIZE_T len);

#ifdef __cplusplus
}
#endif

#endif /* defined(__TransDB__crc32__) */
