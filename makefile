CXXFLAGS+=-Wall -Werror -std=c++14

PROGS=server client

all: $(PROGS)

clean:
	rm -f $(PROGS)

world: clean all