.PHONY: c
c:
	make -C c && c/build/clox
clean:
	make -C c clean
