/*
 * io.c
 * ver.0.1:  Dec 18, 2011  S.Ishihara
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>


int
main(int argc, char *argv[])
{
    int             opt;
    extern char     *optarg;
    extern int      optind, opterr;
    int             width = 1;  /* default byte access */
    unsigned short  ioaddr;
    unsigned int    wdata;


    while ((opt = getopt(argc, argv, "w:")) != -1) {
        if (opt == 'w') {
            width = atoi(optarg);
        } else {
            goto error;
        }
    }


    argc -= optind;
    argv += optind;


    iopl(3);


    if (argc == 1) {
        /* Read I/O */
        ioaddr = strtoul(argv[0], NULL, 16);
        if (width == 1) {
            printf("0x%04x: 0x%02x\n", ioaddr, inb(ioaddr));
        } else if (width == 2) {
            printf("0x%04x: 0x%04x\n", ioaddr, inw(ioaddr));
        } else if (width == 4) {
            printf("0x%04x: 0x%08x\n", ioaddr, inl(ioaddr));
        } else {
            goto error;
        }
    } else if (argc == 2) {
        /* Write I/O */
        ioaddr = strtoul(argv[0], NULL, 16);
        wdata  = strtoul(argv[1], NULL, 16);
        if (width == 1) {
            outb(wdata, ioaddr);
        } else if(width == 2) {
            outw(wdata, ioaddr);
        } else if(width == 4) {
            outl(wdata, ioaddr);
        } else {
            goto error;
        }
    } else {
        goto error;
    }
    return 0;


error:
    printf("Usage: io [-w WIDTH] ADDRESS [DATA]\n"
            "I/O read or write.\n"
            "  -w        number of byte width. permit 1(default), 2, 4\n"
            "\n"
            "This command executable only root user.\n"
            "I/O address possible range 16bit.\n"
            "\n"
            "Examples:\n"
            "  io 3f8               Read I/O from address 0x3f8.\n"
            "  io 3f8 0x31          Write I/O address 0x3f8 to 0x31.\n"
            "  io -w4 cf8 80000000  Write I/O address 0xcf8 to 0x80000000 double word.\n"
            "\n");
    return 1;
}


