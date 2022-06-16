CC = g++
CFLAGS = -Wall -ansi -pedantic -g -std=c++11
SAN = -fsanitize=address
avm: src/main/avm.o src/conversions/conversions.o src/exec/assignexec.o src/exec/functionexec.o src/exec/jumpexec.o src/exec/operationexec.o src/libfunctions/libfunctions.o src/exec/tableexec.o
	@$(CC) $(CFLAGS)  $^ -o $@
%.o: %.cpp
	@$(CC) $(CFLAGS) -o $@ -c $<


clean:
	@rm -f src/avm
	@rm -f src/main/*.o src/exec/*.o
	@rm -f src/conversions/*.o
	@rm -f src/libfunctions/*o
