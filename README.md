# Ratchad

A utility program intended to be used on **wrench-only runs** of *Ratchet & Clank* (2002).  
Bomb Glove ammo is automatically converted and added to your bolt balance at vendor prices (**5 bolts each**).

## Requirements

- Windows 10/11  
- `RPCS3.exe`  
- An NPEA00385 copy of *Ratchet & Clank*  

## Installation

Either download the latest `Ratchad.exe` from the **Releases** tab, or build it yourself using:

```bash
gcc -m64 main.c -o Ratchad.exe -lws2_32
