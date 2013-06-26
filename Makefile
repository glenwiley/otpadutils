# Makefile for One Time Pad tools
# Glen Wiley <glen.wiley@gmail.com>
#

.c.o:
	$(CC) -c -o $@ $^

all : otpadgen

otpadgen : otpadgen.c
	$(CC) $(DEBUG) -o $@ $^ 

otpadmsg : otpadmsg.c
	$(CC) $(DEBUG) -o $@ $^ 

otpadrnd : otpadrnd.c
	$(CC) $(DEBUG) -o $@ $^ 

clean :
	rm -f *.o

clobber : clean
	rm -f otpadgen otpadmsg otpadrnd

# end Makefile
