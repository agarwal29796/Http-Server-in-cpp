CC = g++
CFLAGS = -Wall -std=c++1z
DEPS = MyServer.h parse.h senddata.h thread_pool.h 
OBJ = MyServer.o senddata.o parse.o thread_pool.o main.o  -lpthread
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

main: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
