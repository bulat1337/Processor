#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../commands.h"
#include "../buffer_process/buffer_process.h"
#include "../stack/stack.h"
#include "logging.h"

#define WRITE_JMP(type)\
	fprintf(made_txt_byte_code, "%d ", type);\
	jmp_addresses[jmp_ID].txt_byte_code_offset = ftell(made_txt_byte_code);\
	printf("in byte_code_txt: %ld\n", ftell(made_txt_byte_code));\
	fprintf(made_txt_byte_code, "-1\n");\
	command_code = type;\
\
	write_to_buf(&byte_code, &command_code, sizeof(int));\
\
	jmp_addresses[jmp_ID].byte_code_carriage_value = byte_code.carriage;\
\
	write_to_buf(&byte_code, &poizon_jmp_line_ID, sizeof(int));\
\
	write_to_buf(&byte_code, &void_alignment_value, sizeof(double));\
\
	printf("yo, i detected jmp, its on %lu address\n", line_ID);\
	jmp_addresses[jmp_ID].cmd_ptrs_ID = line_ID;\
	jmp_ID++;\

struct Label
{
	char *name;
	size_t address;
};

struct Jmp_location
{
	size_t cmd_ptrs_ID;
	long txt_byte_code_offset;
	size_t byte_code_carriage_value;
};

void write_to_buf(struct Buf_w_carriage_n_len *byte_code, const void *value, size_t size);
void aligned_fwrite(const int command_value, FILE *dest_file);
void aligned_buf_print(const int command_value, char *buf, size_t *buf_carriage);

int main()
{
	FILE *opened_files[3]= {};
	size_t opened_file_ID = 0;

	FILE *human_commands = fopen("human_commands.txt", "r");
	if(human_commands == NULL)
	{
		printf("Unable to open human_commands.txt\n");

		exit(EXIT_FAILURE);
	}
	opened_files[opened_file_ID++] = human_commands;

	FILE *made_txt_byte_code = fopen("made_byte_code.txt", "w");
	if(made_txt_byte_code == NULL)
	{
		printf("Unable to open made_byte_code.txt\n");

		exit(EXIT_FAILURE);
	}
	opened_files[opened_file_ID++] = made_txt_byte_code;

	FILE *made_bin_byte_code = fopen("made_byte_code.bin", "wb");
	if(made_bin_byte_code == NULL)
	{
		printf("Unable to open made_byte_code.bin\n");

		exit(EXIT_FAILURE);
	}
	opened_files[opened_file_ID++] = made_bin_byte_code;

//human_cmds buf logic begin
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

	size_t amount_of_lines = count_file_lines(human_cmds_n_len);
	printf("LINES = %lu\n", amount_of_lines);

	char * *cmd_ptrs = (char * *)calloc(amount_of_lines, sizeof(char *));
	ptr_arranger(cmd_ptrs, human_cmds_n_len);
//human_cmds buf logic end

	struct Buf_w_carriage_n_len byte_code = {.carriage = 16};
	byte_code.length = amount_of_lines * sizeof(double) * 2 + 1;
	byte_code.buf = (char *)calloc(byte_code.length, sizeof(char));

	struct Label* labels = {};
	size_t label_ID = 0; // буду увеличивать при засечении метки
	labels = (struct Label*)calloc(amount_of_lines, sizeof(Label));
	Jmp_location *jmp_addresses = NULL;
	jmp_addresses = (Jmp_location *)calloc(amount_of_lines, sizeof(Jmp_location));
	size_t jmp_ID = 0; // буду увеличивать при засечении jmp


	const int immediate_const_identifier = 1;
	const double void_alignment_value = 0;
	double argument_value = 0;
	char reg_type = 0;
	size_t buf_carriage = 0;
	int command_code = 0;
	int poizon_jmp_line_ID = -1;


	for(size_t line_ID = 0; line_ID < amount_of_lines; line_ID++)
	{
		if(!strncmp(cmd_ptrs[line_ID], "push", strlen("push"))) // я обязательно сделаю макрос =)
		{
			printf("PUSH log:\n");
			fprintf(made_txt_byte_code, "%d ", PUSH);

			if(sscanf(cmd_ptrs[line_ID] + strlen("push") ,"%lf", &argument_value) == 0)
			{
				printf("reg detected\n");
				reg_type = *(cmd_ptrs[line_ID] + strlen("push") + 1 + 1) - 'a';
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
		else if(!strncmp(cmd_ptrs[line_ID], "add", strlen("add")))
		{
			fprintf(made_txt_byte_code, "%d\n", ADD);

			command_code = ADD;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(cmd_ptrs[line_ID], "in", strlen("in")))
		{
			// printf("yo, i detected in, its on %lu address\n", line_ID);
			fprintf(made_txt_byte_code, "%d\n", IN);

			command_code = IN;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(cmd_ptrs[line_ID], "pop", strlen("pop")))
		{
			printf("POP log:\n");

			fprintf(made_txt_byte_code, "%d ", POP);

			reg_type = *(cmd_ptrs[line_ID] + strlen("pop") + 1 + 1) - 'a';
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
		else if(!strncmp(cmd_ptrs[line_ID], "div", strlen("div")))
		{
			fprintf(made_txt_byte_code, "%d\n", DIV);

			command_code = DIV;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(cmd_ptrs[line_ID], "sub", strlen("sub")))
		{
			fprintf(made_txt_byte_code, "%d\n", SUB);

			command_code = SUB;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(cmd_ptrs[line_ID], ":main", strlen(":main")))
		{
			char *byte_of_main_address_value = (char *)&line_ID;
			size_t written_byte_of_main_address_value_ID = 0;
			for(size_t ID = 0; ID < sizeof(int); ID++)
			{
				snprintf(byte_code.buf + written_byte_of_main_address_value_ID,
					sizeof(char) + 1, "%c", *(byte_of_main_address_value + ID));
				written_byte_of_main_address_value_ID++;
			}
			printf("main is on %lu\n", line_ID);
		}
		else if(!strncmp(cmd_ptrs[line_ID], ":", strlen(":"))) //работает при первом же проходе
		{
			labels[label_ID].name = cmd_ptrs[line_ID] + strlen(":") ;
			labels[label_ID].address = line_ID + 1;
			printf("New label:\n""%s[%lu]\n", (labels[label_ID].name), labels[label_ID].address);
			printf("%lu\n", label_ID);
			label_ID++;
		}
		else if(!strncmp(cmd_ptrs[line_ID], "jmp", strlen("jmp")))
		{
			WRITE_JMP(JMP)
		}
		else if(!strncmp(cmd_ptrs[line_ID], "jb", strlen("jb")))
		{
			WRITE_JMP(JB)
		}
		else if(!strncmp(cmd_ptrs[line_ID], "ja", strlen("ja")))
		{
			WRITE_JMP(JA)
		}
		else if(!strncmp(cmd_ptrs[line_ID], "jae", strlen("jae")))
		{
			WRITE_JMP(JAE)
		}
		else if(!strncmp(cmd_ptrs[line_ID], "je", strlen("je")))
		{
			WRITE_JMP(JE)
		}
		else if(!strncmp(cmd_ptrs[line_ID], "jne", strlen("jne")))
		{
			WRITE_JMP(JNE)
		}
		else if(!strncmp(cmd_ptrs[line_ID], "call", strlen("call")))
		{
			WRITE_JMP(CALL)
		}
		else if(!strncmp(cmd_ptrs[line_ID], "ret", strlen("ret")))
		{
			fprintf(made_txt_byte_code, "%d\n", RET);

			command_code = RET;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));

			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(cmd_ptrs[line_ID], "out", strlen("out")))
		{
			fprintf(made_txt_byte_code, "%d\n", OUT);

			command_code = OUT;
			write_to_buf(&byte_code, &command_code, sizeof(int));
			command_code = ZERO;
			write_to_buf(&byte_code, &command_code, sizeof(int));


			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
		}
		else if(!strncmp(cmd_ptrs[line_ID], "HLT", strlen("HLT")))
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
			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
			write_to_buf(&byte_code, &void_alignment_value, sizeof(double));
			// супер мега суровый алайнгнинг.......
		}

	}

	// jmp address aranger
	for(size_t fixup_ID = 0; fixup_ID < jmp_ID; fixup_ID++)
	{
		// jmp type logic
		char ch = 0;
		size_t ch_ID = 0;
		while((ch = *(cmd_ptrs[jmp_addresses[fixup_ID].cmd_ptrs_ID] + ch_ID)) != ' ')
		{
			ch_ID++;
		}
		printf("jmp type: %lu\n", ch_ID);
		// jmp type logic

		printf("%s needs to be fixed\n",
			cmd_ptrs[jmp_addresses[fixup_ID].cmd_ptrs_ID] + ch_ID + 1);

		for(size_t current_label_ID = 0; current_label_ID < label_ID; current_label_ID++)
		{
			if(labels[current_label_ID].name == NULL)
			{
				printf("its NULL [%lu]\n", current_label_ID);
			}
			else if(!strncmp(cmd_ptrs[jmp_addresses[fixup_ID].cmd_ptrs_ID] + ch_ID + 1,
				labels[current_label_ID].name, strlen(labels[current_label_ID].name)))
			{
				printf("boom, found it its on %lu\n", labels[current_label_ID].address);
				fseek(made_txt_byte_code, jmp_addresses[fixup_ID].txt_byte_code_offset, SEEK_SET);
				printf("fseekin' at %ld\n", jmp_addresses[fixup_ID].txt_byte_code_offset);
				fprintf(made_txt_byte_code, "%lu\n", labels[current_label_ID].address);
			// какая то хрень с лишним \n в текстковом файле пока хз как легко и быстро исправить

				char *byte_of_jmp_value = (char *)&(labels[current_label_ID].address);
				size_t written_byte_counter = 0;
				for(size_t ID = 0; ID < sizeof(int); ID++)
				{
					snprintf(byte_code.buf +
						jmp_addresses[fixup_ID].byte_code_carriage_value + written_byte_counter,
							sizeof(char) + 1, "%c", *(byte_of_jmp_value + ID));
					written_byte_counter += 1;
				}

				break;
			}
			else
			{
				printf("Unknown label: %s\n",
					cmd_ptrs[jmp_addresses[fixup_ID].cmd_ptrs_ID] + strlen("jmp") + 1);
			}

		}

	}



	// byte_code_buf_dump(&byte_code);

	fwrite(byte_code.buf, sizeof(char), byte_code.length, made_bin_byte_code);

	for(size_t file_ID = 0; file_ID < opened_file_ID; file_ID++)
	{
		fclose(opened_files[file_ID]);
	}

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
		// printf("offset in write_to_buf: %lu\n", byte_code->carriage);
		snprintf(byte_code->buf + byte_code->carriage, sizeof(char) + 1, "%c",
			*(byte_of_value + ID));
		byte_code->carriage += 1;
	}

}
