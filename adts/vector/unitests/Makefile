#Project Files
TARGET=vector_utest.out
OBJ=../src/vector.o vector_utest.o

#Compilation Flags
#-Wall -Werror -Wconversion
cc=gcc
#INC_PATH=../hdrs/
VEC_API_PATH=../api/
PRUTEST_PATH=../../adutest/
CFLAGS=-g -pedantic -ansi -Wconversion -Wall -Werror -I $(VEC_API_PATH) -I $(PRUTEST_PATH)
# Linker flags:
LDFLAGS=

#listTest dependencies
#First Rule: The Most Important One!

#ignore files with this names in our HDD
.PHONY:clean run gdb

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
	@echo "CLEAN !"

run: $(TARGET)
	./$(TARGET)


gdb: $(TARGET)
	./$(TARGET)
	gdb ./$(TARGET)


#Object files dependencies
vector.o: $(VEC_API_PATH)vector.h
vector_utest.o: $(VEC_API_PATH)vector.h $(PRUTEST_PATH)prutest.h 
vector_utest.out: $(VEC_API_PATH)vector.h $(PRUTEST_PATH)prutest.h 


