#include "../include/compiler.h"
#include "../include/draw.h"

int main(int argc, stream argv)
{

  if (2 != argc)
  {
    fprintf(stderr, "Err::Improper number of arguments passed to a function.\n");
    return RET_NOK;
  }

  FILE* file2read = fopen(argv[1], "r");

  if (NULL != file2read)
  {
    struct Commands* parsedCMD = parse(file2read);

    if (NULL != parsedCMD)
    {
      for (int i = 0; i < number_of_commands; i++)
      {
        /* Draw objects. */
        draw2canvas(parsedCMD[i]);
      }
    }
    else
    {
      /* There was a defect. We already created log file. Not necessary to do anything. Abort programme. */
      return RET_NOK;
    }
  }
  else
  {
    fprintf(stderr, "Err::File could not be found on the specified location %s.", argv[1]);
    return RET_NOK;
  }


  return RET_OK;
}
