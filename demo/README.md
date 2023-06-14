# Demo

**Demo version of the matrix calculator, with some math functions:**

* addition
* subtraction
* multiplication 
* inverse
* determinant

## License

**GPL-3.0 license**

## What is this

* This is a matrix calculator with basic math functions and a friendly input syntax.
* It can do simple operations with several matrices distinguished by their names.
* It has ability to analysis user's input and convert specific format (supported by PCRE2)


## Getting start

[Download](https://github.com/bajzc/A_tiny_matrix_calculator/releases) from releases

## Using

**For demo version:**

**Define or overwrite a matrix**
Separate the elements with a `space`.
To create a matrix that has multiple rows, separate the rows with `;`.
To create an array with multiple elements in a single row, you don't need `;`.
So far, we support signed decimal number.
```cmd
>>>name=[1.0 -2 3.001 ; -4 1.5 -6]
```

**Print a matrix**
Just type the name of matrix to print, if your matrix has a bad name, it may be recognized as intrinsic functions (see below).
```cmd
>>>name
1.00000 -2.00000 3.00100
-4.00000 1.50000 -6.00000
```

**Use math functions**
If you want to add to matrix, use `add()`, the answer will be shown and stored in matrix "ans".
```cmd
>>>add(name1,name2)
```
To see more math functions, just type `help`.

**Intrinsic functions**
There are some intrinsic functions, like `help`:

```cmd
>>>help
test demo
Matrix calculator
Math Functions:
1.add(matrix_A,matrix_B)                matrix_A + matrix_B
2.sub(matrix_A,matrix_B)                matrix_A - matrix_B
3.mlp(matrix_A,matrix_B)                matrix_A * matrix_B
4.inv(matrix)                           Find the inverse of matrix
5.det(matrix)                           The determinant of matrix
Answer of 1-4 will be stored in "ans"


Intrinsic function:
"name"    Print the matrix
list      Show all initialized matrix
clean     Delete all matrix stored in memory! (ans will remain)
Quit (exit)quit
clear     Clean the screen
help      Show this information.

Have a try:
>>>A_1=[2 4 2 6; 4 9 6 15;2 6 9 18;6 15 18 40]
>>>inv(A_1)
>>>ans
```

## TODO

- [ ] Memory leak
- [ ] Support arithmetic operations
- [ ] Trig functions
- [ ] Fraction