package main

import (
	"errors"
	"fmt"
	"io"
	"io/fs"
	"net"
	"os"
)

func main() {
	info, _ := os.Stat(os.Args[1])
	if info.Mode().Type() != fs.ModeSocket {
		fmt.Fprintln(os.Stderr, "File is not a socket")
		os.Exit(1)
	}

	c, _ := net.Dial("unix", os.Args[1])
	defer c.Close()

	buf := make([]byte, 1024)
	for {
		n, err := c.Read(buf)
		if errors.Is(err, io.EOF) {
			return
		}
		fmt.Print(string(buf[:n]))
	}
}
