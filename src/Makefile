CC=g++
CFLAGS=-c -pipe -O3
#INCLUDE=-I /home/xh/jianguoyun/my/code/cpp/mylib
MODEL_EXE=bbtm infer

all:$(MODEL_EXE) 

bbtm:BurstyBTM.o main.o
	$(CC) $^ -o $@

infer:infer.o
	$(CC) $^ -o $@

mail.o:BurstyBTM.h
BurstyBTM.o:BurstyBTM.h biterm.h doc.h
infer.o:infer.h doc.h

%.o:%.cpp
	$(CC) $(INCLUDE) $(CFLAGS) $< -o $@ 

clean:
	rm -rf *.o $(MODEL_EXE)

