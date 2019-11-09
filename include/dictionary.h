#if !defined(__DICTIONARY__)
#define __DICTIONARY__

#include "platform.h"

#define __SCREEN__BOUNDS__X__ 640
#define __SCREEN__BOUNDS__Y__ 480

enum CommandType
{
  __UNKNOWN_COMMAND,
  __SET_BACKGROUND_COLOUR,
  __DRAW_HORIZONTAL_LINE,
  __DRAW_VERTICAL_LINE,
  __DRAW_RECTANGLE,
};

enum ColourType
{
  __UNKNOWN_COLOUR,
  __RED,
  __BLUE,
  __GREEN,
  __BLACK,
  __YELLOW,
};

struct Pixel
{
  uint16 x_location;
  uint16 y_location;
};

struct Colour
{
  uint16 red   : 5;
  uint16 green : 6;
  uint16 blue  : 5;
};

struct Commands
{
  enum   CommandType command_type;
  enum   ColourType  colour_type;
  struct Pixel*      pixel_array;
  struct Colour      colour2draw;
};

union ColourBitUnison
{
         uint16 _bits2decimal;
  struct Colour _colourBits;
};


#endif  /** __DICTIONARY__ **/
