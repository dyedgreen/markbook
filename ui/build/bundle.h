#pragma once

// Linked from hand-crafted object file
extern char js_bundle_start;
extern char css_bundle_start;

// JavaScript bundle
const char* js_bundle = &js_bundle_start;

// CSS bundle
const char* css_bundle = &css_bundle_start;

// HTML bundle
const char* html_bundle = "data:text/html,%3C%21DOCTYPE%20html%3E%0A%3Chtml%20lang=%22en%22%3E%0A%3Chead%3E%3Cmeta%20charset=%22utf-8%22%3E%3Cmeta%20http-equiv=%22X-UA-Compatible%22%20content=%22IE=edge%22%3E%3C%2Fhead%3E%0A%3Cbody%3E%3C%2Fbody%3E%0A%3C%2Fhtml%3E";
