#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <assert.h>

#include "SPU.h"
#include "../commands.h"

void SPU_ctor(SPU *VM);

int main()
{
	struct SPU VM = {}; // process(SPU, command_list);
	SPU_ctor(&VM);

	enum Commands command = ZERO;

	int command_mode = -1;

	elem_t temp_sub_value_A = NAN;
	elem_t temp_sub_value_B = NAN;
	elem_t temp_div_value_A = NAN;
	elem_t temp_div_value_B = NAN;
	elem_t temp_jcmp_value_A = NAN;
	elem_t temp_jcmp_value_B = NAN;
	elem_t temp_user_entered_value = NAN;

	byte_code_buf_dump(&VM.byte_code);

	for(size_t ID = 0; ID < VM.byte_code.length; ID += 16)
	{
		switch(*(int *)(VM.byte_code.buf + ID)) // *(enum Commands *) ???
		{
			case PUSH:
				if(*(char *)(VM.byte_code.buf + ID + sizeof(int)) != 0)
				{
					if(*(char *)(VM.byte_code.buf + ID + sizeof(int) + sizeof(char)) == 0)
					{
						STACK_PUSH(&(VM.stk),
							*(elem_t *)(VM.byte_code.buf + ID + sizeof(double)));
					}
					else
					{
						; //coming soo
					}

				}
				else if(VM.byte_code.buf + ID + sizeof(int) + sizeof(char) != 0)
				{
					STACK_PUSH(&(VM.stk),
						VM.regs[*(char *)(VM.byte_code.buf + ID + sizeof(int) + sizeof(char))]);
					//нормальная функциональная адресация
				}
				else
				{
					printf("yo bro, tf are u tryin to push\n");
				}
				break;
			case IN:
				printf("Please enter value: ");
				scanf("%lf", &temp_user_entered_value);
				STACK_PUSH(&VM.stk, temp_user_entered_value);
				break;
			case POP:
				VM.regs[*(char *)(VM.byte_code.buf + ID + sizeof(int) + sizeof(char))] =
					STACK_POP(&(VM.stk)).deleted_element;
					printf("%d reg is %lf now\n",
						*(char *)(VM.byte_code.buf + ID + sizeof(int) + sizeof(char)),
						VM.regs[*(char *)(VM.byte_code.buf + ID + sizeof(int) + sizeof(char))]);
				break;
			case SUB:
				temp_sub_value_B = STACK_POP(&(VM.stk)).deleted_element;
				temp_sub_value_A = STACK_POP(&(VM.stk)).deleted_element;

				STACK_PUSH(&(VM.stk), temp_sub_value_A - temp_sub_value_B);
				break;

			case ADD:
				STACK_PUSH(&(VM.stk),
							STACK_POP(&(VM.stk)).deleted_element +
							STACK_POP(&(VM.stk)).deleted_element);
				break;

			case DIV:
				temp_div_value_B = STACK_POP(&(VM.stk)).deleted_element;
				temp_div_value_A = STACK_POP(&(VM.stk)).deleted_element;

				printf(" DIV:\n%lf / %lf = %lf\n",
					temp_div_value_A, temp_div_value_B, temp_div_value_A / temp_div_value_B);

				STACK_PUSH(&(VM.stk), temp_div_value_A / temp_div_value_B);
				break;

			case MUL:
				STACK_PUSH(&(VM.stk),
							STACK_POP(&(VM.stk)).deleted_element *
							STACK_POP(&(VM.stk)).deleted_element);
				break;
			case JMP:
				printf("yo there is jmp to %d\n", *(int *)(VM.byte_code.buf + ID + sizeof(int)) - 1);
				ID = (*(int *)(VM.byte_code.buf + ID + sizeof(int)) - 2) * 16;

				break;
			case JA:
				printf("yo there is ja to %d\n", *(int *)(VM.byte_code.buf + ID + sizeof(int)) - 1);

				temp_jcmp_value_B = STACK_POP(&(VM.stk)).deleted_element;
				temp_jcmp_value_A = STACK_POP(&(VM.stk)).deleted_element;

				printf("im ja'ing cause %lf > %lf\n", temp_jcmp_value_A, temp_jcmp_value_B);

				if(temp_jcmp_value_A > temp_jcmp_value_B)
				{
					ID = (*(int *)(VM.byte_code.buf + ID + sizeof(int)) - 2) * 16;
				}
				break;
			case JB:
				printf("yo there is jb to %d\n", *(int *)(VM.byte_code.buf + ID + sizeof(int)) - 1);

				temp_jcmp_value_B = STACK_POP(&(VM.stk)).deleted_element;
				temp_jcmp_value_A = STACK_POP(&(VM.stk)).deleted_element;

				printf("im jb'ing cause %lf < %lf\n", temp_jcmp_value_A, temp_jcmp_value_B);

				if(temp_jcmp_value_A < temp_jcmp_value_B)
				{
					ID = (*(int *)(VM.byte_code.buf + ID + sizeof(int)) - 2) * 16;
				}

				// printf("here is what on jb address: %d %d\n", *(int *)(VM.byte_code.buf + ID),
				// 	*(int *)(VM.byte_code.buf + ID + 16));
				// printf("here is whats next: %d %d\n", *(int *)(VM.byte_code.buf + ID + 16 * 2),
				// 	*(int *)(VM.byte_code.buf + ID + 16 * 3));

				break;
			case JAE:
				printf("yo there is jae to %d\n", *(int *)(VM.byte_code.buf + ID + sizeof(int)) - 1);

				temp_jcmp_value_B = STACK_POP(&(VM.stk)).deleted_element;
				temp_jcmp_value_A = STACK_POP(&(VM.stk)).deleted_element;

				printf("im jae'ing cause %lf >= %lf\n", temp_jcmp_value_A, temp_jcmp_value_B);

				if(temp_jcmp_value_A >= temp_jcmp_value_B)
				{
					ID = (*(int *)(VM.byte_code.buf + ID + sizeof(int)) - 2) * 16;
				}
				break;
			case JE:
				printf("yo there is je to %d\n", *(int *)(VM.byte_code.buf + ID + sizeof(int)) - 1);

				temp_jcmp_value_B = STACK_POP(&(VM.stk)).deleted_element;
				temp_jcmp_value_A = STACK_POP(&(VM.stk)).deleted_element;

				printf("im je'ing cause %lf == %lf\n", temp_jcmp_value_A, temp_jcmp_value_B);

				if(temp_jcmp_value_A == temp_jcmp_value_B)
				{
					ID = (*(int *)(VM.byte_code.buf + ID + sizeof(int)) - 2) * 16;
				}
				break;
			case JNE:
				printf("yo there is jne to %d\n", *(int *)(VM.byte_code.buf + ID + sizeof(int)) - 1);

				temp_jcmp_value_B = STACK_POP(&(VM.stk)).deleted_element;
				temp_jcmp_value_A = STACK_POP(&(VM.stk)).deleted_element;

				printf("im jne'ing cause %lf != %lf\n", temp_jcmp_value_A, temp_jcmp_value_B);

				if(temp_jcmp_value_A != temp_jcmp_value_B)
				{
					ID = (*(int *)(VM.byte_code.buf + ID + sizeof(int)) - 2) * 16;
				}
				break;
			case OUT:
				printf("ANSWER: %lf\n", STACK_POP(&(VM.stk)).deleted_element);
				break;

			case HLT:
				return 0;
				break;
			default:
				printf("The command is yet unknown\n");
				break;
		}
	}

	return 0;
}

void SPU_ctor(SPU *VM)
{
	FILE *byte_code = fopen("made_byte_code.bin", "rb");
	if(byte_code == NULL)
	{
		printf("Unable to open made_byte_code.bin\n");

		exit(EXIT_FAILURE);
	}

	struct Stack stk = {};
	STACK_CTOR(&(VM->stk), 10);

	for(size_t reg_ID = 0; reg_ID < amount_of_regs; reg_ID++)
	{
		(VM->regs)[reg_ID] = 0;
	}

	VM->byte_code.carriage = 0;
	VM->byte_code.length   = get_file_length(byte_code);
	VM->byte_code.buf      = (char *)calloc(VM->byte_code.length, sizeof(char));

	if(VM->byte_code.buf == NULL)
	{
		printf("Not enough memory for VM->byte_code.buf allocation\n");
		exit(EXIT_FAILURE);
	}

	fread(VM->byte_code.buf, sizeof(char), VM->byte_code.length, byte_code);

	fclose(byte_code);
}

