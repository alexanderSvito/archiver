APP := svt
SRCS := main.cpp
all: $(SRCS)
	gcc -Wall -std=c99 --pedantic -g -o $(APP) $^
lint:
	splint $(SRCS)

indent:
	indent --linux-stye $(SRCS)

valgrind:
	valgrind --leak-check=yes ./$(APP)

clean:
	rm -f *.o

cleanall: clean
	rm -f $(APP)
