.PHONY: clean
CC = gcc
RM = rm
EXE = simple

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

$(EXE):$(OBJS)
	$(CC) -o $@ $^
%.o:%.c
	$(CC) -o $@ -c $^
clean:
	$(RM) $(EXE) $(OBJS)

# DEBUG = -g
# CFLAGS = $(DEBUG) -wall -c
# OBJS = main.o foo.o foo1.o
# $(EXE): $(OBJS)
# 	@echo "simple: main.o foo.o foo1.c"
# 	$(CC) -o $(EXE) $(OBJS)
# main.o: main.c
# 	@echo "gcc -o main.o -c main.c"
#	$(CC) -o main.o -c main.c
# foo.o: foo.c
#	@echo "gcc -o foo.o -c foo.c"
#	$(CC) -o foo.o -c foo.c
# foo1.o: foo1.c
#	@echo "gcc -o foo1.o -c foo1.c"
#	$(CC) -o foo1.o -c foo1.c
# clean:
#	$(RM) $(EXE) $(OBJS)