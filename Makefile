all: lab2.out


lab2.out: lab2.cpp
	g++ lab2.cpp -Wall libggfonts.a -lX11 -lGL -o lab2.out

clean: rm -f lab2.out




# this is how he wants it. 



