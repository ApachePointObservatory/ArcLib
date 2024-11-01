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
	constexpr std::uint32_t PCI_ID				= static_cast<std::uint32_t>( 1 );
	constexpr std::uint32_t TIM_ID				= static_cast<std::uint32_t>( 2 );
	constexpr std::uint32_t UTIL_ID				= static_cast<std::uint32_t>( 3 );
	constexpr std::uint32_t SMALLCAM_DLOAD_ID	= static_cast<std::uint32_t>( 3 );


	// +----------------------------------------------------------------------------------------
	// | Memory Location Id Constants
	// | R_MEM	(Bit 20)  ROM
	// | P_MEM	(Bit 21)  DSP program memory space
	// | X_MEM	(Bit 22)  DSP X memory space
	// | Y_MEM	(Bit 23)  DSP Y memory space
	// +----------------------------------------------------------------------------------------
	constexpr std::uint32_t P_MEM		= static_cast<std::uint32_t>( 0x100000 );
	constexpr std::uint32_t X_MEM 		= static_cast<std::uint32_t>( 0x200000 );
	constexpr std::uint32_t Y_MEM		= static_cast<std::uint32_t>( 0x400000 );
	constexpr std::uint32_t R_MEM		= static_cast<std::uint32_t>( 0x800000 );


	// +----------------------------------------------------------------------------------------
	// | Define controller/PCI replies
	// +----------------------------------------------------------------------------------------
	constexpr std::uint32_t TOUT			= static_cast<std::uint32_t>( 0x544F5554 );		// Timeout
	constexpr std::uint32_t ROUT			= static_cast<std::uint32_t>( 0x524F5554 );		// Readout
	constexpr std::uint32_t HERR			= static_cast<std::uint32_t>( 0x48455252 );		// Header Error
	constexpr std::uint32_t DON			= static_cast<std::uint32_t>( 0x00444F4E );		// Done
	constexpr std::uint32_t ERR			= static_cast<std::uint32_t>( 0x00455252 );		// Error
	constexpr std::uint32_t SYR			= static_cast<std::uint32_t>( 0x00535952 );		// System reset
	constexpr std::uint32_t RST			= static_cast<std::uint32_t>( 0x00525354 );		// reset
	constexpr std::uint32_t CNR			= static_cast<std::uint32_t>( 0x00434E52 );		// Controller Not Ready


	// +----------------------------------------------------------------------------------------
	// | Define controller/manual commands
	// +----------------------------------------------------------------------------------------
	constexpr std::uint32_t DBC			= static_cast<std::uint32_t>( 0x00444243 );		// Download Boot Code
	constexpr std::uint32_t TDL			= static_cast<std::uint32_t>( 0x0054444C );		// Test Data Link
	constexpr std::uint32_t RDM			= static_cast<std::uint32_t>( 0x0052444D );		// ReaD Memory
	constexpr std::uint32_t WRM			= static_cast<std::uint32_t>( 0x0057524D );		// WRite Memory
	constexpr std::uint32_t SEX			= static_cast<std::uint32_t>( 0x00534558 );		// Start EXposure
	constexpr std::uint32_t SET			= static_cast<std::uint32_t>( 0x00534554 );		// Set Exposure Time
	constexpr std::uint32_t PEX			= static_cast<std::uint32_t>( 0x00504558 );		// Pause EXposure
	constexpr std::uint32_t REX			= static_cast<std::uint32_t>( 0x00524558 );		// Resume EXposure
	constexpr std::uint32_t RET			= static_cast<std::uint32_t>( 0x00524554 );		// Read Elapsed Time
	constexpr std::uint32_t PON			= static_cast<std::uint32_t>( 0x00504F4E );		// Power ON
	constexpr std::uint32_t POF			= static_cast<std::uint32_t>( 0x00504F46 );		// Power OFf
	constexpr std::uint32_t RDI			= static_cast<std::uint32_t>( 0x00524449 );		// ReaD Image
	constexpr std::uint32_t SOS			= static_cast<std::uint32_t>( 0x00534F53 );		// Select Output Source
	constexpr std::uint32_t MPP			= static_cast<std::uint32_t>( 0x004D5050 );		// Multi-Pinned Phase mode
	constexpr std::uint32_t DCA			= static_cast<std::uint32_t>( 0x00444341 );		// Download CoAdder
	constexpr std::uint32_t SNF			= static_cast<std::uint32_t>( 0x00534E46 );		// Set Number of Frames
	constexpr std::uint32_t FPB			= static_cast<std::uint32_t>( 0x00465042 );		// Set the Frames-Per-Buffer for coadds
	constexpr std::uint32_t VID			= static_cast<std::uint32_t>( 0x00564944 );		// mnemonic that means VIDeo board
	constexpr std::uint32_t SBN			= static_cast<std::uint32_t>( 0x0053424E );		// Set Bias Number
	constexpr std::uint32_t SBV			= static_cast<std::uint32_t>( 0x00534256 );		// Set Bias Voltage
	constexpr std::uint32_t SGN			= static_cast<std::uint32_t>( 0x0053474E );		// Set GaiN
	constexpr std::uint32_t SMX			= static_cast<std::uint32_t>( 0x00534D58 );		// Select MultipleXer
	constexpr std::uint32_t CLK			= static_cast<std::uint32_t>( 0x00434C4B );		// mnemonic that means CLocK driver board
	constexpr std::uint32_t SSS			= static_cast<std::uint32_t>( 0x00535353 );		// Set Subarray Sizes
	constexpr std::uint32_t SSP			= static_cast<std::uint32_t>( 0x00535350 );		// Set Subarray Positions
	constexpr std::uint32_t LGN			= static_cast<std::uint32_t>( 0x004C474E );		// Set Low Gain
	constexpr std::uint32_t HGN			= static_cast<std::uint32_t>( 0x0048474E );		// Set High Gain
	constexpr std::uint32_t SRM			= static_cast<std::uint32_t>( 0x0053524D );		// Set Readout Mode - either CDS or single
	constexpr std::uint32_t CDS			= static_cast<std::uint32_t>( 0x00434453 );		// Correlated Double Sampling
	constexpr std::uint32_t SFS			= static_cast<std::uint32_t>( 0x00534653 );		// Send Fowler Sample
	constexpr std::uint32_t SPT			= static_cast<std::uint32_t>( 0x00535054 );		// Set Pass Through mode
	constexpr std::uint32_t LDA			= static_cast<std::uint32_t>( 0x004C4441 );		// LoaD Application
	constexpr std::uint32_t RCC			= static_cast<std::uint32_t>( 0x00524343 );		// Read Controller Configuration
	constexpr std::uint32_t CLR			= static_cast<std::uint32_t>( 0x00434C52 );		// CleaR Array
	constexpr std::uint32_t IDL			= static_cast<std::uint32_t>( 0x0049444C );		// IDLe
	constexpr std::uint32_t STP			= static_cast<std::uint32_t>( 0x00535450 );		// SToP idle
	constexpr std::uint32_t CSH			= static_cast<std::uint32_t>( 0x00435348 );		// close SHutter
	constexpr std::uint32_t OSH			= static_cast<std::uint32_t>( 0x004F5348 );		// open SHutter
	constexpr std::uint32_t SUR			= static_cast<std::uint32_t>( 0x00535552 );		// Set Up the Ramp mode
	constexpr std::uint32_t MH1			= static_cast<std::uint32_t>( 0x004D4831 );		// Move NIRIM Filter Wheel 1 Home
	constexpr std::uint32_t MM1			= static_cast<std::uint32_t>( 0x004D4D31 );		// Move NIRIM Filter Wheel 1
	constexpr std::uint32_t MH2			= static_cast<std::uint32_t>( 0x004D4832 );		// Move NIRIM Filter Wheel 2 Home
	constexpr std::uint32_t MM2			= static_cast<std::uint32_t>( 0x004D4D32 );		// Move NIRIM Filter Wheel 2
	constexpr std::uint32_t SBS			= static_cast<std::uint32_t>( 0x00534253 );		// Set Hardware Byte Swapping
	constexpr std::uint32_t TBS			= static_cast<std::uint32_t>( 0x00544253 );		// Test for Hardware Byte Swapping
	constexpr std::uint32_t RNC			= static_cast<std::uint32_t>( 0x00524E43 );		// Read Number of Channels for Hawaii RG array
	constexpr std::uint32_t THG			= static_cast<std::uint32_t>( 0x00544847 );		// Test High Gain - Utility Board Temperature
	constexpr std::uint32_t SID			= static_cast<std::uint32_t>( 0x00534944 );		// System ID - FastCam
	constexpr std::uint32_t JDL			= static_cast<std::uint32_t>( 0x004A444C );		// Jump to DownLoad for C based ARC-22 systems
	constexpr std::uint32_t XMT			= static_cast<std::uint32_t>( 0x00584D54 );		// Set/Clear fiber optic 2x transmitter mode
	constexpr std::uint32_t ABR			= static_cast<std::uint32_t>( 0x00414252 );		// ABort Readout & Exposure
	constexpr std::uint32_t STM			= static_cast<std::uint32_t>( 0x0053544D );		// Set Trigger Mode

	constexpr std::uint32_t CDT			= static_cast<std::uint32_t>( 0x00434454 );		// SmallCam control detector temperature
	constexpr std::uint32_t RDT			= static_cast<std::uint32_t>( 0x00524454 );		// SmallCam read detector temperature
	constexpr std::uint32_t RHV			= static_cast<std::uint32_t>( 0x00524856 );		// SmallCam read heater voltage
	constexpr std::uint32_t RDC			= static_cast<std::uint32_t>( 0x00524443 );		// SmallCam read detector current ( actually, DN )

	constexpr std::uint32_t RSC			= static_cast<std::uint32_t>( 0x00525343 );		// FastCam Only - ReSet Controller


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
	constexpr std::uint32_t CCDVIDREV3B		= static_cast<std::uint32_t>( 0x000000 );	// CCD Video Processor Rev. 3
	constexpr std::uint32_t ARC41			= static_cast<std::uint32_t>( 0x000000 );	
	constexpr std::uint32_t VIDGENI			= static_cast<std::uint32_t>( 0x000001 );	// CCD Video Processor Gen I
	constexpr std::uint32_t IRREV4			= static_cast<std::uint32_t>( 0x000002 );	// IR Video Processor Rev. 4
	constexpr std::uint32_t ARC42			= static_cast<std::uint32_t>( 0x000002 );	
	constexpr std::uint32_t COADDER			= static_cast<std::uint32_t>( 0x000003 );	// IR Coadder
	constexpr std::uint32_t ARC44			= static_cast<std::uint32_t>( 0x000003 );
	constexpr std::uint32_t CCDVIDREV5		= static_cast<std::uint32_t>( 0x000004 );	// Differential input CCD video Rev. 5
	constexpr std::uint32_t ARC45			= static_cast<std::uint32_t>( 0x000004 );
	constexpr std::uint32_t IR8X				= static_cast<std::uint32_t>( 0x000005 );	// 8x IR
	constexpr std::uint32_t ARC46			= static_cast<std::uint32_t>( 0x000005 );	// 8-channel IR video board
	constexpr std::uint32_t ARC48			= static_cast<std::uint32_t>( 0x000006 );	// 8-channel CCD video board
	constexpr std::uint32_t ARC47			= static_cast<std::uint32_t>( 0x000007 );	// 4-channel CCD video board
	constexpr std::uint32_t TIMREV4			= static_cast<std::uint32_t>( 0x000000 );	// Timing Revision 4 = 50 MHz
	constexpr std::uint32_t ARC20			= static_cast<std::uint32_t>( 0x000000 );
	constexpr std::uint32_t TIMGENI			= static_cast<std::uint32_t>( 0x000008 );	// Timing Gen I = 40 MHz
	constexpr std::uint32_t TIMREV5			= static_cast<std::uint32_t>( 0x000010 );	// Timing Revision 5 = 250 MHz
	constexpr std::uint32_t ARC22			= static_cast<std::uint32_t>( 0x000010 );
	constexpr std::uint32_t UTILREV3			= static_cast<std::uint32_t>( 0x000020 );	// Utility Rev. 3 supported
	constexpr std::uint32_t ARC50			= static_cast<std::uint32_t>( 0x000020 );
	constexpr std::uint32_t SHUTTER_CC		= static_cast<std::uint32_t>( 0x000080 );	// Shutter supported
	constexpr std::uint32_t TEMP_SIDIODE		= static_cast<std::uint32_t>( 0x000100 );	// Polynomial calibration
	constexpr std::uint32_t TEMP_LINEAR		= static_cast<std::uint32_t>( 0x000200 );	// Linear calibration
	constexpr std::uint32_t SUBARRAY			= static_cast<std::uint32_t>( 0x000400 );	// Subarray readout supported
	constexpr std::uint32_t BINNING			= static_cast<std::uint32_t>( 0x000800 );	// Binning supported
	constexpr std::uint32_t SPLIT_SERIAL		= static_cast<std::uint32_t>( 0x001000 );	// Split serial supported
	constexpr std::uint32_t SPLIT_PARALLEL	= static_cast<std::uint32_t>( 0x002000 );	// Split parallel supported
	constexpr std::uint32_t MPP_CC			= static_cast<std::uint32_t>( 0x004000 );	// Inverted clocks supported
	constexpr std::uint32_t ARC32			= static_cast<std::uint32_t>( 0x008000 );	// CCD & IR clock driver board
	constexpr std::uint32_t CLKDRVGENI		= static_cast<std::uint32_t>( 0x018000 );	// No clock driver board - Gen I
	constexpr std::uint32_t MLO				= static_cast<std::uint32_t>( 0x020000 );	// Set if Mount Laguna Observatory
	constexpr std::uint32_t NGST				= static_cast<std::uint32_t>( 0x040000 );	// NGST Aladdin implementation
	constexpr std::uint32_t FO_2X_TRANSMITR	= static_cast<std::uint32_t>( 0x060000 );	// 2x FO transmitters
	constexpr std::uint32_t CONT_RD			= static_cast<std::uint32_t>( 0x100000 );	// continuous readout implemented
	constexpr std::uint32_t SEL_READ_SPEED	= static_cast<std::uint32_t>( 0x200000 );	// Selectable readout speeds

	#define	ALL_READOUTS	( SPLIT_SERIAL | SPLIT_PARALLEL )

	// +----------------------------------------------------------------------------------------
	// | 'SOS' Array Amplifier Parameters
	// +----------------------------------------------------------------------------------------
	constexpr std::uint32_t AMP_0			= static_cast<std::uint32_t>( 0x5F5F43 );		//  Ascii __C amp 0
	constexpr std::uint32_t AMP_1			= static_cast<std::uint32_t>( 0x5F5F44 );		//  Ascii __D amp 1
	constexpr std::uint32_t AMP_2			= static_cast<std::uint32_t>( 0x5F5F42 );		//  Ascii __B amp 2
	constexpr std::uint32_t AMP_3			= static_cast<std::uint32_t>( 0x5F5F41 );		//  Ascii __A amp 3
	constexpr std::uint32_t AMP_L			= static_cast<std::uint32_t>( 0x5F5F4C );		//  Ascii __L left amp
	constexpr std::uint32_t AMP_R			= static_cast<std::uint32_t>( 0x5F5F52 );		//  Ascii __R left amp
	constexpr std::uint32_t AMP_LR			= static_cast<std::uint32_t>( 0x5F4C52 );		//  Ascii _LR right two amps
	constexpr std::uint32_t AMP_ALL			= static_cast<std::uint32_t>( 0x414C4C );		//  Ascii ALL four amps (quad)


	// +-----------------------------------------------------------
	// | Define continuous readout modes
	// +-----------------------------------------------------------
	constexpr std::uint32_t CR_WRITE			= static_cast< std::uint32_t >( 0 );
	constexpr std::uint32_t CR_COADD			= static_cast< std::uint32_t >( 1 );
	constexpr std::uint32_t CR_DEBUG			= static_cast< std::uint32_t >( 2 );
 

	// +----------------------------------------------------------------------------------------
	// A valid start address must be less than 0x4000 for
	// the load DSP file in timming or utility boards.
	// +----------------------------------------------------------------------------------------
	constexpr std::uint32_t MAX_DSP_START_LOAD_ADDR = static_cast<std::uint32_t>( 0x4000 );


	// +-----------------------------------------------------------
	// | Shutter Position Constants
	// +-----------------------------------------------------------
	constexpr std::uint32_t OPEN_SHUTTER_POSITION	= static_cast< std::uint32_t >( ( 1 << 11 ) );
	constexpr std::uint32_t CLOSED_SHUTTER_POSITION	= static_cast< std::uint32_t >( ~( 1 << 11 ) );


	// +----------------------------------------------------------------------------
	// |  Macro that returns 'true' if the parameter indicates the ARC-12
	// |  ( SmallCam ) controller; returns 'false' otherwise.
	// +----------------------------------------------------------------------------
	constexpr std::uint32_t IS_ARC12( std::uint32_t id ) { return ( ( ( ( id & 0xFF0000 ) >> 16 ) == 'S' && ( ( id & 0x00FF00 ) >> 8 ) == 'C' ) ? true : false ); }


	// +-----------------------------------------------------------
	// | SmallCam Synthetic Image Commands & Arguments
	// +-----------------------------------------------------------
	constexpr std::uint32_t SIM = static_cast<std::uint32_t>( 0x0053494D );

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
