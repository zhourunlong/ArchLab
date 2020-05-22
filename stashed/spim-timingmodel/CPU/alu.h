#ifndef ALU_H
#define ALU_H

#include "reg.h"

enum ALUOp {
    // You should append more ALUOp, and realize your different ALU operations.
    ALUOP_NULL, ALUOP_ADD, ALUOP_ADDU, ALUOP_AND, ALUOP_SLL, ALUOP_SRL, ALUOP_EQ, ALUOP_NEQ, ALUOP_LUI, ALUOP_NOR, ALUOP_OR, ALUOP_LESS, ALUOP_LESSU, ALUOP_SUB, ALUOP_SUBU, ALUOP_GEZ, ALUOP_SRA
};

class ALU
{
public:
    reg_word Execute(reg_word a, reg_word b, int ALUOp) {
        switch (ALUOp) {
            case ALUOP_ADD: // !!! overflow exception !!!
                return a + b;
            case ALUOP_ADDU:
                return a + b;
            case ALUOP_AND:
                return a & b;
            case ALUOP_SLL:
                return b << a;
            case ALUOP_SRL:
                return (unsigned int)b >> a;
            case ALUOP_EQ:
                return a == b;
            case ALUOP_NEQ:
                return a != b;
            case ALUOP_LUI:
                return b << 16;
            case ALUOP_NOR:
                return ~(a | b);
            case ALUOP_OR:
                return a | b;
            case ALUOP_LESS:
                return a < b;
            case ALUOP_LESSU: // !!! unsigned !!!
                return a < b;
            case ALUOP_SUB: // !!! overflow exception !!!
                return a - b;
            case ALUOP_SUBU:
                return a - b;
            case ALUOP_GEZ:
                return a >= 0;
            case ALUOP_SRA:
                return ((b & 0x7fffffff) >> a) | (b & 0x80000000);
            default:
                return 0x0;
        }
    }
};

#endif // ALU_H
