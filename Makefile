all:	bp

bp:	bp.c tracebp.h libtracebp.a
	gcc -L. -O2 -o bp bp.c -ltracebp

clean:
	rm bp

run:
	./bp ${SS}/trace/fppp_25M.txt.gz
	./bp ${SS}/trace/gcc_25M.txt.gz
	./bp ${SS}/trace/go_25M.txt.gz
	./bp ${SS}/trace/vpr_25M.txt.gz

