/**
    Define args:
    1. Symbol in input.txt
    2. Name for enums etc
    3. Differentiation action
    4. Printing method
        0 (like x+3))
        1 (like sin(x))
*/

OPERATOR(+, PLUS, PLUS(DL, DR), 0) //1
OPERATOR(-, MINUS, MINUS(DL, DR), 0)
OPERATOR(*, MULT, PLUS(MULT(DL, CR), MULT(CL, DR)), -1) //3
OPERATOR(/, DIV, DIV(MINUS(MULT(DL, CR), MULT(CL, DR)), POW(CR, NUM(2))), -1)
OPERATOR(sin, SIN, Slozhna(node, DerivateSIN), 1) //5
OPERATOR(cos, COS, Slozhna(node, DerivateCOS), 1)
OPERATOR(^, POW, DerivateChosePOW(node), -1)
OPERATOR(ln, LN, Slozhna(node, DerivateLN), 1)
OPERATOR(exp, EXP, Slozhna(node, DerivateEXP), -1) //9
OPERATOR(tan, TAN, Slozhna(node, DerivateTAN), 1)
