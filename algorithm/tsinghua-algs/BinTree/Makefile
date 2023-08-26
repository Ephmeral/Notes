INCLUDES := -I../

CFLAGS := -ggdb

build: BinTree_test.cpp
	g++ $(CFLAGS) $(INCLUDES) BinTree_test.cpp -o BinTree_test
	./BinTree_test > tmp

.PHONY: build clean
clean:
	rm tmp BinTree_test