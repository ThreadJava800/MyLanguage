#include "../lang.h"

// G    = E nullptr
// E    = T{[['+' |  '-']]T}*
// T    = ST{['*' | '/' | '^']ST}*
// CP   = X{[compare operators]X}*
// P    = '(' E ')' | X
// X    = [numbers and words] | AS
// AS   = P{['=']P}*

Node_t* getG(Node_t* node) {
    ON_ERROR(!node, "Node was null", nullptr);

    Node_t* 
}