SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=process_pool_server
CC:=gcc
$(ELF):$(OBJS)
	gcc $^ -o $@
clean:
	rm -rf $(OBJS) $(ELF)
