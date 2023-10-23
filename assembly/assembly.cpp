#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../commands.h"
#include "../buffer_process/buffer_process.h"
#include "../stack/stack.h"
#include "logging.h"

void write_to_buf(struct Buf_w_carriage_n_len *byte_code, const void *value, size_t size);
void aligned_fwrite(const int command_value, FILE *dest_file);
void aligned_buf_print(const int command_value, char *buf, size_t *buf_carriage);

int main()
{
	FILE *human_commands = fopen("human_commands.txt", "r");
	if(human_commands == NULL)
	{
		printf("Unable to open human_commands.txt\n");

		exit(EXIT_FAILURE);
	}

	FILE *made_txt_byte_code = fopen("made_byte_code.txt", "w");
	if(made_txt_byte_code == NULL)
	{
		printf("Unable to open made_byte_code.txt\n");

		exit(EXIT_FAILURE);
	}

	FILE *made_bin_byte_code = fopen("made_byte_code.bin", "wb");
	if(made_bin_byte_code == NULL)
	{
		printf("Unable to open made_byte_code.bin\n");

		exit(EXIT_FAILURE);
	}

	size_t file_length = get_file_length(human_commands);
	struct Buffer_w_info human_cmds_n_len = {};
	human_cmds_n_len.length = file_length;
	human_cmds_n_len.buf = (char *)calloc(human_cmds_n_len.length, sizeof(char));
	if(human_cmds_n_len.buf == NULL)
	{
		fprintf(stderr, "Not enough memory for allocation.\n");
	}


	size_t read_elems_amount =
		fread(human_cmds_n_len.buf, sizeof(char), human_cmds_n_len.length, human_commands);

	if(read_elems_amount != human_cmds_n_len.length)
	{
		if(ferror(human_commands))
		{
			perror("ERROR:");

			exit(EXIT_FAILURE);
		}
		else if(feof(human_commands))
		{
			fprintf(stderr ,"read_elems_amount != human_cmds_n_len.length because end of the file "
			"EOF was reached.\n");
		}
	}

	fclose(human_commands);

	size_t amount_of_lines = count_file_lines(human_cmds_n_len);
	printf("LINES = %lu\n", amount_of_lines);

	char * *cmd_ptrs = (char * *)calloc(amount_of_lines, sizeof(char *));
	ptr_arranger(cmd_ptrs, human_cmds_n_len);

	struct Buf_w_carriage_n_len byte_code = {};
	byte_code.length = amount_of_lines * sizeof(double) * 2 + 1;
	byte_code.buf = (char *)calloc(byte_code.length, sizeof(char));


	const size_t buffer_size = 228; //может задать размер как максимальную длну комнады в файле??
	const int immediate_const_identifier = 1;
	const double void_alignment_value = 0;
	char command_buffer[buffer_size] = {};

	double argument_value = 0;
	char reg_type = 0;
	size_t buf_carriage = 0;
	int command_code = 0;
 //strncmp с большим буфером команд и карекой
	for(size_t line_ID = 0; line_ID < amount_of_lines; line_ID++)
	{
		sscanf(cmd_ptrs[line_ID], "%s", command_buffer);

		if(!strncmp(command_buffer, "push", strlen("push"))) // я обязательно сделаю макрос =)
		{
			printf("PUSH log:\n");
			fprintf(made_txt_byte_code, "%d ", PUSH);


			if(sscanf(cmd_ptrs[line_ID] + strlen("push") ,"%lf", &argument_value) == 0)
			{
				printf("reg detected\n");
				sscanf(cmd_ptrs[line_ID] + strlen("push") ,"%s", command_buffer);

				reg_type = command_buffer[1] - 'a';
				fprintf(made_txt_byte_code, "%d\n", reg_type);
				printf("reg type: %d\n", reg_type);

				command_code = PUSH;
				write_to_buf(&byte_code, &command_code, sizeof(int));

				command_code = ZERO;
				write_to_buf(&byte_code, &command_code, sizeof(char));
				write_to_buf(&byte_code, &reg_type, sizeof(char));
				write_to_buf(&byte_code, &command_code, sizeof(char));
				write_to_buf(&byte_code, &command_code, sizeof(char));

				write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
			}
			else
			{
				command_code = PUSH;
				write_to_buf(&byte_code, &command_code, sizeof(int));

				command_code = ZERO;
				write_to_buf(&byte_code, &immediate_const_identifier, sizeof(char));
				write_to_buf(&byte_code, &command_code, sizeof(char));
				write_to_buf(&byte_code, &command_code, sizeof(char));
				write_to_buf(&byte_code, &command_code, sizeof(char));

				printf("Here is whats going into bin file:\n%lf\n", argument_value);
				write_to_buf(&byte_code, &argument_value, sizeof(double));

				fprintf(made_txt_byte_code, "%.2lf\n", argument_value);
			}
		}
		else if(!strncmp(command_buffer, "in", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", IN);

			command_code = IN;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(command_buffer, "pop", buffer_size))
		{
			printf("POP log:\n");

			fprintf(made_txt_byte_code, "%d ", POP);

			sscanf(cmd_ptrs[line_ID] + strlen("pop") ,"%s", command_buffer);

			reg_type = command_buffer[1] - 'a';
			fprintf(made_txt_byte_code, "%d\n", reg_type);
			printf("reg type: %d\n", reg_type);

			command_code = POP;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(char));
			write_to_buf(&byte_code, &reg_type, sizeof(char));
			write_to_buf(&byte_code, &command_code, sizeof(char));
			write_to_buf(&byte_code, &command_code, sizeof(char));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(command_buffer, "div", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", DIV);

			command_code = DIV;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(command_buffer, "sub", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", SUB);

			command_code = SUB;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(command_buffer, "out", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", OUT);

			command_code = OUT;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));


			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(command_buffer, "HLT", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", HLT);

			command_code = HLT;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else
		{
			fprintf(stderr, "Unknown command\n");
		}

	}


	byte_code_buf_dump(&byte_code);

	fwrite(byte_code.buf, sizeof(char), byte_code.length, made_bin_byte_code);

	return 0;
}


void aligned_buf_print(const int command_value, char *buf, size_t *buf_carriage)
{
	snprintf(buf, sizeof(int), "%d", command_value);
	*buf_carriage += sizeof(int);

	snprintf(buf, sizeof(int), "%d", ZERO);
	*buf_carriage += sizeof(int);
}

void write_to_buf(struct Buf_w_carriage_n_len *byte_code, const void *value, size_t size)
{
	char *byte_of_value = (char *)value;

	for(size_t ID = 0; ID < size; ID++)
	{
		snprintf(byte_code->buf + byte_code->carriage, sizeof(char) + 1, "%c",
			*(byte_of_value + ID));
		byte_code->carriage += 1;
	}

}
