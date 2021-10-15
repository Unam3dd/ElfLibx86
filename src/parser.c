#include "elf_hdr.h"
#include <string.h>
#include <dlfcn.h>

//////////////////////////////////////////
//
//          PARSER
//          parser.c
//
/////////////////////////////////////////

int elf_parse(elf_data_t *data, elf_parser_t *parser)
{
    parser->data = data;
    parser->elf_hdr = (Elf32_Ehdr *) (data->data);

    if (!IS_ELF(parser->elf_hdr->e_ident))
        return (1);

    parser->program_hdr = (Elf32_Phdr *) (data->data + parser->elf_hdr->e_phoff);

    parser->section_hdr = (Elf32_Shdr *) (data->data + parser->elf_hdr->e_shoff);
    
    parser->sct_str = (char *) (data->data + parser->section_hdr[parser->elf_hdr->e_shstrndx].sh_offset);

    parser->str_tab = get_section_str_tab(parser);

    parser->dynsym = get_section_global_sym_table(parser);

    parser->symtab = get_section_sym_table(parser);

    return (0);
}


Elf32_Shdr *get_section_global_sym_table(elf_parser_t *parser)
{
    for (uint16_t i = 0; i < parser->elf_hdr->e_shnum; i++) 
    {
        if (parser->section_hdr[i].sh_type == SHT_DYNSYM)
            return (&parser->section_hdr[i]);
    }

    return (NULL);
}

Elf32_Shdr *get_section_sym_table(elf_parser_t *parser)
{
    for (uint16_t i = 0; i < parser->elf_hdr->e_shnum; i++) 
    {
        if (parser->section_hdr[i].sh_type == SHT_SYMTAB)
            return (&parser->section_hdr[i]);
    }

    return (NULL);
}

Elf32_Shdr *get_section_str_tab(elf_parser_t *parser)
{
    for (uint16_t i = 0; i < parser->elf_hdr->e_shnum; i++) 
    {
        if (parser->section_hdr[i].sh_type == SHT_STRTAB)
            return (&parser->section_hdr[i]);
    }

    return (NULL);
}

Elf32_Shdr *get_section_by_name(elf_parser_t *parser, const char *name)
{
    for (uint16_t i = 0; i < parser->elf_hdr->e_shnum; i++) 
    {
        if (strcmp(name, (parser->sct_str + parser->section_hdr[i].sh_name)) == 0)
            return (&parser->section_hdr[i]);
    }

    return (NULL);
}

void *get_symbol(elf_parser_t *parser, const char *sym_name, char *Image)
{
    Elf32_Sym *symtab = (Elf32_Sym *) (parser->data->data + parser->symtab->sh_offset);
    char *sym_str = (char *) (parser->data->data + parser->section_hdr[parser->symtab->sh_link].sh_offset);

    for (int i = 0; i < parser->symtab->sh_size / sizeof(Elf32_Sym); i++) {

        if (strcmp((sym_str + symtab[i].st_name), sym_name) == 0)
            return (Image + symtab[i].st_value);
    }

    return (NULL);
}