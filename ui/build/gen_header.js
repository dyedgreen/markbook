"use strict";

// Generate header files from bundle
const fs = require("fs");


// Load JS bundle
const template = fs.readFileSync(__dirname.concat("/bundle.h.txt"), {encoding:"utf8"});
const js_bundle = fs.readFileSync(__dirname.concat("/bundle.js"), {encoding:"utf8"});
const target = __dirname.concat("/bundle.h");

// Load CSS bundle
let css_bundle = "";
fs.readdirSync(__dirname.replace("/build", "/styles")).forEach(file => {
  css_bundle += fs.readFileSync(__dirname.replace("/build", "/styles/").concat(file), {encoding:"utf8"});
});

// Minify CSS
css_bundle = css_bundle.replace(/\/\*.*\*\//g, "");
css_bundle = css_bundle.replace(/;( |\n)+/g, ";");
css_bundle = css_bundle.replace(/: +/g, ":");
css_bundle = css_bundle.replace(/{( |\n)+/g, "{");
css_bundle = css_bundle.replace(/}( |\n)+/g, "}");

fs.writeFileSync(target, template.replace(
  "{js_bundle}", js_bundle.replace(
    /\"/g, "\\\"").replace(
    /\n/g, "\\n"
)).replace(
  "{css_bundle}", css_bundle.replace(
    /\"/g, "\\\"").replace(
    /\n/g, "\\n"
)));
