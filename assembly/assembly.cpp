#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "/Users/bulatmotygullin/Documents/Processor/Processor/commands.h"
#include "/Users/bulatmotygullin/Documents/Processor/Processor/buffer_process/buffer_process.h"
#include "/Users/bulatmotygullin/Documents/Processor/Processor/stack/stack.h"

void aligned_fwrite(const int command_value, FILE *dest_file);

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


	const size_t buffer_size = 10;

	char command_buffer[buffer_size] = {};

	double temp_value = 0;
	int reg_type = 0;

	while(fscanf(human_commands ,"%s", command_buffer) != EOF) //sscanf аналогично онегину
	{
		if(!strncmp(command_buffer, "push", buffer_size))
		{
			printf("PUSH log:\n");
			fprintf(made_txt_byte_code, "%d ", PUSH);


			if(fscanf(human_commands ,"%lf", &temp_value) == 0)
			{
				printf("reg detected\n"); //log
				fscanf(human_commands ,"%s", command_buffer); //теперь в комманд буфере лежит rax или тп

				reg_type = command_buffer[1] - 'a' + 1;
				fprintf(made_txt_byte_code, "%d ", reg_type);
				printf("reg type: %d\n", reg_type); //log

				//bin logistics:

				int ONE = PUSH; //как без такого костыля???
				fwrite(&ONE, sizeof(int), 1, made_bin_byte_code);

				fwrite(&reg_type, sizeof(int), 1, made_bin_byte_code);
				// int temp_command_value = (reg_type << 16) | PUSH;

				// printf("Here is whats going into bin file:\n%d\n", temp_command_value);//logging
				// show_bits(temp_command_value, stdout);

				// aligned_fwrite(temp_command_value, made_bin_byte_code);

				//value that we push in reg:
				fscanf(human_commands ,"%lf", &temp_value);

				printf("Here is whats going into bin file:\n%lf\n", temp_value);//logging

				fwrite(&temp_value, sizeof(double), 1, made_bin_byte_code); //copypaste
				fprintf(made_txt_byte_code, "%.2lf\n", temp_value);
			}
			else
			{
				aligned_fwrite(PUSH, made_bin_byte_code);
				
				printf("Here is whats going into bin file:\n%lf\n", temp_value);//logging

				fwrite(&temp_value, sizeof(double), 1, made_bin_byte_code); //copypaste
				fprintf(made_txt_byte_code, "%.2lf\n", temp_value);
			}
		}
		else if(!strncmp(command_buffer, "div", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", DIV);
			aligned_fwrite(DIV, made_bin_byte_code);

			//как записать div в бинарном виде в файл?
		}
		else if(!strncmp(command_buffer, "sub", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", SUB);
			aligned_fwrite(SUB, made_bin_byte_code);
		}
		else if(!strncmp(command_buffer, "out", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", OUT);
			aligned_fwrite(OUT, made_bin_byte_code);
		}
		else if(!strncmp(command_buffer, "HLT", buffer_size))
		{
			fprintf(made_txt_byte_code, "%d\n", HLT);
			aligned_fwrite(HLT, made_bin_byte_code);
		}
		else
		{
			fprintf(stderr, "Unknown command\n");
		}
	}

	return 0;
}


void aligned_fwrite(const int command_value, FILE *dest_file)
{
	fwrite(&command_value, sizeof(int), 1, dest_file);

	const int ZERO = 0;
	fwrite(&ZERO, sizeof(int), 1, dest_file); //aligning
	//как fwrite ноль без таких вот костылей?
}
