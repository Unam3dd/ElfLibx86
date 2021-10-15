#include "elf_hdr.h"
#include <sys/mman.h>
#include <dlfcn.h>
#include <string.h>
#include <gnu/lib-names.h>

//////////////////////////////////////////
//
//          LOADER
//          loader.c
//
/////////////////////////////////////////

Elf32_Addr *load_elf(elf_loader_t *loader, elf_parser_t *parser)
{
    if (parser == NULL || loader == NULL)
        return (NULL);
    
    loader->parser = parser;
    uint16_t i = 0;
    char *src = NULL, *dst = NULL;
    
    loader->Image = (char *) mmap(NULL, parser->data->file_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    memset(loader->Image, 0, parser->data->file_size);

    for (; i < parser->elf_hdr->e_phnum; i++) {
        
        if (parser->program_hdr[i].p_type != PT_LOAD || !parser->program_hdr[i].p_memsz)
            continue;
        
        src = (char *) (parser->data->data + parser->program_hdr[i].p_offset);
        dst = (char *) (loader->Image + parser->program_hdr[i].p_vaddr);

        memcpy(dst, src, parser->program_hdr[i].p_memsz);

        if (!(parser->program_hdr[i].p_flags & PF_W))
            mprotect(dst, parser->program_hdr[i].p_memsz, PROT_READ);
        
        if (parser->program_hdr[i].p_flags & PF_X)
            mprotect(dst, parser->program_hdr[i].p_memsz, PROT_EXEC);
    }

    Elf32_Sym *dynsym = (Elf32_Sym *) (parser->data->data + parser->dynsym->sh_offset);
    char *dynsym_str = (char *) (parser->data->data + parser->section_hdr[parser->dynsym->sh_link].sh_offset);
    
    loader->handle = dlopen(LIBC_SO, RTLD_NOW);

    for (i = 0; i < parser->elf_hdr->e_shnum; i++) {

        if (parser->section_hdr[i].sh_type == SHT_REL)
            relocate(&parser->section_hdr[i], dynsym, dynsym_str, parser->data->data, loader->Image, loader->handle);
    }

    return ((Elf32_Addr *) get_symbol(parser, "main", loader->Image));
}

void unload_elf(elf_loader_t *loader)
{
    if (loader->Image)
        munmap(loader->Image, loader->parser->data->file_size);
    
    dlclose(loader->handle);
    
    loader->Image = NULL;
}

void relocate(Elf32_Shdr* section, Elf32_Sym* dynsym, char* strings, char* data, char* image, void *handle)
{
    Elf32_Rel* rel = (Elf32_Rel*)(data + section->sh_offset);

    for(int i = 0; i < section->sh_size / sizeof(Elf32_Rel); i++)
    {
        char* sym_name = strings + dynsym[ELF32_R_SYM(rel[i].r_info)].st_name;

        if (ELF32_R_TYPE(rel[i].r_info) == R_386_GLOB_DAT || ELF32_R_TYPE(rel[i].r_info) == R_386_JMP_SLOT)
            *(uint32_t *) (image + rel[i].r_offset) = (uint32_t) resolve_sym(handle, sym_name);
    }
}

void *resolve_sym(void *handle, const char *sym_name)
{
    return (dlsym(handle, sym_name));
}