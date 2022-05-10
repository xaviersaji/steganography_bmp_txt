#include <stdio.h>
#include <string.h>

#include "common.h"
#include "type.h"


int check_op_type(char *argv[])
{
    // compare the argv[1] to "-e"
    if(strcmp(argv[1],"-e") == 0)
    {
        printf("the operation encodeing\n");
        return e_encode;
    }
    // compare the argv[1] to "-d"
    if(strcmp(argv[1],"-d") == 0)
    {
        printf("the operation decodeing\n");
        return e_decode;
    }
    else
    {
        printf("the operation unsupported");
        return e_unsuported;
    }
   

}

int read_validate(int argc, char *argv[], encodeInfo *encinfo)
{
    // validate input number
    if(argc == 3)
    {
        printf("the secret file is not provided\n");
        return en_failure;
    }
    if(argc == 2)
    {
        printf("only operation type given, please check input\n");
        return en_failure;
    }
    if(argc == 1)
    {
        printf("no input files provided\n");
        return en_failure;
    }
    if(argc < 1)
    {
        printf("the no input provided\n");
        return en_failure;
    }
    // validate the image file
    char *ift;
    ift = strrchr(argv[2],'.');                         // extract the file pointer which start '.'
    if(strcmp(ift,".bmp") == 0)                         // compare the string from ift to ".bmp"
    {
        encinfo->src_image_name = argv[2];              // if same file then assign the name of file to struct
    }
    else
    {
        printf("the image file is not supported\n");    // not .bmp file
        return en_failure;
    }
    
    // validate the secret file type
    char *sft;
    sft = strrchr(argv[3],'.');
    if(strcmp(sft,".txt") == 0)
    {
        encinfo->sec_file_name = argv[3];
    }
    else
    {
        printf("the secret file is not supported\n");
        return en_failure;
    }

    if(argc == 5)                                               // if user given out put file name
    {
        char *stft;
        stft = strrchr(argv[4],'.');
        if(strcmp(stft,".bmp") == 0)
        {
            encinfo->steg_image_name = argv[4];
        }
        else
        {
            printf("the steg file is not supported\n");
            return en_failure;
        }

    }
    else                                                        // else we give output file as stego.bmp
    {
        encinfo->steg_image_name = "stego.bmp";
    }
    printf("validated\n");
    return en_success;

}

// open all files

int file_open(encodeInfo *encinfo)
{
    // open src image
    encinfo->fp_src_image = fopen(encinfo->src_image_name,"r");
    if(encinfo->fp_src_image == NULL)
    {
        printf("Erorr: src image not opened");
        return en_failure;
    }

    // open secret file
    encinfo->fp_secret = fopen(encinfo->sec_file_name,"r");
    if(encinfo->fp_secret == NULL)
    {
        printf("Erorr: secret file not opened");
        return en_failure;
    }
    // open steg image
    encinfo->fp_steg = fopen(encinfo->steg_image_name,"w");
    if(encinfo->fp_steg == NULL)
    {
        printf("Erorr: steg image not opened");
        return en_failure;
    }
    printf("file opened\n");

    return en_success;
}

// close all files

int file_close(encodeInfo *encinfo)
{
    fclose(encinfo->fp_secret);
    fclose(encinfo->fp_src_image);
    fclose(encinfo->fp_steg);
}

// find the secret file size
long size_secret(FILE *fp)
{
    long int size;
    fseek(fp,0L,SEEK_END);
    size = ftell(fp);
    //retrns pointer to start
    fseek(fp,0L,SEEK_SET);
    printf("secret-%ld\n",size);
    return size;
}

// find the image file size
long int size_image(FILE *fp)
{
    long int size;
    fseek(fp,0L,SEEK_END);
    size = ftell(fp);
    //retrns pointer to start
    fseek(fp,0L,SEEK_SET);
    printf("image-%ld\n",size);
    return size;
}

int check_size(encodeInfo *encinfo)
{
    // image file size
    encinfo->size_image = size_image(encinfo->fp_src_image);

    //secret file size
    encinfo->size_secret = size_secret(encinfo->fp_secret);
    if(encinfo->size_image - (166 + (8*encinfo->size_secret)) > 1)
    {
        return en_success;
    }
    else
    {
        return en_failure;
    }
}



// copy header to o/p file

int copy_header(encodeInfo *encinfo)
{
    // make sure all pointer points to first character in file
    fseek(encinfo->fp_src_image, 0L, SEEK_SET);
    fseek(encinfo->fp_steg, 0L, SEEK_SET);
    char buf[55];
    
    
    // read the the 54 bytes of header
    
    fread(buf, sizeof(char), 54, encinfo->fp_src_image);
    
    // write the 54 bytes to steg
    fwrite(buf, sizeof(char), 54, encinfo->fp_steg);

    
    printf("1file header copied\n");
    return en_success;
}

// copy bit to lsb

void bit_tolsb_char(encodeInfo *encinfo, char c, int size, int p)
{
    int mask,i;
    char im;

    // assign each bit to lsb of image
    for(i=(size-1); i>=0; i--)
    {
        // make sure the cursor is at appropriate place
        fseek(encinfo->fp_src_image,(p+(size-(i+1))), SEEK_SET);
       
        //read the byte
        fread(&im, sizeof(char), 1, encinfo->fp_src_image);
        int f = ftell(encinfo->fp_src_image);
        printf("@@@%d@@@\n",f);
        printf("**%c**\n",c);
        // retain all 7 msb bytes of image data
        im = im & 0xFE;
        
        //retreive the each msb bit
        mask = ((c >> i) & 1);
        
        // assign the msb bit to charachter from image
        im = (im | mask);
        
        strcpy(encinfo->data_image,&im);

        
        // make sure the cursor is at appropriate place
        fseek(encinfo->fp_steg,(p+(size-(i+1))), SEEK_SET);

        //write the byte to steg

        fwrite(encinfo->data_image, sizeof(char), 1, encinfo->fp_steg);
        
    }
  //printf("\n");
}

// copy magic string

int copy_magic_string(encodeInfo *encinfo)
{
    //char d[] = {"#*"};
    int i;

    // for each element
    
        bit_tolsb_char(encinfo,'#',8,54);
        bit_tolsb_char(encinfo,'*',8,62);
    
    
    return en_success;
}

// copy bit of int to lsb

void bit_tolsb_int(encodeInfo *encinfo, int c, int size, int p)
{
    int mask,i;
    char im;

    // assign each bit to lsb of image
    for(i=(size-1); i>=0; i--)
    {
        // make sure the cursor is at appropriate place
        fseek(encinfo->fp_src_image,(p+(size-(i+1))), SEEK_SET);
        
        //read the byte
        fread(&im, sizeof(char), 1, encinfo->fp_src_image);
        
        // retain all 7 msb bytes of image data
        im = im & 0xFE;
        
        //retreive the each msb bit
        mask = ((c >> i) & 1);

        // assign the msb bit to charachter from image
        im = (im | mask);

        strcpy(encinfo->data_image,&im);

        
        // make sure the cursor is at appropriate place
        fseek(encinfo->fp_steg,(p+(size-(i+1))), SEEK_SET);

        //write the byte to steg

        fwrite(encinfo->data_image, sizeof(char), 1, encinfo->fp_steg);
        //printf("2\n");
    }
}

//save size of file in steg

int copy_extent_size(encodeInfo *encinfo)
{
    char c[] = ".txt";
    uint size = strlen(c);

    // assign size .txt to steg
    bit_tolsb_int(encinfo,size,32,70);

    return en_success;

}

// copy the extent file type .txt to steg

int copy_extent_filetype(encodeInfo *encinfo)
{
    char s[] = ".txt";
    int i;

    // iterate through the each chara
    for(i=0; i<4; i++)
    {
        printf("%c-\n",s[i]);
        bit_tolsb_char(encinfo,s[i],8,(102+(i*8)));
    }
    //printf("1\n");
    return en_success;
}




// copy the secret file size
int copy_secret_size(encodeInfo *encinfo)
{
    // file size copy
    bit_tolsb_int(encinfo,encinfo->size_secret,32,134);

    return en_success;
}

// copy secret file

int copy_secret_file(encodeInfo *encinfo)
{
    //printf("h");
    int i=0,size = encinfo->size_secret;
    
    char f[20];
    
    fseek(encinfo->fp_secret, 0L, SEEK_SET);
    char c;
    
    fread(&c, 1, 1, encinfo->fp_secret);
    
    //iterate through each char in secret file
    while(1)
    {
        
        if(feof(encinfo->fp_secret))
        {
            break;
        }
        encinfo->data_secret = c;
        
        bit_tolsb_char(encinfo,encinfo->data_secret,8,(166+(i*8)));
      i++;
        
        fread(&c, 1, 1, encinfo->fp_secret); 
       
    }
    return en_success;

}

int copy_file(encodeInfo *encinfo)
{
    int x = (166 + (8*(encinfo->size_secret)));
    fseek(encinfo->fp_src_image, x, SEEK_SET);

    fseek(encinfo->fp_steg, x, SEEK_SET);
    char buf;
    int d = (encinfo->size_image - x);
    while(1)
    {
        if(feof(encinfo->fp_src_image))
        {
            break;
        }

         // read the the 54 bytes of header
        fread(&buf, sizeof(char), 1, encinfo->fp_src_image);

        // write the 54 bytes to steg
        fwrite(&buf, sizeof(char), 1, encinfo->fp_steg);
    }
    
    


    printf("1file balance copied\n");
    return en_success;
}




int do_encoding(encodeInfo *encinfo) 
{
    // open file
    int status = file_open(encinfo);
    if(status == en_success)
    {
        status = check_size(encinfo);
        //printf("a\n");
        // copy header to output file
        status = copy_header(encinfo);
        if(status == en_success)
        {
            
            // copy magic string to o/p file
            status = copy_magic_string(encinfo);
            if(status == en_success)
            {
                
                status = copy_extent_size(encinfo);
                if(status == en_success)
                {
                    
                    status = copy_extent_filetype(encinfo);
                    if(status == en_success)
                    {
                        
                        status = copy_secret_size(encinfo);
                        if(status == en_success)
                        {
                            
                            status = copy_secret_file(encinfo);
                            if(status == en_success)
                            {
                                
                                status = copy_file(encinfo);
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
                                printf("secret file copy failed\n");
                                return en_failure;
                            }
                            
                        }
                        else
                        {
                            printf("secret file size copy failed\n");
                            return en_failure;
                        }
                        
                    }
                    else
                    {
                        printf("file type copy failed\n");
                        return en_failure;
                    }
                    
                }
                else
                {
                    printf("file type size copying failed\n");
                    return en_failure;
                }

            }
            else
            {
                printf("magic string copy failed\n");
                return en_failure;
            }
        }
        else
        {
            printf("header copy failed\n");
            return en_failure;
        }
    }
    else
    {
        printf("file open failed\n");
        return en_failure;
    }
    file_close(encinfo);
    return en_success;
}
    