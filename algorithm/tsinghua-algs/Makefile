INCLUDES := -I./include

CFLAGS := -ggdb

build: BinTree_test.cpp
	g++ $(CFLAGS) $(INCLUDES) BinTree_test.cpp -o BinTree_test
	./BinTree_test > tmp

run: BinTree_test
	./BinTree_test

.PHONY: build clean run
clean:
	rm tmp BinTree_test