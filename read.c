#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TYPE_NUM 6
#define MACHINE_NUM 6

char *readFileToBuff(char *filename)
{
    FILE *pFile;
    long lSize;
    char *buffer;
    size_t result;

    /* 若要一个byte不漏地读入整个文件，只能采用二进制方式打开 */
    pFile = fopen(filename, "rb");
    if (pFile == NULL)
    {
        fputs("File error", stderr);
        exit(1);
    }

    /* 获取文件大小 */
    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);

    /* 分配内存存储整个文件 */
    buffer = (char *)malloc(sizeof(char) * lSize);
    if (buffer == NULL)
    {
        fputs("Memory error", stderr);
        exit(2);
    }

    /* 将文件拷贝到buffer中 */
    result = fread(buffer, 1, lSize, pFile);
    if (result != lSize)
    {
        fputs("Reading error", stderr);
        exit(3);
    }
    /* 结束演示，关闭文件并释放内存 */
    fclose(pFile);
    free(buffer);
    return buffer;
}
void readELFHeader(char *file)
{
    char *buff = readFileToBuff(file);
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)buff;
    unsigned char *magic = (unsigned char *)buff;
    printf("%d", ehdr->e_machine);
    printf("ELF Header:\n");
    printf("Class: \t\t\t");
    switch (magic[EI_CLASS])
    {
    case ELFCLASSNONE:
        printf("Invalid class\n");
        break;
    case ELFCLASS32:
        printf("32-bit objects\n");
        break;
    case ELFCLASS64:
        printf("64-bit objects\n");
        break;
    default:
        break;
    }
    printf("Data: \t\t\t");
    switch (magic[EI_DATA])
    {
    case ELFDATANONE:
        printf("Invalid data encoding\n");
        break;
    case ELFDATA2LSB:
        printf("2's complement, little endian\n");
        break;
    case ELFDATA2MSB:
        printf("2's complement, little endian\n");
        break;
    default:
        break;
    }
    printf("OS ABI identification: \t\t\t");
    switch (magic[EI_OSABI])
    {
    case ELFOSABI_NONE:
        printf("UNIX System V ABI\n");
        break; /* UNIX System V ABI */
    case ELFOSABI_HPUX:
        printf("HP-UX\n");
        break; /* HP-UX */
    case ELFOSABI_NETBSD:
        printf("NetBSD.\n");
        break; /* NetBSD.  */
    case ELFOSABI_GNU:
        printf("Object uses GNU ELF extensions.\n");
        break; /* Object uses GNU ELF extensions.  */
    case ELFOSABI_SOLARIS:
        printf("Sun Solaris.\n");
        break; /* Sun Solaris.  */
    case ELFOSABI_AIX:
        printf("IBM AIX.\n");
        break; /* IBM AIX.  */
    case ELFOSABI_IRIX:
        printf("SGI Irix.\n");
        break; /* SGI Irix.  */
    case ELFOSABI_FREEBSD:
        printf("FreeBSD.\n");
        break; /* FreeBSD.  */
    case ELFOSABI_TRU64:
        printf("Compaq TRU64 UNIX.\n");
        break; /* Compaq TRU64 UNIX.  */
    case ELFOSABI_MODESTO:
        printf("Novell Modesto.\n");
        break; /* Novell Modesto.  */
    case ELFOSABI_OPENBSD:
        printf("OpenBSD.\n");
        break; /* OpenBSD.  */
    case ELFOSABI_ARM_AEABI:
        printf("ARM EABI");
        break; /* ARM EABI */
    case ELFOSABI_ARM:
        printf("ARM");
        break; /* ARM */
    case ELFOSABI_STANDALONE:
        printf("Standalone (embedded) application");
        break; /* Standalone (embedded) application */
    }
    printf("Version: \t\t\t %d\n", magic[EI_VERSION]);
    printf("ABI Version: \t\t\t %d\n", magic[EI_ABIVERSION]);
    printf("Type:\t\t\t ");
    switch (ehdr->e_type)
    {
    case ET_NONE:
        printf("No file type \n");
        break;
    case ET_REL:
        printf("Relocatable file \n");
        break;
    case ET_EXEC:
        printf("Executable file \n");
        break;
    case ET_DYN:
        printf("Shared object file \n");
        break;
    case ET_CORE:
        printf("Core file \n");
        break;
    case ET_NUM:
        printf("Number of defined types \n");
        break;
    default:
        break;
    }
    printf("Machine:\t\t\t ");
    switch (ehdr->e_machine)
    {
    case EM_NONE:
        printf("AMD x86-64 architecture\n");
        break;
    case EM_X86_64:
        printf("AMD x86-64 architecture\n");
        break;
    case EM_AARCH64:
        printf("ARM AARCH64\n");
        break;
    default:
        printf("unsupported");
        break;
    }
    printf("Version:\t\t\t ");
    switch (ehdr->e_version)
    {
    case EV_NONE:
        printf("Invalid ELF version\n");
        break;
    case EV_CURRENT:
        printf("Current version\n");
        break;
    default:
        printf("unsupported");
        break;
    }
    printf("Entry point address:\t\t\t 0x%lx\n", ehdr->e_entry);
    printf("Start of program header:\t\t\t %ld\n", ehdr->e_phoff);
    printf("Start of section headers:\t\t\t %ld\n", ehdr->e_shoff);
    printf("Flags:\t\t\t 0x%x\n", ehdr->e_flags);
    printf("Size of ELF headers:\t\t\t %d\n", ehdr->e_ehsize);
    printf("Number of program headers:\t\t\t %d\n", ehdr->e_phnum);
    printf("Size of program headers:\t\t\t %d\n", ehdr->e_phentsize);
    printf("Number of section headers:\t\t\t %d\n", ehdr->e_shnum);
    printf("Size of section headers:\t\t\t %d\n", ehdr->e_shentsize);
    printf("Section header string table index:\t\t\t %d\n", ehdr->e_shstrndx);
}
void readSections(char *file)
{
    char *buff = readFileToBuff(file);
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)buff;
    printf("%-20s%-20s%-20s%-20s%-20s%-20s%-20s%-20s%-20s%-20s", "Name", "Type", "Address", "Offset", "Size", "EntSize", "Flags", "Link", "Info", "Align");
    Elf64_Shdr *shdr = (Elf64_Shdr *)(buff + ehdr->e_shoff);
    char *shstrtab = buff + shdr[ehdr->e_shstrndx].sh_offset;
    for (int i = 0; i < ehdr->e_shnum; i++)
    {
        printf("%-20s", (shstrtab + shdr[i].sh_name));
        switch (shdr[i].sh_type)
        {
        case SHT_NULL:
            printf("%-20s", "NULL");
            break;
        case SHT_PROGBITS:
            printf("%-20s", "PROGBITS");
            break;
        case SHT_SYMTAB:
            printf("%-20s", "SYMTAB");
            break;
        case SHT_STRTAB:
            printf("%-20s", "STRTAB");
            break;
        case SHT_RELA:
            printf("%-20s", "RELA");
            break;
        case SHT_HASH:
            printf("%-20s", "HASH");
            break;
        case SHT_DYNAMIC:
            printf("%-20s", "DYNAMIC");
            break;
        case SHT_NOTE:
            printf("%-20s", "NOTE");
            break;
        case SHT_NOBITS:
            printf("%-20s", "NOBITS");
            break;
        case SHT_REL:
            printf("%-20s", "REL");
            break;
        case SHT_SHLIB:
            printf("%-20s", "SHLIB");
            break;
        case SHT_DYNSYM:
            printf("%-20s", "DYNSYM");
            break;
        case SHT_INIT_ARRAY:
            printf("%-20s", "INIT_ARRAY");
            break;
        case SHT_FINI_ARRAY:
            printf("%-20s", "FINI_ARRAY");
            break;
        case SHT_PREINIT_ARRAY:
            printf("%-20s", "PREINIT_ARRAY");
            break;
        case SHT_GROUP:
            printf("%-20s", "GROUP");
            break;
        case SHT_SYMTAB_SHNDX:
            printf("%-20s", "SYMTAB_SHNDX");
            break;
        case SHT_NUM:
            printf("%-20s", "NUM");
            break;
        case SHT_LOOS:
            printf("%-20s", "LOOS");
            break;
        case SHT_GNU_ATTRIBUTES:
            printf("%-20s", "GNU_ATTRIBUTES");
            break;
        case SHT_GNU_HASH:
            printf("%-20s", "GNU_HASH");
            break;
        case SHT_GNU_LIBLIST:
            printf("%-20s", "GNU_LIBLIST");
            break;
        case SHT_CHECKSUM:
            printf("%-20s", "CHECKSUM");
            break;
        case SHT_LOSUNW:
            printf("%-20s", "LOSUNW");
            break;
        case SHT_SUNW_COMDAT:
            printf("%-20s", "SUNW_COMDAT");
            break;
        case SHT_SUNW_syminfo:
            printf("%-20s", "SUNW_syminfo");
            break;
        case SHT_GNU_verdef:
            printf("%-20s", "GNU_verdef");
            break;
        case SHT_GNU_verneed:
            printf("%-20s", "GNU_verneed");
            break;
        case SHT_GNU_versym:
            printf("%-20s", "GNU_versym");
            break;
        case SHT_LOPROC:
            printf("%-20s", "LOPROC");
            break;
        case SHT_HIPROC:
            printf("%-20s", "HIPROC");
            break;
        case SHT_LOUSER:
            printf("%-20s", "LOUSER");
            break;
        case SHT_HIUSER:
            printf("%-20s", "HIUSER");
            break;

        default:
            break;
        }
        printf("%-20lx", shdr[i].sh_addr);
        printf("%-20lx", shdr[i].sh_offset);
        printf("%-20lx", shdr[i].sh_size);
        printf("%-20lx", shdr[i].sh_entsize);
        printf("%-20lx", shdr[i].sh_flags);
        printf("%-20x", shdr[i].sh_link);
        printf("%-20d", shdr[i].sh_info);
        printf("%-20lx\n", shdr[i].sh_addralign);
    }
}
void readSymbolTab(char *file)
{
    char *buff = readFileToBuff(file);
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)buff;
    Elf64_Shdr *shdr = (Elf64_Shdr *)(buff + ehdr->e_shoff);
    char *strtab = buff + shdr[ehdr->e_shstrndx].sh_offset;

    for (int i = 0; i < ehdr->e_shnum; i++)
    {
        switch (shdr[i].sh_type)
        {
        case SHT_DYNSYM:
        case SHT_SYMTAB:
        {
            printf("%s ", strtab + shdr[i].sh_name);
            printf("%lx  ", shdr[i].sh_offset);
            printf("\n");
            printf("%-5s%-20s%-20s%-20s%-20s%-20s%-20s\n", "Num", "Value", "Size", "Type", "Bind", "Vis", "Name");
            char *shstrtab = buff + shdr[shdr[i].sh_link].sh_offset;
            Elf64_Sym *sym = (Elf64_Sym *)(buff + shdr[i].sh_offset);
            for (int j = 0; j < shdr[i].sh_size / shdr[i].sh_entsize; j++)
            {
                printf("[%d]  ", j);
                printf("%-20lx ", sym[j].st_value);
                printf("%-20ld ", (sym[j].st_size));

                switch (ELF64_ST_TYPE(sym[j].st_info))
                {
                case STT_NOTYPE: /* Symbol type is unspecified */
                {
                    printf("%-20s", "NOTYPE");
                }
                break;
                case STT_OBJECT: /* Symbol is a data object */
                {
                    printf("%-20s", "OBJECT");
                }
                break;
                case STT_FUNC: /* Symbol is a code object */
                {
                    printf("%-20s", "FUNC");
                }
                break;
                case STT_SECTION: /* Symbol associated with a section */
                {
                    printf("%-20s", "SECTION");
                }
                break;
                case STT_FILE: /* Symbol's name is file name */
                {
                    printf("%-20s", "FILE");
                }
                break;
                case STT_COMMON: /* Symbol is a common data object */
                {
                    printf("%-20s", "COMMON");
                }
                break;
                case STT_TLS: /* Symbol is thread-local data object*/
                {
                    printf("%-20s", "TLS");
                }

                break;
                case STT_NUM: /* Number of defined types.  */
                {
                    printf("%-20s", "NUM");
                }
                break;
                case STT_LOOS: /* Start of OS-specific */
                {
                    printf("%-20s", "LOOS");
                }
                break;
                case STT_HIOS: /* End of OS-specific */
                {
                    printf("%-20s", "HIOS");
                }
                break;
                case STT_LOPROC: /* Start of processor-specific */
                {
                    printf("%-20s", "LOPROC");
                }
                break;
                case STT_HIPROC: /* End of processor-specific */
                {
                    printf("%-20s", "HIPROC");
                }
                break;
                default:
                    break;
                }
                switch (ELF64_ST_BIND(sym[j].st_info))
                {
                case STB_LOCAL:
                {
                    printf("%-20s", "LOCAL");
                }
                break;
                case STB_GLOBAL:
                {
                    printf("%-20s", "GLOBAL");
                }
                break;
                case STB_WEAK:
                {
                    printf("%-20s", "WEAK");
                }
                break;
                case STB_NUM:
                {
                    printf("%-20s", "NUM");
                }
                break;
                case STB_LOOS:
                {
                    printf("%-20s", "LOOS");
                }
                break;
                case STB_HIOS:
                {
                    printf("%-20s", "HIOS");
                }

                break;
                case STB_LOPROC:
                {
                    printf("%-20s", "LOPROC");
                }
                break;
                case STB_HIPROC:
                {
                    printf("%-20s", "HIPROC");
                }
                break;
                default:
                    break;
                }
                switch (ELF64_ST_VISIBILITY(sym[j].st_other))
                {
                case STV_DEFAULT:
                {
                    printf("%-20s", "DEFAULT");
                }
                break;
                case STV_INTERNAL:
                {
                    printf("%-20s", "INTERNAL");
                }
                break;
                case STV_HIDDEN:
                {
                    printf("%-20s", "HIDDEN");
                }
                break;
                case STV_PROTECTED:
                {
                    printf("%-20s", "PROTECTED");
                }
                break;

                default:
                    break;
                }

                printf("%-20s ", (shstrtab + sym[j].st_name));
                printf("\n");
            }
        }
        break;
        default:
            break;
        }
    }
}
int main(int argc, char *argv[])
{
    int opt = 0;

    while ((opt = getopt(argc, argv, "h:s:S:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            readELFHeader(optarg);
            break;
        case 'S':
            readSections(optarg);
            break;
        case 's':
            readSymbolTab(optarg);
            break;
        }
    }
    // readELFHeader();
    // readSections();
    // readSymbolTab();
    return 0;
}