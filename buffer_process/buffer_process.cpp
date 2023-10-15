#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "buffer_process.h"
//fstat //no stat.h
size_t get_file_length(FILE *fp)
{
    size_t length = 0;

    fseek(fp, 0L, SEEK_END);
    length = (size_t)ftell(fp);
    rewind(fp);

    return length;
}

size_t count_file_lines(struct Buffer_w_info buf_n_len)
{
	size_t NLINES = 0;
	for (size_t buf_ID = 0; buf_ID < buf_n_len.length; buf_ID++)
    {
        if(buf_n_len.buf[buf_ID] == '\n')
        {
            NLINES++;
        }
    }

	return NLINES;
}

void end_of_the_string_arranger(struct Buffer_w_info buf_n_len)
{
    for (size_t buf_ID = 0; buf_ID < buf_n_len.length; buf_ID++)
    {
        if(buf_n_len.buf[buf_ID] == '\n')
        {
            buf_n_len.buf[buf_ID] = '\0';
        }
    }
}
