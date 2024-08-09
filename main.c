#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"

#define FLASH_JEDEC_ID_CMD 0x9f
#define FLASH_JEDEC_ID_DUMMY_BYTES 0
#define FLASH_JEDEC_ID_DATA_BYTES 3
#define FLASH_JEDEC_ID_TOTAL_BYTES (1 + FLASH_JEDEC_ID_DUMMY_BYTES + FLASH_JEDEC_ID_DATA_BYTES)

#define FLASH_SFDP_CMD 0x5a
#define FLASH_SFDP_ADDRESS_BYTES 3
#define FLASH_SFDP_DUMMY_BYTES 1
#define FLASH_SFDP_DATA_BYTES 256
#define FLASH_SFDP_TOTAL_BYTES (1 + FLASH_SFDP_ADDRESS_BYTES + FLASH_SFDP_DUMMY_BYTES + FLASH_SFDP_DATA_BYTES)

#define SFDP_MAGIC 0x50444653


int main() {
    stdio_init_all();

    int countdown = 10;
    while (countdown-- > 0) {
        printf("%d...\n", t);
        sleep_ms(1000);
    }

    // Read JEDEC ID
    printf("Reading JEDEC ID...\n");

    uint8_t jedec_id_out[FLASH_JEDEC_ID_DATA_BYTES];

    uint8_t jedec_id_txbuf[FLASH_JEDEC_ID_TOTAL_BYTES] = {0};
    uint8_t jedec_id_rxbuf[FLASH_JEDEC_ID_TOTAL_BYTES] = {0};
    jedec_id_txbuf[0] = FLASH_JEDEC_ID_CMD;
    flash_do_cmd(jedec_id_txbuf, jedec_id_rxbuf, FLASH_JEDEC_ID_TOTAL_BYTES);
    for (int i = 0; i < FLASH_JEDEC_ID_DATA_BYTES; i++)
        jedec_id_out[i] = jedec_id_rxbuf[i + 1 + FLASH_JEDEC_ID_DUMMY_BYTES];

    printf("JEDEC ID is %02x:%02x:%02x\n", jedec_id_out[0], jedec_id_out[1], jedec_id_out[2]);

    sleep_ms(1000);

    // Read SFDP
    printf("Reading SFDP...\n");

    uint8_t sfdp_out[FLASH_SFDP_DATA_BYTES];

    uint8_t sfdp_txbuf[FLASH_SFDP_TOTAL_BYTES] = {0};
    uint8_t sfdp_rxbuf[FLASH_SFDP_TOTAL_BYTES] = {0};
    sfdp_txbuf[0] = FLASH_SFDP_CMD;
    //sfdp_txbuf[1:3] = 3-bytes address (0x000000)
    flash_do_cmd(sfdp_txbuf, sfdp_rxbuf, FLASH_SFDP_TOTAL_BYTES);
    for (int i = 0; i < FLASH_SFDP_DATA_BYTES; i++)
        sfdp_out[i] = sfdp_rxbuf[i + 1 + FLASH_SFDP_ADDRESS_BYTES + FLASH_SFDP_DUMMY_BYTES];


    printf("SFDP magic is %02x:%02x:%02x:%02x\n", sfdp_out[0], sfdp_out[1], sfdp_out[2], sfdp_out[3]);

    if (*((uint32_t*)sfdp_out) == SFDP_MAGIC) {
        printf("SFDP header is valid. Version: %02x.%02x Number of param headers: %d\n", sfdp_out[5], sfdp_out[4], sfdp_out[6]+1);

        for (int param_idx=0; param_idx<=sfdp_out[6]; param_idx++) {
            uint32_t pointer = *((uint32_t*)(&sfdp_out[8*(param_idx+1)+4])) & 0x00ffffff;
            printf("SFDP param #%d: Param: %02x Version: %02x.%02x Length: %d dwords Pointer: %08x\n", param_idx, sfdp_out[8*(param_idx+1)], sfdp_out[8*(param_idx+1)+2], sfdp_out[8*(param_idx+1)+1], sfdp_out[8*(param_idx+1)+3], pointer);
            if (sfdp_out[8*(param_idx+1)] == 0) {
                uint32_t dword_1 = *((uint32_t*)(sfdp_out+pointer));
                uint32_t dword_2 = *((uint32_t*)(sfdp_out+pointer)+1);
                uint32_t size_in_bytes = 0;
                if (dword_2 & (1UL << 31))
                    size_in_bytes = 1UL << ((dword_2 & ~(1UL << 31)) - 3);
                else
                    size_in_bytes = (dword_2 + 1) >> 3;
                printf("SFDP JEDEC DWORD #1: Address bytes: %x\n", (dword_1 & 0x00060000) >> 17);   // 0x00 for 3-bytes addressing, 0x01 for 4-bytes addressing
                printf("SFDP JEDEC DWORD #2: Memory size/density: %d bytes\n", size_in_bytes);
            }
        }
    }

    while (true) {
        printf("DONE!\n");
        sleep_ms(10000);
    }
    return 0;
}
