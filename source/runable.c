#include "../include/compiler.h"
#include "../include/draw.h"

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

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

        int vga_driver = open("/dev/vga_dma", O_RDWR|O_NDELAY);
        if (0 < vga_driver)
        {
          int* pointer2dma = (int*)mmap(0, 640*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, vga_driver, 0);
          memcpy(pointer2dma, image_matrix, 640*480*4);
          munmap(pointer2dma, 640*480*4);
          close(vga_driver);
        }
        else
        {
          /* DMA driver could not be opened. */
          fprintf (stderr, "ERROR::DMA OUTPUT DRIVER FILE COULD NOT BE OPENED!");
          return RET_NOK;
        }
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
