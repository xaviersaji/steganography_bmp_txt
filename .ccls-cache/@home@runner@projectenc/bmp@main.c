#include <stdio.h>
#include <string.h>

#include "common.h"
#include "type.h"
#include "encode.c"
#include "decode.h"


int main(int argc, char *argv[])
{
    encodeInfo encinfo;
    int status;
    int type = check_op_type(argv);
    if(type == e_encode)
    {
        status = read_validate(argc,argv,&encinfo);
        if(status == en_success)
        {
            // check enough space for coding
            
            //status = check_size(&encinfo);
            status = do_encoding(&encinfo);
            if(status == en_success)
            {
                printf("encoding is successs\n");
            } 
            else
            {
                printf("encodeing failed");
            }
           
        }
        else
        {
            printf("read and validation failed\n");
            return en_failure;
        } 
    }
    else if(type == e_decode)
    {
        status = dread_validate(argc,argv,&encinfo);
        if(status == en_success)
            {

                status = do_decoding(&encinfo);
                if(status == en_success)
                {
                    printf("decoding is successs\n");
                } 
                else
                {
                    printf("decodeing failed");
                }
            }
            else
            {
                printf("validation failed\n");
                return en_failure;
            } 

    }
    else
    {
        printf("operation type is unsupported\n");
        return en_failure;
    }


    return 0;
}