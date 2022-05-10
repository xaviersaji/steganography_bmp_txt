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