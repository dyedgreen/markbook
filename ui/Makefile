ROLLUP = ./node_modules/rollup/bin/rollup
NODE = node
NASM = nasm

RED = \033[0;31m
NC = \033[0m # No Color
MSG_NPM = Run 'make npm' to install build dependencies. (Requires Node.JS)
MSG_ASM = Please install the nasm assebler to generate required object files.


bundle: $(SRC)
	$(ROLLUP) --config build/rollup.config.js || echo "\n\n${RED}${MSG}${NC}\n\n"
	$(NODE) build/prepare_bundles.js
	$(NASM) -fmacho64 build/bundle.asm -o build/bundle.o || echo "\n\n${RED}${MSG_ASM}${NC}\n\n"

npm:
	npm install

clean:
	rm build/bundle.o build/bundle.js build/bundle.css || echo "Some file where already cleared."

.PHONY: bundle clean npm
