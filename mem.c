/*
 * mem.c
 * ver.0.1:  Dec 26, 2011  S.Ishihara
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>




#define DEV_PATH "/dev/mem"


int
main(int argc, char *argv[])
{
    int             opt;
    extern char     *optarg;
    extern int      optind, opterr;
    int             width = 1;  /* default byte access */
    unsigned int    memaddr, wdata;
    unsigned int    pgoffset, pgaddr;
    unsigned int    pagesize = sysconf(_SC_PAGESIZE);
    unsigned char   *p;
    int             fd;


    while ((opt = getopt(argc, argv, "w:")) != -1) {
        if (opt == 'w') {
            width = atoi(optarg);
        } else {
            goto error;
        }
    }


    argc -= optind;
    argv += optind;


    fd = open(DEV_PATH, O_RDWR);
    if (fd <= 0) {
        fprintf(stderr, "open error: %s\n", DEV_PATH);
        return 1;
    }


    if (argc == 1) {
        /* Read Mem */
        memaddr  = strtoul(argv[0], NULL, 16);
        pgoffset = memaddr & (pagesize -1);
        pgaddr   = memaddr & ~(pagesize -1);
        p = mmap(NULL, pagesize, PROT_READ, MAP_SHARED, fd, pgaddr);
        if (p < 0) {
            fprintf(stderr, "mmap error\n");
            return 1;
        }
        if (width == 1) {
            printf("0x%08x: 0x%02x\n", memaddr, *(p + pgoffset));
        } else if (width == 2) {
            printf("0x%08x: 0x%04x\n", memaddr, *((unsigned short *)(p + pgoffset)));
        } else if (width == 4) {
            printf("0x%08x: 0x%08x\n", memaddr, *((unsigned int *)(p + pgoffset)));
        } else {
            goto error;
        }
    } else if (argc == 2) {
        /* Write Mem */
        memaddr  = strtoul(argv[0], NULL, 16);
        pgoffset = memaddr & (pagesize -1);
        pgaddr   = memaddr & ~(pagesize -1);
        p = mmap(NULL, pagesize, PROT_WRITE, MAP_SHARED, fd, pgaddr);
        if (p < 0) {
            fprintf(stderr, "mmap error\n");
            return 1;
        }
        wdata  = strtoul(argv[1], NULL, 16);
        if (width == 1) {
            *(p + pgoffset) = (unsigned char)wdata;
        } else if(width == 2) {
            *((unsigned short *)(p + pgoffset)) = (unsigned short)wdata;
        } else if(width == 4) {
            *((unsigned int *)(p + pgoffset)) = (unsigned int)wdata;
        } else {
            goto error;
        }
    } else {
        goto error;
    }
    munmap(p, pagesize);
    close(fd);
    return 0;


error:
    printf("Usage: Mem [-w WIDTH] ADDRESS [DATA]\n"
            "Mem read or write.\n"
            "  -w        number of byte width. permit 1(default), 2, 4\n"
            "\n"
            "This command executable only root user.\n"
            "Mem address possible range 32bit.\n"
            "\n"
            "Examples:\n"
            "  Mem a0000               Read memory from address 0xa0000.\n"
            "  Mem a0000 31            Write memory address 0xa0000 to 0x31.\n"
            "  Mem -w4 20000 5a5a5a5a  Write memory address 0x20000 to 0x5a5a5a5a.\n"
            "\n");
    return 1;
}
