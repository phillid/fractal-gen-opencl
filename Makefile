include config.mk


fractal-gen: fractal-gen.o cl_error.o trampoline.o slurp.o

fractal-gen.o: fractal-gen.c trampoline.h

trampoline.o: trampoline.c slurp.h cl_error.h

.PHONY: clean
clean:
	rm -f {trampoline,slurp,fractal-gen}.o fractal-gen
