PSPSDK = $(shell psp-config --pspsdk-path)

all: lib

lib: adler32.c compress.c crc32.c deflate.c gzio.c infback.c inffast.c inflate.c inftrees.c trees.c uncompr.c zutil.c
	psp-gcc -g -D BUILDDLL -O -c -I. adler32.c -o adler32.o
	psp-gcc -g -D BUILDDLL -O -c -I. compress.c -o compress.o
	psp-gcc -g -D BUILDDLL -O -c -I. crc32.c -o crc32.o
	psp-gcc -g -D BUILDDLL -O -c -I. deflate.c -o deflate.o
	psp-gcc -g -D BUILDDLL -O -c -I. gzio.c -o gzio.o
	psp-gcc -g -D BUILDDLL -O -c -I. infback.c -o infback.o
	psp-gcc -g -D BUILDDLL -O -c -I. inffast.c -o inffast.o
	psp-gcc -g -D BUILDDLL -O -c -I. inflate.c -o inflate.o
	psp-gcc -g -D BUILDDLL -O -c -I. inftrees.c -o inftrees.o
	psp-gcc -g -D BUILDDLL -O -c -I. trees.c -o trees.o
	psp-gcc -g -D BUILDDLL -O -c -I. uncompr.c -o uncompr.o
	psp-gcc -g -D BUILDDLL -O -c -I. zutil.c -o zutil.o
	psp-gcc -g -O -o libz.a adler32.o compress.o crc32.o deflate.o gzio.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o -L$(PSPSDK)/lib -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk -lpspgum -lpspgu -lm -lgcc -lpspaudiolib -lpspaudio -lpsppower -lc -lpspnet \
			-lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility \
			-lpspuser
	