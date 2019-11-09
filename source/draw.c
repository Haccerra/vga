#include "../include/draw.h"

unsigned int image_matrix[__SCREEN__BOUNDS__Y__][__SCREEN__BOUNDS__X__];
struct Colour previous_colour;


static void instantialise_matrix(int**);
static void draw_horizontal_line(int, int, int, struct Colour colour);
static void draw_vertical_line(int, int, int, struct Colour colour);
static void draw_rectangle(int, int, int, int, struct Colour colour);

static struct Colour set_background_colour(struct Colour);

//static union ColourBitUnison colour_bits_2_dec(struct Colour);


static void instantialise_matrix(int** image_matrix)
{
  for (int i = 0; i < __SCREEN__BOUNDS__Y__; i++)
  {
    for (int j = 0; j < __SCREEN__BOUNDS__X__; j++)
    {
      image_matrix[i][j] = 0;
    }
  }
}

static void draw_horizontal_line(int x_beg, int x_end, int y, struct Colour colour)
{
  //union ColourBitUnison colour_u = colour_bits_2_dec(colour);

  for (int x = x_beg; x <= x_end; x++)
  {
    image_matrix[y][x] = colour.colour; //colour_u._bits2decimal;
  }
}

static void draw_vertical_line(int y_beg, int y_end, int x, struct Colour colour)
{
  //union ColourBitUnison colour_u = colour_bits_2_dec(colour);

  for (int y = y_beg; y <= y_end; y++)
  {
    image_matrix[y][x] = colour.colour; //colour_u._bits2decimal;
  }
}

static void draw_rectangle(int x_beg, int x_end, int y_beg, int y_end, struct Colour colour)
{
  //union ColourBitUnison colour_u = colour_bits_2_dec(colour);

  for (int x = x_beg; x <= x_end; x++)
  {
    for (int y = y_beg; y <= y_end; y++)
    {
      image_matrix[y][x] = colour.colour;//colour_u._bits2decimal;
    }
  }
}

static struct Colour set_background_colour(struct Colour newcolour)
{

  //union ColourBitUnison newcolour_u = colour_bits_2_dec(newcolour);
  //union ColourBitUnison previous_u  = colour_bits_2_dec(previous_colour);

  //if ((__UNKNOWN_COLOUR == (enum ColourType)previous_u._bits2decimal) || (newcolour_u._bits2decimal != previous_u._bits2decimal))
  if ((WHITE == previous_colour.colour) || (newcolour.colour != previous_colour.colour))
  {
    for (int x = 0; x < __SCREEN__BOUNDS__X__; x++)
    {
      for (int y = 0; y < __SCREEN__BOUNDS__Y__; y++)
      {
        if (previous_colour.colour == image_matrix[y][x])
        //if (previous_u._bits2decimal == image_matrix[y][x])
        {
          image_matrix[y][x] = newcolour.colour; //newcolour_u._bits2decimal;
        }
        else
        {
          /* There is something drawn on that pixel. Do not overwrite it. */
        }
      }
    }

    return newcolour;
  }
  else
  {
    /* Colour of the background is the same as current colour of the background, so ignore it. */
    return previous_colour;
  }
}

/*static union ColourBitUnison colour_bits_2_dec(struct Colour colour)
{
  union ColourBitUnison colour_u;

  colour_u._colourBits.red   = colour.red;
  colour_u._colourBits.green = colour.green;
  colour_u._colourBits.blue  = colour.blue;

  return colour_u;
}*/

boolean draw2canvas(struct Commands cmd)
{
  if (__UNKNOWN_COMMAND == cmd.command_type)
  {
    /* Ignore. Not possible condition. */
    return FALSE;
  }
  else if (__SET_BACKGROUND_COLOUR == cmd.command_type)
  {
    previous_colour = set_background_colour(
      cmd.colour2draw
    );

    return TRUE;
  }
  else if (__DRAW_HORIZONTAL_LINE == cmd.command_type)
  {
    draw_horizontal_line(
      cmd.pixel_array[0].x_location,
      cmd.pixel_array[1].x_location,
      cmd.pixel_array[0].y_location,
      cmd.colour2draw
    );
    return TRUE;
  }
  else if (__DRAW_VERTICAL_LINE == cmd.command_type)
  {
    draw_vertical_line(
      cmd.pixel_array[0].x_location,
      cmd.pixel_array[0].y_location,
      cmd.pixel_array[1].y_location,
      cmd.colour2draw
    );
    return TRUE;
  }
  else if (__DRAW_RECTANGLE == cmd.command_type)
  {
    draw_rectangle(
      cmd.pixel_array[0].x_location,
      cmd.pixel_array[1].x_location,
      cmd.pixel_array[0].y_location,
      cmd.pixel_array[1].y_location,
      cmd.colour2draw
    );
    return TRUE;
  }

  return FALSE; /* Unreachable. */
}
