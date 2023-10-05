build:
	gcc -std=c99 -Werror quadtree.c qt_f1.c qt_f2.c -o quadtree
run:
	./quadtree
clean:
	rm quadtree
