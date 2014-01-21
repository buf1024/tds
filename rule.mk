.PRECIOUS:%.c
.SUFFIXES:
.SUFFIXES:.c .o
.c.o:
	$(CC) $(CCFLAGS) $(EXT_CCFLAGS) -c -o $*.o $<
