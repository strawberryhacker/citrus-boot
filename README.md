## Summary

Second stage bootloader for the Citrus OS - this readme is deprecated!

## Building

Some packages is required in order to build s-boot. I recomend using Ubuntu, either native or in WSL. 

```
> sudo apt update -y
> sudo apt install build-essential
> sudo apt install -y gcc-arm-none-eabi
```

Then thecitrus-boot binary can be built

```
> git clone https://github.com/strawberryhacker/citrus-boot
> cd citrus-boot
> make
```

This will generate a binary called **boot.bin** in the build/ folder. Place this file in the **root** directory of a FAT formatted SD card and plug it into the board.

## Use

See the Citrus OS readme https://github.com/strawberryhacker/citrusOS
