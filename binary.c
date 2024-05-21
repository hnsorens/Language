#include <stdio.h>
#include <stdint.h>
#include <string.h>

// ELF header structure
typedef struct {
    uint8_t ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} Elf32_Ehdr;

// Program header structure
typedef struct {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
} Elf32_Phdr;

int main() {
    // ELF header
    // Elf32_Ehdr elf_header = {
    //     {0x7F, 'E', 'L', 'F', 1, 1, 1, 0},
    //     2, 3, 1, sizeof(Elf32_Ehdr), sizeof(Elf32_Ehdr), 0, 0, sizeof(Elf32_Ehdr), sizeof(Elf32_Phdr), 1, 0, 0, 0
    // };

    const uint8_t elf_header = 
    {
        0x7F, 'E', 'L', 'F',
        0x01,               // 1=32-bit  2=64-bit
            0x01,           // 1=little-endian  2=big-endian
                0x01,       // Elf header version must be 1
                    0x00,   // Target OS should be 0
        0x00,               // ABI version 0 is okay for linux
            0x00, 0x00, 0x00,   // unused
        0x00, 0x00, 0x00, 0x00,   // 10

        0x02, 0x00,           // 2 = executable
                0x03, 0x00,   // 3 = x86, 3E=amd64
        0x01, 0x00, 0x00, 0x00,   // elf identification version must be 1
        0x54, 0x80, 0x04, 0x08,   // memory address of entry point
        0x34, 0x00, 0x00, 0x00,   // file offset where program header starts

        0x00, 0x00, 0x00, 0x00,   // file offset where section headers begin
        0x00, 0x00, 0x00, 0x00,   // 0 for x86

        0x34, 0x00,           // size of elf header
                0x20, 0x00,   // size of each program header
        0x01, 0x00,           // number of program headers
                0x28, 0x00,   // size of each section header
        0x00, 0x00,           // section headers
                0x00, 0x00,    // index of section header containing section names




        0x01, 0x00, 0x00, 0x00,
        0x54, 0x00, 0x00, 0x00,
        0x54, 0x80, 0x04, 0x08,
        0x00, 0x00, 0x00, 0x00,
        0x0C, 0x00, 0x00, 0x00,
        0x0C, 0x00, 0x00, 0x00,
        0x05, 0x00, 0x00, 0x00,
        0x00, 0x10, 0x00, 0x00,


        0xB8, 0x01, 0x00, 0x00, 0x00,
        0xBB, 0x00, 0x00, 0x00, 0x00,
        0xCD, 0x80
    };

    // Assembly instructions for "Hello, World!" and exit
    const uint8_t assembly[] = {
        // // Write syscall: mov eax, 4; mov ebx, 1; mov ecx, 0x08040034; mov edx, 14; int 0x80
        // 0xB8, 0x04, 0x00, 0x00, 0x00,
        // 0xBB, 0x01, 0x00, 0x00, 0x00,
        // 0xB9, 0x34, 0x00, 0x04, 0x08,
        // 0xBA, 0x0E, 0x00, 0x00, 0x00,
        // 0xCD, 0x80,
        // // Exit syscall: mov eax, 1; mov ebx, 0; int 0x80
        // 0xB8, 0x01, 0x00, 0x00, 0x00,
        // 0xBB, 0x00, 0x00, 0x00, 0x00,
        // 0xCD, 0x80,
        // // "Hello, World!" string
        // 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n'

        0xB8, 0x01, 0x00, 0x00, 0x00,
        0xBB, 0x00, 0x00, 0x00, 0x00,
        0xCD, 0x80
    };

    // // Program header
    // Elf32_Phdr program_header = {
    //     1, sizeof(Elf32_Ehdr), 0x08048000, 0x08048000, sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr), sizeof(assembly), 5, 0x1000
    // };

    const uint8_t program_header[] = 
    {
        0x01, 0x00, 0x00, 0x00,
        0x54, 0x00, 0x00, 0x00,
        0x54, 0x80, 0x04, 0x08,
        0x00, 0x00, 0x00, 0x00,
        0x0C, 0x00, 0x00, 0x00,
        0x0C, 0x00, 0x00, 0x00,
        0x05, 0x00, 0x00, 0x00,
        0x00, 0x10, 0x00, 0x00
    };
    

    // Write ELF header and program header to file
    FILE *file = fopen("hello_world", "wb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    fwrite(&elf_header, sizeof(elf_header), 1, file);
    // fwrite(&program_header, sizeof(program_header), 1, file);

    // // Offset the assembly instructions to start at the entry point specified in the ELF header
    // fseek(file, x54800408, SEEK_SET);
    // fwrite(assembly, sizeof(assembly), 1, file);

    fclose(file);
    printf("Successfully created hello_world\n");
    return 0;
}



# >>>>>>>>>>>>> ELF FILE HEADER <<<<<<<<<<<<< 
                # All numbers (except in names) are in base sixteen (hexadecimal)
                
7F 45 4C 46     
01              
   01           
      01        
         00     
00              
   00 00 00     
00 00 00 00     
02 00           
      03 00     
01 00 00 00     
54 80 04 08     
34 00 00 00     
00 00 00 00     
00 00 00 00     
34 00           
      20 00     
01 00           
      28 00     
00 00           
      00 00     
01 00 00 00     
54 00 00 00     
54 80 04 08     
00 00 00 00     
0C 00 00 00     
0C 00 00 00     
05 00 00 00     
00 10 00 00     

B8 01 00 00 00  
BB 00 00 00 00  
CD 80           
