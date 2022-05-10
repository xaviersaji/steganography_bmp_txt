#include <stdio.h>
#include <string.h>

#include "common.h"
#include "type.h"

int dread_validate(int argc, char *argv[], encodeInfo *encinfo);
int dfile_open(encodeInfo *encinfo);
void dbit_tolsb_int(encodeInfo *encinfo, int size, int p);
int size_srt(encodeInfo *encinfo);
void dbit_tolsb_char(encodeInfo *encinfo, int size, int j);
int dfile_close(encodeInfo *encinfo);
int extract_secret(encodeInfo *encinfo);
int do_decoding(encodeInfo *encinfo);



int dread_validate(int argc, char *argv[], encodeInfo *encinfo)
{
    
    if(argc == 2)
    {
        printf("only operation type given, please check input\n");
        return en_failure;
    }
    if(argc < 2)
    {
        printf("no input  provided\n");
        return en_failure;
    }
    
    // validate the image file
    char *ift;
    ift = strrchr(argv[2],'.');                         // extract the file pointer which start '.'
    if(strcmp(ift,".bmp") == 0)                         // compare the string from ift to ".bmp"
    {
        encinfo->steg_image_name = argv[2];              // if same file then assign the name of file to struct
    }
    else
    {
        printf("the image file is not supported\n");    // not .bmp file
        return en_failure;
    }
    //printf("%s\n",encinfo->steg_image_name);
    
    

    if(argc == 4)                                               // if user given out put file name
    {
        // validate the secret file type
        char *set;
        set = strrchr(argv[3],'.');
        if(strcmp(set,".txt") == 0)
        {
            encinfo->sec_file_name = argv[3];
        }
        else
        {
            printf("the secret file is not supported\n");
            return en_failure;
        }

    }
    else                                                        // else we give output file as stego.bmp
    {
        encinfo->sec_file_name = "default.txt";
    }
    printf("validated\n");
    return en_success;
}


// file open

int dfile_open(encodeInfo *encinfo)
{
    // open steg image
    encinfo->fp_steg = fopen(encinfo->steg_image_name,"r");
    if(encinfo->fp_steg == NULL)
    {
        printf("Erorr: src image not opened");
        return en_failure;
    }

    // open secret file
    encinfo->fp_secret = fopen(encinfo->sec_file_name,"w");
    if(encinfo->fp_secret == NULL)
    {
        printf("Erorr: secret file not opened");
        return en_failure;
    }
    
    
    printf("file opened\n");

    return en_success;
}

// close all files

int dfile_close(encodeInfo *encinfo)
{
    fclose(encinfo->fp_secret);
    
    fclose(encinfo->fp_steg);
}

void dbit_tolsb_int(encodeInfo *encinfo, int size, int p)
{
    int mask,i;
    char im;

    // make sure the cursor is at appropriate place
    fseek(encinfo->fp_steg,(p), SEEK_SET);
    // assign each bit to lsb of image
    uint x = 0;
    for(i=0; i<size; i++)
    {
        //read the byte
        fread(&im, sizeof(char), 1, encinfo->fp_steg);
        //printf("=%d=",im);
        // retrieve lsb 
        mask = im & 1;
        
        

      x = ((x << 1) | mask);
     
    }
  printf("*%d*",x);
  encinfo->size_secret = x;
}

// find the size

int size_srt(encodeInfo *encinfo)
{
    dbit_tolsb_int(encinfo, 32, 134);
    return en_success;
}

// copy  lsb to char

void dbit_tolsb_char(encodeInfo *encinfo, int size, int j)
{
    
    int mask,i,f=0;
  
    char m;
    
    fseek(encinfo->fp_steg,166+j, SEEK_SET);
    
    // assign each bit to lsb of image
    for(i=0; i<size; i++)
    {
        
        //read the byte
        fread(&m, sizeof(char), 1, encinfo->fp_steg);
        
        // retain all 7 msb bytes of image data
        mask = m & 1;
        
        printf("%d==%d ",m,mask);
        // assign the msb bit to charachter from image
        f= ((f << 1) | mask);
        
        
        
    }
  printf("\n");
    encinfo->data_secret=f;
        //-printf("%d",f);
        
        // make sure the cursor is at appropriate place
    fseek(encinfo->fp_steg,(166+j), SEEK_SET);

        //write the byte to steg

    fwrite(&encinfo->data_secret, sizeof(char), 1, encinfo->fp_secret);
  //printf("\n");
}

// secret file extraction

int extract_secret(encodeInfo *encinfo)
{
  
    int i=0;
    int s = encinfo->size_secret;
    printf("%d-\n",s);
    for(i=0; i<encinfo->size_secret; i++)
      {
        dbit_tolsb_char(encinfo,8,i);
      }
  
    return en_success;
}

// decoding fun

int do_decoding(encodeInfo *encinfo)
{
    int status = dfile_open(encinfo);
    if(status == en_success)
    {
        status = size_srt(encinfo);
        if(status == en_success)
        {
          
            status = extract_secret(encinfo);
            if(status == en_success)
            {
                return en_success;
            }
            else
            {
                return en_failure;
            }
        }
        else
        {
          printf("size fialed\n");
          return en_failure;
        }
    }
    else
    {
        printf("file open failed\n");
        return en_failure;
    }
    dfile_close(encinfo);
    return en_success;
}