import resolve from 'rollup-plugin-node-resolve';

export default {
  input: 'src/main.js',
  output: {
    file: 'build/bundle.js',
    format: 'umd'
  },
  plugins: [
    resolve()
  ]
};
