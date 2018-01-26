build:
	jbuilder build

run: build
	./_build/install/default/bin/reason-native-bin

test:
	jbuilder runtest

clean:
	rm -rf _build *.install

.PHONY: build release test
