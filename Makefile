PREF_STK_OBJ = ./stack_obj/
PREF_SPU_OBJ = ./SPU_obj/
PREF_ASM_OBJ = ./assembly_obj/
PREF_BUF_PROC_OBJ = ./buffer_process_obj/
PREF_BUF_PROC = ./buffer_process/
PREF_STK = ./stack/
PREF_SPU = ./SPU/
PREF_ASM = ./assembly/
CC = g++
TARGET_1 = SPU.out
TARGET_2 = assembly.out
FLAGS = -D _DEBUG -ggdb3 \
    -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat        \
    -Wmissing-declarations -Wcast-qual -Wchar-subscripts  \
    -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
    -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual \
    -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo \
    -Wstrict-overflow=2 \
    -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef \
    -Wunreachable-code -Wunused -Wvariadic-macros \
    -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs \
    -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow \
    -fno-omit-frame-pointer -Wlarger-than=8192 \
    -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

LINK_FLAGS = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

$(TARGET_1): $(PREF_SPU_OBJ)SPU.o $(PREF_STK_OBJ)stack.o $(PREF_STK_OBJ)helpers.o $(PREF_BUF_PROC_OBJ)buffer_process.o $(PREF_ASM_OBJ)logging.o
	@ $(CC) $(LINK_FLAGS) $(PREF_SPU_OBJ)SPU.o $(PREF_STK_OBJ)stack.o $(PREF_STK_OBJ)helpers.o $(PREF_BUF_PROC_OBJ)buffer_process.o $(PREF_ASM_OBJ)logging.o -o $(TARGET_1)

$(PREF_SPU_OBJ)SPU.o: $(PREF_SPU)SPU.cpp
	@ $(CC) -c $(PREF_SPU)SPU.cpp -o $(PREF_SPU_OBJ)SPU.o $(FLAGS)

$(PREF_STK_OBJ)stack.o: $(PREF_STK)stack.cpp
	@ $(CC) -c $(PREF_STK)stack.cpp -o $(PREF_STK_OBJ)stack.o $(FLAGS)

$(PREF_STK_OBJ)helpers.o: $(PREF_STK)helpers.cpp
	@ $(CC) -c $(PREF_STK)helpers.cpp -o $(PREF_STK_OBJ)helpers.o $(FLAGS)

$(PREF_BUF_PROC_OBJ)buffer_process.o: $(PREF_BUF_PROC)buffer_process.cpp
	@ $(CC) -c $(PREF_BUF_PROC)buffer_process.cpp -o $(PREF_BUF_PROC_OBJ)buffer_process.o $(FLAGS)

$(TARGET_2): $(PREF_ASM_OBJ)assembly.o $(PREF_ASM_OBJ)logging.o $(PREF_STK_OBJ)stack.o $(PREF_STK_OBJ)helpers.o $(PREF_BUF_PROC_OBJ)buffer_process.o
	@ $(CC) $(LINK_FLAGS) $(PREF_ASM_OBJ)assembly.o $(PREF_ASM_OBJ)logging.o $(PREF_STK_OBJ)stack.o $(PREF_STK_OBJ)helpers.o $(PREF_BUF_PROC_OBJ)buffer_process.o  -o $(TARGET_2)

$(PREF_ASM_OBJ)assembly.o: $(PREF_ASM)assembly.cpp
	@ $(CC) -c $(PREF_ASM)assembly.cpp -o $(PREF_ASM_OBJ)assembly.o $(FLAGS)

$(PREF_ASM_OBJ)logging.o: $(PREF_ASM)logging.cpp
	@ $(CC) -c $(PREF_ASM)logging.cpp -o $(PREF_ASM_OBJ)logging.o $(FLAGS)


clean:
	rm $(PREF_STK_OBJ)*.o $(PREF_SPU_OBJ)*.o $(TARGET_1) $(PREF_ASM_OBJ)*.o $(PREF_ASM_OBJ)*.o $(TARGET_2)
