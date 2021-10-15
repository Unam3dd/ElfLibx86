#include "elf_hdr.h"
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////
//
//          READER
//          reader.c
//
/////////////////////////////////////////

size_t read_file(const char *filename, elf_data_t *data)
{
    FILE *fp = fopen(filename, "rb");

    if (!fp)
        return (0);
    
    fseek(fp, 0L, SEEK_END);
    data->file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    data->data = (char *) malloc((sizeof(char) * data->file_size) + 1);

    if (fread(data->data, sizeof(char), data->file_size, fp) != data->file_size)
        return (0);
    
    data->data[data->file_size] = 0;

    return (data->file_size);
}

void free_file(elf_data_t *data)
{
    if (data->data)
        free(data->data);
    
    data->data = NULL;
    data->file_size = 0;
}