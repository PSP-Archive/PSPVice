all: lib

lib:
	psp-gcc -g -D BUILDDLL -O -c -I. png.c -o png.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngerror.c -o pngerror.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngget.c -o pngget.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngmem.c -o pngmem.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngpread.c -o pngpread.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngread.c -o pngread.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngrio.c -o pngrio.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngrtran.c -o pngrtran.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngrutil.c -o pngrutil.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngset.c -o pngset.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngtrans.c -o pngtrans.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngwio.c -o pngwio.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngwrite.c -o pngwrite.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngwtran.c -o pngwtran.o
	psp-gcc -g -D BUILDDLL -O -c -I. pngwutil.c -o pngwutil.o
	psp-gcc -g -O -o libpng.a png.o pngerror.o pngget.o pngmem.o pngpread.o pngread.o pngrio.o pngrtran.o pngrutil.o pngset.o pngtrans.o pngwio.o pngwrite.o pngwtran.o pngwutil.o 		