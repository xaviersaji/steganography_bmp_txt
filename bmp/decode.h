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