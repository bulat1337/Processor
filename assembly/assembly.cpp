#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../commands.h"
#include "../buffer_process/buffer_process.h"
#include "../stack/stack.h"

void aligned_fwrite(const int command_value, FILE *dest_file);
void aligned_buf_print(const int command_value, char *buf);

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

	struct Buffer_w_info human_cmds_n_len = {};
	struct Buffer_w_info byte_code_n_len = {};
	size_t file_length = get_file_length(human_commands);
	human_cmds_n_len.length = file_length;
	byte_code_n_len.length = file_length;
	human_cmds_n_len.buf = (char *)calloc(human_cmds_n_len.length, sizeof(char));
	byte_code_n_len.buf = (char *)calloc(human_cmds_n_len.length, sizeof(char));

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


	const size_t buffer_size = 228; //может задать размер как максимальную длну комнады в файле??
	char command_buffer[buffer_size] = {};

	double argument_value = 0;
	int reg_type = 0;

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

				reg_type = command_buffer[1] - 'a' + 1;
				fprintf(made_txt_byte_code, "%d ", reg_type);
				printf("reg type: %d\n", reg_type);

				snprintf(byte_code_n_len.buf, sizeof(int), "%d", PUSH);

				snprintf(byte_code_n_len.buf, sizeof(int), "%d", reg_type);

				sscanf(cmd_ptrs[line_ID] + strlen("push") + strlen(command_buffer) + 1 ,
					"%lf", &argument_value);//да простит меня бог за такое извращение....

				printf("Here is whats going into bin file:\n%lf\n", argument_value);

				snprintf(byte_code_n_len.buf, sizeof(int), "%lf", argument_value);

				fprintf(made_txt_byte_code, "%.2lf\n", argument_value);
			}
			else
			{
				aligned_buf_print(PUSH, byte_code_n_len.buf);

				printf("Here is whats going into bin file:\n%lf\n", argument_value);

				snprintf(byte_code_n_len.buf, sizeof(double), "%lf", argument_value);

				fprintf(made_txt_byte_code, "%.2lf\n", argument_value);
			}
		}
		else if(!strncmp(command_buffer, "div", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", DIV);
			aligned_buf_print(DIV, byte_code_n_len.buf);
		}
		else if(!strncmp(command_buffer, "sub", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", SUB);
			aligned_buf_print(SUB, byte_code_n_len.buf);
		}
		else if(!strncmp(command_buffer, "out", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", OUT);
			aligned_buf_print(OUT, byte_code_n_len.buf);
		}
		else if(!strncmp(command_buffer, "HLT", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", HLT);
			aligned_buf_print(HLT, byte_code_n_len.buf);
		}
		else
		{
			fprintf(stderr, "Unknown command\n");
		}

	}

	fwrite(byte_code_n_len.buf, sizeof(char), byte_code_n_len.length, made_bin_byte_code);

	return 0;
}


void aligned_buf_print(const int command_value, char *buf)
{
	snprintf(buf, sizeof(int), "%d", command_value);

	snprintf(buf, sizeof(int), "%d", ZERO);
}
