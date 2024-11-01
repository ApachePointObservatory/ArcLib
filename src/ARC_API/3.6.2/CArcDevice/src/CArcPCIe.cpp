// +----------------------------------------------------------------------+
//  CArcPCIe.cpp : Defines the exported functions for the DLL application.
// +----------------------------------------------------------------------+
//
// KNOWN PROBLEMS:
//
// 1. Dec 15, 2010 - Using DS9 to display image buffer data and then
//    switching devices prevents old gen3 from being closed, thus the
//    module use count doesn't get decremented.  This will result in a
//    "gen3 busy" error.  Only fix is to terminate the application.
//    I think DS9 doesn't release image buffer data, which prevents the
//    close() system call from being called.
//
//    Symptoms:
//    1. open gen3 0, then switch to gen3 1, then back to gen3 0
//       works; UNLESS DS9 is started and images taken and displayed.
//       THEN, gen3 used to take images will not be closed and module
//       use count ( as seen by /sbin/lsmod ) doesn't decrement.
//    2. The system function close() doesn't call the driver xxxClose
//       function when DS9 is used to display images as described in 1.
// +----------------------------------------------------------------------+

#ifdef _WINDOWS
	#define INITGUID
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <list>
#include <cstddef>
#include <algorithm>
#include <array>
#include <ctime>
#include <ios>

#include <CArcBase.h>
#include <CArcStringList.h>
#include <CArcPCIe.h>
#include <Reg9056.h>
#include <PCIRegs.h>

#ifdef _WINDOWS
	#include <setupapi.h>
	#include <devguid.h>
	#include <regstr.h>
	#include <AstroPCIeGUID.h>
#endif

#if defined( linux ) || defined( __linux )
	#include <sys/ioctl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/mman.h>
	#include <dirent.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <cstring>
#endif

#ifdef __APPLE__
	#include <sys/types.h>
	#include <sys/mman.h>
#endif

using namespace std::string_literals;
using namespace std::string_view_literals;

//#include <fstream>
//std::ofstream dbgStream2( "CArcPCIe_Debug.txt" );


namespace arc
{
	namespace gen3
	{

		// +----------------------------------------------------------------------------
		// |  PCIe Device Info
		// +----------------------------------------------------------------------------
		#if defined( linux ) || defined( __linux )

			constexpr auto DEVICE_DIR			= "/dev/"sv;
			constexpr auto DEVICE_NAME			= "AstroPCIe"sv;
			constexpr auto DEVICE_NAME_ALT		= "Arc66PCIe"sv;

		#elif defined( __APPLE__ )

			constexpr auto AstroPCIeClassName	 = com_arc_driver_Arc66PCIe;
			constexpr auto kAstroPCIeClassName	= "com_arc_driver_Arc66PCIe";

		#endif


		// +----------------------------------------------------------------------------
		// |  PLX Register Structure
		// +----------------------------------------------------------------------------
		typedef struct PLX_REG_ITEM
		{
			std::uint32_t	uiAddr;
			std::string		sText;
		} PLXRegItem;



		// +----------------------------------------------------------------------------
		// |  PLX BAR Address To String Translation Tables
		// +----------------------------------------------------------------------------
		const auto LCRMapName = "Local Config (BAR0)"s;
		PLXRegItem LCRMap[ 18 ] = { { PCI9056_SPACE0_RANGE, "Direct Slave Local Address Space 0 Range"s },
									{ PCI9056_SPACE0_REMAP, "Direct Slave Local Address Space 0 ( Remap )"s },
									{ PCI9056_LOCAL_DMA_ARBIT, "Mode/DMA Arbitration"s },
									{ PCI9056_ENDIAN_DESC, "Local Misc Ctrl 2/EEPROM Addr Boundary/Local Misc Ctrl 1/Endian Descriptor"s },
									{ PCI9056_EXP_ROM_RANGE, "Direct Slave Expansion ROM Range"s },
									{ PCI9056_EXP_ROM_REMAP, "Direct Slave Exp ROM Local Base Addr (Remap) & BREQo Ctrl"s },
									{ PCI9056_SPACE0_ROM_DESC, "Local Addr Space 0/Expansion ROM Bus Region Descriptor"s },
									{ PCI9056_DM_RANGE, "Local Range Direct Master-to-PCIe"s },
									{ PCI9056_DM_MEM_BASE, "Local Base Addr Direct Master-to-PCIe Memory"s },
									{ PCI9056_DM_IO_BASE, "Local Base Addr Direct Master-to-PCIe I/O Configuration"s },
									{ PCI9056_DM_PCI_MEM_REMAP, "PCIe Base Addr (Remap) Master-to-PCIe Memory"s },
									{ PCI9056_DM_PCI_IO_CONFIG, "PCI Config Addr Direct Master-to-PCIe I/O Configuration"s },
									{ PCI9056_SPACE1_RANGE, "Direct Slave Local Addr Space 1 Range"s },
									{ PCI9056_SPACE1_REMAP, "Direct Slave Local Addr Space 1 Local Base Addr (Remap)"s },
									{ PCI9056_SPACE1_DESC, "Local Addr Space 1 Bus Region Descriptor"s },
									{ PCI9056_DM_DAC, "Direct Master PCIe Dual Addr Cycles Upper Addr"s },
									{ PCI9056_ARBITER_CTRL, "Internal Arbiter Control"s },
									{ PCI9056_ABORT_ADDRESS, "PCI Abort Address"s } };

		const auto RTRMapName = "Runtime Regs (BAR0)"s;
		PLXRegItem RTRMap[ 14 ] = { { PCI9056_MAILBOX0, "Mailbox 0"s },
									{ PCI9056_MAILBOX1, "Mailbox 1"s },
									{ PCI9056_MAILBOX2, "Mailbox 2"s },
									{ PCI9056_MAILBOX3, "Mailbox 3"s },
									{ PCI9056_MAILBOX4, "Mailbox 4"s },
									{ PCI9056_MAILBOX5, "Mailbox 5"s },
									{ PCI9056_MAILBOX6, "Mailbox 6"s },
									{ PCI9056_MAILBOX7, "Mailbox 7"s },
									{ PCI9056_LOCAL_DOORBELL, "PCIe-to-Local Doorbell"s },
									{ PCI9056_PCI_DOORBELL, "Local-to-PCIe Doorbell"s },
									{ PCI9056_INT_CTRL_STAT, "Interrupt Control/Status"s },
									{ PCI9056_EEPROM_CTRL_STAT, "Serial EEPROM Ctrl, PCI Cmd Codes, User I/O Ctrl, Init Ctrl"s },
									{ PCI9056_PERM_VENDOR_ID, "Device ID / Vendor ID"s },
									{ PCI9056_REVISION_ID, "Reserved / PCI Hardwired Revision ID"s } };

		const auto DMAMapName = "DMA Regs (BAR0)"s;
		PLXRegItem DMAMap[ 15 ] = { { PCI9056_DMA0_MODE, "DMA Channel 0 Mode"s },
									{ PCI9056_DMA0_PCI_ADDR, "DMA Channel 0 PCIe Address"s },
									{ PCI9056_DMA0_LOCAL_ADDR, "DMA Channel 0 Local Address"s },
									{ PCI9056_DMA0_COUNT, "DMA Channel 0 Transfer Size (Bytes)"s },
									{ PCI9056_DMA0_DESC_PTR, "DMA Channel 0 Descriptor Pointer"s },
									{ PCI9056_DMA1_MODE, "DMA Channel 1 Mode"s },
									{ PCI9056_DMA1_PCI_ADDR, "DMA Channel 1 PCIe Address"s },
									{ PCI9056_DMA1_LOCAL_ADDR, "DMA Channel 1 Local Address"s },
									{ PCI9056_DMA1_COUNT, "DMA Channel 1 Transfer Size (Bytes)"s },
									{ PCI9056_DMA1_DESC_PTR, "DMA Channel 1 Descriptor Pointer"s },
									{ PCI9056_DMA_COMMAND_STAT, "Reserved / DMA Ch 1 Cmd-Status / DMA Ch 0 Cmd-Status"s },
									{ PCI9056_DMA_ARBIT, "DMA Arbitration"s },
									{ PCI9056_DMA_THRESHOLD, "DMA Threshold"s },
									{ PCI9056_DMA0_PCI_DAC, "DMA Channel 0 PCIe Dual Addr Cycle Upper Addr"s },
									{ PCI9056_DMA1_PCI_DAC, "DMA Channel 1 PCIe Dual Addr Cycle Upper Addr"s } };

		const auto MSQMapName = "Msg Q Regs (BAR0)"s;
		PLXRegItem MSQMap[ 13 ] = { { PCI9056_OUTPOST_INT_STAT, "Outbound Post Queue Interrupt Status"s },
									{ PCI9056_OUTPOST_INT_MASK, "Outbound Post Queue Interrupt Mask"s },
									{ PCI9056_MU_CONFIG, "Messaging Queue Configuration"s },
									{ PCI9056_FIFO_BASE_ADDR, "Queue Base Address"s },
									{ PCI9056_INFREE_HEAD_PTR, "Inbound Free Head Pointer"s },
									{ PCI9056_INFREE_TAIL_PTR, "Inbound Free Tail Pointer"s },
									{ PCI9056_INPOST_HEAD_PTR, "Inbound Post Head Pointer"s },
									{ PCI9056_INPOST_TAIL_PTR, "Inbound Post Tail Pointer"s },
									{ PCI9056_OUTFREE_HEAD_PTR, "Outbound Free Head Pointer"s },
									{ PCI9056_OUTFREE_TAIL_PTR, "Outbound Free Tail Pointer"s },
									{ PCI9056_OUTPOST_HEAD_PTR, "Outbound Post Head Pointer"s },
									{ PCI9056_OUTPOST_TAIL_PTR, "Outbound Post Tail Pointer"s },
									{ PCI9056_FIFO_CTRL_STAT, "Reserved / Queue Control-Status"s } };


		// +----------------------------------------------------------------------------
		// |  Device Name Sorting Function
		// +----------------------------------------------------------------------------
		bool DevListSort( arc::gen3::arcDev_t i, arc::gen3::arcDev_t j )
		{
			return ( ( i.sName.compare( j.sName ) ) != 0 );
		}


		// +----------------------------------------------------------------------------
		// |  Static Class Member Initialization
		// +----------------------------------------------------------------------------
		std::unique_ptr<std::vector<arc::gen3::arcDev_t>> CArcPCIe::m_pvDevList( new std::vector<arc::gen3::arcDev_t>() );
		std::shared_ptr<std::string[]> CArcPCIe::m_psDevList;


		// +----------------------------------------------------------------------------
		// |  Constructor
		// +----------------------------------------------------------------------------
		// |  See CArcPCIe.h for the class definition
		// +----------------------------------------------------------------------------
		CArcPCIe::CArcPCIe( void )
		{
			m_fnValidDeviceVendorId = []( const std::uint16_t uiDevVenId )
			{
				return ( ( uiDevVenId == 0x9056U || uiDevVenId == 0x10B5U ) ? 1U : 0U );
			};

			m_fnVerify24Bits = []( const std::uint32_t uiValue )
			{
				if ( ( uiValue & 0xFF000000U ) != 0 )
				{
					throwArcGen3Error( "Data value %u [ 0x%X ] too large! Must be 24-bits or less!", uiValue, uiValue );
				}
			};

			fnPCIeStatusReplyRecvd = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000003U ) == 2U ? true : false );
			};

			fnPCIeStatusControllerReset = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000008U ) > 0U ? true : false );
			};

			fnPCIeStatusReadout = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000004U ) > 0U ? true : false );
			};

			fnPCIeStatusIdle = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000003U ) == 0U ? true : false );
			};

			fnPCIeStatusCmdSent = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000003U ) == 1U ? true : false );
			};

			fnPCIeStatusImageReadTimeOut = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000020U ) > 0U ? true : false );
			};

			fnPCIeStatusHeaderError = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000010U ) > 0U ? true : false );
			};

			fnPCIeStatusFiber2xReceiver = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000200U ) > 0U ? true : false );
			};

			fnPCIeStatusFiberAConnected = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000080U ) > 0U ? true : false );
			};

			fnPCIeStatusFiberBConnected = []( const std::uint32_t uiStatus )
			{
				return ( ( uiStatus & 0x00000100U ) > 0U ? true : false );
			};

			m_hDevice = INVALID_HANDLE_VALUE;
		}


		// +----------------------------------------------------------------------------
		// |  Destructor
		// +----------------------------------------------------------------------------
		CArcPCIe::~CArcPCIe( void )
		{
			close();
		}


		// +----------------------------------------------------------------------------
		// |  toString
		// +----------------------------------------------------------------------------
		// |  Returns a std::string that represents the gen3 controlled by this library.
		// +----------------------------------------------------------------------------
		constexpr std::string_view CArcPCIe::toString()
		{
			return "PCIe [ ARC-66 / 67 ]";
		}


		// +----------------------------------------------------------------------------
		// |  findDevices
		// +----------------------------------------------------------------------------
		// |  Searches for available ARC, Inc PCIe devices and stores the list, which
		// |  can be accessed via gen3 number ( 0,1,2... ).
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCIe::findDevices()
		{
			if ( m_pvDevList == nullptr )
			{
				m_pvDevList.reset( new std::vector<arc::gen3::arcDev_t>() );
			}

			if ( m_pvDevList == nullptr )
			{
				throwArcGen3Error( "Failed to allocate resources for PCIe device list!"s );
			}

			#ifdef __APPLE__

				if ( !m_pvDevList->empty() )
				{
					//
					// Don't generate a new list on MAC, the stored service
					// object is currently in use by the open gen3.
					//
					return;
				}

			#else

				if ( !m_pvDevList->empty() )
				{
					m_pvDevList->clear();

					if ( m_pvDevList->size() > 0 )
					{
						throwArcGen3Error( "Failed to free existing device list!"s );
					}
				}

			#endif

			#ifdef _WINDOWS

				PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData = nullptr;
				SP_DEVICE_INTERFACE_DATA tDeviceInterfaceData;
				HDEVINFO hHardwareDeviceInfo;

				BOOL  bResult		   = FALSE;
				DWORD dwRequiredLength = 0;
				DWORD dwMemberIndex	   = 0;

				arc::gen3::CArcBase::zeroMemory( &tDeviceInterfaceData, sizeof( SP_DEVICE_INTERFACE_DATA ) );

				hHardwareDeviceInfo = SetupDiGetClassDevs( ( LPGUID )&GUID_DEVINTERFACE_ARC_PCIE,
														   nullptr,
														   nullptr,
														  ( DIGCF_PRESENT | DIGCF_DEVICEINTERFACE ) );

				if ( hHardwareDeviceInfo == INVALID_HANDLE_VALUE )
				{
					throwArcGen3Error( "SetupDiGetClassDevs failed!"s );
				}

				tDeviceInterfaceData.cbSize = sizeof( SP_DEVICE_INTERFACE_DATA );

				while ( 1 )
				{
					bResult = SetupDiEnumDeviceInterfaces( hHardwareDeviceInfo,
														   0,
														   ( LPGUID )&GUID_DEVINTERFACE_ARC_PCIE,
														   dwMemberIndex,
														   &tDeviceInterfaceData );

					if ( bResult == FALSE )
					{
						SetupDiDestroyDeviceInfoList( hHardwareDeviceInfo );
						break;
					}

					SetupDiGetDeviceInterfaceDetail( hHardwareDeviceInfo,
													 &tDeviceInterfaceData,
													 nullptr,
													 0,
													 &dwRequiredLength,
													 nullptr );

					pDeviceInterfaceDetailData = ( PSP_DEVICE_INTERFACE_DETAIL_DATA )
													LocalAlloc( LMEM_FIXED, dwRequiredLength );

					if ( pDeviceInterfaceDetailData == nullptr )
					{
						SetupDiDestroyDeviceInfoList( hHardwareDeviceInfo );
						break;
					}

					pDeviceInterfaceDetailData->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );

					bResult = SetupDiGetDeviceInterfaceDetail( hHardwareDeviceInfo,
															   &tDeviceInterfaceData,
															   pDeviceInterfaceDetailData,
															   dwRequiredLength,
															   &dwRequiredLength,
															   nullptr );

					if ( bResult == FALSE )
					{
						SetupDiDestroyDeviceInfoList( hHardwareDeviceInfo );
						LocalFree( pDeviceInterfaceDetailData );
						break;
					}

					arc::gen3::arcDev_t tArcDev;

					tArcDev.sName = CArcBase::convertWideToAnsi( ( LPWSTR )pDeviceInterfaceDetailData->DevicePath );

					m_pvDevList->push_back( tArcDev );

					dwMemberIndex++;
				}

			#elif defined( __APPLE__ )

				io_service_t  service;
				io_iterator_t iterator;

				//
				// Look up the objects we wish to open. This example uses simple class
				// matching (IOServiceMatching()) to find instances of the class defined by the kext.
				//
				// Because Mac OS X has no weak-linking support in the kernel, the only way to
				// support mutually-exclusive KPIs is to provide separate kexts. This in turn means that the
				// separate kexts must have their own unique CFBundleIdentifiers and I/O Kit class names.
				//
				// This sample shows how to do this in the SimpleUserClient and SimpleUserClient_10.4 Xcode targets.
				//
				// From the userland perspective, a process must look for any of the class names it is prepared to talk to.
				//
				// This creates an io_iterator_t of all instances of our driver that exist in the I/O Registry.
				//
				kern_return_t kernResult = IOServiceGetMatchingServices( kIOMasterPortDefault,
																		 IOServiceMatching( kAstroPCIeClassName ),
																		 &iterator );

				if ( kernResult != KERN_SUCCESS )
				{
					throwArcGen3Error( "IOServiceGetMatchingServices failed: 0x%X",	kernResult );
				}

				while ( ( service = IOIteratorNext( iterator ) ) != IO_OBJECT_nullptr )
				{
					arc::gen3::arcDev_t tArcDev;

					tArcDev.sName			= kAstroPCIeClassName;
					tArcDev.tService		= service;

					m_pvDevList->push_back( tArcDev );
				}

				//
				// Release the io_iterator_t now that we're done with it.
				//
				IOObjectRelease( iterator );


			#else	// LINUX

				struct dirent* pDirEntry = nullptr;
				DIR	*pDir = nullptr;

				pDir = opendir( DEVICE_DIR.data() );

				if ( pDir == nullptr )
				{
					throwArcGen3Error( "Failed to open dir: %s", DEVICE_DIR.data() );
				}
				else
				{
					while ( ( pDirEntry = readdir( pDir ) ) != nullptr )
					{
						std::string sDirEntry( pDirEntry->d_name );

						if ( sDirEntry.find( DEVICE_NAME ) != std::string::npos || sDirEntry.find( DEVICE_NAME_ALT ) != std::string::npos )
						{
							arc::gen3::arcDev_t tArcDev;

							tArcDev.sName   = std::string( DEVICE_DIR ) + sDirEntry;
							
							m_pvDevList->push_back( tArcDev );
						}
					}

					closedir( pDir );
				}

			#endif

			//
			// Make sure the list isn't empty
			//
			if ( m_pvDevList->empty() )
			{
					throwArcGen3Error( "No device bindings exist! Make sure an ARC, Inc PCIe card is installed!"s );
			}
		}


		// +----------------------------------------------------------------------------
		// |  deviceCount
		// +----------------------------------------------------------------------------
		// |  Returns the number of items in the gen3 list. Must be called after
		// |  findDevices().
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::deviceCount( void ) noexcept
		{
			if ( m_pvDevList != nullptr )
			{
				return static_cast< std::uint32_t >(m_pvDevList->size());
			}

			return 0;
		}


		// +----------------------------------------------------------------------------
		// |  getDeviceStringList
		// +----------------------------------------------------------------------------
		// |  Returns a std::string list representation of the gen3 list. Must be called
		// |  after GetDeviceList().
		// +----------------------------------------------------------------------------
		const std::weak_ptr<std::string[]> CArcPCIe::getDeviceStringList( void ) noexcept
		{
			if ( !m_pvDevList->empty() )
			{
				m_psDevList.reset( new std::string[ m_pvDevList->size() ] );

				for ( decltype( m_pvDevList->size() ) i = 0; i < m_pvDevList->size(); i++ )
				{
					std::ostringstream oss;

					oss << "PCIe Device " << i
					#ifdef _WINDOWS
						<< std::ends;
					#else
						<< m_pvDevList->at( i ).sName << std::ends;
					#endif

					( m_psDevList.get() )[ i ] = std::string( oss.str() );
				}
			}

			else
			{
				m_psDevList.reset( new std::string[ 1 ] );

				( m_psDevList.get() )[ 0 ] = std::string( "No Devices Found!"s );
		   }

			return m_psDevList;
		}


		// +----------------------------------------------------------------------------
		// |  isOpen
		// +----------------------------------------------------------------------------
		// |  Returns 'true' if connected to PCIe gen3; 'false' otherwise.
		// |
		// |  Throws NOTHING on error. No error handling.
		// +----------------------------------------------------------------------------
		bool CArcPCIe::isOpen( void ) noexcept
		{
			return CArcDevice::isOpen();
		}


		// +----------------------------------------------------------------------------
		// |  open
		// +----------------------------------------------------------------------------
		// |  Opens a connection to the gen3 driver
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> uiDeviceNumber - Device number
		// +----------------------------------------------------------------------------
		void CArcPCIe::open( const std::uint32_t uiDeviceNumber )
		{
			if ( uiDeviceNumber > static_cast<std::uint32_t>( m_pvDevList->size() ) )
			{
				throwArcGen3Error( "Invalid device number: %u",	uiDeviceNumber );
			}

			if ( isOpen() )
			{
				throwArcGen3Error( "Device already open, call close() first!"s );
			}

			auto bOpen = arc::gen3::CArcSystem::open( m_hDevice, m_pvDevList->at( uiDeviceNumber ) );

			if ( !bOpen || m_hDevice == INVALID_HANDLE_VALUE )
			{
				throwArcGen3Error( "Failed to open device ( %s ) : %s", m_pvDevList->at( uiDeviceNumber ).sName.c_str(), CArcBase::getSystemMessage( CArcBase::getSystemError() ).c_str() );
			}

			// EXTREMELY IMPORTANT
			//
			// Prevent a forking problem. Forking a new process results in the
			// gen3 file descriptor being copied to the child process, which
			// results in problems when later trying to close the gen3 from
			// the parent process.
			//
			// Example: The CArcDisplay::Launch() method will fork a new DS9.
			// The parent and child processes now hold copies of the gen3 file
			// descriptor. The OS marks the gen3 usage count as being two;
			// instead of one. When the parent then closes and tries to re-open
			// the same gen3 it will fail, because the usage count prevents
			// the gen3 from closing ( since it's greater than zero ). In fact,
			// calling close( hdev ) will not result in the driver close/release
			// function being called because the usage count is not zero.  The
			// fcntl() function sets the gen3 file descriptor to close on exec(),
			// which is used in conjunction with the fork() call, and causes the
			// child's copies of the gen3 descriptor to be closed before exec()
			// is called.
		#if defined( linux ) || defined( __linux )

			fcntl( m_hDevice, F_SETFD, FD_CLOEXEC );

		#endif

			//
			//  Clear the status register
			// +-------------------------------------------------------+
			clearStatus();
		}


		// +----------------------------------------------------------------------------
		// |  open
		// +----------------------------------------------------------------------------
		// |  This version first calls open, then mapCommonBuffer if open
		// |  succeeded. Basically, this function just combines the other two.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> uiDeviceNumber - PCI gen3 number
		// |  <IN>  -> uiBytes - The size of the kernel image buffer in bytes
		// +----------------------------------------------------------------------------
		void CArcPCIe::open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiBytes )
		{
			open( uiDeviceNumber );

			mapCommonBuffer( uiBytes );
		}


		// +----------------------------------------------------------------------------
		// |  open
		// +----------------------------------------------------------------------------
		// |  Convenience method. This version first calls open, then mapCommonBuffer
		// |  if open succeeded. Basically, this function just combines the other two.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> uiDeviceNumber - PCI gen3 number
		// |  <IN>  -> uiRows         - The image buffer row size ( in pixels )
		// |  <IN>  -> uiCols         - The image buffer column size ( in pixels )
		// +----------------------------------------------------------------------------
		void CArcPCIe::open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiRows, const std::uint32_t uiCols )
		{
			open( uiDeviceNumber );

			mapCommonBuffer( static_cast< std::size_t >( uiRows ) * static_cast< std::size_t >( uiCols ) * sizeof( std::uint16_t ) );
		}


		// +----------------------------------------------------------------------------
		// |  close
		// +----------------------------------------------------------------------------
		// |  Closes the currently open driver that was opened with a call to
		// |  open.
		// |
		// |  Throws NOTHING on error. No error handling.
		// +----------------------------------------------------------------------------
		void CArcPCIe::close( void )
		{
			//
			// Prevents access violation from code that follows
			//
			bool bOldStoreCmds = m_bStoreCmds;
			m_bStoreCmds       = false;

			if ( isOpen() )
			{
				unMapCommonBuffer();

				arc::gen3::CArcSystem::close( m_hDevice );
			}

			m_uiCCParam   = 0;
			m_hDevice    = INVALID_HANDLE_VALUE;
			m_bStoreCmds = bOldStoreCmds;
		}


		// +----------------------------------------------------------------------------
		// |  reset
		// +----------------------------------------------------------------------------
		// |  Resets the PCIe board.
		// |
		// |  Throws NOTHING on error. No error handling.
		// +----------------------------------------------------------------------------
		void CArcPCIe::reset( void )
		{
			writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, static_cast<std::uint32_t>( arc::gen3::device::ePCIeRegOffsets::REG_RESET ), 1U );

			//
			// Check that the status is now idle
			//
			std::uint32_t uiStatus = readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, static_cast<std::uint32_t>( arc::gen3::device::ePCIeRegOffsets::REG_STATUS ) );

			if ( !fnPCIeStatusIdle( uiStatus ) )
			{
				throwArcGen3Error( "Reset failed! Device status not idle: 0x%X", uiStatus );
			}
		}


		// +--------------------------------------------------------------------------------------------+
		// |  mapCommonBuffer                                                                           |
		// +--------------------------------------------------------------------------------------------+
		// |  Map the gen3 driver image buffer.                                                         |
		// |                                                                                            |
		// |  Throws std::runtime_error on error                                                        |
		// |                                                                                            |
		// |  <IN>  -> uiBytes - The number of bytes to map as an image buffer. Not used by PCIe.       |
		// +--------------------------------------------------------------------------------------------+
		void CArcPCIe::mapCommonBuffer( const std::size_t uiBytes )
		{
			if ( uiBytes == 0U )
			{
				throwArcGen3Error( "Invalid buffer size: %u. Must be greater than zero!", uiBytes );		
			}

			//  Map the kernel buffer
			// +----------------------------------------------------+
			m_tImgBuffer.pUserAddr = reinterpret_cast<std::uint16_t*>( CArcSystem::mmap( m_hDevice, ARC_MEM_MAP, static_cast<std::uint32_t>( size_t( uiBytes ) ) ) );

			if ( reinterpret_cast<std::uint8_t*>( m_tImgBuffer.pUserAddr ) == MAP_FAILED )
			{
				auto iErrorCode = arc::gen3::CArcBase::getSystemError();

				if ( iErrorCode != 0 )
				{
					arc::gen3::CArcBase::zeroMemory( &m_tImgBuffer, sizeof( arc::gen3::device::ImgBuf_t ) );

					throwArcGen3Error( "Failed to map image buffer : %e", arc::gen3::CArcBase::getSystemError() );
				}
			}

			auto bSuccess = getCommonBufferProperties();

			if ( !bSuccess )
			{
				throwArcGen3Error( "Failed to read image buffer size : %e", arc::gen3::CArcBase::getSystemError() );		
			}

			if ( m_tImgBuffer.ulSize < size_t( uiBytes ) )
			{
				std::ostringstream oss;

				oss << "Failed to allocate buffer of the correct size.\nWanted: "
					<< uiBytes << " bytes [ " << ( uiBytes / 1E6 ) << "MB ] - Received: "
					<< m_tImgBuffer.ulSize << " bytes [ " << ( m_tImgBuffer.ulSize / 1E6 )
					<< "MB ]";

				throwArcGen3Error( oss.str() );		
			}

			// IMPORTANT NOTE: This hack is required for the fill buffer to function properly since the 
			//                 mapped size must match the value expected by Owl.
			m_tImgBuffer.ulSize = uiBytes;
		}


		// +----------------------------------------------------------------------------
		// |  unMapCommonBuffer
		// +----------------------------------------------------------------------------
		// |  Un-Maps the gen3 driver image buffer.
		// |
		// |  Throws NOTHING
		// +----------------------------------------------------------------------------
		void CArcPCIe::unMapCommonBuffer( void )
		{
			if ( m_tImgBuffer.pUserAddr != ( void * )nullptr )
			{
				arc::gen3::CArcSystem::munmap( m_hDevice, ARC_MEM_UNMAP, m_tImgBuffer.pUserAddr, m_tImgBuffer.ulSize );
			}

			CArcBase::zeroMemory( &m_tImgBuffer, sizeof( arc::gen3::device::ImgBuf_t ) );
		}


		// +----------------------------------------------------------------------------
		// |  getId
		// +----------------------------------------------------------------------------
		// |  Returns the PCIe board id, which should be 'ARC6'
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getId( void )
		{
			return readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_ID_HI ) );
		}


		// +----------------------------------------------------------------------------
		// |  getStatus
		// +----------------------------------------------------------------------------
		// |  Returns the PCIe status register value.
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getStatus( void )
		{
			return readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_STATUS ) );
		}


		// +----------------------------------------------------------------------------
		// |  clearStatus
		// +----------------------------------------------------------------------------
		// |  Clears the PCIe status register.
		// +----------------------------------------------------------------------------
		void CArcPCIe::clearStatus( void )
		{
			writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
					  static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_STATUS ),
					  PCIe_STATUS_CLEAR_ALL );
		}


		// +----------------------------------------------------------------------------
		// |  set2xFOTransmitter
		// +----------------------------------------------------------------------------
		// |  Sets the controller to use two fiber optic transmitters.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN> -> bOnOff - True to enable dual transmitters; false otherwise.
		// +----------------------------------------------------------------------------
		void CArcPCIe::set2xFOTransmitter( bool bOnOff )
		{
			if ( bOnOff )
			{
				if ( std::uint32_t uiReply = 0; ( uiReply = command( { TIM_ID, XMT, 1 }  ) ) != DON )
				{
					throwArcGen3Error( "Failed to SET use of 2x fiber optic transmitters on controller, reply: 0x%X", uiReply );
				}

				writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
						  static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_FIBER_2X_CTRL ),
						  static_cast< std::uint32_t >( arc::gen3::device::eFiber2x::FIBER_2X_ENABLE ) );
			}

			else
			{
				if ( std::uint32_t uiReply = 0; ( uiReply = command( { TIM_ID, XMT, 0 } ) ) != DON )
				{
					throwArcGen3Error( "Failed to CLEAR use of 2x fiber optic transmitters on controller, reply: 0x%X", uiReply );
				}

				writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
						  static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_FIBER_2X_CTRL ),
						  static_cast<std::uint32_t>( arc::gen3::device::eFiber2x::FIBER_2X_DISABLE ) );
			}
		}


		// +----------------------------------------------------------------------------
		// |  loadDeviceFile
		// +----------------------------------------------------------------------------
		// |  Not used by PCIe.
		// +----------------------------------------------------------------------------
		void CArcPCIe::loadDeviceFile( [[maybe_unused]] const std::filesystem::path& tFile )
		{
			throwArcGen3Error( "Method not available for PCIe!"s );
		}


		// +----------------------------------------------------------------------------
		// |  command
		// +----------------------------------------------------------------------------
		// |  Send a command to the controller timing or utility board. Returns the
		// |  controller reply, typically DON.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> tCmdList - Controller command <board id> <cmd> <arg0> ... <argN>
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::command( const std::initializer_list<const std::uint32_t>& tCmdList )
		{
			std::uint32_t uiHeader = 0;
			std::uint32_t uiReply  = 0;

			//
			//  Report error if gen3 reports readout in progress
			// +------------------------------------------------------+
			auto uiValue = readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_STATUS ) );

			if ( fnPCIeStatusReadout( uiValue ) )
			{
				throwArcGen3Error( "Device reports readout in progress! Status: 0x%X",
									readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
									static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_STATUS ) ) );
			}

			//
			//  Clear Status Register
			// +-------------------------------------------------+
			clearStatus();

			try
			{
				std::uint32_t uiOffset = static_cast<std::uint32_t>( arc::gen3::device::ePCIeRegOffsets::REG_CMD_HEADER );

				for ( auto it = tCmdList.begin(); it != tCmdList.end(); it++ )
				{
					if ( it == tCmdList.begin() )
					{
						uiHeader = static_cast<std::uint32_t>( ( *it << 8 ) | static_cast< std::uint32_t >( tCmdList.size() ) );

						m_fnVerify24Bits( uiHeader );

						writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, uiOffset, ( 0xAC000000 | uiHeader ) );
					}

					else
					{
						m_fnVerify24Bits( *it );

						writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, uiOffset, ( 0xAC000000 | *it ) );
					}

					uiOffset += 4;
				}
			}
			catch ( ... )
			{
				if ( m_bStoreCmds )
				{
					m_pCLog->put( CArcBase::iterToString( tCmdList.begin(), tCmdList.end() ).c_str() );
				}

				throw;
			}

			//
			//  Return the reply
			// +-------------------------------------------------+
			try
			{
				uiReply = readReply();
			}
			catch ( const std::exception& e )
			{
				if ( m_bStoreCmds )
				{
					m_pCLog->put( CArcBase::iterToString( tCmdList.begin(), tCmdList.end() ).c_str() );
				}

				std::ostringstream oss;

				oss << e.what()
					<< "\nException Details: 0x"
					<< std::hex << uiHeader << std::dec << " "
					<< CArcBase::iterToString( ( tCmdList.begin() + 1 ), tCmdList.end() )
					<< std::dec << '\n';

				throwArcGen3Error( oss.str() );
			}

			//
			// Set the debug message queue.
			//
			if ( m_bStoreCmds )
			{
				m_pCLog->put( ( CArcBase::iterToString( tCmdList.begin(), tCmdList.end() ) + CArcBase::formatString( " -> 0x%X", uiReply ) ).c_str() );
			}

			if ( uiReply == CNR )
			{
				throwArcGen3Error( "Controller not ready! Verify controller has been setup! Reply: 0x%X", uiReply );
			}

			return uiReply;
		}


		// +----------------------------------------------------------------------------
		// |  getControllerId
		// +----------------------------------------------------------------------------
		// |  Returns the controller ID or 'ERR' if none.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getControllerId( void )
		{
			std::uint32_t uiReply = 0;

			//
			//  Clear status register
			// +-------------------------------------------------+
			clearStatus();

			//
			//  Get the controller ID
			// +-------------------------------------------------+
			writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
					  static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_CTLR_SPECIAL_CMD ),
					  static_cast< std::uint32_t >( arc::gen3::device::eRegCmds::CONTROLLER_GET_ID ) );

			//
			//  Read the reply
			//
			//  NOTE: Ignore any error, as Gen III will return
			//  a timeout, which will indicate a Gen III system.
			// +-------------------------------------------------+
			try
			{
				uiReply = readReply( 0.5 );
			}
			catch ( ... )
			{
				uiReply = 0;
			}

			return uiReply;
		}


		// +----------------------------------------------------------------------------
		// |  resetController
		// +----------------------------------------------------------------------------
		// |  Resets the controller.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCIe::resetController( void )
		{
			//
			//  Clear status register
			// +-------------------------------------------------+
			clearStatus();

			//
			//  reset the controller or DSP
			// +-------------------------------------------------+
			writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
					  static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_CTLR_SPECIAL_CMD ),
					  static_cast< std::uint32_t >( arc::gen3::device::eRegCmds::CONTROLLER_RESET ) );

			//
			//  Read the reply
			// +-------------------------------------------------+
			auto uiReply = readReply();

			if ( uiReply != SYR )
			{
				throwArcGen3Error( "Failed to reset controller, reply: 0x%X", uiReply );
			}
		}


		// +----------------------------------------------------------------------------
		// | isControllerConnected
		// +----------------------------------------------------------------------------
		// |  Returns 'true' if a controller is connected to the PCIe board.  This is
		// |  for fiber optic A only.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		bool CArcPCIe::isControllerConnected( void )
		{
			return isFiberConnected();
		}


		// +----------------------------------------------------------------------------
		// | isFiberConnected
		// +----------------------------------------------------------------------------
		// |  Returns 'true' if the specified  PCIe fiber optic is connected to a
		// |  powered-on controller.
		// |
		// |  <IN> -> eFiberId - The ID of the fiber to check: CArcPCIe::FIBER_A or CArcPCIe::FIBER_B. Default: FIBER_A
		// |
		// |  Throws std::runtime_error on invalid parameter
		// +----------------------------------------------------------------------------
		bool CArcPCIe::isFiberConnected( arc::gen3::device::eFiber eFiberId )
		{
			bool bConnected = false;

			if ( eFiberId == arc::gen3::device::eFiber::FIBER_A )
			{
				bConnected = fnPCIeStatusFiberAConnected( getStatus() );
			}

			else if ( eFiberId == arc::gen3::device::eFiber::FIBER_B )
			{
				bConnected = fnPCIeStatusFiberBConnected( getStatus() );
			}

			else
			{
				throwArcGen3Error( "Invalid fiber id: %u", static_cast<std::uint32_t>( eFiberId ) );
			}

			return bConnected;
		}


		// +----------------------------------------------------------------------------
		// |  stopExposure
		// +----------------------------------------------------------------------------
		// |  Stops the current exposure.
		// |
		// |  NOTE: The command is sent manually and NOT using the command() method
		// |        because the command() method checks for readout and fails.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCIe::stopExposure( void )
		{
			//
			//  Send Header
			// +-------------------------------------------------+
			std::uint32_t uiHeader = ( ( TIM_ID << 8 ) | 2 );

			writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
					  static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_CMD_HEADER ),
					  ( 0xAC000000 | uiHeader ) );

			//
			//  Send command
			// +-------------------------------------------------+
			writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
					  static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_CMD_COMMAND ),
					  ( 0xAC000000 | ABR ) );

			//
			//  Read the reply
			// +-------------------------------------------------+
			std::uint32_t uiReply = readReply();

			if ( uiReply != DON )
			{
				throwArcGen3Error( "Failed to stop exposure/readout, reply: 0x%X", uiReply );
			}
		}


		// +----------------------------------------------------------------------------
		// |  isReadout
		// +----------------------------------------------------------------------------
		// |  Returns 'true' if the controller is in readout.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		bool CArcPCIe::isReadout( void )
		{
			return ( ( ( getStatus() & 0x4 ) > 0 ) ? true : false );
		}


		// +----------------------------------------------------------------------------
		// |  getPixelCount
		// +----------------------------------------------------------------------------
		// |  Returns the current pixel count.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getPixelCount( void )
		{
			std::uint32_t uiPixCnt = readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
											  static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_PIXEL_COUNT ) );

			if ( m_bStoreCmds )
			{
				m_pCLog->put( CArcBase::formatString( "[ PIXEL COUNT REG: 0x%X -> %u ]",
							  static_cast<std::uint32_t>( arc::gen3::device::ePCIeRegOffsets::REG_PIXEL_COUNT ),
							  uiPixCnt ).c_str() );
			}

			return uiPixCnt;
		}


		// +----------------------------------------------------------------------------
		// |  getCRPixelCount
		// +----------------------------------------------------------------------------
		// |  Returns the cumulative pixel count as when doing continuous readout.
		// |  This method is used by user applications when reading super dArge images
		// | ( greater than 4K x 4K ). This value increases across all frames being
		// |  readout. This code needs to execute fast, so not pre-checking, such as 
		// |  isControllerConnected() is done.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getCRPixelCount( void )
		{
			throwArcGen3Error( "Method not supported by PCIe!"s );

			return 0;
		}


		// +----------------------------------------------------------------------------
		// |  getFrameCount
		// +----------------------------------------------------------------------------
		// |  Returns the current frame count. The camera MUST be set for continuous
		// |  readout for this to work. This code needs to execute fast, so no 
		// |  pre-checking, such as isControllerConnected() is done.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getFrameCount( void )
		{
			std::uint32_t uiFrameCnt = readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
											    static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_FRAME_COUNT ) );

			if ( m_bStoreCmds )
			{
				m_pCLog->put( CArcBase::formatString( "[ FRAME COUNT REG: 0x%X -> %u ]",
							  static_cast<std::uint32_t>( arc::gen3::device::ePCIeRegOffsets::REG_FRAME_COUNT ),
							  uiFrameCnt ).c_str() );
			}

			return uiFrameCnt;
		}


		// +----------------------------------------------------------------------------
		// |  writeBar
		// +----------------------------------------------------------------------------
		// |  Writes a value to the specified PCI/e BAR offset using mapped registers.
		// |  Automatically calls WriteBarDirect() if no mappings exist.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN> -> eBar    - The PCI BAR number ( 0 - 5 ).
		// |  <IN> -> bOffset - The offset into the specified BAR.
		// |  <IN> -> uiValue  - 32-bit value to write.
		// +----------------------------------------------------------------------------
		void CArcPCIe::writeBar( const arc::gen3::device::ePCIeRegs eBar, const std::uint32_t uiOffset, const std::uint32_t uiValue )
		{
			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			std::array tArgs = { static_cast<std::uint32_t>( eBar ), static_cast<std::uint32_t>( uiOffset ), uiValue };

			if ( static_cast< std::uint32_t >( eBar ) < ARC_MIN_BAR || static_cast< std::uint32_t >( eBar ) > ARC_MAX_BAR )
			{
				throwArcGen3Error( "Invalid BAR number: 0x%X", eBar );
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ARC_WRITE_BAR, tArgs.data(), static_cast<std::uint32_t>( tArgs.size() * sizeof( std::uint32_t ) ) );

			if ( !bSuccess )
			{
				throwArcGen3Error( "Writing 0x%X to 0x%X : 0x%X failed! %e", uiValue, eBar, uiOffset, arc::gen3::CArcBase::getSystemError() );
			}
		}


		// +----------------------------------------------------------------------------
		// |  readBar
		// +----------------------------------------------------------------------------
		// |  Read a value from the specified PCI/e BAR offset using mapped registers.
		// |  Automatically calls ReadBarDirect() if no mappings exist.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN> -> eBar    - The PCI BAR number ( 0 - 5 ).
		// |  <IN> -> bOffset - The offset into the specified BAR.
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::readBar( const arc::gen3::device::ePCIeRegs eBar, const std::uint32_t uiOffset )
		{
			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			std::array tIn = { static_cast< std::uint32_t >( eBar ), static_cast< std::uint32_t >( uiOffset ) };

			if ( static_cast< std::uint32_t >( eBar ) < ARC_MIN_BAR || static_cast< std::uint32_t >( eBar ) > ARC_MAX_BAR )
			{
				throwArcGen3Error( "Invalid BAR number: 0x%X", eBar );
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice,
														  ARC_READ_BAR,
														  tIn.data(),
														  static_cast<std::uint32_t>( tIn.size() * sizeof( std::uint32_t ) ),
														  tIn.data(),
														  static_cast< std::uint32_t >( tIn.size() * sizeof( std::uint32_t ) ) );

			if ( !bSuccess )
			{
				throwArcGen3Error( "Reading 0x%X : 0x%X failed! %e", eBar, uiOffset, arc::gen3::CArcBase::getSystemError() );
			}

			return tIn[ 0 ];
		}


		// +----------------------------------------------------------------------------
		// |  getCommonBufferProperties
		// +----------------------------------------------------------------------------
		// |  Fills in the image buffer structure with its properties, such as
		// |  physical address and size.
		// |
		// |  Throws std::runtime_error on error. No error handling.
		// +----------------------------------------------------------------------------
		bool CArcPCIe::getCommonBufferProperties( void )
		{
			std::uint64_t uiProps[] = { 0, 0 };

			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice,
														  ARC_BUFFER_PROP,
														  uiProps,
														  ( sizeof( std::uint64_t ) * ( sizeof( uiProps ) / sizeof( std::uint64_t ) ) ),
														  uiProps,
														  ( sizeof( std::uint64_t ) * ( sizeof( uiProps ) / sizeof( std::uint64_t ) ) ) );

			if ( !bSuccess )
			{
				return false;
			}

			m_tImgBuffer.ulPhysicalAddr = uiProps[ 0 ];
			m_tImgBuffer.ulSize         = static_cast<size_t>( uiProps[ 1 ] );

			return true;
		}


		// +----------------------------------------------------------------------------
		// |  loadGen23ControllerFile
		// +----------------------------------------------------------------------------
		// |  Loads a timing or utility file (.lod) into a GenII or GenIII controller.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN> -> tFilename   - The TIM or UTIL lod file to load.
		// |  <IN> -> bValidate   - Set to 1 if the download should be read back and
		// |                        checked after every write.
		// |  <IN> -> bAbort      - 'true' to stop; 'false' otherwise. Default: false
		// +----------------------------------------------------------------------------
		void CArcPCIe::loadGen23ControllerFile( const std::filesystem::path& tFilename, bool bValidate, bool* pAbort )
		{
			std::uint32_t	uiBoardId	= 0;
			std::uint32_t	uiType		= 0;
			std::uint32_t	uiAddr		= 0;
			std::uint32_t	uiData		= 0;
			std::uint32_t	uiReply		= 0;
			char			typeChar	= ' ';
			bool			bIsCLodFile	= false;

			std::string sLine;

			if ( pAbort != nullptr && *pAbort ) { return; }

			//
			// Verify gen3 connection
			// -------------------------------------------------------------------
			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			if ( pAbort != nullptr && *pAbort ) { return; }
			//
			// open the file for reading.
			// -------------------------------------------------------------------
			std::ifstream inFile( tFilename );

			if ( !inFile.is_open() )
			{
				throwArcGen3Error( "Cannot open file: %s", tFilename.c_str() );
			}

			if ( pAbort != nullptr && *pAbort ) { inFile.close(); return; }

			//
			// Check for valid TIM or UTIL file
			// -------------------------------------------------------------------
			getline( inFile, sLine );

			if ( sLine.find( "TIM"s ) != std::string::npos )
			{
				uiBoardId = TIM_ID;
			}
			else if ( sLine.find( "CRT"s ) != std::string::npos )
			{
				uiBoardId = TIM_ID;
				bIsCLodFile = true;
			}
			else if ( sLine.find( "UTIL"s ) != std::string::npos )
			{
				uiBoardId = UTIL_ID;
			}
			else
			{
				throwArcGen3Error( "Invalid file. Missing 'TIMBOOT/CRT' or 'UTILBOOT' std::string."s );
			}

			if ( pAbort != nullptr && *pAbort ) { inFile.close(); return; }

			//
			// First, send the stop command. Otherwise, the controller crashes
			// because it is downloading and executing code while you try to
			// overwrite it.
			// -----------------------------------------------------------------
			uiReply = command( { TIM_ID, STP } );

			if ( uiReply != DON )
			{
				throwArcGen3Error( "Stop ('STP') controller failed. Reply: 0x%X", uiReply );
			}

			if ( pAbort != nullptr && *pAbort ) { inFile.close(); return; }

			//
			// Set the PCI status bit #1 (X:0 bit 1 = 1).
			// -------------------------------------------
			// Not Used by PCIe

			//
			// Read in the file one line at a time
			// --------------------------------------
			while ( !inFile.eof() )
			{
				if ( pAbort != nullptr && *pAbort ) { inFile.close(); return; }

				getline( inFile, sLine );

				//
				// Only "_DATA" blocks are valid for download
				// ---------------------------------------------
				if ( sLine.find( '_' ) == 0 && sLine.find( "_DATA "s ) != std::string::npos )
				{
					auto pTokens = CArcBase::splitString( sLine );

					// Dump _DATA std::string

					//
					// Get the memory type and start address
					// ---------------------------------------------
					typeChar = pTokens->at( 1 ).at( 0 );

					auto tFromCharsResult = std::from_chars( pTokens->at( 2 ).data(), ( pTokens->at( 2 ).data() + pTokens->at( 2 ).size() ), uiAddr, 16 );

					if ( tFromCharsResult.ec == std::errc::invalid_argument )
					{
						throwArcGen3InvalidArgument( "Failed to convert memory address"s );
					}

					if ( tFromCharsResult.ec == std::errc::result_out_of_range )
					{
						throwArcGen3LengthError( "Failed to convert memory address"s );
					}

					//
					// The start address must be less than MAX_DSP_START_LOAD_ADDR
					// -------------------------------------------------------------
					if ( uiAddr < MAX_DSP_START_LOAD_ADDR )
					{
						//
						// Set the DSP memory type
						// ----------------------------------
						if      ( typeChar == 'X' ) uiType = X_MEM;
						else if ( typeChar == 'Y' ) uiType = Y_MEM;
						else if ( typeChar == 'P' ) uiType = P_MEM;
						else if ( typeChar == 'R' ) uiType = R_MEM;

						//
						// Read the data block
						// ----------------------------------
						while ( !inFile.eof() && inFile.peek() != '_' )
						{
							if ( pAbort != nullptr && *pAbort ) { inFile.close(); return; }

							getline( inFile, sLine );

							auto pDataTokens = CArcBase::splitString( sLine );

							for (auto it = pDataTokens->begin(); it != pDataTokens->end(); it++)
							{
								if (pAbort != nullptr && *pAbort) { inFile.close(); return; }

								tFromCharsResult = std::from_chars( it->data(), ( it->data() + it->size() ), uiData, 16 );

								if ( tFromCharsResult.ec == std::errc::invalid_argument )
								{
									throwArcGen3InvalidArgument( "Failed to convert memory address"s );
								}

								if ( tFromCharsResult.ec == std::errc::result_out_of_range )
								{
									throwArcGen3LengthError( "Failed to convert memory address"s );
								}

								//
								// Write the data to the controller.
								// --------------------------------------------------------------
								uiReply = command( { uiBoardId, WRM, ( uiType | uiAddr ), uiData } );

								if ( uiReply != DON )
								{
									throwArcGen3Error( "Write ('WRM') to controller %s board failed. WRM 0x%X 0x%X -> 0x%X",
														( uiBoardId == TIM_ID ? "TIMING" : "UTILITY" ),
														( uiType | uiAddr ),
														 uiData,
														 uiReply );
								}

								if ( pAbort != nullptr && *pAbort ) { inFile.close(); return; }

								//
								// Validate the data if required.
								// --------------------------------------------------------------
								if ( bValidate )
								{
									uiReply = command( { uiBoardId, RDM, ( uiType | uiAddr ) } );

									if ( uiReply != uiData )
									{
										throwArcGen3Error( "Write ('WRM') to controller %s board failed. RDM 0x%X -> 0x%X [ Expected: 0x%X ]",
															( uiBoardId == TIM_ID ? "TIMING" : "UTILITY" ),
															( uiType | uiAddr ),
															 uiReply,
															 uiData );
									}
								}	// End if 'validate'

								uiAddr++;

							} // while tokenizer next
						} // if not EOF or '_'
					}	// if address < 0x4000
				}	// if '_DATA'
			}	// if not EOF

			inFile.close();

			if ( pAbort != nullptr && *pAbort ) { inFile.close(); return; }

			//
			// Clear the PCI status bit #1 (X:0 bit 1 = 0)
			// --------------------------------------------------------------
			// Not Used with PCIe


			if ( pAbort != nullptr && *pAbort ) { inFile.close(); return; }

			//
			//  Tell the TIMING board to jump from boot code to
			//  the uploaded application. Don't check the reply,
			//  since it doesn't return one.
			// +------------------------------------------------+
			if ( bIsCLodFile )
			{
				uiReply = command( { TIM_ID, JDL } );

				if ( uiReply != DON )
				{
					throwArcGen3Error( "Jump from boot code failed. Reply: 0x%X", uiReply );
				}
			}
		}


		// +----------------------------------------------------------------------------
		// |  getContinuousImageSize
		// +----------------------------------------------------------------------------
		// |  Returns the boundary adjusted image size for continuous readout.  The PCIe
		// |  card ( ARC-66/67 ) requires no boundary adjustments and writes data
		// |  continuously.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> uiImageSize - The boundary adjusted image size ( in bytes ).
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getContinuousImageSize( const std::uint32_t uiImageSize )
		{
			return uiImageSize;
		}


		// +----------------------------------------------------------------------------
		// |  smallCamDLoad
		// +----------------------------------------------------------------------------
		// |  Sends a .lod download file data stream of up to 6 values to the SmallCam
		// |  controller.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> uiBoardId  - Must be SMALLCAM_DLOAD_ID
		// |  <IN>  -> pvData     - Data vector
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::smallCamDLoad( const std::uint32_t uiBoardId, const std::vector<std::uint32_t>* pvData )
		{
			std::uint32_t uiHeader = 0;
			std::uint32_t uiReply  = 0;

			//
			//  Report error if gen3 reports readout in progress
			// +------------------------------------------------------+
			auto uiValue = readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, static_cast<std::uint32_t>( arc::gen3::device::ePCIeRegOffsets::REG_STATUS ) );

			if ( fnPCIeStatusReadout( uiValue ) )
			{
				throwArcGen3Error( "Device reports readout in progress! Status: 0x%X",
						readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
								 static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_STATUS ) ) );
			}

			//
			//  Verify the size of the data, cannot be greater than 6
			// +------------------------------------------------------+
			if ( pvData->size() > 6U )
			{
				throwArcGen3Error( "Data vector too large: 0x%X! Must be less than 6!", pvData->size() );
			}

			//
			//  Verify the board id equals smallcam download id
			// +------------------------------------------------------+
			if ( uiBoardId != SMALLCAM_DLOAD_ID )
			{
				throwArcGen3Error( "Invalid board id: %u! Must be: %u",	uiBoardId, SMALLCAM_DLOAD_ID );
			}

			//
			//  Clear Status Register
			// +-------------------------------------------------+
			clearStatus();

			try
			{
				//
				//  Send Header
				// +-------------------------------------------------+
				uiHeader = ( ( uiBoardId << 8 ) | static_cast<std::uint32_t>( pvData->size() + 1U ) );

				m_fnVerify24Bits( uiHeader );

				writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR,
						  static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_CMD_HEADER ),
						  ( 0xAC000000U | uiHeader ) );

				//
				//  Send the data
				// +-------------------------------------------------+
				for ( std::vector<std::uint32_t>::size_type i = 0; i < pvData->size(); i++ )
				{
					m_fnVerify24Bits( pvData->at( i ) );

					auto uiOffset = static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_CMD_COMMAND ) + ( static_cast< std::uint32_t >( i ) * 4U );

					writeBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, uiOffset, ( 0xAC000000U | pvData->at( i ) ) );
				}
			}
			catch ( ... )
			{
				if ( m_bStoreCmds )
				{
					m_pCLog->put( formatDLoadString( uiReply, uiBoardId, pvData ).c_str() );
				}

				throw;
			}

			//
			//  Return the reply
			// +-------------------------------------------------+
			try
			{
				uiReply = readReply();
			}
			catch ( const std::exception& e )
			{
				if ( m_bStoreCmds )
				{
					m_pCLog->put( formatDLoadString( uiReply, uiBoardId, pvData ).c_str() );
				}

				std::ostringstream oss;

				oss << e.what() << "\nException Details: 0x"sv << std::hex << uiHeader << std::dec;

				for ( std::vector<std::uint32_t>::size_type i = 0; i < pvData->size(); i++ )
				{
					oss << " 0x" << std::hex << pvData->at( i ) << std::dec;
				}

				oss	<< std::ends;

				throwArcGen3Error( oss.str() );
			}

			//
			// Set the debug message queue.
			//
			if ( m_bStoreCmds )
			{
				m_pCLog->put( formatDLoadString( uiReply, uiBoardId, pvData ).c_str() );
			}

			return uiReply;
		}


		// +----------------------------------------------------------------------------
		// |  setByteSwapping
		// +----------------------------------------------------------------------------
		// |  Sets the hardware byte-swapping if system architecture is solaris.
		// |  Otherwise, does nothing; compiles to empty function.
		// +----------------------------------------------------------------------------
		void CArcPCIe::setByteSwapping( void )
		{
			// Not Used !!!!
		}

		// +----------------------------------------------------------------------------
		// |  readReply
		// +----------------------------------------------------------------------------
		// |  Reads the reply register value. This method will time-out if the
		// |  specified number of seconds passes before the reply received register
		// |  bit or an error bit ( PCIe time-out, header error, controller reset ) is
		// |  set.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN> -> gTimeOutSecs - Seconds to wait before time-out occurs.
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::readReply( const double gTimeOutSecs )
		{
			std::uint32_t   uiStatus  = 0;
			std::uint32_t   uiReply   = 0;
			double			gDiffTime = 0.0;
			time_t			tStart    = time( nullptr );

			do
			{
				uiStatus = getStatus();

				if ( fnPCIeStatusHeaderError( uiStatus ) )
				{
					uiReply = HERR;

					break;
				}

				else if( fnPCIeStatusControllerReset( uiStatus ) )
				{
					uiReply = SYR;
	
					break;
				}

				if ( ( gDiffTime = difftime( time( nullptr ), tStart ) ) > gTimeOutSecs )
				{
					throwArcGen3Error( "Time Out [ %f sec ] while waiting for status [ 0x%X ]!", gDiffTime, uiStatus );
				}

			} while ( !fnPCIeStatusReplyRecvd( uiStatus ) );


			if ( uiReply != HERR && uiReply != SYR )
			{
				uiReply = readBar( arc::gen3::device::ePCIeRegs::DEV_REG_BAR, static_cast< std::uint32_t >( arc::gen3::device::ePCIeRegOffsets::REG_CMD_REPLY ) );
			}

			return uiReply;
		}


		// +----------------------------------------------------------------------------
		// |  getCfgSpByte
		// +----------------------------------------------------------------------------
		// |  Returns the specified BYTE from the specified PCI configuration space
		// |  register.
		// |
		// |  <IN> -> uiOffset - The byte offset from the start of PCI config space
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getCfgSpByte( const std::uint32_t uiOffset )
		{
			std::uint32_t uiRegValue = uiOffset;

			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ARC_READ_CFG_8, const_cast<std::uint32_t*>( &uiOffset ), sizeof( uiOffset ), &uiRegValue, sizeof( uiRegValue ) );

			if ( !bSuccess )
			{
				throwArcGen3Error( "Reading configuration BYTE offset 0x%X failed : %e", uiOffset, arc::gen3::CArcBase::getSystemError() );
			}

			return uiRegValue;
		}


		// +----------------------------------------------------------------------------
		// |  getCfgSpWord
		// +----------------------------------------------------------------------------
		// |  Returns the specified WORD from the specified PCI configuration space
		// |  register.
		// |
		// |  <IN> -> uiOffset - The byte offset from the start of PCI config space
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getCfgSpWord( const std::uint32_t uiOffset )
		{
			std::uint32_t uiRegValue = uiOffset;

			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ARC_READ_CFG_16, const_cast<std::uint32_t*>( &uiOffset ), sizeof( uiOffset ), &uiRegValue, sizeof( uiRegValue ) );

			if ( !bSuccess )
			{
				throwArcGen3Error( "Reading configuration WORD offset 0x%X failed : %e", uiOffset, arc::gen3::CArcBase::getSystemError() );
			}

			return uiRegValue;
		}


		// +----------------------------------------------------------------------------
		// |  getCfgSpDWord
		// +----------------------------------------------------------------------------
		// |  Returns the specified DWORD from the specified PCI configuration space
		// |  register.
		// |
		// |  <IN> -> uiOffset - The byte offset from the start of PCI config space
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCIe::getCfgSpDWord( const std::uint32_t uiOffset )
		{
			std::uint32_t uiRegValue = uiOffset;

			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ARC_READ_CFG_32, const_cast<std::uint32_t*>( &uiOffset ), sizeof( uiOffset ), &uiRegValue, sizeof( uiRegValue ) );

			if ( !bSuccess )
			{
				throwArcGen3Error( "Reading configuration DWORD offset 0x%X failed : %e", uiOffset, arc::gen3::CArcBase::getSystemError() );
			}

			return uiRegValue;
		}


		// +----------------------------------------------------------------------------
		// |  setCfgSpByte
		// +----------------------------------------------------------------------------
		// |  Writes the specified BYTE to the specified PCI configuration space
		// |  register.
		// |
		// |  <IN> -> uiOffset - The byte offset from the start of PCI config space
		// |  <IN> -> uiValue  - The BYTE value to write
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCIe::setCfgSpByte( const std::uint32_t uiOffset, const std::uint32_t uiValue )
		{
			std::uint32_t uiArgs[] = { uiOffset, uiValue };

			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ARC_WRITE_CFG_8, uiArgs, sizeof( uiArgs ) );

			if ( !bSuccess )
			{
				throwArcGen3Error( "Writing configuration BYTE 0x%X to offset 0x%X failed : %e", uiValue, uiOffset, arc::gen3::CArcBase::getSystemError() );
			}
		}


		// +----------------------------------------------------------------------------
		// |  setCfgSpWord
		// +----------------------------------------------------------------------------
		// |  Writes the specified WORD to the specified PCI configuration space
		// |  register.
		// |
		// |  <IN> -> uiOffset - The byte offset from the start of PCI config space
		// |  <IN> -> uiValue  - The WORD value to write
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCIe::setCfgSpWord( const std::uint32_t uiOffset, const std::uint32_t uiValue )
		{
			std::uint32_t uiArgs[] = { uiOffset, uiValue };

			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ARC_WRITE_CFG_16, uiArgs, sizeof( uiArgs ) );

			if ( !bSuccess )
			{
				throwArcGen3Error( "Writing configuration WORD 0x%X to offset 0x%X failed : %e", uiValue, uiOffset, arc::gen3::CArcBase::getSystemError() );
			}
		}


		// +----------------------------------------------------------------------------
		// |  setCfgSpDWord
		// +----------------------------------------------------------------------------
		// |  Writes the specified DWORD to the specified PCI configuration space
		// |  register.
		// |
		// |  <IN> -> uiOffset - The byte offset from the start of PCI config space
		// |  <IN> -> uiValue  - The DWORD value to write
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCIe::setCfgSpDWord( const std::uint32_t uiOffset, const std::uint32_t uiValue )
		{
			std::uint32_t uiArgs[] = { uiOffset, uiValue };

			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ARC_WRITE_CFG_32, uiArgs, sizeof( uiArgs ) );

			if ( !bSuccess )
			{
				throwArcGen3Error( "Writing configuration DWORD 0x%X to offset 0x%X failed : %e", uiValue, uiOffset, arc::gen3::CArcBase::getSystemError() );
			}
		}


		// +----------------------------------------------------------------------------
		// |  getCfgSp
		// +----------------------------------------------------------------------------
		// |  Reads and parses the entire PCIe configuration space header into readable
		// |  text and bit definitions that are stored in a member list variable. The
		// |  public methods of this class allow access to this list. This method will
		// |  create the member list if it doesn't already exist and clears it if it
		// |  does.
		// +----------------------------------------------------------------------------
		void CArcPCIe::getCfgSp( void )
		{
			std::uint32_t uiRegValue = 0;
			std::uint32_t uiRegAddr  = 0;

			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			CArcPCIBase::getCfgSp();

			uiRegAddr  = PCI9056_PM_CAP_ID;
			uiRegValue = getCfgSpDWord( uiRegAddr );
			addRegItem( m_pCfgSpList.get(),
						uiRegAddr,
						"Power Management Capability / Next Item Ptr / Capability ID"s,
						uiRegValue );

			uiRegAddr  = PCI9056_PM_CSR;
			uiRegValue = getCfgSpDWord( uiRegAddr );
			addRegItem( m_pCfgSpList.get(),
						uiRegAddr,
						"PM Cap: PM Data / Bridge Ext / PM Control & Status"s,
						uiRegValue );

			uiRegAddr  = PCI9056_HS_CAP_ID;
			uiRegValue = getCfgSpDWord( uiRegAddr );
			addRegItem( m_pCfgSpList.get(),
						uiRegAddr,
						"Hot Swap Capability / Next Item Pointer / Capability ID"s,
						uiRegValue );

			uiRegAddr  = PCI9056_VPD_CAP_ID;
			uiRegValue = getCfgSpDWord( uiRegAddr );
			addRegItem( m_pCfgSpList.get(),
						uiRegAddr,
						"VPD Capability / VPD Address / Next Item Ptr / Capability ID"s,
						uiRegValue );

			uiRegAddr  = PCI9056_VPD_DATA;
			uiRegValue = getCfgSpDWord( uiRegAddr );
			addRegItem( m_pCfgSpList.get(),
						uiRegAddr,
						"VPD Data"s,
						uiRegValue );
		}


		// +----------------------------------------------------------------------------
		// |  getBarSp
		// +----------------------------------------------------------------------------
		// |  Reads and parses the entire PCIe Base Address Registers (BAR) into
		// |  readable text and bit definitions that are stored in a member list
		// |  variable. The public methods of this class allow access to this list.
		// |  This method will create the member list if it doesn't already exist and
		// |  clears it if it does. NOTE: Not all BARS or PCI boards have data.
		// +----------------------------------------------------------------------------
		void CArcPCIe::getBarSp( void )
		{
			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			CArcPCIBase::getBarSp();

			//
			//  Access the register data
			// +-------------------------------------------------------+
			getLocalConfiguration();
		}


		// +----------------------------------------------------------------------------
		// |  getLocalConfiguration
		// +----------------------------------------------------------------------------
		// |  Reads and parses the entire PLX PCIe local registers located within BAR0.
		// +----------------------------------------------------------------------------
		void CArcPCIe::getLocalConfiguration( void )
		{
			auto uiRegValue = 0U;

			if ( m_pBarList == nullptr )
			{
				throwArcGen3Error( "Unable to read PCI/e base address register!"s );
			}

			//
			// Get and Add PLX Local Configuration Registers
			//
			std::unique_ptr<PCIRegList_t> pList( new PCIRegList_t() );

			for ( auto i = 0; i < static_cast<int>( sizeof( LCRMap ) / sizeof( PLXRegItem ) ); i++ )
			{
				uiRegValue = readBar( arc::gen3::device::ePCIeRegs::LCL_CFG_BAR, LCRMap[ i ].uiAddr );

				addRegItem( pList.get(),
							LCRMap[ i ].uiAddr,
							LCRMap[ i ].sText.c_str(),
							uiRegValue );
			}

			addBarItem( LCRMapName, pList.release() );

			//
			// Get and Add PLX Runtime Registers
			//
			pList.reset( new PCIRegList_t() );

			for ( auto i = 0; i < static_cast<int>( sizeof( RTRMap ) / sizeof( PLXRegItem ) ); i++ )
			{
				uiRegValue = readBar( arc::gen3::device::ePCIeRegs::LCL_CFG_BAR, RTRMap[ i ].uiAddr );

				if ( RTRMap[ i ].uiAddr == PCI9056_PERM_VENDOR_ID )
				{
					addRegItem( pList.get(),
								RTRMap[ i ].uiAddr,
								RTRMap[ i ].sText.c_str(),
								uiRegValue,
								getDevVenBitList( uiRegValue ).release() );
				}

				else
				{
					addRegItem( pList.get(),
								RTRMap[ i ].uiAddr,
								RTRMap[ i ].sText.c_str(),
								uiRegValue );
				}
			}

			addBarItem( RTRMapName, pList.release() );

			//
			// Get and Add PLX DMA Registers
			//
			pList.reset( new PCIRegList_t() );

			for ( int i=0; i<int( sizeof( DMAMap ) / sizeof( PLXRegItem ) ); i++ )
			{
				uiRegValue = readBar( arc::gen3::device::ePCIeRegs::LCL_CFG_BAR, DMAMap[ i ].uiAddr );

				addRegItem( pList.get(), DMAMap[ i ].uiAddr, DMAMap[ i ].sText.c_str(), uiRegValue );
			}

			addBarItem( DMAMapName, pList.release() );

			//
			// Get and Add PLX Messaging Queue Registers
			//
			pList.reset( new PCIRegList_t() );

			for ( auto i = 0; i < static_cast<int>( sizeof( MSQMap ) / sizeof( PLXRegItem ) ); i++ )
			{
				uiRegValue = readBar( arc::gen3::device::ePCIeRegs::LCL_CFG_BAR, MSQMap[ i ].uiAddr );

				addRegItem( pList.get(),
							MSQMap[ i ].uiAddr,
							MSQMap[ i ].sText.c_str(),
							uiRegValue );
			}

			addBarItem( MSQMapName, pList.release() );

			return;
		}

	}	// end gen3 namespace 
}	// end arc namespace