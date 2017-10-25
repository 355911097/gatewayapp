/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "w25qxx.h"

/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_EX_FLASH	0


#define FLASH_SECTOR_SIZE 		512
#define FLASH_BLOCK_SIZE		8
u16	FLASH_SECTOR_COUNT = 2048*3;


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;

	stat = RES_OK;

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) 
	{
	
		case DEV_EX_FLASH:
			w25qxx_init();
			FLASH_SECTOR_COUNT = 2048*3;			
			stat = RES_OK;
		break;
		
		default:
			stat = RES_PARERR;
		break;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;


	if (!count)
	return RES_PARERR;//count不能等于0，否则返回参数错误	
	
	switch (pdrv) 
	{

		case DEV_EX_FLASH:
			for(; count>0; count--)
			{
				w25qxx_read(buff, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
				sector++;
				buff += FLASH_SECTOR_SIZE; 
			}
			res = RES_OK;
		break;
			
		default:
			res = RES_PARERR;
		break;
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	if (!count)
	return RES_PARERR;	//count不能等于0，否则返回参数错误	
	
	switch (pdrv) 
	{	
		case DEV_EX_FLASH:
			for(; count>0; count--)
			{
				w25qxx_write((u8*)buff, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
				sector++;
				buff += FLASH_SECTOR_SIZE; 
			}
			res = RES_OK;
		break;
		default:
			res = RES_PARERR;
		break;
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	if(DEV_EX_FLASH == pdrv)
	{
		switch(cmd)
		{
			case CTRL_SYNC:
				res = RES_OK; 
			break;
			case GET_SECTOR_SIZE:
				 *(WORD*)buff = FLASH_SECTOR_SIZE;
				res = RES_OK;
			break;
			case GET_BLOCK_SIZE:
				*(WORD*)buff = FLASH_BLOCK_SIZE;
				res = RES_OK;
			break;
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = FLASH_SECTOR_COUNT;
				res = RES_OK;
			break;
		
			default:
				res = RES_PARERR;
			break;
		}

	}
	else
	{
		res = RES_PARERR;
	}
	

	return res;
}




DWORD get_fattime()
{
	return 0;
}

