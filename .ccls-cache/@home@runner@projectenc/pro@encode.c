#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "e_encode.h"
#include "common.h"


int size_file(FILE *fp_file);
operationtype check_operation_type(char *argv[]);
status do_encoding(encodeInfo *encinfo);
status read_and_validate(char *argv[],int argc, encodeInfo *encinfo);


// check the input 


// check the operation tyype

operationtype check_operation_type(char *argv[])
{
    // for encode 
    if(strcmp(argv[1],"-e") == 0)
    {
        return e_encode;
    }
    // for decode
    else if(strcmp(argv[1],"-d") == 0)
    {
        return e_decode;
    }
    // if other strings provided
    else
    {
        return e_unsupported;
    }
}


// read input and validate the input

status read_and_validate(char *argv[],int argc, encodeInfo *encinfo)
{
  // check inputs
    if(argc < 4)
    {
      printf("erorr: the secret file is not given");
      return e_failure;
    }
    if(argc < 3)
    {
      printf("erorr: the   source image is not given");
      return e_failure;
    }
    if(argc < 2)
    {
      printf("erorr: type of operation is not given");
      return e_failure;
    }
    
    // check source file type
    const char *ext1 = strrchr(argv[2],'.');
    int value = strcmp(ext1,".bmp");
    if(value == 0)
    {
      printf("v1\n");
        encinfo->src_image_fname = argv[2];

    }
    else
        return e_failure;

        // check secret file type
    const char *ext2 = strrchr(argv[3],'.');
    
    if(strcmp(ext2,".txt") == 0)
    {
       printf("v2\n");
        encinfo->secret_fname = argv[3];

    }
    else
        return e_failure;

        //check argv[4] is bmp

        
    if(argc == 5)
    {
        const char *ext3 = strrchr(argv[4],'.');
    
        if(strcmp(ext3,".bmp") == 0)
        {
            encinfo->steg_image_name = argv[4];

        }
        else
            return e_failure;
    }

  return e_success;
       
}
// find the size of fun
int size_file(FILE *fp_file)
{
    int size;
    fseek(fp_file,0L,SEEK_END);
    size = ftell(fp_file);
    fseek(fp_file,0L,SEEK_SET);
    return size;
}

// copy header

int header_copy(encodeInfo *encinfo)
{
    rewind(encinfo->fptr_src_image); // make sure the cursor in the begning
    // read the data to image_data array and write the data to steg file
    fread(encinfo->image_data,sizeof(char),54,encinfo->fptr_src_image);
    fwrite(encinfo->image_data,sizeof(char),54,encinfo->fptr_steg_image);
    return e_success;
}

// copy bits of character

int bit_chara_copy(encodeInfo *encinfo, char *ch)
{
    int i,mask;
    char data;
    for(i=7;i<=0;i--)
    {
       mask = (*ch >> i) & 1;
       fread(&data, sizeof(char), 1, encinfo->fptr_src_image);
       data = ((data & 0xFE) | mask);
       strcpy(encinfo->image_data,&data); 
       fwrite(encinfo->image_data, sizeof(char), 1, encinfo->fptr_steg_image);
    }
    return e_success;
}

// copy bits of integer

int bit_inte_copy(encodeInfo *encinfo,int size)
{
    int i,mask;
    //int size = (4*sizeof(char));
    char data;

    for(i=31;i<=0;i--)
    {
       mask = (size >> i) & 1;
       fread(&data, sizeof(char), 1, encinfo->fptr_src_image);
       data = ((data & 0xFE) | mask);
       strcpy(encinfo->image_data,&data); 
       fwrite(encinfo->image_data, sizeof(char), 1, encinfo->fptr_steg_image);
    }
    return e_success;
}

// do encoding in the image

status do_encoding(encodeInfo *encinfo)
{
    // find the image size
    int i;
    encinfo->image_capacity = size_file(encinfo->fptr_src_image);
    printf("image_cap=%d\n",encinfo->image_capacity);
    // secret file size
    encinfo->secret_size = size_file(encinfo->fptr_secret);
    printf("sec_cap=%d\n",encinfo->secret_size);
    //check if is it possible for to encode
    
    uint a = (encinfo->image_capacity);
    printf("kl");
    int b= encinfo->secret_size;
    if(a > (166+(8 * (b))))
    {
      printf("kl");
        // copy the 54 header bytes
        int status = header_copy(encinfo);
        if(status == e_success)
        {
            printf("header copy success\n");
            char ch[] = MAGIC_STRING;
            for(i=0;i<2;i++)
            {
                status = bit_chara_copy(encinfo,ch+i);
                if(status == e_success)
                {
                    printf("magic string %d copied\n",i);
                }
                else
                {
                    printf("magic string copying faild\n");
                    return e_failure;
                }
            }

            // copy extent file type size
            status = bit_inte_copy(encinfo,(4*sizeof(char)));
            if(status == e_success)
                {
                    printf("size of extent file is copied\n");
                }
                else
                {
                    printf("size of extent file copying faild\n");
                    return e_failure;
                }

                // copy the exetnt file type
                char c[] = ".txt";
                for(i=0;i<4;i++)
                {
                    status = bit_chara_copy(encinfo,(c+i));
                    if(status == e_success)
                    {
                        printf("extent file character %c copied\n",c[i]);
                    }
                    else
                    {
                        printf("extent file character %c copying failed\n",c[i] );
                        return e_failure;
                    }
                }

                // copy secret file data                
                status = bit_inte_copy(encinfo,encinfo->secret_size);
                if(status == e_success)
                {
                    printf("size of extent file is copied\n");
                }
                else
                {
                    printf("size of extent file copying faild\n");
                    return e_failure;
                }
                
                // copy image data
                
                char sec;
                fread(&sec,sizeof(char), 1, encinfo->fptr_secret);
                printf("%C",sec);
                //printf("hix");
                strcpy(encinfo->secret_data,&sec);
                
                
                printf("hiy");
                //strcpy(ch,encinfo->secret_data);
                while(feof(encinfo->fptr_secret) != '\0')
                {
                    status = bit_chara_copy(encinfo, (encinfo->secret_data));
                    
                    if(status == e_failure)
                    {
                        printf("secret filr copying failed\n" );
                        return e_failure;
                    }
                    fread(encinfo->secret_data,sizeof(char), 1, encinfo->fptr_secret);
                    printf("secret data copied");
                }

                // copy balance file in the image
                fread(encinfo->image_data, sizeof(char), 1, encinfo->fptr_src_image);
                fwrite(encinfo->image_data, sizeof(char), 1, encinfo->fptr_steg_image);
                printf("balance bytes of image is copied");


        }
        else
        {
            printf("header copying failed\n");
            return e_failure;
        }
    }
    else
    {
        printf("image capacity is not enough to encode\\n");
        return e_failure;
    }

  return e_success;
}

