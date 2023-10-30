### Build:

1. ``bash ./autogen.sh``
2. ``./configure``
3. ``make``

### Doc:

[PDF](https://github.com/bajzc/Matrix_calculator/blob/dev/gnu-main/doc/MatCal.pdf) (in development)

### RUN:

``./src/CLI``

### test:

``./src/CLI -f test/100x100``

### grammar

![grammar](./doc/grammar.png)

### TODO:
- [ ] Optimize the process from exp -> matrix
- [ ] More functions:exp ln sin cos tan (diff integral)?
- [x] A new Turing-complete language
- [x] Link with GCC JIT
