/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* Declarations of disk functions */
#include "../sd.h"

/* Definitions of physical drive number for each drive */
#define DEV_SD 0 /* Example: Map MMC/SD card to physical drive 1 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv) {
	if (pdrv != DEV_SD) {
		return STA_NOINIT;
	}

	return 0;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv) {
	if (pdrv != DEV_SD) {
		return STA_NOINIT;
	}

	if (sd_init() == SD_OK) {
		return 0;
	}

	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv,	/* Physical drive nmuber to identify the drive */
				  BYTE* buff,	/* Data buffer to store read data */
				  LBA_t sector, /* Start sector in LBA */
				  UINT count	/* Number of sectors to read */
) {
	if (pdrv != DEV_SD) {
		return RES_PARERR;
	}

	int res = sd_readblock(sector, buff, count);
	if (count == res) {
		return RES_OK;
	}

	return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv,		 /* Physical drive nmuber to identify the drive */
				   const BYTE* buff, /* Data to be written */
				   LBA_t sector,	 /* Start sector in LBA */
				   UINT count		 /* Number of sectors to write */
) {
	if (pdrv != DEV_SD) {
		return RES_PARERR;
	}

	if (sd_writeblock(buff, sector, count) == 0) {
		return RES_OK; /* Write successful */
	}

	return RES_ERROR;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, /* Physical drive nmuber (0..) */
				   BYTE cmd,  /* Control code */
				   void* buff /* Buffer to send/receive control data */
) {
	if (pdrv != DEV_SD) {
		return RES_PARERR;
	}

	switch (cmd) {
	case GET_SECTOR_COUNT:
		*(LBA_t*)buff = 8388608; // Karta 4GB, 512B na sektor
		return RES_OK;

	case GET_SECTOR_SIZE:
		*(WORD*)buff = 512;
		return RES_OK;

	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 1;
		return RES_OK;
	}

	return RES_PARERR;
}
