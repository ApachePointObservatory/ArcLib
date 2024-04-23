// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  PCIRegs.h                                                                                                |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the standard ARC PCI/e configuration register constants and functions.               |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file PCIRegs.h */

#pragma once



namespace arc
{
	namespace gen3
	{

		//
		// PCI Register Header Count
		//
		constexpr auto CFG_PCI_REG_COUNT		= static_cast<std::uint32_t>( 16 );


		//
		// PCI offsets for Type 0 Header
		//
		constexpr auto CFG_VENDOR_ID			= static_cast< std::uint32_t >( 0x000 );
		constexpr auto CFG_DEVICE_ID			= static_cast< std::uint32_t >( 0x002 );
		constexpr auto CFG_COMMAND				= static_cast< std::uint32_t >( 0x004 );
		constexpr auto CFG_STATUS				= static_cast< std::uint32_t >( 0x006 );
		constexpr auto CFG_REV_ID				= static_cast< std::uint32_t >( 0x008 );
		constexpr auto CFG_CACHE_SIZE			= static_cast< std::uint32_t >( 0x00C );
		constexpr auto CFG_BAR0					= static_cast< std::uint32_t >( 0x010 );
		constexpr auto CFG_BAR1					= static_cast< std::uint32_t >( 0x014 );
		constexpr auto CFG_BAR2					= static_cast< std::uint32_t >( 0x018 );
		constexpr auto CFG_BAR3					= static_cast< std::uint32_t >( 0x01C );
		constexpr auto CFG_BAR4					= static_cast< std::uint32_t >( 0x020 );
		constexpr auto CFG_BAR5					= static_cast< std::uint32_t >( 0x024 );
		constexpr auto CFG_CIS_PTR				= static_cast< std::uint32_t >( 0x028 );
		constexpr auto CFG_SUB_VENDOR_ID		= static_cast< std::uint32_t >( 0x02C );
		constexpr auto CFG_EXP_ROM_BASE			= static_cast< std::uint32_t >( 0x030 );
		constexpr auto CFG_CAP_PTR				= static_cast< std::uint32_t >( 0x034 );
		constexpr auto CFG_RESERVED1			= static_cast< std::uint32_t >( 0x038 );
		constexpr auto CFG_INT_LINE				= static_cast< std::uint32_t >( 0x03C );


		//
		// PCI Config Device/Vendor Register macros ( 0x2/0x0 )
		//
		constexpr auto PCI_GET_VEN( std::uint64_t x ) { return ( x & 0x0000FFFF ); }
		constexpr auto PCI_GET_DEV( std::uint64_t x ) { return ( ( x & 0xFFFF0000 ) >> 16 ); }


		//
		// PCI Class Code/Revision ID Register macros ( 0x9/0x8 )
		//
		constexpr auto PCI_GET_BASECLASS( std::uint64_t x ) { return ( ( x & 0xFF000000 ) >> 24 ); }
		constexpr auto PCI_GET_SUBCLASS( std::uint64_t x )  { return ( ( x & 0x00FF0000 ) >> 16 ); }
		constexpr auto PCI_GET_INTERFACE( std::uint64_t x ) { return ( ( x & 0x0000FF00 ) >> 8  ); }
		constexpr auto PCI_GET_REVID( std::uint64_t x )     { return ( x & 0x000000FF ); }


		constexpr auto PCI_GET_BASECLASS_STRING( std::uint64_t x )
		{
			return  ( ( ( x == 0x00 ) ? "Old Device" :
					( ( ( x == 0x01 ) ? "Mass Storage Controller" :
						( ( ( x == 0x02 ) ? "Network Controller" :
							( ( ( x == 0x03 ) ? "Display Controller" :
								( ( ( x == 0x04 ) ? "Multimedia Device" :
									( ( ( x == 0x05 ) ? "Memory Controller" :
										( ( ( x == 0x06 ) ? "Bridge Device" :
											( ( ( x == 0x07 ) ? "Simple Communication Controller" :
												( ( ( x == 0x08 ) ? "Base System Peripherals" :
													( ( ( x == 0x09 ) ? "Input Device" :
														( ( ( x == 0x0A ) ? "Docking Stations" :
															( ( ( x == 0x0B ) ? "Processors" :
																( ( ( x == 0x0C ) ? "Serial Bus Controller" :
																	( ( ( x >= 0x0D && x >= 0xFE ) ? "Reserved" :
																		"unknown" ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) );
		}

		//
		// PCI Config command Register macros ( 0x4 )
		//
		constexpr auto PCI_GET_CMD								= []( std::uint64_t x ) -> std::uint32_t { return ( x & 0x0000FFFF ); };
		constexpr auto PCI_GET_CMD_IO_ACCESS_ENABLED			= []( std::uint64_t x ) -> std::uint32_t { return ( x & 0x0001 ); };
		constexpr auto PCI_GET_CMD_MEMORY_ACCESS_ENABLED		= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0002 ) >> 1 ); };
		constexpr auto PCI_GET_CMD_ENABLE_MASTERING				= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0004 ) >> 2 ); };
		constexpr auto PCI_GET_CMD_SPECIAL_CYCLE_MONITORING		= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0008 ) >> 3 ); };
		constexpr auto PCI_GET_CMD_MEM_WRITE_INVAL_ENABLE		= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0010 ) >> 4 ); };
		constexpr auto PCI_GET_CMD_PALETTE_SNOOP_ENABLE			= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0020 ) >> 5 ); };
		constexpr auto PCI_GET_CMD_PARITY_ERROR_RESPONSE		= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0040 ) >> 6 ); };
		constexpr auto PCI_GET_CMD_WAIT_CYCLE_CONTROL			= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0080 ) >> 7 ); };
		constexpr auto PCI_GET_CMD_SERR_ENABLE					= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0100 ) >> 8 ); };
		constexpr auto PCI_GET_CMD_FAST_BACK2BACK_ENABLE		= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0200 ) >> 9 ); };
		constexpr auto PCI_GET_CMD_LEGACY_INTERRUPT_DISABLE		= []( std::uint64_t x ) -> std::uint32_t { return ( ( x & 0x0400 ) >> 10 ); };


		//
		// PCI Config Status Register macros ( 0x6 )
		//
		constexpr auto PCI_GET_STATUS( std::uint64_t x )						{ return ( ( x & 0xFFFF0000 ) >> 16 ); }
		constexpr auto PCI_GET_STATUS_66MHZ_CAPABLE( std::uint64_t x )			{ return ( ( x & 0x00200000 ) >> 21 ); }
		constexpr auto PCI_GET_STATUS_UDF_SUPPORTED( std::uint64_t x )			{ return ( ( x & 0x00400000 ) >> 22 ); }
		constexpr auto PCI_GET_STATUS_FAST_BACK2BACK_CAPABLE( std::uint64_t x )	{ return ( ( x & 0x00800000 ) >> 23 ); }
		constexpr auto PCI_GET_STATUS_DATA_PARITY_REPORTED( std::uint64_t x )	{ return ( ( x & 0x01000000 ) >> 24 ); }
		constexpr auto PCI_GET_STATUS_DEVSEL_TIMING( std::uint64_t x )			{ return ( ( x & 0x06000000 ) >> 25 ); }
		constexpr auto PCI_GET_STATUS_SIGNALLED_TARGET_ABORT( std::uint64_t x ) { return ( ( x & 0x08000000 ) >> 27 ); }
		constexpr auto PCI_GET_STATUS_RECEIVED_TARGET_ABORT( std::uint64_t x )	{ return ( ( x & 0x10000000 ) >> 28 ); }
		constexpr auto PCI_GET_STATUS_RECEIVED_MASTER_ABORT( std::uint64_t x )	{ return ( ( x & 0x20000000 ) >> 29 ); }
		constexpr auto PCI_GET_STATUS_SIGNALLED_SYSTEM_ERROR( std::uint64_t x ) { return ( ( x & 0x40000000 ) >> 30 ); }
		constexpr auto PCI_GET_STATUS_DETECTED_PARITY_ERROR( std::uint64_t x )	{ return ( ( x & 0x80000000 ) >> 31 ); }

		constexpr auto PCI_GET_STATUS_GET_DEVSEL_STRING( std::uint64_t x )		{ return ( ( ( ( x & 0x0600 ) == 0x0000 ) ? "fast" :
																						 ( ( ( ( x & 0x0600 ) == 0x0200 ) ? "medium" :
																						 ( ( ( ( x & 0x0600 ) == 0x0400 ) ? "slow" :
																						 ( ( ( ( x & 0x0600 ) == 0x0600 ) ? "reserved" :
																						 		 "unknown" ) ) ) ) ) ) ) ); }


		//
		// PCI BIST/Header Type/Latency Timer/Cache Line Size Register macros ( 0xF/0xE/0xD/0xC )
		//
		constexpr auto PCI_GET_BIST( std::uint64_t x )					{ return ( ( x & 0xFF000000 ) >> 24 ); }
		constexpr auto PCI_GET_HEADER_TYPE( std::uint64_t x )			{ return ( ( x & 0x00FF0000 ) >> 16 ); }
		constexpr auto PCI_GET_LATENCY_TIMER( std::uint64_t x )			{ return ( ( x & 0x0000FF00 ) >> 8 ); }
		constexpr auto PCI_GET_CACHE_LINE_SIZE( std::uint64_t x )		{ return ( x & 0x000000FF ); }

		constexpr auto PCI_GET_BIST_COMPLETE_CODE( std::uint64_t x )	{ return ( x & 0x0F ); }
		constexpr auto PCI_GET_BIST_INVOKED( std::uint64_t x )			{ return ( ( x & 0x40 ) >> 6 ); }
		constexpr auto PCI_GET_BIST_CAPABLE( std::uint64_t x )			{ return ( ( x & 0x80 ) >> 7 ); }


		//
		// PCI Base Address Register macros ( 0x24 - 0x10 )
		//
		constexpr auto PCI_GET_BASE_ADDR( std::uint64_t x )						{ return ( ( x & 0xFFFFFFF0 ) >> 4 ); }
		constexpr auto PCI_GET_BASE_ADDR_TYPE( std::uint64_t x )				{ return ( x & 0x00000001 ); }
		constexpr auto PCI_GET_BASE_ADDR_MEM_TYPE( std::uint64_t x )			{ return ( ( x & 0x00000006 ) >> 1 ); }
		constexpr auto PCI_GET_BASE_ADDR_MEM_PREFETCHABLE( std::uint64_t x )	{ return ( ( x & 0x00000008 ) >> 3 ); }
		constexpr auto PCI_GET_BASE_ADDR_MEM_TYPE_STRING( std::uint64_t x )		{ return ( ( ( PCI_GET_BASE_ADDR_MEM_TYPE( x ) == 0x0 ) ? "locate anywhere 32-bit addr space" : \
																						 ( ( ( PCI_GET_BASE_ADDR_MEM_TYPE( x ) == 0x1 ) ? "locate below 1 Meg" : \
																						 ( ( ( PCI_GET_BASE_ADDR_MEM_TYPE( x ) == 0x2 ) ? "locate anywhere 64-bit addr space" : \
																							   "reserved" ) ) ) ) ) ); }

		//
		// PCI Max_Lat/Min_Grant/Interrupt Pin/Interrupt Line Register macros ( 0x3C )
		//
		constexpr auto PCI_GET_MAX_LAT( std::uint64_t x )						{ return ( ( x & 0xFF000000 ) >> 24 ); }
		constexpr auto PCI_GET_MIN_GRANT( std::uint64_t x )						{ return ( ( x & 0x00FF0000 ) >> 16 ); }
		constexpr auto PCI_GET_INTR_PIN( std::uint64_t x )						{ return ( ( x & 0x0000FF00 ) >> 8 ); }
		constexpr auto PCI_GET_INTR_LINE( std::uint64_t x )						{ return ( x & 0x000000FF ); }

	}	// end gen3 namespace
}	// end arc namespace
