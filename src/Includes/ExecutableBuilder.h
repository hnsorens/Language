#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "SemanticAnalysis.h"

typedef struct
{
    uint8_t e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf32_Ehdr __attribute__((packed));

typedef struct
{
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr __attribute__((packed));

typedef struct
{
    uint8_t* bytes;
    uint32_t count;
    uint32_t capacity;
    char* name;
    uint32_t start_address;


    const char** call_requests;
    uint32_t* call_request_locations;
    uint32_t call_request_capacity;
    uint32_t call_request_count;
} program_chunk;

void BuildExecutable(ASTNode *node);