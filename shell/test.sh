#!/bin/bash
make
echo "Start testing"
for i in {1..4}
do
	echo "Runing test $i"
	./shell.out < ./test/part1/"$i"_in.txt > outp.txt
	diff outp.txt ./test/part1/"$i"_out.txt
	[ $? -eq 0 ] && echo "OK" || echo "Error"
done
rm -f outp.txt
