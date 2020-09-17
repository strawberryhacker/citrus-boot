## Summary

Second stage bootloader / program loader for the Cinnamon® OS

## Building

Some packages is required in order to build s-boot. I recomend using Ubuntu, either native or in WSL. 

```
> sudo apt update -y
> sudo apt install build-essential
> sudo apt install -y gcc-arm-none-eabi
```

Then the c-boot binary can be built

```
> git clone https://github.com/strawberryhacker/c-boot
> cd c-boot
> make
```

This will generate a binary called **boot.bin** in the build/ folder. Place this file in the **root** directory of a FAT formatted SD card and plug it into the board.

## Use

See the Cinnamon® OS readme https://github.com/strawberryhacker/cinnamon
