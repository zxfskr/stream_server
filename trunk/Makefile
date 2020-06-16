TARGET=test_g
CPP=g++
CPPFLAGS=-g -Wall -std=c++11
INC_DIR=-I./src/core
LIB_DIR=-L.
SRC_LIB=libmyAPI.a
LIBS= -lmyAPI -lst -lpthread

RM=rm -f

SRC = ${wildcard  ./src/core/*.cpp}

OBJS = ${patsubst %.cpp, %.o, ${SRC}}

${TARGET}:${OBJS} ${SRC_LIB}
	${CPP} ${CPPFLAGS} src/main/srs_main_server.cpp -o $@ ${INC_DIR} ${LIB_DIR} $(LIBS)

${OBJS}:%.o:%.cpp
	${CPP} ${CPPFLAGS} ${INC_DIR} -c $< -o $@

${SRC_LIB}:
	ar crv ${SRC_LIB}  ${OBJS}

.PHONY:clean
clean:
	$(RM) ${OBJS} ${TARGET} ${SRC_LIB}