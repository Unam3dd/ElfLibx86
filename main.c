#include "elf_hdr.h"
#include <stdio.h>

int main(int argc, char **argv, char **envp)
{
    elf_data_t data;
    elf_parser_t parser;
    elf_loader_t loader;

    if (!read_file(argv[1], &data))
        fprintf(stderr, "[-] Error file not found !\n");
    
    if (elf_parse(&data, &parser))
        goto error;

    printf("%04x\n", parser.elf_hdr->e_entry);
    
    int (*jump_entrypoint)(int, char **, char **);

    jump_entrypoint = load_elf(&loader, &parser);

    if (jump_entrypoint)
        jump_entrypoint(argc, argv, envp);
    
    unload_elf(&loader);
    
    free_file(&data);

    return (0);

    error:
        fprintf(stderr, "[-] Error !\n");
        free_file(&data);
        return (1);
}