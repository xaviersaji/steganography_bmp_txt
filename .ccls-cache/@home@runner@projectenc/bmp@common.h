#ifndef COMMON_H
#define COMMON_H

#define MAGIC_STR "#*"
#define MAX_IMAGE_BUF 8

typedef unsigned int uint;
typedef struct encodeInfo
{
    // image data
    char *src_image_name;
    FILE *fp_src_image;
    uint size_image;
    char data_image[MAX_IMAGE_BUF];

    // secret data
    char *sec_file_name;
    FILE *fp_secret;
    uint size_secret;
    char data_secret;

    // stego image data
    char *steg_image_name;
    FILE *fp_steg;
}encodeInfo;

#endif