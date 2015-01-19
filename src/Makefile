CC=g++
CFLAGS=-c -pipe -O3
INCLUDE=-I /home/xh/jianguoyun/my/code/cpp/mylib
#SRC=$(wildcard *.cpp)
MODEL_OBJ=BurstyBTM.o main.o
MODEL_EXE=bbtm

all:$(MODEL_EXE) 

$(MODEL_EXE):$(MODEL_OBJ)
	$(CC) $(MODEL_OBJ) -o $@

main.o:BurstyBTM.h
BurstyBTM.o:BurstyBTM.h biterm.h doc.h

%.o:%.cpp
	$(CC) $(INCLUDE) $(CFLAGS) $< -o $@ 

clean:
	rm -rf $(MODEL_OBJ) $(MODEL_EXE)

