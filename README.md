# Ratchad

A utility program intended to be used on **wrench-only runs** of *Ratchet & Clank* (2002).  
Bomb Glove ammo is automatically converted and added to your bolt balance at vendor prices (**5 bolts each**).

[![Ratchad Demo](https://img.youtube.com/vi/DhFUby_-EJI/0.jpg)](https://youtu.be/DhFUby_-EJI)

## Requirements

- Windows 10/11  
- `RPCS3.exe`  
- An NPEA00385 copy of *Ratchet & Clank*  

## Installation

Either download the latest `Ratchad.exe` from the **Releases** tab, or build it yourself using:

```bash
gcc -m64 main.c ratchad_res.o -o Ratchad.exe -lws2_32
```
If you don't have `gcc` you can get pre-built binaries [`here`](https://winlibs.com/).
