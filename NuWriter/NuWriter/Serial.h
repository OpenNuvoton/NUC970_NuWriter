#ifndef __SERIAL_H
#define __SERIAL_H

#include "devioctl.h"

#define READ_TIMEOUT 10000
#define MAX_FILESIZE 50*1024*1024 //50M

#define BURN	0xb0
#define BURN_TYPE 0x80

#define BUF_SIZE 4096

#define IMAGE_ACTIVE 0 //(-a) 1<<0
#define	IMAGE_COPY2RAM 1 //(-c) 
#define	IMAGE_EXEC 2 //(-x) 
#define	IMAGE_FILE 3 //(-f) 
#define	IMAGE_COMPRESSED 4 //(-z) 
#define	IMAGE_ROMFS 5 //(-r)
#define	IMAGE_LINUX 6 //(-l)
#define	IMAGE_NOFOOTER 7 //

#define ADD		0
#define MODIFY	1
#define ERASE	2
#define FORMAT	3

#if 0
#define DATA 0
#define EXECUTE 1
#define ROMFS 2
#define	SYSTEMIMAGE 3
#else
#define DATA		0
#define ENV			1
#define UBOOT		2
#define	PACK		3
#define	IMAGE		4
#define DATA_OOB	5

#define	PMTP 	15
#endif

#define NAND_VERIFY_FILESYSTEM_ERROR -2
#define NAND_VERIFY_PACK_ERROR -3

#define BLOCK_16K 0x01
#define BLOCK_128K 0x02
#define BLOCK_256K 0x03

#define FILE_BLOCK 4096

#define SPI64K 64*1024
#define SPI63K 63*1024

#define NAND_SIZE 64*2048

#define MMC512B 512

typedef struct _INFO_T
{
	DWORD	Nand_uPagePerBlock;
	DWORD	Nand_uPageSize;
	DWORD   Nand_uSectorPerBlock;
	DWORD   Nand_uBlockPerFlash;
	DWORD	Nand_uBadBlockCount;
	DWORD   Nand_uSpareSize;
	DWORD   Nand_uIsUserConfig;
	DWORD   SPI_ID;
	DWORD   EMMC_uBlock;
	DWORD   EMMC_uReserved;
	DWORD	MTP_uNumber;
}INFO_T,*PINFO_T;

typedef struct _PACK_CHILD_HEAD
{
	DWORD filelen;
	DWORD startaddr;
	DWORD imagetype;
	DWORD reserve[1];
}PACK_CHILD_HEAD,*PPACK_CHILD_HEAD;

typedef struct _PACK_HEAD
{
	DWORD actionFlag;
	DWORD fileLength;
	DWORD num;		 
	DWORD reserve[1];
}PACK_HEAD,*PPACK_HEAD;


typedef struct _MaxOffset
{
	UINT32 maxoffset;
	UINT32 imageno;
}MaxOffset;

typedef struct _BINHEADER{

	DWORD signature; //'W' 'B' 0x5A, 0xA5
	DWORD address;
	DWORD length;
	DWORD resv;

}BINHEADER,*PBINHEADER;

typedef struct _NAND_BINHEADER{

	DWORD signature; //'W' 'B' 0x5A, 0xA5
	DWORD count;
	DWORD blocksize;

}NAND_BINHEADER,*PNAND_BINHEADER;


typedef enum _DOWNMETHOD
{
	SDRAM=0,
	NOR,
	RAWNOR,
	NAND,
	RAWNAND,
    MMC,
	RAWMMC,
	SPI,
	RAWSPI,
	MTP,
	INFO,
	NOR_IMAGELIST,
	NAND_IMAGELIST,
	MMC_IMAGELIST,
	SPI_IMAGELIST,
	NORTYPE,	
}DOWNMETHOD;

#define WRITE_ACTION	0
#define MODIFY_ACTION	1
#define ERASE_ACTION	2
#define VERIFY_ACTION	3
#define READ_ACTION		4
#define PACK_ACTION  	5
#define FORMAT_ACTION	6
#define PACK_VERIFY_ACTION	7

#define NEED_AUTORUN	0x80000000

typedef struct _SDRAM_RAW_TYPEHEAD //SDRAM ,NOR(raw) and SPI
{
	DWORD flag;//write,modify and erase
	DWORD filelen;
	DWORD address; //if bit 31 is "1" ,autorun it
	DWORD dtbaddress;

}SDRAM_RAW_TYPEHEAD,*PSDRAM_RAW_TYPEHEAD;


typedef struct _MMC_RAW_TYPEHEAD //SDRAM ,NOR(raw) and SPI
{
	DWORD flag;//write,modify and erase
	DWORD filelen;
	DWORD address; //if bit 31 is "1" ,autorun it
	DWORD formatsize;

}MMC_RAW_TYPEHEAD,*PMMC_RAW_TYPEHEAD;

typedef struct _AUTOTYPEHEAD
{
	DWORD filelen;
	DWORD address;

}AUTOTYPEHEAD,*PAUTOTYPEHEAD;

typedef struct _XBINHEAD
{
	DWORD sign;
	DWORD address;
	DWORD length;
	DWORD version;

}XBINHEAD,*PXBINHEAD;


typedef struct _NORBOOT_NAND_HEAD //NAND and NOR(bootloader)
{
	DWORD flag;//write,modify and erase
	DWORD filelen;
	DWORD no;
	CHAR name[16];
	DWORD type;
	DWORD execaddr;    //end block = excute address
	DWORD flashoffset; //start block =  start address
	DWORD endaddr;	
	UCHAR macaddr[8];  //mac address
	DWORD initSize;
}NORBOOT_NAND_HEAD,*PNORBOOT_NAND_HEAD;

typedef struct _NORBOOT_MMC_HEAD //MMC(bootloader)
{
	DWORD flag;//write,modify and erase
	DWORD filelen;
	DWORD no;
	CHAR name[16];
	DWORD type;
	DWORD execaddr;
	DWORD flashoffset;
	DWORD endaddr;	
	DWORD ReserveSize;  //unit of sector
	UCHAR macaddr[8];
	DWORD initSize;
}NORBOOT_MMC_HEAD,*PNORBOOT_MMC_HEAD;

typedef struct _NORBOOT_MTP_HEAD //MMC(bootloader)
{
	UCHAR 	Mode;	
	UCHAR	Option;
	UCHAR   Lock;
	UCHAR   Reserved[2];	
	DWORD	KeyLen;
	DWORD	KeyOTP[8];
}NORBOOT_MTP_HEAD,*PNORBOOT_MTP_HEAD;

typedef struct _NANDROM_HEAD //NAND ROM Head
{
	DWORD no;
	CHAR name[16];
	DWORD type;
	DWORD execaddr;
	DWORD startblock;
	BINHEADER head;

}NANDROM_HEAD,*PNANDROM_HEAD;


typedef struct _SPIIMAGE_HEAD{

	USHORT no;
	USHORT type;
	DWORD startoffset;
	DWORD execaddr;
	DWORD filelen;
	CHAR name[16];	
}SPIIMAGE_HEAD,*PSPIIMAGE_HEAD;


typedef struct _MMCIMAGE_HEAD{

	USHORT no;
	USHORT type;
	DWORD startoffset;
	DWORD execaddr;
	DWORD filelen;
	CHAR name[16];		
}MMCIMAGE_HEAD,*PMMCIMAGE_HEAD;

typedef struct _MMCROM_HEAD{

	DWORD signature0; //'W' 'B' 0x55, 0xAA
	DWORD count; // total image count (include image 0)
	DWORD resv;
	DWORD signature1;//'W' 'B' 0x59, 0x63

}MMCROM_HEAD,*PMMCROM_HEAD;

#pragma warning(disable: 4200)

typedef struct _SPIROM_HEAD{

	DWORD signature0; //'W' 'B' 0x55, 0xAA
	DWORD count; // total image count (include image 0)
	DWORD resv;
	DWORD signature1;//'W' 'B' 0x59, 0x63

}SPIROM_HEAD,*PSPIROM_HEAD;


#define IOCTL_SERIAL_SET_FLASH      CTL_CODE(FILE_DEVICE_SERIAL_PORT, 100,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_DOWNLOAD   CTL_CODE(FILE_DEVICE_SERIAL_PORT, 101,METHOD_BUFFERED,FILE_ANY_ACCESS)

typedef struct _VENCMD
{
	UCHAR bRequest;
	UCHAR wValue;
	//UCHAR ack;

}VENCMD,*PVENCMD;
	
//////////////////////////////////////////////////////////////////////
//
// CSerial - Win32 wrapper for serial communications
//
class CSerial
{
// Class enumerations
public:
	// Communication event
	typedef enum
	{
		EEventNone  = -1,			// Event trigged without cause
		EEventBreak = EV_BREAK,		// A break was detected on input
		EEventCTS   = EV_CTS,		// The CTS signal changed state
		EEventDSR   = EV_DSR,		// The DSR signal changed state
		EEventError = EV_ERR,		// A line-status error occurred
		EEventRing  = EV_RING,		// A ring indicator was detected
		EEventRLSD  = EV_RLSD,		// The RLSD signal changed state
		EEventRecv  = EV_RXCHAR,	// Data is received on input
		EEventRcvEv = EV_RXFLAG,	// Event character was received on input
		EEventSend  = EV_TXEMPTY,	// Last character on output was sent
	} 
	EEvent;

	// Baudrate
	typedef enum
	{
		EBaudUnknown = -1,			// Unknown
		EBaud110     = CBR_110,		// 110 bits/sec
		EBaud300     = CBR_300,		// 300 bits/sec
		EBaud600     = CBR_600,		// 600 bits/sec
		EBaud1200    = CBR_1200,	// 1200 bits/sec
		EBaud2400    = CBR_2400,	// 2400 bits/sec
		EBaud4800    = CBR_4800,	// 4800 bits/sec
		EBaud9600    = CBR_9600,	// 9600 bits/sec
		EBaud14400   = CBR_14400,	// 14400 bits/sec
		EBaud19200   = CBR_19200,	// 19200 bits/sec (default)
		EBaud38400   = CBR_38400,	// 38400 bits/sec
		EBaud56000   = CBR_56000,	// 56000 bits/sec
		EBaud57600   = CBR_57600,	// 57600 bits/sec
		EBaud115200  = CBR_115200,	// 115200 bits/sec
		EBaud128000  = CBR_128000,	// 128000 bits/sec
		EBaud256000  = CBR_256000,	// 256000 bits/sec
	}
	EBaudrate;

	// Data bits (5-8)
	typedef enum
	{
		EDataUnknown = -1,			// Unknown
		EData5       =  5,			// 5 bits per byte
		EData6       =  6,			// 6 bits per byte
		EData7       =  7,			// 7 bits per byte
		EData8       =  8			// 8 bits per byte (default)
	}
	EDataBits;

	// Parity scheme
	typedef enum
	{
		EParUnknown = -1,			// Unknown
		EParNone    = NOPARITY,		// No parity (default)
		EParOdd     = ODDPARITY,	// Odd parity
		EParEven    = EVENPARITY,	// Even parity
		EParMark    = MARKPARITY,	// Mark parity
		EParSpace   = SPACEPARITY	// Space parity
	}
	EParity;

	// Stop bits
	typedef enum
	{
		EStopUnknown = -1,			// Unknown
		EStop1       = ONESTOPBIT,	// 1 stopbit (default)
		EStop1_5     = ONE5STOPBITS,// 1.5 stopbit
		EStop2       = TWOSTOPBITS	// 2 stopbits
	} 
	EStopBits;

	// Handshaking
	typedef enum
	{
		EHandshakeUnknown		= -1,	// Unknown
		EHandshakeOff			=  0,	// No handshaking
		EHandshakeHardware		=  1,	// Hardware handshaking (RTS/CTS)
		EHandshakeSoftware		=  2	// Software handshaking (XON/XOFF)
	} 
	EHandshake;

	// Timeout settings
	typedef enum
	{
		EReadTimeoutUnknown		= -1,	// Unknown
		EReadTimeoutNonblocking	=  0,	// Always return immediately
		EReadTimeoutBlocking	=  1	// Block until everything is retrieved
	}
	EReadTimeout;

	// Communication errors
	typedef enum
	{
		EErrorUnknown = 0,			// Unknown
		EErrorBreak   = CE_BREAK,	// Break condition detected
		EErrorFrame   = CE_FRAME,	// Framing error
		EErrorIOE     = CE_IOE,		// I/O device error
		EErrorMode    = CE_MODE,	// Unsupported mode
		EErrorOverrun = CE_OVERRUN,	// Character buffer overrun, next byte is lost
		EErrorRxOver  = CE_RXOVER,	// Input buffer overflow, byte lost
		EErrorParity  = CE_RXPARITY,// Input parity error
		EErrorTxFull  = CE_TXFULL	// Output buffer full
	}
	EError;

	// Port availability
	typedef enum
	{
		EPortUnknownError = -1,		// Unknown error occurred
		EPortAvailable    =  0,		// Port is available
		EPortNotAvailable =  1,		// Port is not present
		EPortInUse        =  2		// Port is in use

	} 
	EPort;

// Construction
public:
	CSerial();
	virtual ~CSerial();

// Operations
public:
	// Check if pareticular COM-port is available.
	EPort CheckPort (LPCTSTR lpszDevice);

	// Open the serial communications for a particular COM port. You
	// need to use the full devicename (i.e. "COM1") to open the port.
	// It's possible to specify the size of the input/output queues.
	virtual LONG Open (LPCTSTR lpszDevice, DWORD dwInQueue = 2048, DWORD dwOutQueue = 2048);

	// Close the serial port.
	virtual LONG Close (void);

	// Setup the communication settings such as baudrate, databits,
	// parity and stopbits. The default settings are applied when the
	// device has been opened. Call this function if these settings do
	// not apply for your application. If you prefer to use integers
	// instead of the enumerated types then just cast the integer to
	// the required type. So the following two initializations are
	// equivalent:
	//
	//   Setup(EBaud9600,EData8,EParNone,EStop1)
	//
	// or
	//
	//   Setup(EBaudrate(9600),EDataBits(8),EParity(NOPARITY),EStopBits(ONESTOPBIT))
	//
	// In the latter case, the types are not validated. So make sure
	// that you specify the appropriate values.
	virtual LONG Setup (EBaudrate eBaudrate = EBaud9600,
						EDataBits eDataBits = EData8,
						EParity   eParity   = EParNone,
						EStopBits eStopBits = EStop1);

	// Set/clear the event character. When this byte is being received
	// on the serial port then the EEventRcvEv event is signalled,
	// when the mask has been set appropriately. If the fAdjustMask flag
	// has been set, then the event mask is automatically adjusted.
	virtual LONG SetEventChar (BYTE bEventChar, bool fAdjustMask = true);

	// Set the event mask, which indicates what events should be
	// monitored. The WaitEvent method can only monitor events that
	// have been enabled. The default setting only monitors the
	// error events and data events. An application may choose to
	// monitor CTS. DSR, RLSD, etc as well.
	virtual LONG SetMask (DWORD dwMask = EEventBreak|EEventError|EEventRecv);

	// The WaitEvent method waits for one of the events that are
	// enabled (see SetMask).
	virtual LONG WaitEvent (LPOVERLAPPED lpOverlapped = 0, DWORD dwTimeout = INFINITE);

	// Setup the handshaking protocol. There are three forms of
	// handshaking:
	//
	// 1) No handshaking, so data is always send even if the receiver
	//    cannot handle the data anymore. This can lead to data loss,
	//    when the sender is able to transmit data faster then the
	//    receiver can handle.
	// 2) Hardware handshaking, where the RTS/CTS lines are used to
	//    indicate if data can be sent. This mode requires that both
	//    ports and the cable support hardware handshaking. Hardware
	//    handshaking is the most reliable and efficient form of
	//    handshaking available, but is hardware dependant.
	// 3) Software handshaking, where the XON/XOFF characters are used
	//    to throttle the data. A major drawback of this method is that
	//    these characters cannot be used for data anymore.
	virtual LONG SetupHandshaking (EHandshake eHandshake);

	// Read operations can be blocking or non-blocking. You can use
	// this method to setup wether to use blocking or non-blocking
	// reads. Non-blocking reads is the default, which is required
	// for most applications.
	//
	// 1) Blocking reads, which will cause the 'Read' method to block
	//    until the requested number of bytes have been read. This is
	//    useful if you know how many data you will receive.
	// 2) Non-blocking reads, which will read as many bytes into your
	//    buffer and returns almost immediately. This is often the
	//    preferred setting.
	virtual LONG SetupReadTimeouts (EReadTimeout eReadTimeout,DWORD rdTimeout=READ_TIMEOUT);

	// Obtain communication settings
	virtual EBaudrate  GetBaudrate    (void);
	virtual EDataBits  GetDataBits    (void);
	virtual EParity    GetParity      (void);
	virtual EStopBits  GetStopBits    (void);
	virtual EHandshake GetHandshaking (void);
	virtual DWORD      GetEventMask   (void);
	virtual BYTE       GetEventChar   (void);

	// Write data to the serial port. Note that we are only able to
	// send ANSI strings, because it probably doesn't make sense to
	// transmit Unicode strings to an application.
	virtual LONG Write (const void* pData, size_t iLen, DWORD* pdwWritten = 0, LPOVERLAPPED lpOverlapped = 0, DWORD dwTimeout = INFINITE);
	virtual LONG Write (LPCSTR pString, DWORD* pdwWritten = 0, LPOVERLAPPED lpOverlapped = 0, DWORD dwTimeout = INFINITE);

	// Read data from the serial port. Refer to the description of
	// the 'SetupReadTimeouts' for an explanation about (non) blocking
	// reads and how to use this.
	virtual LONG Read (void* pData, size_t iLen, DWORD* pdwRead = 0, LPOVERLAPPED lpOverlapped = 0, DWORD dwTimeout = INFINITE);

	// Determine what caused the event to trigger
	EEvent GetEventType (void);

	// Obtain the error
	EError GetError (void);

	// Obtain the COMM and event handle
	HANDLE GetCommHandle (void)		{ return m_hFile; }

	// Check if com-port is opened
	bool IsOpen (void) const		{ return (m_hFile != 0); }

	// Obtain last error status
	LONG GetLastError (void) const	{ return m_lLastError; }

	// Obtain CTS/DSR/RING/RLSD settings
	bool GetCTS (void);
	bool GetDSR (void);
	bool GetRing (void);
	bool GetRLSD (void);

	// Flush all buffers
	LONG Flush (void);
	BYTE ChooseMethod (DOWNMETHOD method);

protected:
	// Internal helper class which wraps DCB structure
	class CDCB : public DCB
	{
	public:
		CDCB() { DCBlength = sizeof(DCB); }
	};

// Attributes
protected:
	LONG	m_lLastError;		// Last serial error
	HANDLE	m_hFile;			// File handle
	EEvent	m_eEvent;			// Event type
	HANDLE	m_hevtOverlapped;	// Event handle for internal overlapped operations
};

#endif	// __SERIAL_H
