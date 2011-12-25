CFLAGS=-O2 -s -Wall 

all:main
	tmpHacker
OBJS = objs

main:$(OBJS) $(OBJS)\main.o $(OBJS)\res.o
	gcc $(CFLAGS) -mwindows -o tmpHacker $(OBJS)\main.o $(OBJS)\res.o nvidia\nvapi.lib -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwinspool -lwinmm -lshell32 -lcomctl32 -lole32 -loleaut32 -luuid -lrpcrt4 -ladvapi32 -lwsock32 -lodbc32 
$(OBJS):
	-if not exist objs mkdir objs
$(OBJS)\main.o:main.cpp main.h
	gcc $(CFLAGS) -c -o $@ ./main.cpp 
$(OBJS)\res.o:main.rc Logo.ico
	windres -imain.rc -o $@ 



