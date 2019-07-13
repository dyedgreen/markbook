"use strict";

// Generate header files from bundle
const fs = require("fs");


const target = __dirname.concat("/bundle.");
const target_js = target.concat("js");
const target_css = target.concat("css");

// Load JS bundle
let js_bundle = fs.readFileSync(__dirname.concat("/bundle.js"), {encoding:"utf8"});

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
css_bundle = css_bundle.replace(/^( |\n)+/, "");

// Append NULL characters
js_bundle = js_bundle.concat("\0");
css_bundle = css_bundle.concat("\0");

// Write resulting files
fs.writeFileSync(target_js, js_bundle, {encoding:"utf8"});
fs.writeFileSync(target_css, css_bundle, {encoding:"utf8"});
