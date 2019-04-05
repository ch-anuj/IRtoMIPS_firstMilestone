all:
	sh shell.sh < input.ir > newinput.ir
	g++ -o translate file.cpp
clean:
	rm -rf out
