# SPI flash identification

This firmware can be flashed on a pico board (or a clone) to identify its flash chip. It dumps its JEDEC identifier and some SFDP parameters every 5 seconds.

## Build instructions

This project makes modifications to board definitions to embed the generic SPI boot2. Modified board files are provided for `pico` and `pico_w`.

```
mkdir build
cd build
cmake -DPICO_SDK_PATH=/path/to/pico-sdk-2.0.0 -DPICO_BOARD=pico ..
make
```
