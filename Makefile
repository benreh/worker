# worker
CC          = g++
CCFLAGS     = -c -Wall -O0 -Funrollloop  -D_GNU_SOURCE=1 -D_REENTRANT 
LDFLAGS     = -lm -O0  -D_REENTRANT
CCFLAGS    += -MD -MP -MT $(*F).o -MF dep/$(@F).d
MY_NAME     = worker
LIBS        = 
MYOBJECTS   = main.o shm.o



all: $(MYOBJECTS) $(MYHEADER) Makefile 
	$(CC) $(MYOBJECTS) $(LIBS) $(LDFLAGS)  -o $(MY_NAME)

.cpp.o: $< $(MYHEADER) Makefile
	 $(CC) $(INCLUDES) $(CCFLAGS) $<

clean:
	rm -rf $(MYOBJECTS) $(MY_NAME) dep/
install:
	install $(MY_NAME) ~/bin/
	install mkthumb.sh ~/bin/

-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)
