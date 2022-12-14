# Secured stack
This is my realization of secured stack on C language (it has basic push(), pop(), resize() and shrinkToFir() functions).

I added some protections to it:
- Canary protection 
- Hash protection (GNU-Hash algorithm)

To run my program (for linux):
> git clone https://github.com/ThreadJava800/Secured-Stack

> cd Secured-Stack

> cmake ../Secured-stack/ && cmake --build . && ./Stack

BTW, my BASED mentor is [@demqa](https://github.com/demqa)