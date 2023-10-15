#ifndef BUFFER_PROCESS
#define BUFFER_PROCESS

struct Buffer_w_info
{
	char *buf;
	size_t length;
};

size_t get_file_length(FILE *fp);
size_t count_file_lines(struct Buffer_w_info buf_n_len);
void end_of_the_string_arranger(struct Buffer_w_info buf_n_len);

#endif
