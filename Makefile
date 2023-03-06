STD=c99

CXX=gcc
CXXFLAGS=-std=${std} -Werror -Wall -Wpedantic -MMD

OBJECTS=main.o http_server.o log.o request.o
DEPENDS=${OBJECTS:.o=.d}
EXEC=server.out

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC}
