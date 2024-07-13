# EXT2 Corruption Recovery Program

This repository contains a corruption recovery program for the ext2 filesystem written in C++. The program can recover corrupted inode and block bitmaps. Additionally, it provides a utility to print the entire directory structure of the filesystem as a tree.


## Features

- Bitmap recovery
  - Inode bitmap recovery: Ensures that all used inodes are correctly marked in the inode bitmap.
  - Block bitmap recovery: Ensures that all used blocks are correctly marked in the block bitmap.
  
- Filesystem Tree Printing:
  - Prints the entire directory structure of the filesystem as a tree with depth, providing a visual representation of the filesystem hierarchy.

## Usage

1. Clone the repository:
```bash
git clone https://github.com/ramazantokay/EXT2-Corruption-Recovery-Program.git
```

2. Go to the project directory:
```bash
cd EXT2-Corruption-Recovery-Program/src
```

3. Compile the program:
```bash
make all
```

4. Run the program:
```bash
./recext2fs <image_location> <data_identifier>
```

- `<image_location>`: The location of the ext2 filesystem image file.
- `<data_identifier>`: The 32-byte identifier located at the start of each non-empty data block containing user file data.

## Testing

You can test the program by using the provided test image files in the `test_images` directory. 

## Utility Scripts

The `scripts` directory contains utility scripts to help with testing and debugging the program.

- `image_creation.sh`: Creates an ext2 filesystem image file with the given name. You can change the image size and block size by modifying the script.
- `dump_image.sh`: Dumps the contents of the ext2 filesystem image file to the terminal.
- `mount_image.sh`: Mounts the ext2 filesystem image file to given directory.
- `unmount_image.sh`: Unmounts the ext2 filesystem image file from the given directory.
- `create_files.sh`: Creates files in the mounted ext2 filesystem image file for testing purposes. The files are created in the directory specified by the variable `MOUNT_POINT` in the script. You can change the number of files and their sizes by modifying the script.

## Example
Here is an example output of the directory tree structure printed by the program:


```bash
- root/
-- home/
--- user1/
---- file1.txt
---- file2.txt
--- user2/
---- file3.txt
-- etc/
--- config.txt
```

## Disclaimer

This project is for educational purposes only. It is provided "as is" without warranty of any kind, express or implied. The authors are not responsible for any damage or data loss that may occur as a result of using this software. Use it at your own risk and always ensure you have appropriate backups and safeguards in place.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.


## Resources for Further Reading

- [ext2 documentation by Dave Poirier](https://www.nongnu.org/ext2-doc/ext2.html)
- [OSDev wiki article on ext2](https://wiki.osdev.org/Ext2)
- [Design and Implementation of the Second Extended Filesystem](https://web.mit.edu/tytso/www/linux/ext2intro.html)
