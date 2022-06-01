# BitBuster 2 - (c) 2003-2004,2022 Team Bomba
BitBuster 2 aims to generate fairly well compressed files that can be decompressed very fast on 8-bit (mainly Z80) home computers.

BitBuster 2 is a new version of BitBuster that has never been officially released before (although it was included in the tool BMP2G9B). It was originally named BitBuster 1.3 but has been renamed to BitBuster 2 because it's not compatible with BitBuster 1.2.

## Improvements in BitBuster 2
- Faster compressor
- Faster decompressor
- Block-mode to easily handle big files

## How to use

```
BitBuster2.exe [-hoad] [-e <extension>] [-b <size>] <filename> [<filename>...]

-h Show the help text
-a Append extension instead of replacing it
-o Forces every second filename to be treated as the output filename for the filename preceding it
-d Decompress data instead of compressing
-e <extension> Set the extension used for (de)compresed file(s)
   Default extension for compressed files is .pck
   Default extension for decompressed files is .org
-b Set size of data to be compressed per block [128-32768]
   Default block size is 32768
```

## Decompression
Include the file bitbuster.asm in your project to easily decompress data.

There are two functions that can be used:
- depack : The standard function to decompress data. If data is split up in multiple blocks and all the data is loaded in memory this function can be called multiple times to decompress each block.
- depack_block : Decompress one block of data. Use this if data has been split up in multiple blocks but can't be loaded in memory all at once.

The decompressor can be optimized for speed by enabling the option BITBUSTER_OPTIMIZE_SPEED. Some code will be inlined and/or unrolled to gain more speed (~15%) at the expense of some memory (203 bytes instead of 133 bytes).

In example/example.asm you can find an example of an MSX-program that loads a compressed SCREEN 8 image into VRAM.


## Benchmark - Compressed Size
File sizes in bytes.

|File|Original|BitBuster 1.2|BitBuster 2|ZX0|
|--|--|--|--|--|
|bitbuster.asm|7796|2962|2951|2792|
|BitBuster2.exe|42496|21728|21710|18503|
|compress.cpp|10449|4099|4079|3756|
|intro4.ge5|30375|4133|4118|3758|
|level4.ge5|30375|7942|7914|7276|
|multi6.mbm|7833|2160|2145|1788|
|multi6.mwm|11134|2980|2948|2386|
|pack.exe|12288|5432|5425|4964|
|skooted.bin|5357|4116|4110|3879|
|zx0.exe|18432|9296|9280|8437|

## Benchmark - Compression Speed
Times in seconds, tested on i3-10100 @ 3.60GHz.

|File|BitBuster 1.2|BitBuster 2|ZX0|
|--|--|--|--|
|bitbuster.asm|0.199|0.002|0.53|
|BitBuster2.exe|0.75|0.011|50.71|
|compress.cpp|0.325|0.002|1.03|
|intro4.ge5|0.369|0.056|6.97|
|level4.ge5|0.63|0.016|16.17|
|multi6.mbm|0.139|0.004|0.41|
|multi6.mwm|0.219|0.004|0.94|
|pack.exe|0.209|0.002|1.33|
|skooted.bin|0.109|0.001|0.17|
|zx0.exe|0.35|0.005|3.24|

## Benchmark - Decompression Speed
Times in seconds, tested on MSX2 @ 3.58Mhz. Using BitBuster 2 decompressor optimized for speed, ZX0 Turbo decompressor.

|File|BitBuster 1.2|BitBuster 2|ZX0|
|--|--|--|--|
|bitbuster.asm|0.24|0.2|0.18|
|BitBuster2.exe|1.22|1.12|0.94|
|compress.cpp|0.34|0.3|0.26|
|intro4.ge5|0.52|0.44|0.4|
|level4.ge5|0.74|0.62|0.54|
|multi6.mbm|0.20|0.16|0.14|
|multi6.mwm|0.28|0.22|0.18|
|pack.exe|0.34|0.3|0.26|
|skooted.bin|0.2|0.16|0.16|
|zx0.exe|0.56|0.48|0.42|
