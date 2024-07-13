example-1024.img: Unmodified image

example-1024-baseline.img: Image with the guarantees of the baseline implementation

example-1024-blockbitmap.img: example-1024-baseline.img with deleted block bitmaps

example-1024-inodebitmap.img: example-1024-baseline.img with deleted inode bitmaps

example-1024-bitmap.img: example-1024-baseline.img with deleted bitmaps

identifier: 0x01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

example terminal command: ./recext2fs ./example-1024-bitmap-pointer.img 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00