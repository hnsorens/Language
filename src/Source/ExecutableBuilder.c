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

// Immediate += -=...
#define IMT 0x83

#define ADD_IMT 0x00
#define OR_IMT 0x01
#define ADC_IMT 0x02
#define SBB_IMT 0x03
#define AND_IMT 0x04
#define SUB_IMT 0x05
#define XOR_IMT 0x06
#define CMP_IMT 0x07

#define PUSH_IMT 0x68
#define MOV_DWORD_IMT 0xC7
#define MOV_WORD_IMT 0x66, 0xC7
#define MOV_BYTE_IMT 0xC6

#define ADD 0x03
#define SUB 0x2B

uint8_t getRegisterByteDirect(uint8_t dest, uint8_t move)
{
    return 192 + (move << 3) + dest;
}

uint8_t getImmediateByte(uint8_t action, uint8_t dest)
{
    return 192 + (action << 3) + dest;
}

uint8_t getPushRegByte(uint8_t reg)
{
    return 0x50 + reg;
}

uint8_t getPopRegByte(uint8_t reg)
{
    return 0x58 + reg;
}

uint8_t getMovImmediateByte(uint8_t reg)
{
    return 0xB8 + reg;
}

uint8_t getMovOffsetByte(uint8_t reg)
{
    return 0x40 + reg;
}

#define IMMEDIATE 0x83
#define PUSH_EBP 0x55
#define MOV 0x89
#define POP_EBP 0x5D
#define RET 0xC3






program_chunk* create_program_chunk()
{
    program_chunk* chunk = (program_chunk*)malloc(sizeof(program_chunk));   
    chunk->count = 0;
    chunk->capacity = 256;
    chunk->bytes = (uint8_t *)malloc(sizeof(uint8_t) * chunk->capacity);
    return chunk;
}

void addByte(program_chunk* chunk, uint8_t byte)
{
    if (chunk->count >= chunk->capacity)
    {
        chunk->capacity *= 2;
        chunk->bytes = (uint8_t*)realloc(chunk->bytes, sizeof(uint8_t) * chunk->capacity);
    }
    chunk->bytes[chunk->count++] = byte;
}

void addint32Bytes(program_chunk* chunk, int32_t num)
{
    addByte(chunk, (uint8_t)num);
    addByte(chunk, (uint8_t)(num >> 8));
    addByte(chunk, (uint8_t)(num >> 16));
    addByte(chunk, (uint8_t)(num >> 24));
}

void addint16Bytes(program_chunk* chunk, int32_t num)
{
    addByte(chunk, (uint8_t)num);
    addByte(chunk, (uint8_t)(num >> 8));
}

void push_register(program_chunk* chunk, uint8_t reg){addByte(chunk, 0x50 + reg);}
void mov_number_into_register(program_chunk* chunk, uint8_t dest, uint32_t num){addByte(chunk, 0xB8 + dest);            addint32Bytes(chunk, num);/*             addByte(chunk, num);addByte(chunk, 0);addByte(chunk, 0);addByte(chunk, 0);*/}
void pop_register(program_chunk* chunk, uint8_t reg){addByte(chunk, 0x58 + reg);}

void add_byte_number_to_register(program_chunk *chunk, uint8_t dest, uint8_t num){addByte(chunk, 0x80); addByte(chunk, 192 + dest); addByte(chunk, num);}
void sub_number_from_esp(program_chunk* chunk, uint8_t dest, uint8_t num){addByte(chunk, IMT);addByte(chunk, 192 + (SUB_IMT << 3) + dest);addByte(chunk, num);}
void end_function(program_chunk* chunk){addByte(chunk, RET);}
void call_kernal(program_chunk* chunk){addByte(chunk, 0xCD); addByte(chunk, 0x80);}
void clear_register(program_chunk* chunk, uint8_t reg){addByte(chunk, 0x31); addByte(chunk, 192 + (reg << 3) + reg);}
void mov_value_byte_with_offset(program_chunk* chunk, int8_t offset, uint8_t dest, uint8_t byte){addByte(chunk, MOV_DWORD_IMT); addByte(chunk, getMovOffsetByte(EBP)); addByte(chunk, offset); addByte(chunk, byte);}
void mov_reg_byte_with_offset(program_chunk* chunk, int8_t offset, uint8_t dest, uint8_t move){addByte(chunk, 0x89); addByte(chunk, 64 + (move << 3) + dest); addByte(chunk, offset);} // doesnt work
void test_2_reg(program_chunk* chunk, uint8_t reg1, uint8_t reg2){addByte(chunk, 0x85); addByte(chunk, 192 + (reg1 << 3) + reg2);}

// void add_to_8_register(program_chunk* chunk, uint8_t dest, int8_t num){addByte(chunk, 0x80)}


//Good and tested functions
void mov_32_register(program_chunk* chunk, uint8_t dest, uint8_t move){addByte(chunk, 0x89);addByte(chunk, 192 + (move << 3) + dest);}
void mov_16_register(program_chunk* chunk, uint8_t dest, uint8_t move){addByte(chunk, 0x66);addByte(chunk, 0x89);addByte(chunk, 192 + (move << 3) + dest);}
void mov_8L_register(program_chunk* chunk, uint8_t dest, uint8_t move){addByte(chunk, 0x88);addByte(chunk, 192 + (move << 3) + dest);} // works for A-D
void mov_8H_register(program_chunk* chunk, uint8_t dest, uint8_t move){addByte(chunk, 0x88);addByte(chunk, 192 + (move*2 << 3) + dest*2);} // works for A-D

void add_8signed_to_register(program_chunk* chunk, uint8_t dest, int8_t num){addByte(chunk, 0x83);addByte(chunk, 192 + dest);addByte(chunk, (uint8_t)num);}
void add_to_8_register(program_chunk* chunk, uint8_t dest, int8_t num){addByte(chunk, 0x80);addByte(chunk, 192 + dest);addByte(chunk, (uint8_t)num);}
void add_to_16_register(program_chunk* chunk, uint8_t dest, uint16_t num){addByte(chunk, 0x66);switch (dest){case EAX:addByte(chunk, 0x05);break;case ECX:addByte(chunk, 0x0D);break;case EDX:addByte(chunk, 0x15);break;default:addByte(chunk, 0x81);addByte(chunk, 192 + (ADD_IMT << 3) + dest);}addint16Bytes(chunk, num);}
void add_to_register(program_chunk* chunk, uint8_t dest, uint32_t num){switch (dest){case EAX:addByte(chunk, 0x05);break;case ECX:addByte(chunk, 0x0D);break;case EDX:addByte(chunk, 0x15);break;default:addByte(chunk, 0x81);addByte(chunk, 192 + (ADD_IMT << 3) + dest);}addint32Bytes(chunk, num);}


void BuildExecutable(ASTNode *node)
{

    // External Functions
    printf("%i\n\n\n\n\n", 1234 >> 8);

    // mov number into eax register

    program_chunk* print_num_function = create_program_chunk();

    mov_number_into_register(print_num_function, EAX, 123);

    push_register(print_num_function, EBP);
    mov_32_register(print_num_function, EBP, ESP);
    
    
    clear_register(print_num_function, ESI);


// add enter
sub_number_from_esp(print_num_function, ESP, 1);

addByte(print_num_function, 0xC6);
addByte(print_num_function, 0x04);
addByte(print_num_function, 0x24);
addByte(print_num_function, 0x0A);

  addByte(print_num_function, 0x83);
    addByte(print_num_function, 0xC6);
    addByte(print_num_function, 0x01);






    // sub_number_from_esp(print_num_function, ESP, 4);

    mov_number_into_register(print_num_function, EBX, 0);
    // Begining of loop
    clear_register(print_num_function, EDX);

    mov_number_into_register(print_num_function, ECX, 10);
    // div ecx ; divides 10 and puts remainder into EDX
    addByte(print_num_function, 0xF7);
    addByte(print_num_function, 0xF1);


    add_byte_number_to_register(print_num_function, EDX, '0');
    sub_number_from_esp(print_num_function, ESP, 1);
    // mov_reg_byte_with_offset(print_num_function, 4, ESP, EDX);
    addByte(print_num_function, 0x88);
    addByte(print_num_function, 0x14);
    addByte(print_num_function, 0x24);

    // add_byte_number_to_register(print_num_function, ESI, 1);
    addByte(print_num_function, 0x83);
    addByte(print_num_function, 0xC6);
    addByte(print_num_function, 0x01);

    test_2_reg(print_num_function, EAX, EAX);
    //jnz back to beginning of loop

    addByte(print_num_function, 0x0F);
    addByte(print_num_function, 0x85);
    addByte(print_num_function, 256-34);
    addByte(print_num_function, 0xFF);
    addByte(print_num_function, 0xFF);
    addByte(print_num_function, 0xFF);

    // Print
    mov_32_register(print_num_function, EDX, ESI);
    mov_number_into_register(print_num_function, EAX, 4);
    mov_number_into_register(print_num_function, EBX, 1);
    // move the address of the number to ecx
    mov_32_register(print_num_function, ECX, ESP);
    call_kernal(print_num_function);

    mov_32_register(print_num_function, ESP, EBP);
    pop_register(print_num_function, EBP);
    // end_function(print_num_function);




#ifdef TESTS
    // TEST
    program_chunk* test = create_program_chunk();
    
    // all 32 bit register mov
    for (int dest = 0; dest < 8; dest++)
    {
        add_8signed_to_register(test, dest, 123);
    }
    for (int dest = 0; dest < 8; dest++)
    {
        add_to_8_register(test, dest, 123);
    }
    for (int dest = 0; dest < 8; dest++)
    {
        add_to_16_register(test, dest, 123);
    }
    for (int dest = 0; dest < 8; dest++)
    {
        add_to_register(test, dest, -123);
    }




#endif 
    // const uint8_t print_num_function[] = 
    // {
    //     PUSH_EBP,       // push ebp
    //     MOV, getRegisterByteDirect(EBP, ESP), // mov ebp esp
    //     IMT, getImmediateByte(SUB_IMT, ESP), 4, // sub esp 4
        

    //     IMT, getImmediateByte(ADD_IMT, ESP), 4,
    //     MOV, getRegisterByteDirect(ESP, EBP),// mov esp ebp
    //     POP_EBP, // pop ebp
    //     RET // ret
    // };

    // Code

    const uint8_t Hello_World_Program[] = 
    {


        // RET // ret
        getMovImmediateByte(EAX), 0x04, 0x00, 0x00, 0x00, // mov eax 12
        getMovImmediateByte(EBX), 0x01, 0x00, 0x00, 0x00, // mov ebx 1
        getMovImmediateByte(ECX), 0x76, 0x80, 0x04, 0x08, // mov ecx 0x8048054 + 34
        getMovImmediateByte(EDX), 0x0E, 0x00, 0x00, 0x00, // mov edx 0
        0xCD, 0x80,  // int 0x80
    };

    const uint8_t program_segment_end[] = 
    {
        0xB8, 0x01, 0x00, 0x00, 0x00, // mov eax 1
        0xBB, 0x00, 0x00, 0x00, 0x00, // mov ebx 0
        0xCD, 0x80, // int 80
    };

    const uint8_t hello_world_string[] =
    {
        'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n'
    };

    // Create Code Here



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
        /*sizeof(Hello_World_Program) + sizeof(program_segment_end) + sizeof(hello_world_string) +*/ test->count, 0x00, 0x00, 0x00, // code size
        /*sizeof(Hello_World_Program) + sizeof(program_segment_end) + sizeof(hello_world_string) +*/ test->count, 0x00, 0x00, 0x00, // code size
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

    fwrite(test->bytes, test->count, 1, file);
    // fwrite(&program_segment_end, sizeof(program_segment_end), 1, file);
    // fwrite(Hello_World_Program, sizeof(Hello_World_Program), 1, file);
    // fwrite(program_segment_end, sizeof(uint8_t) * 12, 1, file);
    // fwrite(hello_world_string, sizeof(hello_world_string), 1, file);

    fclose(file);
    printf("Successfully created executable!\n");
}