#if !defined(__PLATFORM__)
#define __PLATFORM__

#include <stdint.h>

#define TRUE    0x1u
#define FALSE   0x0u

#define RET_OK       0x00u    /* Okay validation status. */
#define RET_NOK      0x01u    /* Not okay validation status. */
#define RET_NOK_SYN  0x02u    /* Not correct syntax in command file. */
#define RET_NOK_ALOC 0x03u    /* Bad allocation. */
#define RET_N_EXP    0x04u    /* Not expected. */

#define RET     uint8_t
#define boolean uint8_t
#define uint8   uint8_t
#define uint16  uint16_t
#define uint32  uint32_t
#define uint64  uint64_t
#define string  char*
#define stream  char**

#endif /** __PLATFORM__ **/
