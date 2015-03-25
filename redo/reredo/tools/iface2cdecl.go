// 24 march 2015
package main

import (
	"fmt"
	"os"
	"bufio"
	"strings"
)

type method struct {
	Ret		string
	Name	string
	Args		[]string
}

type ifaceentry struct {
	Raw		string
	Name	string
	Method	method
}

type iface []ifaceentry

func die(format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, format, args...)
	fmt.Fprintf(os.Stderr, "\n")
	os.Exit(1)
}

func readiface(filename string) (i iface) {
	f, err := os.Open(filename)
	if err != nil {
		die("error opening %s: %v", filename, err)
	}
	defer f.Close()

	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		s := scanner.Text()
		if s == "" {
			continue
		}
		if s[0] == 'r' {		// raw line
			i = append(i, ifaceentry{
				Raw:		s[1:],
			})
			continue
		}
		p := strings.Split(strings.TrimSpace(s), " ")
		if p[0][0] == '#' {	// comment
			continue
		}
		if p[0] == "i" {		// new interface
			i = append(i, ifaceentry{
				Name:	p[1],
			})
			continue
		}
		if p[0] == "m" {		// method
			i = append(i, ifaceentry{
				Method:	method{
					Ret:		p[1],
					Name:	p[2],
					Args:	p[3:],
				},
			})
			continue
		}
		die("unknown line %q\n", s)
	}
	if err := scanner.Err(); err != nil {
		die("error reading %s: %v", filename, err)
	}

	return i
}

func main() {
	if len(os.Args) != 2 {
		die("usage: %s ifacefile", os.Args[0])
	}

	iface := readiface(os.Args[1])
	fmt.Printf("%+v\n", iface)
}
