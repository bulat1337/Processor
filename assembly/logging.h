#ifndef LOGGING_H
#define LOGGING_H

struct Buf_w_carriage_n_len //я хочу это и в основном файле и тут что делать
{
	char *buf;
	size_t carriage;
	size_t length;
};

void byte_code_buf_dump(const struct Buf_w_carriage_n_len *byte_code);



#endif
