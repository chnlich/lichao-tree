.PHONY: all clean run

all:
	$(MAKE) -C benchmark

clean:
	$(MAKE) -C benchmark clean

run:
	$(MAKE) -C benchmark run
