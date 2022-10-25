#include "lopcodes.h"

Instruction CREATE_ABC(Instruction o, Instruction a, Instruction b, Instruction c){
	return o << POS_OP | a << POS_A | b << POS_B | c << POS_C;
}
