bits 64

section .rodata

global _js_bundle_start
global _js_bundle_end
global _js_bundle_size

_js_bundle_start: incbin "build/bundle.js"
_js_bundle_end:
_js_bundle_size:  dd $-_js_bundle_start


global _css_bundle_start
global _css_bundle_end
global _css_bundle_size

_css_bundle_start: incbin "build/bundle.css"
_css_bundle_end:
_css_bundle_size:  dd $-_css_bundle_start
