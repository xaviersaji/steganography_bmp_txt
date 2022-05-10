#include <stdio.h>
#include <string.h>

#include "e_encode.h"
//#include "types.h"


// open all files
void file_open(int argc, char *argv[], encodeInfo *encinfo)
{
  // open src image
  
  encinfo->fptr_src_image = fopen(encinfo->src_image_fname,"r");
  if(encinfo->fptr_src_image == NULL)
  {
    printf("erorr: source image do not exist\n");
  }
  // open secret file
  encinfo->fptr_secret = fopen(encinfo->secret_fname,"r");
  if(encinfo->fptr_secret == NULL)
  {
    printf("erorr: secret file do not exist\n");
  }
  // open output file if given
  if(argc == 5)
  {
    
    encinfo->fptr_steg_image= fopen(encinfo->steg_image_name,"w");
  }
    // open o/p if not given
  else
  {
    encinfo->fptr_steg_image = fopen("stego.bmp","w");
  }
  
  
}

void close_file(FILE *fp_image, FILE *fp_secret, FILE *fp_stego)
{
  fclose(fp_image);
  fclose(fp_secret);
  fclose(fp_stego);
}