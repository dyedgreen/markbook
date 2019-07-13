BABEL = ./node_modules/@babel/cli/bin/babel.js
ROLLUP = ./node_modules/rollup/bin/rollup
NODE = node

RED = \033[0;31m
NC = \033[0m # No Color
MSG = Run 'make npm' to install build dependencies.


bundle:
	$(ROLLUP) --config build/rollup.config.js || echo "\n\n${RED}${MSG}${NC}\n\n"
	$(BABEL) build/bundle.js -o build/bundle.js
	$(NODE) build/gen_header.js

npm:
	npm install

.PHONY: bundle npm
