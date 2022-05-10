#ifndef ENCODE_H
#define ENCODE_H
#include <stdio.h>
#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8) 
#define MAX_FILE_SUFIX 4


typedef struct encodeInfo
{
    // src releted info storing
    char *src_image_fname; // name of src image / argv[2]
    FILE *fptr_src_image;  // file pointer for src image
    uint image_capacity;    // size of image
    int bits_per_pixel;    // since it is rgb it is 3*8
    char image_data[MAX_IMAGE_BUF_SIZE]; // image_data[8]

    //secret file info
    char *secret_fname; // secret file name/ argv[3]
    FILE *fptr_secret;  // f pointer for secret file
    char extn_secret_file[MAX_FILE_SUFIX];  // file type of secret file
    char secret_data[MAX_SECRET_BUF_SIZE];  // each byte from the secret file
    int secret_size;   // size of secret file

    //steg filr info
    char *steg_image_name;     // output image name
    FILE *fptr_steg_image;     // pointer for o/p image
}encodeInfo;


#endif



