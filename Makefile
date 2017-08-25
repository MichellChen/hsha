OBJS = stress_test.o server.o http_conn.o

all : server stress_test


server : http_conn.o server.o
	g++ -o server server.o http_conn.o -lpthread

stress_test : stress_test.o
	g++ -o stress_test stress_test.o

stress_test.o : stress_test.cpp

server.o : server.cpp synchro.hpp threadpool.hpp http_conn.h

http_conn.o : http_conn.cpp http_conn.h synchro.hpp


.PHONY : clean
clean:
	rm server stress_test $(OBJS)