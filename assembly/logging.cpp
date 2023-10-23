#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"

void byte_code_buf_dump(const struct Buf_w_carriage_n_len *byte_code)
{
	printf("----------------------------\n");
	printf("IN BUF:\n");
	for(size_t command_ID = 0; command_ID < byte_code->length / (sizeof(double) * 2); command_ID++)
	{
		printf("COMMAND %lu:\n", command_ID + 1);

		printf("command: %d\n", *(int *)(byte_code->buf + command_ID * sizeof(double) * 2));
		printf("immediate const ID: %d\n",
			*(char *)(byte_code->buf + sizeof(int) + command_ID * sizeof(double) * 2));
		printf("register type: %d\n",
			*(char *)(byte_code->buf + sizeof(int) + sizeof(char) + command_ID * sizeof(double) * 2));
		printf("value: %lf\n",
			*(double *)(byte_code->buf + sizeof(double) + command_ID * sizeof(double) * 2));

		printf("\n");
	}

	printf("----------------------------\n");
}
