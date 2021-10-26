CPP = g++

main: main.cpp btree.o chunk.o ord.o tag.o linear.h
	$(CPP) main.cpp btree.o chunk.o ord.o tag.h linear.h -o main
	
symulator: symulator.cpp btree.o chunk.o ord.o tag.o linear.h
	$(CPP) symulator.cpp btree.o chunk.o ord.o tag.h linear.h -o symulator
	
tag.o: tag.h
	$(CPP) tag.h -o tag.o
	
btree.o: btree.cpp btree.h tag.o
	$(CPP) -c btree.cpp -o btree.o
	
chunk.o: chunk.cpp btree.o ord.cpp tag.o
	$(CPP) -c chunk.cpp -o chunk.o
	
ord.o: ord.cpp btree.o chunk.cpp btree.o chunk.o
	$(CPP) -c ord.cpp -o ord.o

	
clean:
	rm -f *.o *.out *.txt
