
/*
 * pci.c
 * ver.0.1:  Dec 18, 2011  S.Ishihara
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>

#define PCI_INDEX 0xcf8
#define PCI_DATA  0xcfc

unsigned char
pciRead8(unsigned char bus, unsigned char dev, unsigned char fnc, unsigned char reg)
{
    outl((1 << 31) + (bus << 16) + ((dev & 0x1f) << 11) +\
            ((fnc & 0x07) << 8) + (reg & ~(0x03)), PCI_INDEX);
    return inb(PCI_DATA + (reg & 0x03));
}

unsigned short
pciRead16(unsigned char bus, unsigned char dev, unsigned char fnc, unsigned char reg)
{
    outl((1 << 31) + (bus << 16) + ((dev & 0x1f) << 11) +\
            ((fnc & 0x07) << 8) + (reg & ~(0x03)), PCI_INDEX);
    return inw(PCI_DATA + (reg & 0x02));
}

unsigned int
pciRead32(unsigned char bus, unsigned char dev, unsigned char fnc, unsigned char reg)
{
    outl((1 << 31) + (bus << 16) + ((dev & 0x1f) << 11) +\
            ((fnc & 0x07) << 8) + (reg & ~(0x03)), PCI_INDEX);
    return inl(PCI_DATA);
}

void
pciWrite8(unsigned char bus, unsigned char dev, unsigned char fnc, unsigned char reg, unsigned char value)
{
    outl((1 << 31) + (bus << 16) + ((dev & 0x1f) << 11) +\
            ((fnc & 0x07) << 8) + (reg & ~(0x03)), PCI_INDEX);
    outb(value, PCI_DATA + (reg & 0x03));
}

void
pciWrite16(unsigned char bus, unsigned char dev, unsigned char fnc, unsigned char reg, unsigned short value)
{
    outl((1 << 31) + (bus << 16) + ((dev & 0x1f) << 11) +\
            ((fnc & 0x07) << 8) + (reg & ~(0x03)), PCI_INDEX);
    outw(value, PCI_DATA + (reg & 0x02));
}

void
pciWrite32(unsigned char bus, unsigned char dev, unsigned char fnc, unsigned char reg, unsigned int value)
{
    outl((1 << 31) + (bus << 16) + ((dev & 0x1f) << 11) +\
            ((fnc & 0x07) << 8) + (reg & ~(0x03)), PCI_INDEX);
    outl(value, PCI_DATA);
}


int
main(int argc, char *argv[])
{
    int             opt;
    extern char     *optarg;
    extern int      optind, opterr;
    int             width = 1;  /* default byte access */
    unsigned char   bus, dev, fnc, reg;
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

    if (argc == 4) {
        /* Read Pci */
        bus = strtoul(argv[0], NULL, 16);
        dev = strtoul(argv[1], NULL, 16);
        fnc = strtoul(argv[2], NULL, 16);
        reg = strtoul(argv[3], NULL, 16);
        if (width == 1) {
            printf("B:0x%02x/ D:0x%02x/ F:0x%02x/ R:0x%02x: 0x%02x\n",
                    bus, dev, fnc, reg, pciRead8(bus, dev, fnc, reg));
        } else if (width == 2) {
            printf("B:0x%02x/ D:0x%02x/ F:0x%02x/ R:0x%02x: 0x%04x\n",
                    bus, dev, fnc, reg, pciRead16(bus, dev, fnc, reg));
        } else if (width == 4) {
            printf("B:0x%02x/ D:0x%02x/ F:0x%02x/ R:0x%02x: 0x%08x\n",
                    bus, dev, fnc, reg, pciRead32(bus, dev, fnc, reg));
        } else {
            goto error;
        }
    } else if (argc == 5) {
        /* Write Pci */
        bus   = strtoul(argv[0], NULL, 16);
        dev   = strtoul(argv[1], NULL, 16);
        fnc   = strtoul(argv[2], NULL, 16);
        reg   = strtoul(argv[3], NULL, 16);
        wdata = strtoul(argv[4], NULL, 16);
        if (width == 1) {
            pciWrite8(bus, dev, fnc, reg, wdata);
        } else if(width == 2) {
            pciWrite16(bus, dev, fnc, reg, wdata);
        } else if(width == 4) {
            pciWrite32(bus, dev, fnc, reg, wdata);
        } else {
            goto error;
        }
    } else {
        goto error;
    }
    return 0;

error:
    printf("Usage: pci [-w WIDTH] BUS DEVICE FUNCTION REGISTER [DATA]\n"
            "Pci configuration space read or write.\n"
            "  -w        number of byte width. permit 1(default), 2, 4\n"
            "\n"
            "This command executable only root user.\n"
            "\n"
            "Examples:\n"
            "  pci 00 1f 00 00          Read pci from bus 0x00 dev 0x1f func 0x00 reg 0x00.\n"
            "  pci -w2 00 1f 00 10 500  Write pci bus 0x00 dev 0x1f func 0x00 reg 0x10 to 0x500.\n"
            "\n");
    return 1;
}

