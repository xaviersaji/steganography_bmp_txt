#include <stdio.h>
#include <string.h>

#include "common.h"
#include "types.h"
#include "encode.c"
#include "files.c"
///#include "f_encode.c"

int main(int argc, char *argv[])
{
  encodeInfo encinfo;
  int type;
  // checking the operation type
  type = check_operation_type(argv);
  // if it is encodeing do encodeing
  if(type == e_encode)
  {
    printf("operation type is encodeing\n");
    int value;
    // read and validate the input
    value = read_and_validate(argv,argc,&encinfo);
    if(value == e_success)
    {
      printf("input is validated\n");
      int status1;
      // do encode the data
      printf("how\n");
      file_open(argc, argv, &encinfo);
      status1 = do_encoding(&encinfo);
      
      if(status1 == e_success)
      {
        printf("encodeing is sucess");
      }
      else
      {
        printf("encodeing failed");
      }
    }
    else
    {
      // if the input not validated
      printf("erorr: input is not validated");
      return 1;
    }
  }
  
}

