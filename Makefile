# Makefile for One Time Pad tools
# Glen Wiley <glen.wiley@gmail.com>
#

.c.o:
	$(CC) -c -o $@ $^

all : otpadgen otpadmsg

otpadgen : otpadgen.o otpadutils.o
	$(CC) $(DEBUG) -o $@ $^ 

otpadmsg : otpadmsg.o otpadutils.o
	$(CC) $(DEBUG) -o $@ $^ 

otpadrnd : otpadrnd.o
	$(CC) $(DEBUG) -o $@ $^ 

clean :
	rm -f *.o

clobber : clean
	rm -f otpadgen otpadmsg otpadrnd

# end Makefile
