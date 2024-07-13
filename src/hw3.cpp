#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <sys/stat.h>

#include "utility.h"
#include "ext2fs_print.h"
#include "hw3.h"

#define DEBUG_PRINT 0


unsigned char* disk_image;

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <disk_image> <identifier> \n ", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDWR);
    if (fd < 0)
    {
        perror("Error opening file");
        exit(1);
    }

    struct stat sb;
    if (fstat(fd, &sb) < 0)
    {
        perror("Error fstat");
        exit(1);
    }

    disk_image = (unsigned char*)mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (disk_image == MAP_FAILED)
    {
        perror("Error mmap");
        exit(1);
    }
    // print arguments
    if (DEBUG_PRINT)
    {
        for (int i = 0; i < argc; i++)
        {
            printf("argv[%d]: %s\n", i, argv[i]);
        }
    }

    // DEBUG OPERATIONS
    if (DEBUG_PRINT)
    {

        std::cout << "██████╗ ███████╗██████╗ ██╗   ██╗ ██████╗      ██████╗ ██████╗ ███████╗██████╗  █████╗ ████████╗██╗ ██████╗ ███╗   ██╗███████╗" << std::endl;
        std::cout << "██╔══██╗██╔════╝██╔══██╗██║   ██║██╔════╝     ██╔═══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝" << std::endl;
        std::cout << "██║  ██║█████╗  ██████╔╝██║   ██║██║  ███╗    ██║   ██║██████╔╝█████╗  ██████╔╝███████║   ██║   ██║██║   ██║██╔██╗ ██║███████╗" << std::endl;
        std::cout << "██║  ██║██╔══╝  ██╔══██╗██║   ██║██║   ██║    ██║   ██║██╔═══╝ ██╔══╝  ██╔══██╗██╔══██║   ██║   ██║██║   ██║██║╚██╗██║╚════██║" << std::endl;
        std::cout << "██████╔╝███████╗██████╔╝╚██████╔╝╚██████╔╝    ╚██████╔╝██║     ███████╗██║  ██║██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║███████║" << std::endl;
        std::cout << "╚═════╝ ╚══════╝╚═════╝  ╚═════╝  ╚═════╝      ╚═════╝ ╚═╝     ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝" << std::endl;
    }

    uint8_t* identifier = parse_identifier(argc, argv);
    if (DEBUG_PRINT)
    {

        for (int i = 0; i < argc - 2; i++)
        {
            printf("identifier[%d]: %x\n", i, identifier[i]);
        }
    }


    recover_inode_bitmaps();

    recover_block_bitmaps(); // %95 recover block bitmaps, will be fixed in the future

    print_image(&sb);

    
    
    munmap(disk_image, sb.st_size);
    close(fd);
    // delete indentier;

    return 0;
}
