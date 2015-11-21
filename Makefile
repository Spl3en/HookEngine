main:
	gcc -I../ -std=gnu99 -g -o test.exe HookEngine.c ../dbg/dbg.c ../BbQueue/BbQueue.c main.c
	# /d/Logiciels/Codeblocks/MinGW/bin/gcc -I../ -std=gnu99 -g -o test.exe HookEngine.c ../dbg/dbg.c ../BbQueue/BbQueue.c main.c

clean:
	rm -f *.o
	rm -f *.exe