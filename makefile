
butterworth: butterworth.o Four2.o imageIo.o
	gcc butterworth.o Four2.o imgIo.o -o butterworth -lm

butterworth.o: butterworth.c
	gcc -c butterworth.c

imageIo.o: imgIo.c imgIo.h
	gcc -c imgIo.c

Four2.o: Four2.c Four2.h
	gcc -c Four2.c Four2_Private.h

clean:
	rm *.o
