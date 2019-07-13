ROLLUP = ./node_modules/rollup/bin/rollup
NODE = node

RED = \033[0;31m
NC = \033[0m # No Color
MSG = Run 'make npm' to install build dependencies.


bundle:
	$(ROLLUP) --config build/rollup.config.js || echo "\n\n${RED}${MSG}${NC}\n\n"
	$(NODE) build/gen_header.js
	touch ../src/ui/view.c # To recompile with new bundle

npm:
	npm install

.PHONY: bundle npm
