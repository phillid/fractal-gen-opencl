include config.mk


fractal-gen: fractal-gen.o cl_error.o trampoline.o slurp.o


.PHONY: clean
clean:
	rm -f {trampoline,slurp,fractal-gen}.o fractal-gen
