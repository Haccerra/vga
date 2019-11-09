#include "../include/compiler.h"

uint8 number_of_commands = 0x00u;

static string line;
static struct Commands execute_instructions[__BUFFER__SIZE__];

static boolean whitespace_remove(string);

static RET command_process(string, uint8);
static RET argument_process(string, uint8);
static RET colour_process(string, uint8);

static RET     colour_command_validate_helper_function(string, uint8);
static string  argument_cutting_from_string_helper_function(uint8, uint8, string);
static uint8   evaluate_comma_numbers_in_command_helper_function(string);
static boolean is_arg_int_type_number(string);
static boolean check_if_coordinates_are_within_bounds(uint16, uint16);
static void    set_colour_bits(int);

static boolean file_process(FILE*);

static boolean whitespace_remove(string line)
{

  boolean blanksFound = FALSE;

  for (int i = 0; '\0' != line[i]; (FALSE == blanksFound) ? (i++) : (i = 0))
  {
    if (' ' == line[i])
    {
      blanksFound = TRUE;
      for (int j = i; '\0' != line[j]; j++)
      {
        line[j] = line[j+1];
      }
    }
    else if ('\n' == line[i])
    {
      blanksFound = FALSE;
      line[i] = line[i+1];  /**< Null character is following the End-of-Line character. */
    }
    else
    {
      blanksFound = FALSE;
    }
  }

  return RET_OK;
}

static RET command_process(string command, uint8 queue_number)
{

  uint8   colonPosition = 0x00u;
  boolean isColonFound  = FALSE;


  for (int i = 0; i < strlen(command); i++)
  {
    if (':' != command[i])
    {
      colonPosition++;
    }
    else
    {
      /* Command delimiter found. */
      isColonFound = TRUE;
      break;
    }
  }

  if (FALSE != isColonFound)
  {
    string possibleCommandIdentifier = (string)calloc(colonPosition+1, sizeof(char));

    if (NULL == possibleCommandIdentifier)
    {
      fprintf (stderr, "Err::Allocation failed.\nNot enough operating memory was found.\n");
      return RET_NOK_ALOC;
    }

    for (int i = 0; i < colonPosition; i++)
    {
      possibleCommandIdentifier[i] = command[i];
    }
    possibleCommandIdentifier[colonPosition+1] = '\0';

    if (RET_OK == strcmp(possibleCommandIdentifier, "BCKG"))
    {
      execute_instructions[queue_number].command_type = (enum CommandType)__SET_BACKGROUND_COLOUR;
      return RET_OK;
    }
    else if (RET_OK == strcmp(possibleCommandIdentifier, "LINE_H"))
    {
      execute_instructions[queue_number].command_type = (enum CommandType)__DRAW_HORIZONTAL_LINE;
      return RET_OK;
    }
    else if (RET_OK == strcmp(possibleCommandIdentifier, "LINE_V"))
    {
      execute_instructions[queue_number].command_type = (enum CommandType)__DRAW_VERTICAL_LINE;
      return RET_OK;
    }
    else if (RET_OK == strcmp(possibleCommandIdentifier, "RECT"))
    {
      execute_instructions[queue_number].command_type = (enum CommandType)__DRAW_RECTANGLE;
      return RET_OK;
    }
    else
    {
      /* Invalid command. Abort the programme. */
      execute_instructions[queue_number].command_type = (enum CommandType)__UNKNOWN_COMMAND;
      return RET_NOK;
    }
  }
  else
  {
    /* Invalid syntax. */
    return RET_NOK_SYN;
  }

}

static RET argument_process(string command, uint8 queue_number)
{

  uint8 startDelimiterIterator = 0x00u;
  uint8 endDelimiterIterator   = 0x00u;

  if ((enum CommandType)__SET_BACKGROUND_COLOUR != execute_instructions[queue_number].command_type)
  {
    for (int i = 0; i < strlen(command); i++)
    {
      startDelimiterIterator++;

      if (':' != command[i])
      {
        /* Do nothing. */
      }
      else
      {
        /* Command delimiter found. */
        break;
      }
    }
    for (int i = 0; i < strlen(command); i++)
    {
      endDelimiterIterator++;

      if (';' != command[i])
      {
        /* Do nothing. */
      }
      else
      {
        /* Command delimiter found. */
        break;
      }
    }

    if ((enum CommandType)__DRAW_HORIZONTAL_LINE == execute_instructions[queue_number].command_type)
    {

      execute_instructions[queue_number].pixel_array = (struct Pixel*)calloc(2, sizeof(struct Pixel));
      if (NULL == execute_instructions[queue_number].pixel_array)
      {
        return RET_NOK_ALOC;
      }

      string args = argument_cutting_from_string_helper_function(startDelimiterIterator, endDelimiterIterator, command);
      if (NULL == args)
      {
        return RET_NOK_ALOC;
      }

      uint8 commaCharCount = evaluate_comma_numbers_in_command_helper_function(args);
      if (2 != commaCharCount)
      {
        /* Error. */
        return RET_NOK_SYN;
      }
      else
      {
        /* Syntax is fine. Proceed. */
      }
#ifndef __HORIZONTAL__LINE__
#define __HORIZONTAL__LINE__
#define MAX_HLINE_POINTS_BUFFER 3
#endif
      char temporary_buffer[MAX_HLINE_POINTS_BUFFER][4];
      uint8 buffer_counter = 0x00u;
      uint8 iternal_buffer_counter_2nd_dim = 0x00u;
      for (int i = 0; i < strlen(args); i++)
      {
        if (MAX_HLINE_POINTS_BUFFER-1 != buffer_counter)
        {
          if (',' == args[i])
          {
            /* End of Number. */
            temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = '\0';
            buffer_counter++;
            iternal_buffer_counter_2nd_dim = 0x00u;
          }
          else
          {
            temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = args[i];
            iternal_buffer_counter_2nd_dim++;
          }
        }
        else
        {
          /* There will no longer be any ',' character. Last argument in command. */
          temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = args[i];
          iternal_buffer_counter_2nd_dim++;
        }
      }
      temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = '\0';

      for (int i = 0; i < MAX_HLINE_POINTS_BUFFER; i++)
      {
        boolean isNumber = is_arg_int_type_number(temporary_buffer[i]);

        if (FALSE == isNumber)
        {
          /* Error. */
          return RET_NOK_SYN;
        }
        else
        {
          if (0 == i || 1 == i)
          {
            execute_instructions[queue_number].pixel_array[i].x_location = atoi (temporary_buffer[i]);
          }
          else
          {
            execute_instructions[queue_number].pixel_array[0].y_location = atoi (temporary_buffer[i]);
            execute_instructions[queue_number].pixel_array[1].y_location = atoi (temporary_buffer[i]);
          }
        }
      }

      return RET_OK;
    }
    else if ((enum CommandType)__DRAW_VERTICAL_LINE == execute_instructions[queue_number].command_type)
    {

      execute_instructions[queue_number].pixel_array = (struct Pixel*)calloc(2, sizeof(struct Pixel));
      if (NULL == execute_instructions[queue_number].pixel_array)
      {
        return RET_NOK_ALOC;
      }

      string args = argument_cutting_from_string_helper_function(startDelimiterIterator, endDelimiterIterator, command);
      if (NULL == args)
      {
        return RET_NOK_ALOC;
      }

      uint8 commaCharCount = evaluate_comma_numbers_in_command_helper_function(args);
      if (2 != commaCharCount)
      {
        /* Error. */
        return RET_NOK_SYN;
      }
      else
      {
        /* Syntax is fine. Proceed. */
      }
#ifndef __VERTICAL__LINE__
#define __VERTICAL__LINE__
#define MAX_VLINE_POINTS_BUFFER 3
#endif
      char temporary_buffer[MAX_VLINE_POINTS_BUFFER][4];
      uint8 buffer_counter = 0x00u;
      uint8 iternal_buffer_counter_2nd_dim = 0x00u;
      for (int i = 0; i < strlen(args); i++)
      {
        if (MAX_VLINE_POINTS_BUFFER-1 != buffer_counter)
        {
          if (',' == args[i])
          {
            /* End of Number. */
            temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = '\0';
            buffer_counter++;
            iternal_buffer_counter_2nd_dim = 0x00u;
          }
          else
          {
            temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = args[i];
            iternal_buffer_counter_2nd_dim++;
          }
        }
        else
        {
          /* There will no longer be any ',' character. Last argument in command. */
          temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = args[i];
          iternal_buffer_counter_2nd_dim++;
        }
      }
      temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = '\0';

      for (int i = 0; i < MAX_VLINE_POINTS_BUFFER; i++)
      {
        boolean isNumber = is_arg_int_type_number(temporary_buffer[i]);

        if (FALSE == isNumber)
        {
          /* Error. */
          return RET_NOK_SYN;
        }
        else
        {
          if (0 == i || 1 == i)
          {
            execute_instructions[queue_number].pixel_array[i].x_location = atoi (temporary_buffer[i]);
          }
          else
          {
            execute_instructions[queue_number].pixel_array[0].y_location = atoi (temporary_buffer[i]);
            execute_instructions[queue_number].pixel_array[1].y_location = atoi (temporary_buffer[i]);
          }
        }
      }

      return RET_OK;
    }
    else if ((enum CommandType)__DRAW_RECTANGLE == execute_instructions[queue_number].command_type)
    {

      execute_instructions[queue_number].pixel_array = (struct Pixel*)calloc(4, sizeof(struct Pixel));
      if (NULL == execute_instructions[queue_number].pixel_array)
      {
        return RET_NOK_ALOC;
      }

      string args = argument_cutting_from_string_helper_function(startDelimiterIterator, endDelimiterIterator, command);
      if (NULL == args)
      {
        return RET_NOK_ALOC;
      }

      uint8 commaCharCount = evaluate_comma_numbers_in_command_helper_function(args);
      if (3 != commaCharCount)
      {
        /* Error. */
        return RET_NOK_SYN;
      }
      else
      {
        /* Syntax is fine. Proceed. */
      }
#ifndef __RECTANGLE__
#define __RECTANGLE__
#define MAX_RECT_POINTS_BUFFER 4
#endif
      char temporary_buffer[MAX_RECT_POINTS_BUFFER][4];
      uint8 buffer_counter = 0x00u;
      uint8 iternal_buffer_counter_2nd_dim = 0x00u;
      for (int i = 0; i < strlen(args); i++)
      {
        if (MAX_RECT_POINTS_BUFFER-1 != buffer_counter)
        {
          if (',' == args[i])
          {
            /* End of Number. */
            temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = '\0';
            buffer_counter++;
            iternal_buffer_counter_2nd_dim = 0x00u;
          }
          else
          {
            temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = args[i];
            iternal_buffer_counter_2nd_dim++;
          }
        }
        else
        {
          /* There will no longer be any ',' character. Last argument in command. */
          temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = args[i];
          iternal_buffer_counter_2nd_dim++;
        }
      }
      temporary_buffer[buffer_counter][iternal_buffer_counter_2nd_dim] = '\0';

      for (int i = 0; i < MAX_RECT_POINTS_BUFFER; i++)
      {
        boolean isNumber = is_arg_int_type_number(temporary_buffer[i]);

        if (FALSE == isNumber)
        {
          /* Error. */
          return RET_NOK_SYN;
        }
        else
        {
          if (0 == i)
          {
            execute_instructions[queue_number].pixel_array[0].x_location = atoi (temporary_buffer[i]);
          }
          else if (1 == i)
          {
            execute_instructions[queue_number].pixel_array[1].x_location = atoi (temporary_buffer[i]);
          }
          else if (2 == i)
          {
            execute_instructions[queue_number].pixel_array[0].y_location = atoi (temporary_buffer[i]);
          }
          else if (3 == i)
          {
            execute_instructions[queue_number].pixel_array[1].y_location = atoi (temporary_buffer[i]);
          }
          else
          {
            /* Not reachable. */
          }
        }
      }

      return RET_OK;
    }
    else if ((enum CommandType)__UNKNOWN_COMMAND == execute_instructions[queue_number].command_type)
    {
      /* Abort. Not valid command. */
      execute_instructions[queue_number].pixel_array = (struct Pixel*)calloc(0, sizeof(struct Pixel));
      return RET_NOK;
    }
    else
    {
      /* Unexpected value received. */
      execute_instructions[queue_number].pixel_array = (struct Pixel*)calloc(0, sizeof(struct Pixel));
      return RET_N_EXP;
    }
  }
  else
  {
    /* Special command (SET BACKGROUND COLOUR). */
    execute_instructions[queue_number].pixel_array = (struct Pixel*)calloc(0, sizeof(struct Pixel));
    return RET_OK;
  }

  return RET_N_EXP;

}

static RET colour_process(string command, uint8 queue_number)
{

  uint8   semicolonPosition = 0x00u;
  boolean isSemicolonFound      = FALSE;

  if ((enum CommandType)__SET_BACKGROUND_COLOUR != execute_instructions[queue_number].command_type)
  {
    for (int i = 0; i < strlen(command); i++)
    {
      if (';' != command[i])
      {
        semicolonPosition++;
      }
      else
      {
        /* Command delimiter found. */
        isSemicolonFound = TRUE;
        break;
      }
    }

    if (FALSE != isSemicolonFound)
    {
      string possibleColourIdentifier = (string)calloc(strlen(command)-semicolonPosition+1, sizeof(char));

      if (NULL == possibleColourIdentifier)
      {
        fprintf (stderr, "Err::Allocation failed.\nNot enough operating memory was found.\n");
        return RET_NOK_ALOC;
      }

      for (int i = 0; i < strlen(command)-semicolonPosition; i++)
      {
        possibleColourIdentifier[i] = command[semicolonPosition+i+1];
      }
      possibleColourIdentifier[strlen(command)-semicolonPosition+1] = '\0';

      RET helper_function_return_value = colour_command_validate_helper_function(possibleColourIdentifier, queue_number);
      return helper_function_return_value;
    }
    else
    {
      /* Invalid syntax. */
      return RET_NOK_SYN;
    }
  }
  /* Special function was invoked. Special handling is involved. */
  else
  {
    uint8 start_iterator_for_special_function = (uint8)strlen("BCKG:");

    string possibleColourIdentifier = (string)calloc(strlen(command)-start_iterator_for_special_function+1, sizeof(char));

    if (NULL == possibleColourIdentifier)
    {
      fprintf (stderr, "Err::Allocation failed.\nNot enough operating memory was found.\n");
      return RET_NOK_ALOC;
    }

    for (; start_iterator_for_special_function < strlen(command); start_iterator_for_special_function++)
    {
      possibleColourIdentifier[start_iterator_for_special_function-(uint8)strlen("BCKG:")] = command[start_iterator_for_special_function];
    }
    possibleColourIdentifier[strlen(command)-strlen("BCKG:")] = '\0';

    RET helper_function_return_value = colour_command_validate_helper_function(possibleColourIdentifier, queue_number);
    return helper_function_return_value;
  }

  return RET_N_EXP;
}

static RET colour_command_validate_helper_function(string possibleColourIdentifier, uint8 queue_number)
{

  if (RET_OK == strcmp(possibleColourIdentifier, "RED"))
  {
    execute_instructions[queue_number].colour_type = (enum ColourType)__RED;
    return RET_OK;
  }
  else if (RET_OK == strcmp(possibleColourIdentifier, "BLUE"))
  {
    execute_instructions[queue_number].colour_type = (enum ColourType)__BLUE;
    return RET_OK;
  }
  else if (RET_OK == strcmp(possibleColourIdentifier, "GREEN"))
  {
    execute_instructions[queue_number].colour_type = (enum ColourType)__GREEN;
    return RET_OK;
  }
  else if (RET_OK == strcmp(possibleColourIdentifier, "BLACK"))
  {
    execute_instructions[queue_number].colour_type = (enum ColourType)__BLACK;
    return RET_OK;
  }
  else if (RET_OK == strcmp(possibleColourIdentifier, "YELLOW"))
  {
    execute_instructions[queue_number].colour_type = (enum ColourType)__YELLOW;
    return RET_OK;
  }
  else
  {
    /* Invalid colour. Abort the programme. */
    execute_instructions[queue_number].colour_type = (enum ColourType)__UNKNOWN_COLOUR;
    return RET_NOK;
  }

  return RET_N_EXP;
}

static string argument_cutting_from_string_helper_function(uint8 startDelimiterIterator, uint8 endDelimiterIterator, string command)
{
  string arguments_cut_part = (string)calloc(endDelimiterIterator-startDelimiterIterator+1, sizeof(char));

  if (NULL == arguments_cut_part)
  {
    return NULL;
  }

  for (int i = startDelimiterIterator; i < endDelimiterIterator-1; i++)
  {
    arguments_cut_part[i-startDelimiterIterator] = command[i];
  }
  arguments_cut_part[endDelimiterIterator] = '\0';

  return arguments_cut_part;
}

static uint8 evaluate_comma_numbers_in_command_helper_function(string args2process)
{
  uint8 commaCounter = 0x00u;

  for (int i = 0; i < strlen(args2process); i++)
  {
    if (',' == args2process[i])
    {
      commaCounter++;
    }
    else
    {
      /* Ignore. */
    }
  }

  return commaCounter;
}

static boolean is_arg_int_type_number(string arg)
{
  for (int i = 0; i < strlen(arg); i++)
  {
    if ('0' <= arg[i] && '9' >= arg[i])
    {
      /* This is digit. */
    }
    else
    {
      /* Not a digit. Raise an error. */
      return FALSE;
    }
  }
  return TRUE;
}

static boolean check_if_coordinates_are_within_bounds(uint16 x_location, uint16 y_location)
{
  boolean isLocationValid = FALSE;

  if (x_location >= 0 && x_location <= __SCREEN__BOUNDS__X__)
  {
    /* No action is necessary. This is valid location. */
    isLocationValid = TRUE;
  }
  if (y_location >= 0 && y_location <= __SCREEN__BOUNDS__Y__)
  {
    /* No action is necessary. This is valid location. */
    isLocationValid = TRUE;
  }

  if (FALSE != isLocationValid)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

  return FALSE; /**< Unreachable statement. */
}

static void set_colour_bits(int queue_number)
{
  if ((enum ColourType)__UNKNOWN_COLOUR == execute_instructions[queue_number-1].colour_type)
  {
    /* Set colour to white. Unexpected entry to this body. */
    execute_instructions[queue_number].colour2draw.red   = 31u;
    execute_instructions[queue_number].colour2draw.green = 63u;
    execute_instructions[queue_number].colour2draw.blue  = 31u;
  }
  else if ((enum ColourType)__RED == execute_instructions[queue_number].colour_type)
  {
    execute_instructions[queue_number].colour2draw.red   = 31u;
    execute_instructions[queue_number].colour2draw.green =  0u;
    execute_instructions[queue_number].colour2draw.blue  =  0u;
  }
  else if ((enum ColourType)__BLUE == execute_instructions[queue_number].colour_type)
  {
    execute_instructions[queue_number].colour2draw.red   =  0u;
    execute_instructions[queue_number].colour2draw.green =  0u;
    execute_instructions[queue_number].colour2draw.blue  = 31u;
  }
  else if ((enum ColourType)__GREEN == execute_instructions[queue_number].colour_type)
  {
    execute_instructions[queue_number].colour2draw.red   =  0u;
    execute_instructions[queue_number].colour2draw.green = 63u;
    execute_instructions[queue_number].colour2draw.blue  =  0u;
  }
  else if ((enum ColourType)__BLACK == execute_instructions[queue_number].colour_type)
  {
    execute_instructions[queue_number].colour2draw.red   =  0u;
    execute_instructions[queue_number].colour2draw.green =  0u;
    execute_instructions[queue_number].colour2draw.blue  =  0u;
  }
  else if ((enum ColourType)__YELLOW == execute_instructions[queue_number].colour_type)
  {
    execute_instructions[queue_number].colour2draw.red   = 31u;
    execute_instructions[queue_number].colour2draw.green = 63u;
    execute_instructions[queue_number].colour2draw.blue  =  0u;
  }
  else
  {
    /* Unreachable entry. */
  }
}

static boolean file_process(FILE* file2read)
{
  if (NULL != file2read)
  {
    line = (string)calloc(__BUFFER__SIZE__, sizeof(char));

    if (NULL == line)
    {
      /* Allocation failed. */
      fprintf(stderr, "Err::Allocation failed.\nNot enough operating memory was found.\n");
      return FALSE;
    }

    for (;;)
    {
      static uint8 numberOfLinesReadFromTheFile = 0x00u;
      static uint8 queue_number                 = 0x00u;
      char* isEOFreached = fgets(line, __BUFFER__SIZE__, file2read);

      (void)whitespace_remove(line);

      if (NULL != isEOFreached)
      {
        RET isCmdCorrect       = command_process(line, queue_number);
        RET isColourCorrect    = colour_process(line, queue_number);
        RET isArgFormatCorrect = argument_process(line, queue_number);

        set_colour_bits(queue_number);

        queue_number++;
        numberOfLinesReadFromTheFile++;
        number_of_commands++;

        if (RET_OK != isCmdCorrect)
        {
          if (RET_NOK == isCmdCorrect)
          {
            fprintf(stderr, "ERR::Unknown command found at the line %d.\n", numberOfLinesReadFromTheFile);
            return FALSE;
          }
          if (RET_NOK_SYN == isCmdCorrect)
          {
            fprintf(stderr, "ERR::Unrecognised syntax found at the line %d.\n", numberOfLinesReadFromTheFile);
            return FALSE;
          }
          if (RET_NOK_ALOC == isCmdCorrect)
          {
            /* Allocation failed. Abort programme. */
            return FALSE;
          }
          if (RET_N_EXP == isCmdCorrect)
          {
            fprintf(stderr, "FATAL::Something unexpected had occured.\n");
            return FALSE;
          }
        }
        else
        {
          /* Command is valid. */
        }


        if (RET_OK != isColourCorrect)
        {
          if (RET_NOK == isColourCorrect)
          {
            fprintf(stderr, "ERR::Unknown command found at the line %d.\n", numberOfLinesReadFromTheFile);
            return FALSE;
          }
          if (RET_NOK_SYN == isColourCorrect)
          {
            fprintf(stderr, "ERR::Unrecognised syntax found at the line %d.\n", numberOfLinesReadFromTheFile);
            return FALSE;
          }
          if (RET_NOK_ALOC == isColourCorrect)
          {
            /* Allocation failed. Abort programme. */
            return FALSE;
          }
          if (RET_N_EXP == isColourCorrect)
          {
            fprintf(stderr, "FATAL::Something unexpected had occured.\n");
            return FALSE;
          }
        }
        else
        {
          /* Colour is valid.*/
        }


        if (RET_OK != isArgFormatCorrect)
        {
          if (RET_NOK == isArgFormatCorrect)
          {
            fprintf(stderr, "ERR::Unknown command found at the line %d.\n", numberOfLinesReadFromTheFile);
            return FALSE;
          }
          if (RET_NOK_SYN == isArgFormatCorrect)
          {
            fprintf(stderr, "ERR::Unrecognised syntax found at the line %d.\n", numberOfLinesReadFromTheFile);
            return FALSE;
          }
          if (RET_NOK_ALOC == isArgFormatCorrect)
          {
            /* Allocation failed. Abort programme. */
            return FALSE;
          }
          if (RET_N_EXP == isArgFormatCorrect)
          {
            fprintf(stderr, "FATAL::Something unexpected had occured.\n");
            return FALSE;
          }
        }
        else
        {
          /* Specified argument format is valid. Necessary check is whether coordinates are out of bounds. */
          if ((enum CommandType)__DRAW_HORIZONTAL_LINE == execute_instructions[queue_number-1].command_type)
          {
            for (int i = 0; i < 2; i++)
            {
              boolean areCoordinatesWithinBounds = check_if_coordinates_are_within_bounds(
                  execute_instructions[queue_number-1].pixel_array[i].x_location,
                  execute_instructions[queue_number-1].pixel_array[i].y_location
              );
              if (FALSE != areCoordinatesWithinBounds)
              {
                /* Do not do anything. */
              }
              else
              {
                /* Out of bounds. */
                fprintf (stderr, "Err::Coordinates are out of bounds. Line %d.", numberOfLinesReadFromTheFile);
                return FALSE;
              }
            }

          }
          if ((enum CommandType)__DRAW_VERTICAL_LINE == execute_instructions[queue_number-1].command_type)
          {
            for (int i = 0; i < 2; i++)
            {
              boolean areCoordinatesWithinBounds = check_if_coordinates_are_within_bounds(
                  execute_instructions[queue_number-1].pixel_array[i].x_location,
                  execute_instructions[queue_number-1].pixel_array[i].y_location
              );
              if (FALSE != areCoordinatesWithinBounds)
              {
                /* Do not do anything. */
              }
              else
              {
                /* Out of bounds. */
                fprintf (stderr, "Err::Coordinates are out of bounds. Line %d.", numberOfLinesReadFromTheFile);
                return FALSE;
              }
            }
          }
          if ((enum CommandType)__DRAW_RECTANGLE == execute_instructions[queue_number-1].command_type)
          {
            for (int i = 0; i < 4; i++)
            {
              boolean areCoordinatesWithinBounds = check_if_coordinates_are_within_bounds(
                  execute_instructions[queue_number-1].pixel_array[i].x_location,
                  execute_instructions[queue_number-1].pixel_array[i].y_location
              );
              if (FALSE != areCoordinatesWithinBounds)
              {
                /* Do not do anything. */
              }
              else
              {
                /* Out of bounds. */
                fprintf (stderr, "Err::Coordinates are out of bounds. Line %d.", numberOfLinesReadFromTheFile);
                return FALSE;
              }
            }
          }
          else
          {
            /* Not applicable condition. */
          }
        }
      }
      else
      {
        /* All command from the file have been processed. */
        break;
      }
    }
  }
  else
  {
    fprintf(stderr, "No file was found at the \"%s\" specified location!\n", "var");
    return FALSE;
  }

  return TRUE;
}

struct Commands* parse(FILE* file2read)
{
  boolean isFileProcessedWithoutAnyMistake = file_process(file2read);

  if (FALSE != isFileProcessedWithoutAnyMistake)
  {
    return &execute_instructions[0];
  }
  else
  {
    return NULL;
  }

}
