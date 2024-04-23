// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  ArcDefs.h                                                                                                |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the standard ARC board, command and reply definitions.                               |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file ArcDefs.h */

#pragma once


#ifdef __cplusplus
namespace arc
{
#endif


	// +----------------------------------------------------------------------------------------
	// | Define board id's
	// +----------------------------------------------------------------------------------------
	constexpr auto PCI_ID				= static_cast<std::uint32_t>( 1 );
	constexpr auto TIM_ID				= static_cast<std::uint32_t>( 2 );
	constexpr auto UTIL_ID				= static_cast<std::uint32_t>( 3 );
	constexpr auto SMALLCAM_DLOAD_ID	= static_cast<std::uint32_t>( 3 );


	// +----------------------------------------------------------------------------------------
	// | Memory Location Id Constants
	// | R_MEM	(Bit 20)  ROM
	// | P_MEM	(Bit 21)  DSP program memory space
	// | X_MEM	(Bit 22)  DSP X memory space
	// | Y_MEM	(Bit 23)  DSP Y memory space
	// +----------------------------------------------------------------------------------------
	constexpr auto P_MEM		= static_cast<std::uint32_t>( 0x100000 );
	constexpr auto X_MEM 		= static_cast<std::uint32_t>( 0x200000 );
	constexpr auto Y_MEM		= static_cast<std::uint32_t>( 0x400000 );
	constexpr auto R_MEM		= static_cast<std::uint32_t>( 0x800000 );


	// +----------------------------------------------------------------------------------------
	// | Define controller/PCI replies
	// +----------------------------------------------------------------------------------------
	constexpr auto TOUT			= static_cast<std::uint32_t>( 0x544F5554 );		// Timeout
	constexpr auto ROUT			= static_cast<std::uint32_t>( 0x524F5554 );		// Readout
	constexpr auto HERR			= static_cast<std::uint32_t>( 0x48455252 );		// Header Error
	constexpr auto DON			= static_cast<std::uint32_t>( 0x00444F4E );		// Done
	constexpr auto ERR			= static_cast<std::uint32_t>( 0x00455252 );		// Error
	constexpr auto SYR			= static_cast<std::uint32_t>( 0x00535952 );		// System reset
	constexpr auto RST			= static_cast<std::uint32_t>( 0x00525354 );		// reset
	constexpr auto CNR			= static_cast<std::uint32_t>( 0x00434E52 );		// Controller Not Ready


	// +----------------------------------------------------------------------------------------
	// | Define controller/manual commands
	// +----------------------------------------------------------------------------------------
	constexpr auto DBC			= static_cast<std::uint32_t>( 0x00444243 );		// Download Boot Code
	constexpr auto TDL			= static_cast<std::uint32_t>( 0x0054444C );		// Test Data Link
	constexpr auto RDM			= static_cast<std::uint32_t>( 0x0052444D );		// ReaD Memory
	constexpr auto WRM			= static_cast<std::uint32_t>( 0x0057524D );		// WRite Memory
	constexpr auto SEX			= static_cast<std::uint32_t>( 0x00534558 );		// Start EXposure
	constexpr auto SET			= static_cast<std::uint32_t>( 0x00534554 );		// Set Exposure Time
	constexpr auto PEX			= static_cast<std::uint32_t>( 0x00504558 );		// Pause EXposure
	constexpr auto REX			= static_cast<std::uint32_t>( 0x00524558 );		// Resume EXposure
	constexpr auto RET			= static_cast<std::uint32_t>( 0x00524554 );		// Read Elapsed Time
	constexpr auto PON			= static_cast<std::uint32_t>( 0x00504F4E );		// Power ON
	constexpr auto POF			= static_cast<std::uint32_t>( 0x00504F46 );		// Power OFf
	constexpr auto RDI			= static_cast<std::uint32_t>( 0x00524449 );		// ReaD Image
	constexpr auto SOS			= static_cast<std::uint32_t>( 0x00534F53 );		// Select Output Source
	constexpr auto MPP			= static_cast<std::uint32_t>( 0x004D5050 );		// Multi-Pinned Phase mode
	constexpr auto DCA			= static_cast<std::uint32_t>( 0x00444341 );		// Download CoAdder
	constexpr auto SNF			= static_cast<std::uint32_t>( 0x00534E46 );		// Set Number of Frames
	constexpr auto FPB			= static_cast<std::uint32_t>( 0x00465042 );		// Set the Frames-Per-Buffer for coadds
	constexpr auto VID			= static_cast<std::uint32_t>( 0x00564944 );		// mnemonic that means VIDeo board
	constexpr auto SBN			= static_cast<std::uint32_t>( 0x0053424E );		// Set Bias Number
	constexpr auto SBV			= static_cast<std::uint32_t>( 0x00534256 );		// Set Bias Voltage
	constexpr auto SGN			= static_cast<std::uint32_t>( 0x0053474E );		// Set GaiN
	constexpr auto SMX			= static_cast<std::uint32_t>( 0x00534D58 );		// Select MultipleXer
	constexpr auto CLK			= static_cast<std::uint32_t>( 0x00434C4B );		// mnemonic that means CLocK driver board
	constexpr auto SSS			= static_cast<std::uint32_t>( 0x00535353 );		// Set Subarray Sizes
	constexpr auto SSP			= static_cast<std::uint32_t>( 0x00535350 );		// Set Subarray Positions
	constexpr auto LGN			= static_cast<std::uint32_t>( 0x004C474E );		// Set Low Gain
	constexpr auto HGN			= static_cast<std::uint32_t>( 0x0048474E );		// Set High Gain
	constexpr auto SRM			= static_cast<std::uint32_t>( 0x0053524D );		// Set Readout Mode - either CDS or single
	constexpr auto CDS			= static_cast<std::uint32_t>( 0x00434453 );		// Correlated Double Sampling
	constexpr auto SFS			= static_cast<std::uint32_t>( 0x00534653 );		// Send Fowler Sample
	constexpr auto SPT			= static_cast<std::uint32_t>( 0x00535054 );		// Set Pass Through mode
	constexpr auto LDA			= static_cast<std::uint32_t>( 0x004C4441 );		// LoaD Application
	constexpr auto RCC			= static_cast<std::uint32_t>( 0x00524343 );		// Read Controller Configuration
	constexpr auto CLR			= static_cast<std::uint32_t>( 0x00434C52 );		// CleaR Array
	constexpr auto IDL			= static_cast<std::uint32_t>( 0x0049444C );		// IDLe
	constexpr auto STP			= static_cast<std::uint32_t>( 0x00535450 );		// SToP idle
	constexpr auto CSH			= static_cast<std::uint32_t>( 0x00435348 );		// close SHutter
	constexpr auto OSH			= static_cast<std::uint32_t>( 0x004F5348 );		// open SHutter
	constexpr auto SUR			= static_cast<std::uint32_t>( 0x00535552 );		// Set Up the Ramp mode
	constexpr auto MH1			= static_cast<std::uint32_t>( 0x004D4831 );		// Move NIRIM Filter Wheel 1 Home
	constexpr auto MM1			= static_cast<std::uint32_t>( 0x004D4D31 );		// Move NIRIM Filter Wheel 1
	constexpr auto MH2			= static_cast<std::uint32_t>( 0x004D4832 );		// Move NIRIM Filter Wheel 2 Home
	constexpr auto MM2			= static_cast<std::uint32_t>( 0x004D4D32 );		// Move NIRIM Filter Wheel 2
	constexpr auto SBS			= static_cast<std::uint32_t>( 0x00534253 );		// Set Hardware Byte Swapping
	constexpr auto TBS			= static_cast<std::uint32_t>( 0x00544253 );		// Test for Hardware Byte Swapping
	constexpr auto RNC			= static_cast<std::uint32_t>( 0x00524E43 );		// Read Number of Channels for Hawaii RG array
	constexpr auto THG			= static_cast<std::uint32_t>( 0x00544847 );		// Test High Gain - Utility Board Temperature
	constexpr auto SID			= static_cast<std::uint32_t>( 0x00534944 );		// System ID - FastCam
	constexpr auto JDL			= static_cast<std::uint32_t>( 0x004A444C );		// Jump to DownLoad for C based ARC-22 systems
	constexpr auto XMT			= static_cast<std::uint32_t>( 0x00584D54 );		// Set/Clear fiber optic 2x transmitter mode
	constexpr auto ABR			= static_cast<std::uint32_t>( 0x00414252 );		// ABort Readout & Exposure
	constexpr auto STM			= static_cast<std::uint32_t>( 0x0053544D );		// Set Trigger Mode

	constexpr auto CDT			= static_cast<std::uint32_t>( 0x00434454 );		// SmallCam control detector temperature
	constexpr auto RDT			= static_cast<std::uint32_t>( 0x00524454 );		// SmallCam read detector temperature
	constexpr auto RHV			= static_cast<std::uint32_t>( 0x00524856 );		// SmallCam read heater voltage
	constexpr auto RDC			= static_cast<std::uint32_t>( 0x00524443 );		// SmallCam read detector current ( actually, DN )

	constexpr auto RSC			= static_cast<std::uint32_t>( 0x00525343 );		// FastCam Only - ReSet Controller


	// +----------------------------------------------------------------------------------------
	// | Controller Configuration Bit Definitions
	// +----------------------------------------------------------------------------------------
	// |
	// | BIT #'s		FUNCTION
	// | 2,1,0		Video Processor
	// | 			000	ARC-41 Dual Readout CCD
	// | 			001	CCD Gen I
	// | 			010	ARC-42 Dual Readout IR
	// | 			011	ARC-44 Four Readout IR Coadder ( obsolete )
	// | 			100	ARC-45 Dual Readout CCD
	// | 			101	ARC-46 8-Channel IR
	// |			110 ARC-48 8-Channel CCD
	// |			111 ARC-47 4-Channel CCD
	// | 
	// | 4,3		Timing Board
	// | 			00	Rev. 4, Gen II
	// | 			01	Gen I
	// | 			10	Rev. 5, Gen III, 250 MHz
	// | 
	// | 6,5		Utility Board
	// | 			00	No utility board
	// | 			01	Utility Rev. 3
	// | 
	// | 7		Shutter
	// | 			0	No shutter support
	// | 			1	Yes shutter support
	// | 
	// | 9,8		Temperature readout
	// | 			00	No temperature readout
	// | 			01	Polynomial Diode calibration
	// | 			10	Linear temperature sensor calibration
	// | 
	// | 10		Subarray readout
	// | 			0	Not supported
	// | 			1	Yes supported
	// | 
	// | 11		Binning
	// | 			0	Not supported
	// | 			1	Yes supported
	// | 
	// | 12		Split-Serial readout
	// | 			0	Not supported
	// | 			1	Yes supported
	// | 
	// | 13		Split-Parallel readout
	// | 			0	Not supported
	// | 			1	Yes supported
	// | 
	// | 14		MPP = Inverted parallel clocks
	// | 			0	Not supported
	// | 			1	Yes supported
	// | 
	// | 16,15		Clock Driver Board
	// | 			00	Rev. 3
	// | 			11	No clock driver board (Gen I)
	// | 
	// | 19,18,17		Special implementations
	// | 			000 	Somewhere else
	// | 			001	Mount Laguna Observatory
	// | 			010	NGST Aladdin
	// |            011 2x FO Transmitter
	// | 			xxx	Other	
	// +----------------------------------------------------------------------------------------
	constexpr auto CCDVIDREV3B		= static_cast<std::uint32_t>( 0x000000 );	// CCD Video Processor Rev. 3
	constexpr auto ARC41			= static_cast<std::uint32_t>( 0x000000 );	
	constexpr auto VIDGENI			= static_cast<std::uint32_t>( 0x000001 );	// CCD Video Processor Gen I
	constexpr auto IRREV4			= static_cast<std::uint32_t>( 0x000002 );	// IR Video Processor Rev. 4
	constexpr auto ARC42			= static_cast<std::uint32_t>( 0x000002 );	
	constexpr auto COADDER			= static_cast<std::uint32_t>( 0x000003 );	// IR Coadder
	constexpr auto ARC44			= static_cast<std::uint32_t>( 0x000003 );
	constexpr auto CCDVIDREV5		= static_cast<std::uint32_t>( 0x000004 );	// Differential input CCD video Rev. 5
	constexpr auto ARC45			= static_cast<std::uint32_t>( 0x000004 );
	constexpr auto IR8X				= static_cast<std::uint32_t>( 0x000005 );	// 8x IR
	constexpr auto ARC46			= static_cast<std::uint32_t>( 0x000005 );	// 8-channel IR video board
	constexpr auto ARC48			= static_cast<std::uint32_t>( 0x000006 );	// 8-channel CCD video board
	constexpr auto ARC47			= static_cast<std::uint32_t>( 0x000007 );	// 4-channel CCD video board
	constexpr auto TIMREV4			= static_cast<std::uint32_t>( 0x000000 );	// Timing Revision 4 = 50 MHz
	constexpr auto ARC20			= static_cast<std::uint32_t>( 0x000000 );
	constexpr auto TIMGENI			= static_cast<std::uint32_t>( 0x000008 );	// Timing Gen I = 40 MHz
	constexpr auto TIMREV5			= static_cast<std::uint32_t>( 0x000010 );	// Timing Revision 5 = 250 MHz
	constexpr auto ARC22			= static_cast<std::uint32_t>( 0x000010 );
	constexpr auto UTILREV3			= static_cast<std::uint32_t>( 0x000020 );	// Utility Rev. 3 supported
	constexpr auto ARC50			= static_cast<std::uint32_t>( 0x000020 );
	constexpr auto SHUTTER_CC		= static_cast<std::uint32_t>( 0x000080 );	// Shutter supported
	constexpr auto TEMP_SIDIODE		= static_cast<std::uint32_t>( 0x000100 );	// Polynomial calibration
	constexpr auto TEMP_LINEAR		= static_cast<std::uint32_t>( 0x000200 );	// Linear calibration
	constexpr auto SUBARRAY			= static_cast<std::uint32_t>( 0x000400 );	// Subarray readout supported
	constexpr auto BINNING			= static_cast<std::uint32_t>( 0x000800 );	// Binning supported
	constexpr auto SPLIT_SERIAL		= static_cast<std::uint32_t>( 0x001000 );	// Split serial supported
	constexpr auto SPLIT_PARALLEL	= static_cast<std::uint32_t>( 0x002000 );	// Split parallel supported
	constexpr auto MPP_CC			= static_cast<std::uint32_t>( 0x004000 );	// Inverted clocks supported
	constexpr auto ARC32			= static_cast<std::uint32_t>( 0x008000 );	// CCD & IR clock driver board
	constexpr auto CLKDRVGENI		= static_cast<std::uint32_t>( 0x018000 );	// No clock driver board - Gen I
	constexpr auto MLO				= static_cast<std::uint32_t>( 0x020000 );	// Set if Mount Laguna Observatory
	constexpr auto NGST				= static_cast<std::uint32_t>( 0x040000 );	// NGST Aladdin implementation
	constexpr auto FO_2X_TRANSMITR	= static_cast<std::uint32_t>( 0x060000 );	// 2x FO transmitters
	constexpr auto CONT_RD			= static_cast<std::uint32_t>( 0x100000 );	// continuous readout implemented
	constexpr auto SEL_READ_SPEED	= static_cast<std::uint32_t>( 0x200000 );	// Selectable readout speeds

	#define	ALL_READOUTS	( SPLIT_SERIAL | SPLIT_PARALLEL )

	// +----------------------------------------------------------------------------------------
	// | 'SOS' Array Amplifier Parameters
	// +----------------------------------------------------------------------------------------
	constexpr auto AMP_0			= static_cast<std::uint32_t>( 0x5F5F43 );		//  Ascii __C amp 0
	constexpr auto AMP_1			= static_cast<std::uint32_t>( 0x5F5F44 );		//  Ascii __D amp 1
	constexpr auto AMP_2			= static_cast<std::uint32_t>( 0x5F5F42 );		//  Ascii __B amp 2
	constexpr auto AMP_3			= static_cast<std::uint32_t>( 0x5F5F41 );		//  Ascii __A amp 3
	constexpr auto AMP_L			= static_cast<std::uint32_t>( 0x5F5F4C );		//  Ascii __L left amp
	constexpr auto AMP_R			= static_cast<std::uint32_t>( 0x5F5F52 );		//  Ascii __R left amp
	constexpr auto AMP_LR			= static_cast<std::uint32_t>( 0x5F4C52 );		//  Ascii _LR right two amps
	constexpr auto AMP_ALL			= static_cast<std::uint32_t>( 0x414C4C );		//  Ascii ALL four amps (quad)


	// +-----------------------------------------------------------
	// | Define continuous readout modes
	// +-----------------------------------------------------------
	constexpr auto CR_WRITE			= static_cast< std::uint32_t >( 0 );
	constexpr auto CR_COADD			= static_cast< std::uint32_t >( 1 );
	constexpr auto CR_DEBUG			= static_cast< std::uint32_t >( 2 );
 

	// +----------------------------------------------------------------------------------------
	// A valid start address must be less than 0x4000 for
	// the load DSP file in timming or utility boards.
	// +----------------------------------------------------------------------------------------
	constexpr auto MAX_DSP_START_LOAD_ADDR = static_cast<std::uint32_t>( 0x4000 );


	// +-----------------------------------------------------------
	// | Shutter Position Constants
	// +-----------------------------------------------------------
	constexpr auto OPEN_SHUTTER_POSITION	= static_cast< std::uint32_t >( ( 1 << 11 ) );
	constexpr auto CLOSED_SHUTTER_POSITION	= static_cast< std::uint32_t >( ~( 1 << 11 ) );


	// +----------------------------------------------------------------------------
	// |  Macro that returns 'true' if the parameter indicates the ARC-12
	// |  ( SmallCam ) controller; returns 'false' otherwise.
	// +----------------------------------------------------------------------------
	constexpr auto IS_ARC12( std::uint32_t id ) { return ( ( ( ( id & 0xFF0000 ) >> 16 ) == 'S' && ( ( id & 0x00FF00 ) >> 8 ) == 'C' ) ? true : false ); }


	// +-----------------------------------------------------------
	// | SmallCam Synthetic Image Commands & Arguments
	// +-----------------------------------------------------------
	constexpr auto SIM = static_cast<std::uint32_t>( 0x0053494D );

	enum class syntheticImage_t : std::uint32_t
	{
		SYN_IMG_DISABLE = 0,
		SYN_IMG_FIXED,
		SYN_IMG_RAMP,
		SYN_IMG_RESET
	};

#ifdef __cplusplus
}	// end namespace
#endif
