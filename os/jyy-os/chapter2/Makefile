a.out: minimal.S
	gcc -S minimal.S > minimal.s
	as minimal.s -o minimal.o
	ld minimal.o

clean:
	rm a.out *.o