package main

import "fmt"

var stream = make(chan int, 10)

const n = 4

func produce() {
	for i := 0; ; i++ {
		fmt.Println("produce", i)
		stream <- i
	}
}

func consume() {
	for {
		x := <-stream
		fmt.Println("consume", x)
	}
}

func main() {
	for i := 0; i < n; i++ {
		go produce()
	}
	consume()
}
