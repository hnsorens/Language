#include "ExecutableBuilder.h"

// section .text
//     global _start

// _start:
//     ; Convert the number to a string
//     mov eax, 12345 ; Move the number to be printed into a register
//     mov ecx, 10 ; Set divisor to 10 for decimal conversion
//     mov edi, number + len - 1 ; Point to the end of the string buffer

// convert_loop:
//     xor edx, edx ; Clear edx for division
//     div ecx ; Divide eax by 10, quotient in eax, remainder in edx
//     add dl, '0' ; Convert remainder to ASCII character
//     dec edi ; Move pointer to the left in the string buffer
//     mov [edi], dl ; Store ASCII character in string buffer
//     test eax, eax ; Check if quotient is zero
//     jnz convert_loop ; Loop until quotient is zero

//     ; Print the string
//     mov eax, 4 ; System call number for sys_write
//     mov ebx, 1 ; File descriptor (stdout)
//     mov ecx, number ; Address of the string buffer
//     mov edx, len ; Length of the string
//     int 0x80 ; Call kernel

//     ; Print a newline
//     mov eax, 4 ; System call number for sys_write
//     mov ebx, 1 ; File descriptor (stdout)
//     mov ecx, nl ; Address of the newline character
//     mov edx, 1 ; Length of the newline
//     int 0x80 ; Call kernel

//     ; Exit the program
//     mov eax, 1 ; System call number for sys_exit
//     xor ebx, ebx ; Exit code 0
//     int 0x80 ; Call kernel





// Examples
// MOV_DWORD_IMT, getMovOffsetByte(EBP), 0xFC, 0x64, 0x00, 0x00, 0x00, // mov dword ptr [ebp-4], 100

#define TESTS

// Registers
#define EAX 0x00
#define ECX 0x01
#define EDX 0x02
#define EBX 0x03
#define ESP 0x04
#define EBP 0x05
#define ESI 0x06
#define EDI 0x07

program_chunk* create_program_chunk()
{
    program_chunk* chunk = (program_chunk*)malloc(sizeof(program_chunk));   
    chunk->count = 0;
    chunk->capacity = 256;
    chunk->bytes = (uint8_t *)malloc(sizeof(uint8_t) * chunk->capacity);

    chunk->call_request_capacity = 16;
    chunk->call_request_count = 0;
    chunk->call_requests = (char**)malloc(sizeof(char*)*chunk->call_request_capacity);
    chunk->call_request_locations = (uint32_t*)malloc(sizeof(uint32_t)*chunk->call_request_count);
    return chunk;
}

void add_byte(program_chunk* chunk, uint8_t byte)
{
    if (chunk->count >= chunk->capacity)
    {
        chunk->capacity *= 2;
        chunk->bytes = (uint8_t*)realloc(chunk->bytes, sizeof(uint8_t) * chunk->capacity);
    }
    chunk->bytes[chunk->count++] = byte;
}


// 'mov' Instruction

/**
 * Move a 32-bit Register to Another 32-bit Register
*/
void mov_reg32_reg32(program_chunk* chunk, uint8_t dest_reg, uint8_t src_reg) {
    add_byte(chunk, 0x89);             // Opcode for 'mov r/m32, r32'
    add_byte(chunk, 0xC0 | (src_reg << 3) | dest_reg); // ModR/M byte
}

/**
 * Move a 16-bit Register to Another 16-bit Register
*/
void mov_reg16_reg16(program_chunk* chunk, uint8_t dest_reg, uint8_t src_reg) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0x89);             // Opcode for 'mov r/m16, r16'
    add_byte(chunk, 0xC0 | (src_reg << 3) | dest_reg); // ModR/M byte
}

/**
 * Move an 8-bit Register to Another 8-bit Register
*/
void mov_reg8_reg8(program_chunk* chunk, uint8_t dest_reg, uint8_t src_reg) {
    add_byte(chunk, 0x88);             // Opcode for 'mov r/m8, r8'
    add_byte(chunk, 0xC0 | (src_reg << 3) | dest_reg); // ModR/M byte
}

/**
 * Move an Immediate 32-bit Value to a Register
*/
void mov_reg32_imm32(program_chunk* chunk, uint8_t dest_reg, uint32_t imm) {
    add_byte(chunk, 0xB8 | dest_reg);  // Opcode for 'mov r32, imm32'
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 24) & 0xFF));
}

/**
 * Move an Immediate 16-bit Value to a Register
*/
void mov_reg16_imm16(program_chunk* chunk, uint8_t dest_reg, uint16_t imm) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0xB8 | dest_reg);  // Opcode for 'mov r16, imm16'
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

/**
 * Move an Immediate 8-bit Value to a Register
*/
void mov_reg8_imm8(program_chunk* chunk, uint8_t dest_reg, uint8_t imm) {
    add_byte(chunk, 0xB0 | dest_reg);  // Opcode for 'mov r8, imm8'
    add_byte(chunk, imm);
}




// 'add' Instruction

/**
 * Add a 32-bit Register to Another 32-bit Register
*/
void add_reg32_reg32(program_chunk* chunk, uint8_t dest_reg, uint8_t src_reg) {
    add_byte(chunk, 0x01);             // Opcode for 'add r/m32, r32'
    add_byte(chunk, 0xC0 | (src_reg << 3) | dest_reg); // ModR/M byte
}

/**
 * Add a 16-bit Register to Another 16-bit Register
*/
void add_reg16_reg16(program_chunk* chunk, uint8_t dest_reg, uint8_t src_reg) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0x01);             // Opcode for 'add r/m16, r16'
    add_byte(chunk, 0xC0 | (src_reg << 3) | dest_reg); // ModR/M byte
}

/**
 * Add an 8-bit Register to Another 8-bit Register
*/
void add_reg8_reg8(program_chunk* chunk, uint8_t dest_reg, uint8_t src_reg) {
    add_byte(chunk, 0x00);             // Opcode for 'add r/m8, r8'
    add_byte(chunk, 0xC0 | (src_reg << 3) | dest_reg); // ModR/M byte
}

/**
 * Add an Immediate 32-bit Value to a Register
*/
void add_reg32_imm32(program_chunk* chunk, uint8_t dest_reg, uint32_t imm) {
    add_byte(chunk, 0x81);             // Opcode for 'add r/m32, imm32'
    add_byte(chunk, 0xC0 | dest_reg);  // ModR/M byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 24) & 0xFF));
}

/**
 * Add an Immediate 16-bit Value to a Register
*/
void add_reg16_imm16(program_chunk* chunk, uint8_t dest_reg, uint16_t imm) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0x81);             // Opcode for 'add r/m16, imm16'
    add_byte(chunk, 0xC0 | dest_reg);  // ModR/M byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

/**
 * Add a 16-bit Immediate Value to a 32-bit Register
*/
void add_reg32_imm16(program_chunk* chunk, uint8_t dest_reg, uint16_t imm) {
    add_byte(chunk, 0x81);             // Opcode for 'add r/m32, imm16'
    add_byte(chunk, 0xC0 | dest_reg);  // ModR/M byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

/**
 * Add an Immediate 8-bit Value to a Register
*/
void add_reg8_imm8(program_chunk* chunk, uint8_t dest_reg, uint8_t imm) {
    add_byte(chunk, 0x80);             // Opcode for 'add r/m8, imm8'
    add_byte(chunk, 0xC0 | dest_reg);  // ModR/M byte
    add_byte(chunk, imm);
}

/**
 * Add an 8-bit Immediate Value to a 16-bit Register
*/
void add_reg16_imm8(program_chunk* chunk, uint8_t dest_reg, uint8_t imm) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0x83);             // Opcode for 'add r/m16, imm8'
    add_byte(chunk, 0xC0 | dest_reg);  // ModR/M byte
    add_byte(chunk, imm);
}

/**
 * Add an 8-bit Immediate Value to a 32-bit Register
*/
void add_reg32_imm8(program_chunk* chunk, uint8_t dest_reg, uint8_t imm) {
    add_byte(chunk, 0x83);             // Opcode for 'add r/m32, imm8'
    add_byte(chunk, 0xC0 | dest_reg);  // ModR/M byte
    add_byte(chunk, imm);
}

// 'sub' Instruction

/**
 * Subtract a 32-bit Register from Another 32-bit Register
*/

void sub_reg32_reg32(program_chunk* chunk, uint8_t dest_reg, uint8_t src_reg) {
    add_byte(chunk, 0x29);             // Opcode for 'sub r/m32, r32'
    add_byte(chunk, 0xC0 | (src_reg << 3) | dest_reg); // ModR/M byte
}

/**
 * Subtract a 16-bit Register from Another 16-bit Register
*/
void sub_reg16_reg16(program_chunk* chunk, uint8_t dest_reg, uint8_t src_reg) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0x29);             // Opcode for 'sub r/m16, r16'
    add_byte(chunk, 0xC0 | (src_reg << 3) | dest_reg); // ModR/M byte
}

/**
 * Subtract an 8-bit Register from Another 8-bit Register
*/
void sub_reg8_reg8(program_chunk* chunk, uint8_t dest_reg, uint8_t src_reg) {
    add_byte(chunk, 0x28);             // Opcode for 'sub r/m8, r8'
    add_byte(chunk, 0xC0 | (src_reg << 3) | dest_reg); // ModR/M byte
}

/**
 * Subtract an Immediate 32-bit Value from a Register
*/
void sub_reg32_imm32(program_chunk* chunk, uint8_t dest_reg, uint32_t imm) {
    add_byte(chunk, 0x81);             // Opcode for 'sub r/m32, imm32'
    add_byte(chunk, 0xE8 | dest_reg);  // ModR/M byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 24) & 0xFF));
}

/**
 * Subtract an Immediate 16-bit Value from a Register
*/
void sub_reg16_imm16(program_chunk* chunk, uint8_t dest_reg, uint16_t imm) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0x81);             // Opcode for 'sub r/m16, imm16'
    add_byte(chunk, 0xE8 | dest_reg);  // ModR/M byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

/**
 * Subtract a 16-bit Immediate Value from a 32-bit Register
*/
void sub_reg32_imm16(program_chunk* chunk, uint8_t dest_reg, uint16_t imm) {
    add_byte(chunk, 0x81);             // Opcode for 'sub r/m32, imm16'
    add_byte(chunk, 0xE8 | dest_reg);  // ModR/M byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

/**
 * Subtract an Immediate 8-bit Value from a Register
*/
void sub_reg8_imm8(program_chunk* chunk, uint8_t dest_reg, uint8_t imm) {
    add_byte(chunk, 0x80);             // Opcode for 'sub r/m8, imm8'
    add_byte(chunk, 0xE8 | dest_reg);  // ModR/M byte
    add_byte(chunk, imm);
}

/**
 * Subtract an 8-bit Immediate Value from a 16-bit Register
*/
void sub_reg16_imm8(program_chunk* chunk, uint8_t dest_reg, uint8_t imm) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0x83);             // Opcode for 'sub r/m16, imm8'
    add_byte(chunk, 0xE8 | dest_reg);  // ModR/M byte
    add_byte(chunk, imm);
}

/**
 * Subtract an 8-bit Immediate Value from a 32-bit Register
*/
void sub_reg32_imm8(program_chunk* chunk, uint8_t dest_reg, uint8_t imm) {
    add_byte(chunk, 0x83);             // Opcode for 'sub r/m32, imm8'
    add_byte(chunk, 0xE8 | dest_reg);  // ModR/M byte
    add_byte(chunk, imm);
}

// 'cmp' Instruction

/**
 * Compare a 32-bit Register with Another 32-bit Register
*/
void cmp_reg32_reg32(program_chunk* chunk, uint8_t reg1, uint8_t reg2) {
    add_byte(chunk, 0x39);             // Opcode for 'cmp r/m32, r32'
    add_byte(chunk, 0xC0 | (reg2 << 3) | reg1); // ModR/M byte
}

/**
 * Compare a 16-bit Register with Another 16-bit Register
*/
void cmp_reg16_reg16(program_chunk* chunk, uint8_t reg1, uint8_t reg2) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0x39);             // Opcode for 'cmp r/m16, r16'
    add_byte(chunk, 0xC0 | (reg2 << 3) | reg1); // ModR/M byte
}

/**
 * Compare an 8-bit Register with Another 8-bit Register
*/
void cmp_reg8_reg8(program_chunk* chunk, uint8_t reg1, uint8_t reg2) {
    add_byte(chunk, 0x38);             // Opcode for 'cmp r/m8, r8'
    add_byte(chunk, 0xC0 | (reg2 << 3) | reg1); // ModR/M byte
}

/**
 * Compares a 32-bit Immediate Value with a Register
*/
void cmp_reg32_imm32(program_chunk* chunk, uint8_t reg, uint32_t imm)
{
    add_byte(chunk, 0x81);             // Opcode for 'cmp r/m32, imm32'
    add_byte(chunk, 0xF8 | reg);       // ModR/M byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 24) & 0xFF));
}

/**
 * Compare a 16-bit Immediate Value with a Register
*/
void cmp_reg16_imm16(program_chunk* chunk, uint8_t reg, uint16_t imm) {
    add_byte(chunk, 0x66);             // Operand-size override prefix for 16-bit
    add_byte(chunk, 0x81);             // Opcode for 'cmp r/m16, imm16'
    add_byte(chunk, 0xF8 | reg);       // ModR/M byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

/**
 * Compare an 8-bit Immediate Value with a Register
*/
void cmp_reg8_imm8(program_chunk* chunk, uint8_t reg, uint8_t imm) {
    add_byte(chunk, 0x80);             // Opcode for 'cmp r/m8, imm8'
    add_byte(chunk, 0xF8 | reg);       // ModR/M byte
    add_byte(chunk, imm);
}

// 'test' Instruction

/**
 * Tests an 8-bit Register with an 8-bit Immediate Value
*/
void test_reg8_imm8(program_chunk* chunk, uint8_t reg, uint8_t imm) {
    add_byte(chunk, 0xF6);         // 0xF6 is the opcode for TEST r/m8, imm8 when using ModR/M byte
    add_byte(chunk, 0xC0 | reg);   // 0xC0 | reg is the ModR/M byte for TEST r8, imm8 (C0 = 11000000 for 8-bit registers)
    add_byte(chunk, imm);          // Add the 8-bit immediate value
}

/**
 * Tests a 32-bit register with a 32-bit immediate value.
*/
void test_reg32_imm32(program_chunk* chunk, uint8_t reg, uint32_t imm) {
    // 0xF7 is the opcode for TEST r/m32, imm32 when using ModR/M byte
    add_byte(chunk, 0xF7);
    // 0xC0 | reg is the ModR/M byte for TEST r32, imm32 (C0 = 11000000 for 32-bit registers)
    add_byte(chunk, 0xC0 | reg);
    // Add the 32-bit immediate value, byte by byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 24) & 0xFF));
}

/**
 * Tests an 8-bit register with another 8-bit register.
*/
void test_reg8_reg8(program_chunk* chunk, uint8_t reg1, uint8_t reg2) {
    // 0x84 is the opcode for TEST r/m8, r8
    add_byte(chunk, 0x84);
    // 0xC0 | (reg2 << 3) | reg1 is the ModR/M byte for TEST r8, r8 (C0 = 11000000 for 8-bit registers)
    add_byte(chunk, 0xC0 | (reg2 << 3) | reg1);
}

/**
 * Tests a 32-bit register with another 32-bit register.
*/
void test_reg32_reg32(program_chunk* chunk, uint8_t reg1, uint8_t reg2) {
    // 0x85 is the opcode for TEST r/m32, r32
    add_byte(chunk, 0x85);
    // 0xC0 | (reg2 << 3) | reg1 is the ModR/M byte for TEST r32, r32 (C0 = 11000000 for 32-bit registers)
    add_byte(chunk, 0xC0 | (reg2 << 3) | reg1);
}

// 'jmp' Instruction

/**
 * Short jump (8-bit displacement)
*/
void jmp_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0xEB);  // Opcode for short jump
    add_byte(chunk, (uint8_t)offset);
}

/**
 * Near Jump (32-bit displacement)
*/
void jmp_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0xE9);  // Opcode for near jump
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

/**
 * Far Jump
*/
void jmp_far(program_chunk* chunk, uint16_t segment, uint32_t offset) {
    add_byte(chunk, 0xEA);  // Opcode for far jump
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
    add_byte(chunk, (uint8_t)(segment & 0xFF));
    add_byte(chunk, (uint8_t)((segment >> 8) & 0xFF));
}

// 'je/jz' Instruction

void je_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x74);  // Opcode for short JE/JZ
    add_byte(chunk, (uint8_t)offset);
}

void je_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x84);  // Opcode for near JE/JZ
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jne/jnz' Instruction

void jne_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x75);  // Opcode for short JNE/JNZ
    add_byte(chunk, (uint8_t)offset);
}

void jne_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x85);  // Opcode for near JNE/JNZ
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jg' Instruction

void jg_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x7F);  // Opcode for short JG
    add_byte(chunk, (uint8_t)offset);
}

void jg_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x8F);  // Opcode for near JG
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jge' Instruction

void jge_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x7D);  // Opcode for short JGE
    add_byte(chunk, (uint8_t)offset);
}

void jge_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x8D);  // Opcode for near JGE
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jl' Instruction

void jl_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x7C);  // Opcode for short JL
    add_byte(chunk, (uint8_t)offset);
}

void jl_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x8C);  // Opcode for near JL
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jle' Instruction

void jle_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x7E);  // Opcode for short JLE
    add_byte(chunk, (uint8_t)offset);
}

void jle_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x8E);  // Opcode for near JLE
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'ja' Instruction

void ja_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x77);  // Opcode for short JA
    add_byte(chunk, (uint8_t)offset);
}

void ja_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x87);  // Opcode for near JA
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jae' Instruction

void jae_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x73);  // Opcode for short JAE
    add_byte(chunk, (uint8_t)offset);
}

void jae_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x83);  // Opcode for near JAE
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jb' Instruction

void jb_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x72);  // Opcode for short JB
    add_byte(chunk, (uint8_t)offset);
}

void jb_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x82);  // Opcode for near JB
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jbe' Instruction

void jbe_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x76);  // Opcode for short JBE
    add_byte(chunk, (uint8_t)offset);
}

void jbe_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x86);  // Opcode for near JBE
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jo' Instruction

void jo_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x70);  // Opcode for short JO
    add_byte(chunk, (uint8_t)offset);
}

void jo_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x80);  // Opcode for near JO
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jno' Instruction

void jno_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x71);  // Opcode for short JNO
    add_byte(chunk, (uint8_t)offset);
}

void jno_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x81);  // Opcode for near JNO
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jns' Instruction

void jns_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x79);  // Opcode for short JNS
    add_byte(chunk, (uint8_t)offset);
}

void jns_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x89);  // Opcode for near JNS
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jp/jpe' Instruction

void jp_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x7A);  // Opcode for short JP/JPE
    add_byte(chunk, (uint8_t)offset);
}

void jp_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x8A);  // Opcode for near JP/JPE
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jnp/jpo' Instruction

void jnp_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0x7B);  // Opcode for short JNP/JPO
    add_byte(chunk, (uint8_t)offset);
}

void jnp_near(program_chunk* chunk, int32_t offset) {
    add_byte(chunk, 0x0F);  // Prefix for extended opcode
    add_byte(chunk, 0x8B);  // Opcode for near JNP/JPO
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}

// 'jcxz' Instruction

void jcxz_short(program_chunk* chunk, int8_t offset) {
    add_byte(chunk, 0xE3);  // Opcode for short JCXZ
    add_byte(chunk, (uint8_t)offset);
}

// 'mul' Instruction

void mul_reg8(program_chunk* chunk, uint8_t reg) {
    // 0xF6 is the opcode for various instructions with 8-bit operand
    add_byte(chunk, 0xF6);
    // 0xE0 | reg specifies the ModR/M byte for MUL r/m8 (E0 = 11100000 for reg)
    add_byte(chunk, 0xE0 | reg);
}

void mul_reg16(program_chunk* chunk, uint8_t reg) {
    // 0xF7 is the opcode for various instructions with 16/32-bit operand
    add_byte(chunk, 0xF7);
    // 0xE0 | reg specifies the ModR/M byte for MUL r/m16 (E0 = 11100000 for reg)
    add_byte(chunk, 0xE0 | reg);
}

void mul_reg32(program_chunk* chunk, uint8_t reg) {
    // 0xF7 is the opcode for various instructions with 16/32-bit operand
    add_byte(chunk, 0xF7);
    // 0xE0 | reg specifies the ModR/M byte for MUL r/m32 (E0 = 11100000 for reg)
    add_byte(chunk, 0xE0 | reg);
}

// 'imul' Instruction

void imul_reg8(program_chunk* chunk, uint8_t reg) {
    // 0xF6 is the opcode for various instructions with 8-bit operand
    add_byte(chunk, 0xF6);
    // 0xE8 | reg specifies the ModR/M byte for IMUL r/m8 (E8 = 11101000 for reg)
    add_byte(chunk, 0xE8 | reg);
}

void imul_reg16(program_chunk* chunk, uint8_t reg) {
    // 0xF7 is the opcode for various instructions with 16/32-bit operand
    add_byte(chunk, 0xF7);
    // 0xE8 | reg specifies the ModR/M byte for IMUL r/m16 (E8 = 11101000 for reg)
    add_byte(chunk, 0xE8 | reg);
}

void imul_reg32(program_chunk* chunk, uint8_t reg) {
    // 0xF7 is the opcode for various instructions with 16/32-bit operand
    add_byte(chunk, 0xF7);
    // 0xE8 | reg specifies the ModR/M byte for IMUL r/m32 (E8 = 11101000 for reg)
    add_byte(chunk, 0xE8 | reg);
}

void imul_reg8_imm8(program_chunk* chunk, uint8_t reg, int8_t imm) {
    // 0x6B is the opcode for IMUL r/m8, imm8
    add_byte(chunk, 0x6B);
    // 0xC0 | reg specifies the ModR/M byte for IMUL r/m8, imm8
    add_byte(chunk, 0xC0 | reg);
    // Add the 8-bit immediate value
    add_byte(chunk, (uint8_t)imm);
}

void imul_reg16_imm16(program_chunk* chunk, uint8_t reg, int16_t imm) {
    // 0x69 is the opcode for IMUL r16, imm16
    add_byte(chunk, 0x69);
    // 0xC0 | reg specifies the ModR/M byte for IMUL r16, imm16
    add_byte(chunk, 0xC0 | reg);
    // Add the 16-bit immediate value, byte by byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

void imul_reg32_imm32(program_chunk* chunk, uint8_t reg, int32_t imm) {
    // 0x69 is the opcode for IMUL r32, imm32
    add_byte(chunk, 0x69);
    // 0xC0 | reg specifies the ModR/M byte for IMUL r32, imm32
    add_byte(chunk, 0xC0 | reg);
    // Add the 32-bit immediate value, byte by byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 24) & 0xFF));
}

void imul_reg32_imm16(program_chunk* chunk, uint8_t reg, int16_t imm) {
    // 0x69 is the opcode for IMUL r/m32, imm16
    add_byte(chunk, 0x69);
    // 0xC0 | reg specifies the ModR/M byte for IMUL r/m32, imm16
    add_byte(chunk, 0xC0 | reg);
    // Add the 16-bit immediate value, byte by byte
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

void imul_reg32_imm8(program_chunk* chunk, uint8_t reg, int8_t imm) {
    // 0x6B is the opcode for IMUL r32, imm8
    add_byte(chunk, 0x6B);
    // 0xC0 | reg specifies the ModR/M byte for IMUL r32, imm8
    add_byte(chunk, 0xC0 | reg);
    // Add the 8-bit immediate value
    add_byte(chunk, (uint8_t)imm);
}

void imul_reg16_imm8(program_chunk* chunk, uint8_t reg, int8_t imm) {
    // 0x6B is the opcode for IMUL r/m16, imm8
    add_byte(chunk, 0x6B);
    // 0xC0 | reg specifies the ModR/M byte for IMUL r/m16, imm8
    add_byte(chunk, 0xC0 | reg);
    // Add the 8-bit immediate value
    add_byte(chunk, (uint8_t)imm);
}

// 'div' Instruction

void div_reg8(program_chunk* chunk, uint8_t reg) {
    // Opcode: F6 /6 (DIV r/m8)
    add_byte(chunk, 0xF6);
    add_byte(chunk, 0xF0 | reg); // ModR/M byte: 11 (register addressing) 110 (DIV)
}

void div_reg16(program_chunk* chunk, uint8_t reg) {
    // Opcode: F7 /6 (DIV r/m16)
    add_byte(chunk, 0xF7);
    add_byte(chunk, 0xF0 | reg); // ModR/M byte: 11 (register addressing) 110 (DIV)
}

void div_reg32(program_chunk* chunk, uint8_t reg) {
    // Opcode: F7 /6 (DIV r/m32)
    add_byte(chunk, 0xF7);
    add_byte(chunk, 0xF0 | reg); // ModR/M byte: 11 (register addressing) 110 (DIV)
}

// 'idiv' Instruction

void idiv_reg8(program_chunk* chunk, uint8_t reg) {
    // Opcode: F6 /7 (IDIV r/m8)
    add_byte(chunk, 0xF6);
    add_byte(chunk, 0xF8 | reg); // ModR/M byte: 11 (register addressing) 111 (IDIV)
}

void idiv_reg16(program_chunk* chunk, uint8_t reg) {
    // Opcode: F7 /7 (IDIV r/m16)
    add_byte(chunk, 0xF7);
    add_byte(chunk, 0xF8 | reg); // ModR/M byte: 11 (register addressing) 111 (IDIV)
}

void idiv_reg32(program_chunk* chunk, uint8_t reg) {
    // Opcode: F7 /7 (IDIV r/m32)
    add_byte(chunk, 0xF7);
    add_byte(chunk, 0xF8 | reg); // ModR/M byte: 11 (register addressing) 111 (IDIV)
}

// 'push' Instruction

void push_imm8(program_chunk* chunk, uint8_t imm) {
    // Opcode: 6A ib (PUSH imm8)
    add_byte(chunk, 0x6A);
    add_byte(chunk, imm);
}

void push_imm16(program_chunk* chunk, uint16_t imm) {
    // Opcode: 68 iw (PUSH imm16)
    add_byte(chunk, 0x68);
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

void push_imm32(program_chunk* chunk, uint32_t imm) {
    // Opcode: 68 id (PUSH imm32)
    add_byte(chunk, 0x68);
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 24) & 0xFF));
}

void push_reg16(program_chunk* chunk, uint8_t reg) {
    // Opcode: 50 + rw (PUSH r16)
    add_byte(chunk, 0x50 + reg);
}

void push_reg32(program_chunk* chunk, uint8_t reg) {
    // Opcode: 50 + rd (PUSH r32)
    add_byte(chunk, 0x50 + reg);
}

// 'pop' Instruction

void pop_reg16(program_chunk* chunk, uint8_t reg) {
    // Opcode: 58 + rw (POP r16)
    add_byte(chunk, 0x58 + reg);
}

void pop_reg32(program_chunk* chunk, uint8_t reg) {
    // Opcode: 58 + rd (POP r32)
    add_byte(chunk, 0x58 + reg);
}

// 'xor' Instruction

void xor_reg8_imm8(program_chunk* chunk, uint8_t reg, uint8_t imm) {
    // Opcode: 80 /6 ib (XOR r/m8, imm8)
    add_byte(chunk, 0x80);
    add_byte(chunk, 0xF0 | reg); // ModR/M byte: 11 (register addressing) 110 (XOR)
    add_byte(chunk, imm);
}

void xor_reg8_imm32(program_chunk* chunk, uint8_t reg, uint32_t imm) {
    // Move immediate value to a temporary register (e.g., BL)
    add_byte(chunk, 0xB3);  // MOV BL, imm8
    add_byte(chunk, imm);
    
    // Perform the XOR operation
    add_byte(chunk, 0x32);  // XOR r/m8, r8
    add_byte(chunk, 0xD8 | reg);  // ModR/M byte: 11 (register) 110 (XOR), BL
}

void xor_reg16_imm16(program_chunk* chunk, uint8_t reg, uint16_t imm) {
    // Opcode: 81 /6 iw (XOR r/m16, imm16)
    add_byte(chunk, 0x81);
    add_byte(chunk, 0xF0 | reg); // ModR/M byte: 11 (register addressing) 110 (XOR)
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
}

void xor_reg32_imm32(program_chunk* chunk, uint8_t reg, uint32_t imm) {
    // Opcode: 81 /6 id (XOR r/m32, imm32)
    add_byte(chunk, 0x81);
    add_byte(chunk, 0xF0 | reg); // ModR/M byte: 11 (register addressing) 110 (XOR)
    add_byte(chunk, (uint8_t)(imm & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((imm >> 24) & 0xFF));
}

void xor_reg8_reg8(program_chunk* chunk, uint8_t reg1, uint8_t reg2) {
    // Opcode: 30 /r (XOR r/m8, r8)
    add_byte(chunk, 0x30);
    add_byte(chunk, 0xC0 | (reg1 << 3) | reg2); // ModR/M byte: 11 (register addressing)
}

void xor_reg16_reg16(program_chunk* chunk, uint8_t reg1, uint8_t reg2) {
    // Opcode: 31 /r (XOR r/m16, r16)
    add_byte(chunk, 0x31);
    add_byte(chunk, 0xC0 | (reg1 << 3) | reg2); // ModR/M byte: 11 (register addressing)
}

void xor_reg32_reg32(program_chunk* chunk, uint8_t reg1, uint8_t reg2) {
    // Opcode: 31 /r (XOR r/m32, r32)
    add_byte(chunk, 0x31);
    add_byte(chunk, 0xC0 | (reg1 << 3) | reg2); // ModR/M byte: 11 (register addressing)
}

// 'int' Instruction

void int_kernal(program_chunk* chunk)
{
    add_byte(chunk, 0xCD);
    add_byte(chunk, 0x80);
}

void ret(program_chunk* chunk)
{
    add_byte(chunk, 0xC3);
}

void call(program_chunk* chunk, int32_t offset)
{
    add_byte(chunk, 0xE8);
    add_byte(chunk, (uint8_t)(offset & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 8) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 16) & 0xFF));
    add_byte(chunk, (uint8_t)((offset >> 24) & 0xFF));
}


void build_print_num_function(program_chunk* chunk)
{
    push_reg32(chunk, EBP);
    mov_reg32_reg32(chunk, EBP, ESP);
    xor_reg32_reg32(chunk, ESI, ESI);
    sub_reg32_imm32(chunk, ESP, 1);
    add_byte(chunk, 0xC6);
    add_byte(chunk, 0x04);
    add_byte(chunk, 0x24);
    add_byte(chunk, 0x0A);
    add_reg32_imm32(chunk, ESI, 1);
    xor_reg32_reg32(chunk, EBX, EBX);
    xor_reg32_reg32(chunk, EDX, EDX);
    mov_reg32_imm32(chunk, ECX, 10);
    div_reg32(chunk, ECX);
    add_reg32_imm32(chunk, EDX, '0');
    sub_reg32_imm32(chunk, ESP, 1);
    add_byte(chunk, 0x88);
    add_byte(chunk, 0x14);
    add_byte(chunk, 0x24);
    add_reg32_imm32(chunk, ESI, 1);
    test_reg32_reg32(chunk, EAX, EAX);
    jne_short(chunk, -34);
    mov_reg32_reg32(chunk, EDX, ESI);
    mov_reg32_imm32(chunk, EAX, 4);
    mov_reg32_imm32(chunk, EBX, 1);
    mov_reg32_reg32(chunk, ECX, ESP);
    int_kernal(chunk);
    mov_reg32_reg32(chunk, ESP, EBP);
    pop_reg32(chunk, EBP);
    ret(chunk);
}




typedef struct
{
    program_chunk** functions;
    uint32_t functionCount;
    uint32_t functionCapacity;

    program_chunk** memories;
    uint32_t memoryCount;
    uint32_t memoryCapacity;
} function_pool;

function_pool* create_function_pool()
{
    function_pool* pool = (function_pool *)malloc(sizeof(function_pool));
    pool->memoryCapacity = 255;
    pool->functionCapacity = 255;
    pool->memoryCount = 0;
    pool->functionCount = 0;

    pool->functions = (program_chunk **)malloc(sizeof(program_chunk*) * pool->functionCapacity);
    pool->memories = (program_chunk **)malloc(sizeof(program_chunk*) * pool->memoryCapacity);

    return pool;
}

void add_memory(function_pool* pool, program_chunk* chunk)
{
    if (pool->memoryCount == pool->memoryCapacity)
    {
        pool->memoryCapacity += 255;
        pool->memories = (program_chunk **)realloc(pool->memories, sizeof(program_chunk*) * pool->memoryCapacity);
    }

    pool->memories[pool->memoryCount++] = chunk;
}

void add_function(function_pool* pool, program_chunk* chunk)
{
    if (pool->functionCount == pool->functionCapacity)
    {
        pool->functionCapacity += 255;
        pool->functions = (program_chunk **)realloc(pool->functions, sizeof(program_chunk*) * pool->functionCapacity);
    }

    pool->functions[pool->functionCount++] = chunk;
}





add_call_request(program_chunk* chunk, char* function_name)
{
    if (chunk->call_request_count == chunk->call_request_capacity)
    {
        chunk->call_request_capacity+=16;
        chunk->call_request_locations = (uint32_t*)realloc(chunk->call_request_locations, sizeof(uint32_t) * chunk->call_request_capacity);
        chunk->call_requests = (char**)realloc(chunk->call_requests, sizeof(char*)*chunk->call_request_capacity);
    }

    chunk->call_request_locations[chunk->call_request_count] = chunk->count;
    printf("ASKDJHGAJKHLSDGJKHASGDKJHAGSDKJHAGKSJD: %i\n", chunk->call_request_locations[chunk->call_request_count]);
    chunk->call_requests[chunk->call_request_count++] = function_name;
}

void build_chunk(ASTNode *node, function_pool* pool, program_chunk* chunk)
{
    switch (node->type)
    {
    case AST_COMPOUND_GLOBAL_CONSTRUCT:
        for (int i = 0; i < node->data.compound_global_construct.global_construct_count; i++)
        {
            build_chunk(node->data.compound_global_construct.global_constructs[i], pool, NULL);
        }
        break;
    case AST_FUNCTION_DECLARATION:
        program_chunk* function = create_program_chunk();
        function->name = node->data.function_global_construct.name;
        add_function(pool, function);
        if (strcmp(function->name, "main") != 0)
        {
            push_reg32(function, EBP);
            mov_reg32_reg32(function, EBP, ESP);
            sub_reg32_imm32(function, ESP, 0);

            build_chunk(node->data.function_global_construct.statements, pool, function);

            mov_reg32_reg32(function, ESP, EBP);
            pop_reg32(function, EBP);
            ret(function);
        }
        else
        {
            mov_reg32_reg32(function, EBP, ESP);
            sub_reg32_imm32(function, ESP, 0);

            build_chunk(node->data.function_global_construct.statements, pool, function);
            
            mov_reg32_reg32(function, ESP, EBP);
            pop_reg32(function, EBP);

            mov_reg32_imm32(function, EAX, 1);
            mov_reg32_imm32(function, EBX, 0);
            int_kernal(function);
        }
        
        break;
    case AST_VARIABLE_DECLARATION:
        /* code */
        break;
    case AST_COMPOUND_STATEMENT:
        for (int i = 0; i < node->data.compound_statement.statement_count; i++)
        {
            build_chunk(node->data.compound_statement.statements[i], pool, chunk);
        }
        break;
    case AST_EXPRESSION_STATEMENT:
        build_chunk(node->data.expression_statement.expression, pool, chunk);
        break;
    case AST_FUNCTION_CALL:
        if (strcmp(node->data.function_call.name, "print_num")==0 && node->data.function_call.argumentList->data.compound_argument.arguments[0])
        {
            mov_reg32_imm32(chunk, EAX, node->data.function_call.argumentList->data.compound_argument.arguments[0]->data.argument.argument->data.number_value);
            // call(chunk, 0);

        }
        add_call_request(chunk, node->data.function_call.name);
        call(chunk, 0);
    
    default:
        break;
    }
}


void reorder_functions(function_pool* functions)
{
    for (int i = 0; i < functions->functionCount; i++)
    {
        if (strcmp(functions->functions[i]->name, "main") == 0)
        {
            program_chunk* last_chunk = functions->functions[i];
            for (int i2 = 0; i2 < i+1; i2++)
            {
                program_chunk* set_chunk = last_chunk;
                last_chunk = functions->functions[i2];
                functions->functions[i2] = set_chunk;
            }
        }
    }
}

calculate_function_call_addresses(function_pool* pool)
{
    uint32_t current_address = 0;
    for (int i = 0; i < pool->functionCount; i++)
    {
        pool->functions[i]->start_address = current_address;
        current_address += pool->functions[i]->count;
    }
}


solve_calls(function_pool *pool)
{
    for (int function_index = 0; function_index < pool->functionCount; function_index++)
    {
        for (int call_request_index = 0; call_request_index < pool->functions[function_index]->call_request_count; call_request_index++)
        {
            printf("Call Request: %s\n", pool->functions[function_index]->call_requests[call_request_index]);
            for (int i = 0; i < pool->functionCount; i++)
            {
                printf("Call Request: %s %s\n", pool->functions[function_index]->call_requests[call_request_index], pool->functions[i]->name);
                if (strcmp(pool->functions[function_index]->call_requests[call_request_index], pool->functions[i]->name) == 0)
                {
                    
                    uint32_t address_offset = (pool->functions[i]->start_address - (pool->functions[function_index]->call_request_locations[call_request_index] + pool->functions[function_index]->start_address)) - 5;
                    printf("hjkasdk %i %i %i\n", address_offset, pool->functions[i]->start_address, pool->functions[function_index]->call_request_locations[call_request_index]);
                    pool->functions[function_index]->bytes[pool->functions[function_index]->call_request_locations[call_request_index] + 1] = (uint8_t)(address_offset & 0xFF);
                    pool->functions[function_index]->bytes[pool->functions[function_index]->call_request_locations[call_request_index] + 2] = (uint8_t)((address_offset >> 8) & 0xFF);
                    pool->functions[function_index]->bytes[pool->functions[function_index]->call_request_locations[call_request_index] + 3] = (uint8_t)((address_offset >> 16) & 0xFF);
                    pool->functions[function_index]->bytes[pool->functions[function_index]->call_request_locations[call_request_index] + 4] = (uint8_t)((address_offset >> 24) & 0xFF);
                }
            }
        }
    }
}



void BuildExecutable(ASTNode *node)
{


    // mov number into eax register

    program_chunk* print_num_function = create_program_chunk();
    build_print_num_function(print_num_function);
    print_num_function->start_address = 0;
    print_num_function->name = "print_num";



    function_pool* pool = create_function_pool();
    add_function(pool, print_num_function);
    build_chunk(node, pool, NULL);

    for (int i = 0; i < pool->functionCount; i++)
    {
        printf("FUNCTION: %s\n", pool->functions[i]->name);
    }


    reorder_functions(pool);

    for (int i = 0; i < pool->functionCount; i++)
    {
        printf("FUNCTION: %s\n", pool->functions[i]->name);
    }

    calculate_function_call_addresses(pool);
    solve_calls(pool);



    // Gets size of program by adding the size of each function
    int program_size = 0;
    for (int i = 0; i < pool->functionCount; i++)
    {
        program_size += pool->functions[i]->count;
    }







    uint8_t elf_header[] = 
    {
        0x7F, 0x45, 0x4C, 0x46,
        0x01, 
            0x01, 
                0x01,
                    0x00,
        0x00,
            0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        
        0x02, 0x00,
                0x03, 0x00,
        0x01, 0x00, 0x00, 0x00,
        
        0x54, 0x80, 0x04, 0x08,
        0x34, 0x00, 0x00, 0x00,

        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,

        0x34, 0x00,
                0x20, 0x00,
        0x01, 0x00,
                0x28, 0x00,
        
        0x00, 0x00,
                0x00, 0x00
    };

    uint8_t program_header[] = 
    {
        0x01, 0x00, 0x00, 0x00,
        0x54, 0x00, 0x00, 0x00,
        0x54, 0x80, 0x04, 0x08,
        0x00, 0x00, 0x00, 0x00,
        program_size, 0x00, 0x00, 0x00, // code size
        program_size, 0x00, 0x00, 0x00, // code size
        0x05, 0x00, 0x00, 0x00,
        0x00, 0x10, 0x00, 0x00
    };

    FILE* file = fopen("code", "wb");
    if (!file)
    {
        perror("Failed to open file!");
    }

    fwrite(&elf_header, sizeof(elf_header), 1, file);
    fwrite(&program_header, sizeof(program_header), 1, file);

    for (int i = 0; i < pool->functionCount; i++)
    {
        fwrite(pool->functions[i]->bytes, pool->functions[i]->count, 1, file);
    }

    fclose(file);
    printf("Successfully created executable!\n");
}