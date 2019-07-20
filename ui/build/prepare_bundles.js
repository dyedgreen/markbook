"use strict";

// Generate header files from bundle
const fs = require("fs");

// Parameters
const target = __dirname.concat("/bundle.");
const target_js = target.concat("js");
const target_css = target.concat("css");
const font_css = __dirname.concat("/font.css");


// Script for bundling fonts
function fontStyle(file) {
  // files are names as {name-of-font}-{style}.woff
  const name   = file.split("-").slice(0, -1).join("");
  const suffix = file.split("-").reverse()[0].replace(".woff", "").toLowerCase();
  const style  = suffix.indexOf("italic") === -1 ? "normal" : "italic";
  const weight = suffix.indexOf("bold") === -1 ? "normal" : "bold";
  const data   = fs.readFileSync(__dirname.replace("/build", "/fonts/").concat(file), {encoding:"base64"});
  return `@font-face {font-family:'${name}';src:url(data:font/woff;charset=utf-8;base64,${data}) format('woff');font-weight:normal;font-style:normal}\n`;
}

let font_css_data = "";
fs.readdirSync(__dirname.replace("/build", "/fonts")).forEach(file => {
  font_css_data += fontStyle(file);
});
fs.writeFileSync(font_css, font_css_data, {encoding:"utf8"});

// Load JS bundle
let js_bundle = fs.readFileSync(__dirname.concat("/bundle.js"), {encoding:"utf8"});

// Load JS browser-style libraries
fs.readdirSync(__dirname.replace("/build", "/lib")).forEach(file => {
  js_bundle += fs.readFileSync(__dirname.replace("/build", "/lib/").concat(file), {encoding:"utf8"});
});

// Load CSS bundle
let css_bundle = "";
fs.readdirSync(__dirname.replace("/build", "/styles")).forEach(file => {
  css_bundle += fs.readFileSync(__dirname.replace("/build", "/styles/").concat(file), {encoding:"utf8"});
});
css_bundle += fs.readFileSync(font_css, {encoding:"utf8"});

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
