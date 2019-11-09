#if !defined(__DRAW__)
#define __DRAW__

#include "dictionary.h"
#include "platform.h"
#include "compiler.h"


boolean draw2canvas(struct Commands);

extern unsigned int image_matrix[__SCREEN__BOUNDS__X__][__SCREEN__BOUNDS__Y__];



#endif /* __DRAW__ */
