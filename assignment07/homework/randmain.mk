#builds the program randmain using three types of linking

#First, it should use static linking to combine randmain.o and randcpuid.o into a single program executable randmain
#Second, it should use dynamic linking as usual to link the C library and any other necessary system-supplied files before its main function is called.
#Third, after main is called, it should use dynamic linking via dlsym as described above. randmain.mk should link randmain with the options '-ldl -Wl,-rpath=$PWD'.

#It should compile randlibhw.c and randlibsw.c with the -fPIC options as well as the other GCC options already used.
#And it should build shared object files randlibhw.so and randlibsw.so by linking the corresponding object modules with the -shared option, e.g., 'gcc ... -shared randlibsw.o -o randlibsw.so'.

randmain: randmain.o randcpuid.o
	$(CC) $(CFLAGS) -ldl -Wl,-rpath=$PWD randcpuid.o randmain.o -o randmain

randmain.o: randcpuid.h randmain.c
	$(CC) $(CFLAGS) -c randmain.c

randcpuid.o: randcpuid.h randcpuid.c
	$(CC) $(CFLAGS) -c randcpuid.c

randlibhw.so: randlib.h randlibhw.c
	$(CC) $(CFLAGS) -fPIC -shared randlibhw.c -o randlibhw.so

randlibsw.so: randlib.h randlibsw.c
	$(CC) $(CFLAGS) -fPIC -shared randlibsw.c -o randlibsw.so
