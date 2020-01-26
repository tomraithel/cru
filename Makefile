build:
	dune build

run: build
	./_build/install/default/bin/cru

test:
	dune runtest

clean:
	rm -rf _build *.install

.PHONY: build release test
