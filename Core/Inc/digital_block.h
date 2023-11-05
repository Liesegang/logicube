/*
 * digital_block.h
 *
 *  Created on: Nov 3, 2023
 *      Author: y-yam
 */

#ifndef INC_DIGITAL_BLOCK_H_
#define INC_DIGITAL_BLOCK_H_

const char code_false[] = "dd";
const char code_true[] = "aa";
const uint8_t code_len = 1;

// Logics
bool op_true(bool a, bool b) {
	return true;
}

bool op_false(bool a, bool b) {
	return false;
}

bool op_and(bool a, bool b) {
	return a && b;
}

bool op_or(bool a, bool b) {
	return a || b;
}

bool op_nota(bool a, bool b) {
	return !a;
}

bool op_notb(bool a, bool b) {
	return !b;
}

bool op_nand(bool a, bool b) {
	return !(a && b);
}

bool op_nor(bool a, bool b) {
	return !(a || b);
}

bool op_xor(bool a, bool b) {
	return a != b;
}

bool op_eq(bool a, bool b) {
	return a == b;
}

bool op_a(bool a, bool b) {
	return a;
}

bool op_b(bool a, bool b) {
	return b;
}

enum Mode {
	MODE_TRUE,
	MODE_FALSE,
	MODE_AND,
	MODE_OR,
	MODE_NOTA,
	MODE_NOTB,
	MODE_NAND,
	MODE_NOR,
	MODE_XOR,
	MODE_EQ,
	MODE_A,
	MODE_B,
	MODE_END
};

#endif /* INC_DIGITAL_BLOCK_H_ */
