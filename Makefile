all: inputFilter

inputFilter: inputFilter.o configParser.o
	g++ $^ -g -o $@ 

%.o: %.cpp
	g++ $< -g -c -o $@

clean:
	rm inputFilter
	rm *.o


