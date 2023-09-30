## AST-test

The implement by building an AST tree


### Build

`make all`

## Examples

### Fibonacci

``` C
a=0;b=1;temp=0;i=1;
while(i<10){temp=a+b;a=b;b=temp;i++;}
b;
```

`time cat fibonacci.test | ./a.out`

output:

```
b = 55.000000
./a.out  0.00s user 0.00s system 88% cpu 0.001 total
```

### Multipilcation

```C
i=1;
while(i<=9){
    j=1;
    while(j<=i){
        printf("%d x %d = %d\t",j,i,i*j);
        j++;
    }
    printf("\n");
    i++;
}
```

output:
```
1 x 1 = 1
1 x 2 = 2       2 x 2 = 4
1 x 3 = 3       2 x 3 = 6       3 x 3 = 9
1 x 4 = 4       2 x 4 = 8       3 x 4 = 12      4 x 4 = 16
1 x 5 = 5       2 x 5 = 10      3 x 5 = 15      4 x 5 = 20      5 x 5 = 25
1 x 6 = 6       2 x 6 = 12      3 x 6 = 18      4 x 6 = 24      5 x 6 = 30      6 x 6 = 36
1 x 7 = 7       2 x 7 = 14      3 x 7 = 21      4 x 7 = 28      5 x 7 = 35      6 x 7 = 42      7 x 7 = 49
1 x 8 = 8       2 x 8 = 16      3 x 8 = 24      4 x 8 = 32      5 x 8 = 40      6 x 8 = 48      7 x 8 = 56      8 x 8 = 64
1 x 9 = 9       2 x 9 = 18      3 x 9 = 27      4 x 9 = 36      5 x 9 = 45      6 x 9 = 54      7 x 9 = 63      8 x 9 = 72      9 x 9 = 81
./a.out  0.00s user 0.00s system 81% cpu 0.001 total
```