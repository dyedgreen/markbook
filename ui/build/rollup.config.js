import babel from 'rollup-plugin-babel';
import resolve from 'rollup-plugin-node-resolve';
import {uglify} from "rollup-plugin-uglify";

export default {
  input: 'src/main.js',
  output: {
    file: 'build/bundle.js',
    format: 'iife'
  },
  plugins: [
    resolve(),
    babel(),
    uglify()
  ]
};
