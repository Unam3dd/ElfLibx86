#pragma once
#include <stdint.h>
#include <stddef.h>
#include <elf.h>

//////////////////////////////////////////
//
//          DEFINES
//
/////////////////////////////////////////
#define IS_ELF(str) (str[0] == 0x7F && str[1] == 'E' && str[2] == 'L' && str[3] == 'F')

//////////////////////////////////////////
//
//          TYPEDEF
//
/////////////////////////////////////////

typedef struct elf_parser_t elf_parser_t;
typedef struct elf_data_t elf_data_t;
typedef struct elf_loader_t elf_loader_t;


//////////////////////////////////////////
//
//          STRUCTS
//
/////////////////////////////////////////

struct elf_data_t
{
    char *data;
    size_t file_size;
};

struct elf_parser_t
{
    elf_data_t *data;
    Elf32_Ehdr *elf_hdr;
    Elf32_Shdr *section_hdr;
    Elf32_Phdr *program_hdr;
    Elf32_Shdr *dynsym;
    Elf32_Shdr *symtab;
    Elf32_Shdr *str_tab;
    char *sct_str;
};

struct elf_loader_t
{
    elf_parser_t *parser;
    char *Image;
    void *handle;
};

//////////////////////////////////////////
//
//          READER
//          reader.c
//
/////////////////////////////////////////

size_t read_file(const char *filename, elf_data_t *data);
void free_file(elf_data_t *data);

//////////////////////////////////////////
//
//          PARSER
//          parser.c
//
/////////////////////////////////////////

int elf_parse(elf_data_t *data, elf_parser_t *parser);
Elf32_Shdr *get_section_global_sym_table(elf_parser_t *parser);
Elf32_Shdr *get_section_sym_table(elf_parser_t *parser);
Elf32_Shdr *get_section_str_tab(elf_parser_t *parser);
Elf32_Shdr *get_section_by_name(elf_parser_t *parser, const char *name);
void *get_symbol(elf_parser_t *parser, const char *sym_name, char *Image);


//////////////////////////////////////////
//
//          LOADER
//          loader.c
//
/////////////////////////////////////////

Elf32_Addr *load_elf(elf_loader_t *loader, elf_parser_t *parser);
void unload_elf(elf_loader_t *loader);
void relocate(Elf32_Shdr* section, Elf32_Sym* dynsym, char* strings, char* data, char* image, void *handle);
void *resolve_sym(void *handle, const char *sym_name);