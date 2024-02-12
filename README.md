# Programming language
It is realization of my own programming language, which is compiled to [my own Assembly](https://github.com/ThreadJava800/Processor). It contains lexical analyzer, syntax analyzer (uses recursive descent) & compilation to Assembly.

# Execution
```make```

```./lang <pass to file>```


And run the 'ass.txt' using my stack processor

# Syntax
## Initialize variables

```var b = cos 3;```

## Print values

```getout b (or expression)```

```getout 2```

## Input values

```var a = gimme;```

## If-else statements

```c
if (d < 0) {
    // code here
} else {
    // code here        
} ;
```
## Call a function with a, b & c arguments

```runmf solveTwo( a b c );```

## Function declaration

```c
hh func( a b c ) {
    // code

    to_production 0 ; // return from func
} ;
```
# Conclusion
It is a turing-complete language (it is able to calculate Fibonacci numbers using this language).
