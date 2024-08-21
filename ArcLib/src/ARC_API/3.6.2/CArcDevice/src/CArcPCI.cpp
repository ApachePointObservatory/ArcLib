#ifdef _WINDOWS
	#define INITGUID
#endif


#ifdef _WINDOWS
	#include <windows.h>
	#include <setupapi.h>
	#include <devguid.h>
	#include <regstr.h>
	#include <astropciGUID.h>

#elif defined( linux ) || defined( __linux ) || defined( __APPLE__ )
	#include <sys/types.h>
	#include <sys/mman.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <errno.h>
	#include <dirent.h>
	#include <cstring>

#else						// Unix Systems Only
	#include <sys/types.h>
	#include <sys/mman.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <stropts.h>
	#include <errno.h>
	#include <dirent.h>
#endif


#include <charconv>
#include <sstream>
#include <fstream>
#include <cstddef>
#include <array>
#include <cmath>
#include <ios>
#include <version>

#ifdef __cpp_lib_format
	#include <format>
#endif

#include <CArcBase.h>
#include <CArcSystem.h>
#include <CArcDevice.h>
#include <CArcPCI.h>
#include <ArcDefs.h>
#include <PCIRegs.h>

using namespace std::string_literals;
using namespace std::string_view_literals;


namespace arc
{
	namespace gen3
	{

		#if defined( linux ) || defined( __linux )

			constexpr auto DEVICE_DIR			= "/dev/"sv;
			constexpr auto DEVICE_NAME			= "AstroPCI"sv;
			constexpr auto DEVICE_NAME_ALT		= "Arc64PCI"sv;

		#elif defined( __APPLE__ )

			// +----------------------------------------------------------------------------
			// | Define driver names
			// +----------------------------------------------------------------------------
			constexpr auto AstroPCIClassName	= com_arc_driver_Arc64PCI;
			constexpr auto kAstroPCIClassName	= "com_arc_driver_Arc64PCI";

		#endif


		//#include <fstream>
		//std::ofstream dbgStream( "CArcPCI_Debug.txt" );


		// +----------------------------------------------------------------------------
		// |  Initialize Static Class Members
		// +----------------------------------------------------------------------------
		std::unique_ptr<std::vector<arc::gen3::arcDev_t>> arc::gen3::CArcPCI::m_pvDevList( new std::vector<arc::gen3::arcDev_t>() );

		std::shared_ptr<std::string[]> CArcPCI::m_psDevList;


		// +----------------------------------------------------------------------------
		// |  Constructor
		// +----------------------------------------------------------------------------
		// |  See CArcPCI.h for the class definition
		// +----------------------------------------------------------------------------
		CArcPCI::CArcPCI( void )
		{
			m_fnValidDeviceVendorId = []( const std::uint16_t uiDevVenId )
			{
				return ( ( uiDevVenId == 0x9056U || uiDevVenId == 0x10B5U ) ? 1U : 0U );
			};

			m_hDevice = INVALID_HANDLE_VALUE;
		}


		// +----------------------------------------------------------------------------
		// |  Destructor
		// +----------------------------------------------------------------------------
		CArcPCI::~CArcPCI( void )
		{
			close();
		}


		// +----------------------------------------------------------------------------
		// |  toString
		// +----------------------------------------------------------------------------
		// |  Returns a std::string that represents the gen3 controlled by this library.
		// +----------------------------------------------------------------------------
		constexpr std::string_view CArcPCI::toString( void )
		{
			return "PCI [ ARC-63 / 64 ]"sv;
		}


		// +----------------------------------------------------------------------------
		// |  findDevices
		// +----------------------------------------------------------------------------
		// |  Searches for available ARC, Inc PCI devices and stores the list, which
		// |  can be accessed via gen3 number ( 0,1,2... ).
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCI::findDevices( void )
		{
			if ( m_pvDevList == nullptr )
			{
				m_pvDevList.reset( new std::vector<arc::gen3::arcDev_t>() );
			}

			if ( m_pvDevList == nullptr )
			{
				throwArcGen3Error( "Failed to allocate resources for PCI device list!"s );
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
	
				//
				// Dump any existing bindings
				//
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

				PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData = NULL;
				SP_DEVICE_INTERFACE_DATA tDeviceInterfaceData;
				HDEVINFO hHardwareDeviceInfo;

				BOOL  bResult		   = FALSE;
				DWORD dwRequiredLength = 0;
				DWORD dwMemberIndex	   = 0;

				arc::gen3::CArcBase::zeroMemory( &tDeviceInterfaceData, sizeof( SP_DEVICE_INTERFACE_DATA ) );

				hHardwareDeviceInfo = SetupDiGetClassDevs( ( LPGUID )&GUID_DEVINTERFACE_ARC_PCI,
														   NULL,
														   NULL,
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
														   ( LPGUID )&GUID_DEVINTERFACE_ARC_PCI,
														   dwMemberIndex,
														   &tDeviceInterfaceData );

					if ( bResult == FALSE )
					{
						SetupDiDestroyDeviceInfoList( hHardwareDeviceInfo );
						break;
					}

					SetupDiGetDeviceInterfaceDetail( hHardwareDeviceInfo,
													 &tDeviceInterfaceData,
													 NULL,
													 0,
													 &dwRequiredLength,
													 NULL );

					pDeviceInterfaceDetailData = ( PSP_DEVICE_INTERFACE_DETAIL_DATA )
													LocalAlloc( LMEM_FIXED, dwRequiredLength );

					if ( pDeviceInterfaceDetailData == NULL )
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
															   NULL );

					if ( bResult == FALSE )
					{
						SetupDiDestroyDeviceInfoList( hHardwareDeviceInfo );
						LocalFree( pDeviceInterfaceDetailData );
						break;
					}

					arc::gen3::arcDev_t tArcDev;

					tArcDev.sName = arc::gen3::CArcBase::convertWideToAnsi( ( LPWSTR )pDeviceInterfaceDetailData->DevicePath );

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
																		 IOServiceMatching( kAstroPCIClassName ),
																		 &iterator );
	
				if ( kernResult != KERN_SUCCESS )
				{
					throwArcGen3Error( "IOServiceGetMatchingServices failed: 0x%X", kernResult );
				}
	
				while ( ( service = IOIteratorNext( iterator ) ) != IO_OBJECT_NULL )
				{
					arc::gen3::arcDev_t tArcDev;
		
					tArcDev.sName			= kAstroPCIClassName;
					tArcDev.tService		= service;
		
					m_pvDevList->push_back( tArcDev );
				}
	
				//
				// Release the io_iterator_t now that we're done with it.
				//
				IOObjectRelease( iterator );
	
			#else	// LINUX
	
				struct dirent *pDirEntry = NULL;
				DIR *pDir = NULL;

				pDir = opendir( DEVICE_DIR.data() );

				if ( pDir == NULL )
				{
					throwArcGen3Error( "Failed to open dir: %s", DEVICE_DIR );
				}

				else
				{
					while ( ( pDirEntry = readdir( pDir ) ) != NULL )
					{
						std::string sDirEntry( pDirEntry->d_name );

						if ( ( sDirEntry.find( DEVICE_NAME ) != std::string::npos ||
							   sDirEntry.find( DEVICE_NAME_ALT ) != std::string::npos ) &&
							   sDirEntry.find( "PCIe" ) == std::string::npos )
						{
							arc::gen3::arcDev_t tArcDev;
							tArcDev.sName = std::string( DEVICE_DIR ) + sDirEntry;
							m_pvDevList->push_back( tArcDev );
						}
					}

					closedir( pDir );
				}

			#endif

			//
			// Make sure the bindings exist
			//
			if ( m_pvDevList->empty() )
			{
				throwArcGen3Error( "No device bindings exist! Make sure an ARC, Inc PCI card is installed!"s );
			}
		}


		// +----------------------------------------------------------------------------
		// |  deviceCount
		// +----------------------------------------------------------------------------
		// |  Returns the number of items in the gen3 list. Must be called after
		// |  findDevices().
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::deviceCount( void ) noexcept
		{
			if ( m_pvDevList != nullptr )
			{
				return static_cast<std::uint32_t>( m_pvDevList->size() );
			}

			return 0;
		}


		// +----------------------------------------------------------------------------
		// |  getDeviceStringList
		// +----------------------------------------------------------------------------
		// |  Returns a std::string list representation of the gen3 list. Must be called
		// |  after findDevices().
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		const std::weak_ptr<std::string[]> CArcPCI::getDeviceStringList( void )
		{
			if ( !m_pvDevList->empty() )
			{
				m_psDevList.reset( new std::string[ m_pvDevList->size() ] );

				if ( m_psDevList == nullptr )
				{
					throwArcGen3Error( "Failed to allocate storage for PCI device std::string list"s );
				}

				for ( std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>( m_pvDevList->size() ); i++ )
				{
					std::ostringstream oss;

		#ifdef __APPLE__
					oss << "PCI Device " << i << m_vDevList->at( i ).sName << ends;
		#else
					oss << "PCI Device " << i << std::ends;
		#endif
					m_psDevList[ i ] = oss.str();
				}
			}

			else
			{
				m_psDevList.reset( new std::string[ 1 ] );

				if ( m_psDevList == nullptr )
				{
					throwArcGen3Error( "Failed to allocate storage for PCI device std::string list"s );
				}

				m_psDevList[ 0 ] = std::string( "No Devices Found!"s );
			}

			return m_psDevList;
		}


		// +----------------------------------------------------------------------------
		// |  isOpen
		// +----------------------------------------------------------------------------
		// |  Returns 'true' if connected to PCI gen3; 'false' otherwise.
		// |
		// |  Throws NOTHING on error. No error handling.
		// +----------------------------------------------------------------------------
		bool CArcPCI::isOpen( void ) noexcept
		{
			return CArcDevice::isOpen();
		}


		// +----------------------------------------------------------------------------
		// |  open
		// +----------------------------------------------------------------------------
		// |  Opens a connection to the gen3 driver associated with the specified
		// |  gen3.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> uiDeviceNumber - Device number
		// +----------------------------------------------------------------------------
		void CArcPCI::open( const std::uint32_t uiDeviceNumber )
		{
			if ( isOpen() )
			{
				throwArcGen3Error( "Device already open, call close() first!"s );
			}

			//
			// Make sure the bindings exist
			//
			if ( m_pvDevList->empty() )
			{
				throwArcGen3Error( "No device bindings exist!"s );
			}

			// Verify gen3 number
			//
			if ( uiDeviceNumber > static_cast<std::uint32_t>( m_pvDevList->size() ) )
			{
				throwArcGen3Error( "Invalid device number: %u", uiDeviceNumber );
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
		void CArcPCI::open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiBytes )
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
		void CArcPCI::open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiRows, const std::uint32_t uiCols )
		{
			open( uiDeviceNumber );

			mapCommonBuffer( static_cast<std::size_t>( uiRows ) * static_cast<std::size_t>( uiCols ) * sizeof( std::uint16_t ) );
		}
		

		// +----------------------------------------------------------------------------
		// |  close
		// +----------------------------------------------------------------------------
		// |  Closes the currently open driver that was opened with a call to
		// |  open.
		// |
		// |  Throws NOTHING on error. No error handling.
		// +----------------------------------------------------------------------------
		void CArcPCI::close( void )
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

			m_hDevice    = INVALID_HANDLE_VALUE;
			m_bStoreCmds = bOldStoreCmds;
		}


		// +----------------------------------------------------------------------------
		// |  reset
		// +----------------------------------------------------------------------------
		// |  Resets the PCI board.
		// |
		// |  Throws NOTHING on error. No error handling.
		// +----------------------------------------------------------------------------
		void CArcPCI::reset( void )
		{
			std::uint32_t uiReply = PCICommand( PCI_RESET );

			if ( uiReply != DON )
			{
				throwArcGen3Error( "PCI reset failed! Expected: 'DON' [ 0x444F4E ], Received: 0x%X", uiReply );
			}
		}


		// +----------------------------------------------------------------------------
		// |  mapCommonBuffer
		// +----------------------------------------------------------------------------
		// |  Map the gen3 driver image buffer.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> uiBytes - The number of bytes to map as an image buffer. Not used by PCI.
		// +----------------------------------------------------------------------------
		void CArcPCI::mapCommonBuffer( const std::size_t uiBytes )
		{
			if ( uiBytes <= 0 )
			{
				throwArcGen3Error( "Invalid buffer size: %u. Must be greater than zero!", uiBytes );		
			}

			m_tImgBuffer.pUserAddr = reinterpret_cast<std::uint16_t*>( CArcSystem::mmap( m_hDevice, ASTROPCI_MEM_MAP, static_cast<std::uint32_t>( uiBytes ) ) );

			if ( reinterpret_cast< std::uint8_t* >( m_tImgBuffer.pUserAddr ) == MAP_FAILED )
			{
				arc::gen3::CArcBase::zeroMemory( &m_tImgBuffer, sizeof( arc::gen3::device::ImgBuf_t ) );

				throwArcGen3Error( "Failed to map image buffer : [ %d ] %e", arc::gen3::CArcBase::getSystemError(), arc::gen3::CArcBase::getSystemError() );		
			}

			auto bSuccess = getCommonBufferProperties();

			if ( !bSuccess )
			{
				throwArcGen3Error( "Failed to read image buffer size : [ %d ] %e", arc::gen3::CArcBase::getSystemError(), arc::gen3::CArcBase::getSystemError() );		
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
		void CArcPCI::unMapCommonBuffer( void )
		{
			if ( m_tImgBuffer.pUserAddr != ( void * )nullptr )
			{
				arc::gen3::CArcSystem::munmap( m_hDevice, ASTROPCI_MEM_UNMAP, m_tImgBuffer.pUserAddr, m_tImgBuffer.ulSize );
			}
	
			arc::gen3::CArcBase::zeroMemory( &m_tImgBuffer, sizeof( arc::gen3::device::ImgBuf_t ) );
		}


		// +----------------------------------------------------------------------------
		// |  getId
		// +----------------------------------------------------------------------------
		// |  Returns 0, since the PCI board has no ID!
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::getId( void )
		{
			return 0U;
		}


		// +----------------------------------------------------------------------------
		// |  getStatus
		// +----------------------------------------------------------------------------
		// |  Returns the current value of the PCI boards HTF ( Host Transfer Flags )
		// |  bits from the HSTR register.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::getStatus( void )
		{
			auto uiRetVal = getHSTR();

			uiRetVal = ( ( uiRetVal & HTF_BIT_MASK ) >> 3 );

			return uiRetVal;
		}


		// +----------------------------------------------------------------------------
		// |  clearStatus
		// +----------------------------------------------------------------------------
		// |  Clears the PCI status register.
		// +----------------------------------------------------------------------------
		void CArcPCI::clearStatus( void )
		{
			// Not Used ????
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
		void CArcPCI::set2xFOTransmitter( bool bOnOff )
		{
			auto uiReply = 0U;

			if ( bOnOff )
			{
				if ( ( uiReply = command( { TIM_ID, XMT, 1U } ) ) != DON )
				{
					throwArcGen3Error( "Failed to SET use of 2x fiber optic transmitters on controller, reply: 0x%X", uiReply );
				}
			}

			else
			{
				if ( ( uiReply = command( { TIM_ID, XMT, 0U } ) ) != DON )
				{
					throwArcGen3Error( "Failed to CLEAR use of 2x fiber optic transmitters on controller, reply: 0x%X", uiReply );
				}
			}
		}


		// +----------------------------------------------------------------------------
		// |  loadDeviceFile
		// +----------------------------------------------------------------------------
		// |  Loads a PCI '.lod' file into the PCI boards DSP for execution, which
		// |  begins immediately following upload.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN> -> tFile - The PCI '.lod' file to load.
		// +----------------------------------------------------------------------------
		void CArcPCI::loadDeviceFile( const std::filesystem::path& tFile )
		{
			std::uint32_t uiHctrValue	= 0U;
			std::uint32_t uiWordTotal	= 0U;
			std::uint32_t uiWordCount	= 0U;
			std::uint32_t uiStartAddr	= 0U;
			std::uint32_t uiData		= 0U;
			std::uint32_t uiFailedCount	= 0U;
			bool bPCITransferModeSet	= false;

			std::string sLine;

			//
			// open the file for reading
			// -------------------------------------------------------------------
			std::ifstream inFile( tFile );

			if ( !inFile.is_open() )
			{
				throwArcGen3Error( "Cannot open file: %s", tFile.c_str() );
			}

			//
			// Check for valid PCI file
			// -------------------------------------------------------------------
			getline( inFile, sLine );

			if ( sLine.find( "PCI" ) == std::string::npos )
			{
				throwArcGen3Error( "Invalid PCI file, no PCIBOOT std::string found."s );
			}

			//
			// Verify gen3 connection
			// -------------------------------------------------------------------
			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			//
			// Set the PCI board HCTR bits. Bits 8 & 9 (HTF bits) are cleared to
			// allow 32-bit values to be written to the PCI board without loss
			// of bytes. The 32-bit values are broken up into two 16-bit values.
			// -------------------------------------------------------------------
			uiHctrValue = getHCTR();

			//
			// Clear the HTF bits (8 and 9) and bit3
			//
			uiHctrValue = uiHctrValue & HTF_CLEAR_MASK;

			//
			// Set the HTF bits
			//
			uiHctrValue = ( uiHctrValue | HTF_MASK );
			setHCTR( uiHctrValue );
			bPCITransferModeSet = true;

			//
			// Inform the DSP that new pci boot code will be downloaded
			//
			auto uiDload = 0U;

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_PCI_DOWNLOAD, &uiDload, sizeof( uiDload ) ) )
			{
				throwArcGen3Error( "Failed to write PCI download indicator. %e", arc::gen3::CArcBase::getSystemError() );
			}

			//
			// Set the magic number that indicates a pci download
			//
			uiData = 0x00555AAAU;

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_HCVR_DATA, &uiData, sizeof( uiData ) ) )
			{
				throwArcGen3Error( "Failed to write PCI download magic number. %e", arc::gen3::CArcBase::getSystemError() );
			}

			//
			// Write the data to the PCI
			//
			while ( !inFile.eof() )
			{
				getline( inFile, sLine );

				if ( sLine.find( "_DATA P"s ) != std::string::npos )
				{
					getline( inFile, sLine );

					auto pTokens = arc::gen3::CArcBase::splitString( sLine );

					//
					// Set the number of words and start address
					//
					auto tFromCharsResult = std::from_chars( pTokens->at( 0 ).data(), ( pTokens->at( 0 ).data() + pTokens->at( 0 ).size() ), uiWordTotal, 16 );

					if ( tFromCharsResult.ec == std::errc::invalid_argument )
					{
						throwArcGen3InvalidArgument( "Failed to convert word total"s );
					}

					if ( tFromCharsResult.ec == std::errc::result_out_of_range )
					{
						throwArcGen3LengthError( "Failed to convert word total"s );
					}

					if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_HCVR_DATA, &uiWordTotal, sizeof( uiWordTotal ) ) )
					{
						throwArcGen3Error( "Failed to write word total. %e", arc::gen3::CArcBase::getSystemError() );
					}

					tFromCharsResult = std::from_chars( pTokens->at( 1 ).data(), ( pTokens->at( 1 ).data() + pTokens->at( 1 ).size() ), uiStartAddr, 16 );

					if ( tFromCharsResult.ec == std::errc::invalid_argument )
					{
						throwArcGen3InvalidArgument( "Failed to convert word total"s );
					}

					if ( tFromCharsResult.ec == std::errc::result_out_of_range )
					{
						throwArcGen3LengthError( "Failed to convert word total"s );
					}

					if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_HCVR_DATA, &uiStartAddr, sizeof( uiStartAddr ) ) )
					{
						throwArcGen3Error( "Failed to write start address. %e", arc::gen3::CArcBase::getSystemError() );
					}

					//
					// Throw away the next line (example: _DATA P 000002)
					//
					getline( inFile, sLine );

					//
					// Load the data
					//
					while ( uiWordCount < uiWordTotal )
					{
						//
						// Get the next line, this is the data start
						//
						getline( inFile, sLine );

						//
						// Check for intermixed "_DATA" strings
						//
						if ( sLine.find( "_DATA P"s ) == std::string::npos )
						{
							auto pDataTokens = arc::gen3::CArcBase::splitString( sLine );

							for ( auto it = pDataTokens->begin(); it != pDataTokens->end(); it++ )
							{
								if ( uiWordCount >= uiWordTotal )
								{
									break;
								}

								tFromCharsResult = std::from_chars( ( *it ).data(), ( ( *it ).data() + ( *it ).size() ), uiData, 16 );

								if ( tFromCharsResult.ec == std::errc::invalid_argument )
								{
									throwArcGen3InvalidArgument( "Failed to convert word total"s );
								}

								if ( tFromCharsResult.ec == std::errc::result_out_of_range )
								{
									throwArcGen3LengthError( "Failed to convert word total"s );
								}

								if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_HCVR_DATA, &uiData, sizeof( uiData ) ) )
								{
									throwArcGen3Error( "Failed to write start address. %e", arc::gen3::CArcBase::getSystemError() );
								}

								uiWordCount++;
							}
						}	// End if _DATA P
					}	// End while wordCount < wordTotal
			
					break;

				}	// End if strstr != null
			}	// End while not eof

			//
			// Set the PCI data size transfer mode
			//
			if ( bPCITransferModeSet )
			{
				uiHctrValue = getHCTR();
				setHCTR( ( uiHctrValue & 0xCFF ) | 0x900 );
			}

			//
			// Wait for the PCI DSP to finish initialization
			//
			if ( bPCITransferModeSet )
			{
				auto uiInData	= PCI_ID;
				auto uiRetVal	= 0U;

				if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_PCI_DOWNLOAD_WAIT, &uiInData, sizeof( uiInData ), &uiRetVal, sizeof( uiRetVal ) ) )
				{
					throwArcGen3Error( "PCI download wait failed! %e", arc::gen3::CArcBase::getSystemError() );
				}

				//
				// Make sure a DON is received
				//
				if ( uiRetVal != DON )
				{
					throwArcGen3Error( "PCI download failed. Reply: 0x%X", uiRetVal );
				}
			}

			//
			// Test PCI communications
			//
			std::uint32_t uiReply = 0;

			for ( std::uint32_t i = 0; i < MAX_PCI_COMM_TEST; i++ )
			{
				uiData  = PCI_COM_TEST_VALUE * i;

				uiReply = command( { PCI_ID, TDL, uiData } );

				if ( uiReply != uiData )
				{
					uiFailedCount++;
				}
			}

			//
			// If ALL the communication tests failed, then report an error.
			//
			if ( uiFailedCount >= MAX_PCI_COMM_TEST )
			{
				throwArcGen3Error( "PCI communications test failed."s );
			}

			inFile.close();
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
		std::uint32_t CArcPCI::command( const std::initializer_list<const std::uint32_t>& tCmdList )
		{
			if ( !isOpen() )
			{
				throwArcGen3Error( "Not connected to any device!"s );
			}

			if ( tCmdList.size() > CTLR_CMD_MAX )
			{
				throwArcGen3Error( "Command list too large. Cannot exceed four arguments!"s );
			}

			std::unique_ptr<std::uint32_t[]> pCmdData( new std::uint32_t[ CTLR_CMD_MAX ] );

			auto pInserter = pCmdData.get();

			for ( auto it = tCmdList.begin(); it != tCmdList.end(); it++ )
			{
				if ( it == tCmdList.begin() )
				{
					*pInserter = ( ( *it << 8 ) | static_cast<std::uint32_t>( tCmdList.size() ) );
				}

				else
				{
					*pInserter = *it;
				}

				pInserter++;
			}

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice,
														  ASTROPCI_COMMAND,
														  pCmdData.get(),
														  (CTLR_CMD_MAX * sizeof( std::uint32_t ) ),
														  pCmdData.get(),
														  sizeof( std::uint32_t ) );

			auto uiReply = pCmdData[ 0 ];

			if ( !bSuccess )
			{
				std::string sCmd = arc::gen3::CArcBase::iterToString( tCmdList.begin(), tCmdList.end() );
	
				if ( m_bStoreCmds )
				{
					m_pCLog->put( sCmd.c_str() );
				}

				throwArcGen3Error( sCmd );
			}

			// Set the debug message queue. Can't pass dCmdData[ 0 ] because
			// linux/unix systems overwrite this in the driver with the reply.
			if ( m_bStoreCmds )
			{
				std::string sCmd = arc::gen3::CArcBase::iterToString( tCmdList.begin(), tCmdList.end() );
	
				m_pCLog->put( sCmd.c_str() );
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
		std::uint32_t CArcPCI::getControllerId( void )
		{
			//
			//  Check the controller id  ... And YES,
			//  PCI_ID is correct for ARC-12!
			// +--------------------------------------+
			std::uint32_t uiId = command( { PCI_ID, SID } );

			if ( !IS_ARC12( uiId ) )
			{
				uiId = command( { TIM_ID, SID } );
			}

			return uiId;
		}


		// +----------------------------------------------------------------------------
		// |  resetController
		// +----------------------------------------------------------------------------
		// |  Resets the controller.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCI::resetController( void )
		{
			std::uint32_t uiRetVal = PCICommand( RESET_CONTROLLER );

			if ( uiRetVal != SYR )
			{
				throwArcGen3Error( "Reset controller failed. Reply: 0x%X", uiRetVal );
			}
		}


		// +----------------------------------------------------------------------------
		// | isControllerConnected
		// +----------------------------------------------------------------------------
		// |  Returns 'true' if a controller is connected to the PCI board. This is
		// |  tested by sending a TDL to the controller. If it succeeds, then the
		// |  controller is ready.
		// |
		// |  Throws std::runtime_error on error ( indirectly via command() )
		// +----------------------------------------------------------------------------
		bool CArcPCI::isControllerConnected( void )
		{
			bool bIsSetup	= false;
			auto uiVal     = 0x112233U;
			auto uiRetVal  = 0U;

			try
			{
				uiRetVal = command( { TIM_ID, TDL, uiVal } );

				if ( uiRetVal == uiVal )
				{
					bIsSetup = true;
				}
			}
			catch ( ... ) {}

			return bIsSetup;
		}


		// +----------------------------------------------------------------------------
		// |  stopExposure
		// +----------------------------------------------------------------------------
		// |  Stops the current exposure.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCI::stopExposure( void )
		{
			PCICommand( ABORT_READOUT );
		}


		// +----------------------------------------------------------------------------
		// |  isReadout
		// +----------------------------------------------------------------------------
		// |  Returns 'true' if the controller is in readout.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		bool CArcPCI::isReadout( void )
		{
			std::uint32_t uiStatus = ( ( getHSTR() & HTF_BIT_MASK ) >> 3 );

			return ( uiStatus == static_cast<std::uint32_t>( ePCIStatus::READOUT_STATUS ) ? true : false );
		}


		// +----------------------------------------------------------------------------
		// |  getPixelCount
		// +----------------------------------------------------------------------------
		// |  Returns the current pixel count.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::getPixelCount( void )
		{
			auto uiRetVal = 0U;

			//
			// Verify gen3 connection
			// -------------------------------------------------------------------
			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			else
			{
				if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_PROGRESS, nullptr, 0, &uiRetVal, sizeof( uiRetVal ) ) )
				{
					throwArcGen3Error( "Failed to read pixel count. %e", arc::gen3::CArcBase::getSystemError() );
				}
			}

			return uiRetVal;
		}


		// +----------------------------------------------------------------------------
		// |  getCRPixelCount
		// +----------------------------------------------------------------------------
		// |  Returns the cumulative pixel count as when doing continuous readout.
		// |  This method is used by user applications when reading large images
		// | ( greater than 4K x 4K ). This value increases across all frames being
		// |  readout. This code needs to execute fast, so not pre-checking, such as 
		// |  isControllerConnected() is done.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::getCRPixelCount( void )
		{
			auto uiPixelCount = 0U;

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_CR_PROGRESS, nullptr, 0, &uiPixelCount, sizeof( uiPixelCount ) ) )
			{
				throwArcGen3Error( "Failed to read pixel count. %e", arc::gen3::CArcBase::getSystemError() );
			}

			return uiPixelCount;
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
		std::uint32_t CArcPCI::getFrameCount( void )
		{
			auto uiFrameCount = 0U;

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_FRAMES_READ, nullptr, 0, &uiFrameCount, sizeof( uiFrameCount ) ) )
			{
				throwArcGen3Error( "Failed to read frame count. %e", arc::gen3::CArcBase::getSystemError() );
			}

			return uiFrameCount;
		}


		// +----------------------------------------------------------------------------
		// |  setHCTR
		// +----------------------------------------------------------------------------
		// |  Sets the current value of the PCI boards HCTR register.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN> -> uiVal - The value to set the PCI board HCTR register to.
		// +----------------------------------------------------------------------------
		void CArcPCI::setHCTR( const std::uint32_t uiVal )
		{
			//
			// Verify gen3 connection
			// -------------------------------------------------------------------
			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			else
			{
				auto uiValue = uiVal;

				if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_SET_HCTR, &uiValue, sizeof( uiValue ) ) )
				{
					throwArcGen3Error( "Failed to write to HCTR. %e", arc::gen3::CArcBase::getSystemError() );
				}
			}
		}


		// +----------------------------------------------------------------------------
		// |  getHSTR
		// +----------------------------------------------------------------------------
		// |  Returns the current value of the PCI boards HSTR register.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::getHSTR()
		{
			auto uiRetVal = 0U;

			//
			// Verify gen3 connection
			// -------------------------------------------------------------------
			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			else
			{
				if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_HSTR, nullptr, 0, &uiRetVal, sizeof( uiRetVal ) ) )
				{
					throwArcGen3Error( "Failed to read HSTR. %e", arc::gen3::CArcBase::getSystemError() );
				}
			}

			return uiRetVal;
		}


		// +----------------------------------------------------------------------------
		// |  getHCTR
		// +----------------------------------------------------------------------------
		// |  Returns the current value of the PCI boards HCTR register.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::getHCTR( void )
		{
			auto uiRetVal = 0U;

			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			else
			{
				if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_HCTR, nullptr, 0, &uiRetVal, sizeof( uiRetVal ) ) )
				{
					throwArcGen3Error( "Failed to read HCTR. %e", arc::gen3::CArcBase::getSystemError() );
				}
			}

			return uiRetVal;
		}


		// +----------------------------------------------------------------------------
		// |  PCICommand
		// +----------------------------------------------------------------------------
		// |  Send a command to the PCI board.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> command - PCI command
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::PCICommand( const std::uint32_t uiCommand )
		{
			if ( !isOpen() )
			{
				throwArcGen3NoDeviceError();
			}

			std::uint32_t uiRetVal = uiCommand;

			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_SET_HCVR, const_cast<std::uint32_t*>( &uiCommand ), sizeof( uiCommand ), &uiRetVal, sizeof(uiRetVal));

			if ( !bSuccess )
			{
				if ( m_bStoreCmds )
				{
					const std::string sCmd = formatPCICommand( uiCommand, uiRetVal );

					m_pCLog->put( sCmd.c_str() );
				}

				const std::string sErr = formatPCICommand( uiCommand, uiRetVal, true );

				throwArcGen3Error( sErr.c_str() );
			}

			// Set the debug message queue
			if ( m_bStoreCmds )
			{
				const std::string sCmd = formatPCICommand( uiCommand, uiRetVal );

				m_pCLog->put( sCmd.c_str() );
			}

			return uiRetVal;
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
		// |  <IN> -> bAbort      - 'true' to stop; 'false' otherwise. Default: nullptr
		// +----------------------------------------------------------------------------
		void CArcPCI::loadGen23ControllerFile( const std::filesystem::path& tFilename, bool bValidate, bool* pAbort )
		{
			std::uint32_t  uiBoardId		= 0U;
			std::uint32_t  uiType			= 0U;
			std::uint32_t  uiAddr			= 0U;
			std::uint32_t  uiData			= 0U;
			std::uint32_t  uiReply			= 0U;
			std::uint32_t  uiPciStatus		= 0U;
			char           typeChar			= ' ';
			bool           bPciStatusSet	= false;
			bool           bIsCLodFile  	= false;

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
			bPciStatusSet = true;

			uiPciStatus = command( { PCI_ID, RDM, ( X_MEM | 0U ) } );

			uiReply = command( { PCI_ID, WRM, ( X_MEM | 0U ), ( uiPciStatus | 0x00000002U ) } );

			if ( uiReply != DON )
			{
				throwArcGen3Error( "Set PCI status bit 1 failed. Reply: 0x%X", uiReply );
			}

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
					auto pTokens = arc::gen3::CArcBase::splitString( sLine );

					// Dump _DATA std::string ( first std::string )

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

							auto pBlockTokens = arc::gen3::CArcBase::splitString( sLine );

							for ( auto it = pBlockTokens->begin(); it != pBlockTokens->end(); it++ )
							{
								if ( pAbort != nullptr && *pAbort ) { inFile.close(); return; }
							
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
			//
			// NOTE: This should be undone for sure. Otherwise, the board may
			// be stuck until the computer is turned off and unplugged!! Also,
			// "cmdStatus" is used in place of "status" because this set of
			// commands may override "status" with API_OK, when it should be
			// API_ERROR from a previous error.
	
			if ( bPciStatusSet )
			{
				auto uiPciStatus = command( { PCI_ID, RDM, ( X_MEM | 0U ) } );

				uiReply = command( { PCI_ID, WRM, ( X_MEM | 0U ), ( uiPciStatus & 0xFFFFFFFDU ) } );

				if ( uiReply != DON )
				{
					throwArcGen3Error( "Clear PCI status bit 1 failed. Reply: 0x%X", uiReply );
				}
			}

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
		// |  Returns the boundary adjusted image size for continuous readout.  The PCI
		// |  card ( ARC-63/64 ) requires that each image within the image buffer starts
		// |  on a 1024 byte boundary.
		// |
		// |  Throws std::runtime_error on error
		// |
		// |  <IN>  -> uiImageSize - The boundary adjusted image size ( in bytes ).
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::getContinuousImageSize( const std::uint32_t uiImageSize )
		{
			auto uiBoundedImageSize = 0U;

			if ( ( uiImageSize & 0x3FF ) != 0U )
			{
				uiBoundedImageSize = ( uiImageSize - ( uiImageSize & 0x3FFU ) + 1024U );
			}
			else
			{
				uiBoundedImageSize = uiImageSize;
			}

			return uiBoundedImageSize;
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
		std::uint32_t CArcPCI::smallCamDLoad( const std::uint32_t uiBoardId, const std::vector<std::uint32_t>* pvData )
		{
			auto uiHeader	= 0U;
			auto uiReply	= 0U;

			//
			//  Report error if gen3 reports readout in progress
			// +------------------------------------------------------+
			if ( isReadout() )
			{
				throwArcGen3Error( "Device reports readout in progress! Status: 0x%X", getStatus() );
			}

			//
			//  Verify the size of the data, cannot be greater than 6
			// +------------------------------------------------------+
			if ( pvData->size() > 6 )
			{
				throwArcGen3Error( "Data vector too large: 0x%X! Must be less than 6!",	pvData->size() );
			}

			//
			//  Verify the board id equals smallcam download id
			// +------------------------------------------------------+
			if ( uiBoardId != SMALLCAM_DLOAD_ID )
			{
				throwArcGen3Error( "Invalid board id: %u! Must be: %u",	uiBoardId, SMALLCAM_DLOAD_ID );
			}

			try
			{
				//
				//  Send Header
				// +-------------------------------------------------+
				uiHeader = ( ( uiBoardId << 8 ) | static_cast<std::uint32_t>( pvData->size() + 1 ) );

				if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_HCVR_DATA, &uiHeader, sizeof( uiHeader ) ) )
				{
					throwArcGen3Error( "Failed to write command header. %e", arc::gen3::CArcBase::getSystemError() );
				}

				auto uiData = 0U;

				//
				//  Send the data
				// +-------------------------------------------------+
				for ( decltype( pvData->size() ) i = 0; i < pvData->size(); i++ )
				{
					uiData = ( 0xAC000000U | pvData->at( i ) );

					if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_HCVR_DATA, &uiData, sizeof( uiData ) ) )
					{
						throwArcGen3Error( "Failed to write data value #%j. %e", i, arc::gen3::CArcBase::getSystemError() );
					}
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
				uiReply = ( ( getHSTR() & HTF_BIT_MASK ) >> 3 );
			}
			catch ( const std::exception& e )
			{
				if ( m_bStoreCmds )
				{
					m_pCLog->put( formatDLoadString( uiReply, uiBoardId, pvData ).c_str() );
				}

				std::ostringstream oss;

				oss << e.what() << "\nException Details:\n0x" << std::hex << uiHeader << std::dec;

				for ( decltype( pvData->size() ) i = 0; i < pvData->size(); i++ )
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
		// |  Turns the PCI hardware byte-swapping on if system architecture is solaris.
		// |  Otherwise, does nothing; compiles to empty function.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		void CArcPCI::setByteSwapping( void )
		{
			#if !defined( _WINDOWS ) && !defined( linux ) && !defined( __linux ) && !defined( __APPLE__ )

				//
				// Test if byte swapping is available
				//
				std::uint32_t uiReply = command( { PCI_ID, TBS } );

				if ( containsError( uiReply ) )
				{
					std::string sCmdMsg = CArcTools::CmdToString( uiReply, PCI_ID, TBS );

					throwArcGen3Error( sCmdMsg );
				}

				//
				// Turn hardware byte swapping ON
				//
				uiReply = command( { PCI_ID, SBS, 1 } );

				if ( containsError( uiReply ) )
				{
					std::string sCmdMsg = CArcTools::CmdToString( uiReply, PCI_ID, SBS, 1 );

					throwArcGen3Error( sCmdMsg );
				}

				if ( m_bStoreCmds )
				{
					m_pCLog->put( ( char * )"Hardware byte swapping on!"s );
				}

			#endif
		}


		// +----------------------------------------------------------------------------
		// |  getCommonBufferProperties
		// +----------------------------------------------------------------------------
		// |  Fills in the image buffer structure with its properties, such as
		// |  physical address and size.
		// |
		// |  Throws NOTHING on error. No error handling.
		// +----------------------------------------------------------------------------
		bool CArcPCI::getCommonBufferProperties()
		{
			auto bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_DMA_SIZE, nullptr, 0, &m_tImgBuffer.ulSize, sizeof( m_tImgBuffer.ulSize ) );

			if ( bSuccess )
			{
				bSuccess = arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_DMA_ADDR, nullptr, 0, &m_tImgBuffer.ulPhysicalAddr, sizeof( m_tImgBuffer.ulPhysicalAddr ) );
			}

			return bSuccess;
		}


		// +----------------------------------------------------------------------------
		// |  FormatPCICommand
		// +----------------------------------------------------------------------------
		// |  Formats an IOCTL command into a std::string that can be passed into a throw
		// |  exception method.
		// |
		// |  <IN> -> uiCmd      : The command value.
		// |  <IN> -> uiReply    : The received command reply value.
		// |  <IN> -> uiArg      : Any argument that may go with the specified command.
		// |  <IN> -> bGetSysErr : A system error number, used to get system message.
		// +----------------------------------------------------------------------------
		const std::string CArcPCI::formatPCICommand( const std::uint32_t uiCmd, const std::uint64_t uiReply, const std::uint32_t uiArg, bool bGetSysErr )
		{
			std::ostringstream oss;

			oss.setf( std::ios::hex, std::ios::basefield );
			oss.setf( std::ios::uppercase );

			oss << "[ "sv;

				 if ( uiCmd == ASTROPCI_GET_HCTR ) { oss << "ASTROPCI_GET_HCTR"sv; }
			else if ( uiCmd == ASTROPCI_GET_PROGRESS ) { oss << "ASTROPCI_GET_PROGRESS"sv; }
			else if ( uiCmd == ASTROPCI_GET_DMA_ADDR ) { oss << "ASTROPCI_GET_DMA_ADDR"sv; }
			else if ( uiCmd == ASTROPCI_GET_HSTR ) { oss << "ASTROPCI_GET_HSTR"sv; }
			else if ( uiCmd == ASTROPCI_GET_DMA_SIZE ) { oss << "ASTROPCI_GET_DMA_SIZE"sv; }
			else if ( uiCmd == ASTROPCI_GET_FRAMES_READ ) { oss << "ASTROPCI_GET_FRAMES_READ"sv; }
			else if ( uiCmd == ASTROPCI_HCVR_DATA ) { oss << "ASTROPCI_HCVR_DATA"sv; }
			else if ( uiCmd == ASTROPCI_SET_HCTR ) { oss << "ASTROPCI_SET_HCTR"sv; }
			else if ( uiCmd == ASTROPCI_SET_HCVR ) { oss << "ASTROPCI_SET_HCVR"sv; }
			else if ( uiCmd == ASTROPCI_PCI_DOWNLOAD ) { oss << "ASTROPCI_PCI_DOWNLOAD"sv; }
			else if ( uiCmd == ASTROPCI_PCI_DOWNLOAD_WAIT ) { oss << "ASTROPCI_PCI_DOWNLOAD_WAIT"sv; }
			else if ( uiCmd == ASTROPCI_COMMAND ) { oss << "ASTROPCI_COMMAND"sv; }
			else if ( uiCmd == ASTROPCI_GET_CONFIG_BYTE ) { oss << "ASTROPCI_GET_CONFIG_BYTE"sv; }
			else if ( uiCmd == ASTROPCI_GET_CONFIG_WORD ) { oss << "ASTROPCI_GET_CONFIG_WORD"sv; }
			else if ( uiCmd == ASTROPCI_GET_CONFIG_DWORD ) { oss << "ASTROPCI_GET_CONFIG_DWORD"sv; }
			else if ( uiCmd == ASTROPCI_SET_CONFIG_BYTE ) { oss << "ASTROPCI_SET_CONFIG_BYTE"sv; }
			else if ( uiCmd == ASTROPCI_SET_CONFIG_WORD ) { oss << "ASTROPCI_SET_CONFIG_WORD"sv; }
			else if ( uiCmd == ASTROPCI_SET_CONFIG_DWORD ) { oss << "ASTROPCI_SET_CONFIG_DWORD"sv; }

		#ifdef _WINDOWS
			else if ( uiCmd == ASTROPCI_MEM_MAP ) { oss << "ASTROPCI_MEM_MAP"sv; }
			else if ( uiCmd == ASTROPCI_MEM_UNMAP ) { oss << "ASTROPCI_MEM_UNMAP"sv; }
		#endif

			else oss << "0x"sv << uiCmd;

			if ( uiArg != CArcDevice::NOPARAM )
			{
				oss << " 0x"sv << uiArg;
			}

			oss << " -> 0x"sv << uiReply << " ]"sv;

			if ( bGetSysErr )
			{
				oss << "\n" << std::dec << CArcBase::getSystemMessage( CArcBase::getSystemError() );
			}

			oss << std::ends;

			return oss.str();
		}


		// +----------------------------------------------------------------------------
		// |  FormatPCICommand
		// +----------------------------------------------------------------------------
		// |  Formats an IOCTL command into a std::string that can be passed into a throw
		// |  exception method.
		// |
		// |  <IN> -> uiCmd      : The command value.
		// |  <IN> -> uiReply    : The received command reply value.
		// |  <IN> -> tArgList   : List of arguments that go with the specified command.
		// |  <IN> -> bGetSysErr : A system error number, used to get system message.
		// +----------------------------------------------------------------------------
		const std::string CArcPCI::formatPCICommand( const std::uint32_t uiCmd, const std::uint64_t uiReply, const std::initializer_list<const std::uint32_t>& tArgList, bool bGetSysErr )
		{
			std::ostringstream oss;

			oss.setf( std::ios::hex, std::ios::basefield );
			oss.setf( std::ios::uppercase );

			oss << "[ "sv;

				 if ( uiCmd == ASTROPCI_GET_HCTR ) { oss << "ASTROPCI_GET_HCTR"sv; }
			else if ( uiCmd == ASTROPCI_GET_PROGRESS ) { oss << "ASTROPCI_GET_PROGRESS"sv; }
			else if ( uiCmd == ASTROPCI_GET_DMA_ADDR ) { oss << "ASTROPCI_GET_DMA_ADDR"sv; }
			else if ( uiCmd == ASTROPCI_GET_HSTR ) { oss << "ASTROPCI_GET_HSTR"sv; }
			else if ( uiCmd == ASTROPCI_GET_DMA_SIZE ) { oss << "ASTROPCI_GET_DMA_SIZE"sv; }
			else if ( uiCmd == ASTROPCI_GET_FRAMES_READ ) { oss << "ASTROPCI_GET_FRAMES_READ"sv; }
			else if ( uiCmd == ASTROPCI_HCVR_DATA ) { oss << "ASTROPCI_HCVR_DATA"sv; }
			else if ( uiCmd == ASTROPCI_SET_HCTR ) { oss << "ASTROPCI_SET_HCTR"sv; }
			else if ( uiCmd == ASTROPCI_SET_HCVR ) { oss << "ASTROPCI_SET_HCVR"sv; }
			else if ( uiCmd == ASTROPCI_PCI_DOWNLOAD ) { oss << "ASTROPCI_PCI_DOWNLOAD"sv; }
			else if ( uiCmd == ASTROPCI_PCI_DOWNLOAD_WAIT ) { oss << "ASTROPCI_PCI_DOWNLOAD_WAIT"sv; }
			else if ( uiCmd == ASTROPCI_COMMAND ) { oss << "ASTROPCI_COMMAND"sv; }
			else if ( uiCmd == ASTROPCI_GET_CONFIG_BYTE ) { oss << "ASTROPCI_GET_CONFIG_BYTE"sv; }
			else if ( uiCmd == ASTROPCI_GET_CONFIG_WORD ) { oss << "ASTROPCI_GET_CONFIG_WORD"sv; }
			else if ( uiCmd == ASTROPCI_GET_CONFIG_DWORD ) { oss << "ASTROPCI_GET_CONFIG_DWORD"sv; }
			else if ( uiCmd == ASTROPCI_SET_CONFIG_BYTE ) { oss << "ASTROPCI_SET_CONFIG_BYTE"sv; }
			else if ( uiCmd == ASTROPCI_SET_CONFIG_WORD ) { oss << "ASTROPCI_SET_CONFIG_WORD"sv; }
			else if ( uiCmd == ASTROPCI_SET_CONFIG_DWORD ) { oss << "ASTROPCI_SET_CONFIG_DWORD"sv; }

		#ifdef _WINDOWS
			else if ( uiCmd == ASTROPCI_MEM_MAP ) { oss << "ASTROPCI_MEM_MAP"sv; }
			else if ( uiCmd == ASTROPCI_MEM_UNMAP ) { oss << "ASTROPCI_MEM_UNMAP"sv; }
		#endif

			else oss << "0x"sv << uiCmd;

			for ( auto it = tArgList.begin(); it != tArgList.end(); it++ )
			{
				oss << " 0x"sv << *it;
			}

			oss << " -> 0x"sv << uiReply << " ]"sv;

			if ( bGetSysErr )
			{
				oss << "\n" << std::dec << CArcBase::getSystemMessage( CArcBase::getSystemError() );
			}

			oss << std::ends;

			return oss.str();
		}


		/////////////////////////////////////////////////////////////////////////////////////////////////
		//             PCI CONFIGURATION SPACE TESTS
		/////////////////////////////////////////////////////////////////////////////////////////////////
		// +----------------------------------------------------------------------------
		// |  getCfgSpByte
		// +----------------------------------------------------------------------------
		// |  Returns the specified BYTE from the specified PCI configuration space
		// |  register.
		// |
		// |  <IN> -> dOffset - The byte offset from the start of PCI config space
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::getCfgSpByte( const std::uint32_t uiOffset )
		{
			auto uiValue = 0U;

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_CONFIG_BYTE, const_cast<std::uint32_t*>( &uiOffset ), sizeof( uiOffset ), &uiValue, sizeof( uiValue ) ) )
			{
				throwArcGen3Error( "Failed to read configuration byte. %e", arc::gen3::CArcBase::getSystemError() );
			}

			return uiValue;
		}


		// +----------------------------------------------------------------------------
		// |  getCfgSpWord
		// +----------------------------------------------------------------------------
		// |  Returns the specified WORD from the specified PCI configuration space
		// |  register.
		// |
		// |  <IN> -> dOffset - The byte offset from the start of PCI config space
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::uint32_t CArcPCI::getCfgSpWord( const std::uint32_t uiOffset )
		{
			auto uiValue = 0U;

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_CONFIG_WORD, const_cast<std::uint32_t*>(&uiOffset), sizeof( uiOffset ), &uiValue, sizeof( uiValue ) ) )
			{
				throwArcGen3Error( "Failed to read configuration byte. %e", arc::gen3::CArcBase::getSystemError() );
			}

			return uiValue;
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
		std::uint32_t CArcPCI::getCfgSpDWord( const std::uint32_t uiOffset )
		{
			auto uiValue = 0U;

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_CONFIG_DWORD, const_cast<std::uint32_t*>(&uiOffset), sizeof( uiOffset ), &uiValue, sizeof( uiValue ) ) )
			{
				throwArcGen3Error( "Failed to read configuration byte. %e", arc::gen3::CArcBase::getSystemError() );
			}

			return uiValue;
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
		void CArcPCI::setCfgSpByte( const std::uint32_t uiOffset, const std::uint32_t uiValue )
		{
			std::array tData { uiOffset, uiValue };

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_SET_CONFIG_BYTE, tData.data(), static_cast<std::uint32_t>( tData.size() ) ) )
			{
				throwArcGen3Error( "Failed to read configuration byte. %e", arc::gen3::CArcBase::getSystemError() );
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
		void CArcPCI::setCfgSpWord( const std::uint32_t uiOffset, const std::uint32_t uiValue )
		{
			std::array tData { uiOffset, uiValue };

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_SET_CONFIG_WORD, tData.data(), static_cast<std::uint32_t>( tData.size() ) ) )
			{
				throwArcGen3Error( "Failed to read configuration byte. %e", arc::gen3::CArcBase::getSystemError() );
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
		void CArcPCI::setCfgSpDWord( const std::uint32_t uiOffset, const std::uint32_t uiValue )
		{
			std::array tData { uiOffset, uiValue };

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_SET_CONFIG_DWORD, tData.data(), static_cast<std::uint32_t>( tData.size() ) ) )
			{
				throwArcGen3Error( "Failed to read configuration byte. %e", arc::gen3::CArcBase::getSystemError() );
			}
		}


		// +----------------------------------------------------------------------------
		// |  getCfgSp
		// +----------------------------------------------------------------------------
		// |  Reads and parses the entire PCI configuration space header into readable
		// |  text and bit definitions that are stored in a member list variable. The
		// |  public methods of this class allow access to this list. This method will
		// |  create the member list if it doesn't already exist and clears it if it
		// |  does.
		// +----------------------------------------------------------------------------
		void CArcPCI::getCfgSp( void )
		{
			CArcPCIBase::getCfgSp();
		}


		// +----------------------------------------------------------------------------
		// |  getBarSp
		// +----------------------------------------------------------------------------
		// |  Reads and parses the entire PCI Base Address Registers (BAR) into readable
		// |  text and bit definitions that are stored in a member list variable. The
		// |  public methods of this class allow access to this list. This method will
		// |  create the member list if it doesn't already exist and clears it if it
		// |  does. NOTE: Not all BARS or PCI boards have data.
		// +----------------------------------------------------------------------------
		void CArcPCI::getBarSp( void )
		{
			CArcPCIBase::getBarSp();

			//
			//  Access the register data
			// +-------------------------------------------------------+
			std::unique_ptr<PCIRegList_t> pList( new PCIRegList_t() );

			auto uiRegValue = 0U;

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_HCTR, nullptr, 0U, &uiRegValue, sizeof( uiRegValue ) ) )
			{
				throwArcGen3Error( "Failed to read configuration byte. %e", arc::gen3::CArcBase::getSystemError() );
			}

			addRegItem( pList.get(), 0x10U, "Host Control Register ( HCTR )"s, uiRegValue );

			if ( !arc::gen3::CArcSystem::ioctl( m_hDevice, ASTROPCI_GET_HSTR, nullptr, 0U, &uiRegValue, sizeof( uiRegValue ) ) )
			{
				throwArcGen3Error( "Failed to read configuration byte. %e", arc::gen3::CArcBase::getSystemError() );
			}

			addRegItem( pList.get(), 0x14U, "Host Status Register ( HSTR )"s, uiRegValue, getHSTRBitList( uiRegValue ).release() );

			addBarItem( "DSP Regs ( BAR0 )"s, pList.release() );
		}


		// +----------------------------------------------------------------------------
		// |  getHSTRBitList
		// +----------------------------------------------------------------------------
		// |  Sets the bit list strings for the PCI DSP HSTR register.
		// |
		// |  <IN> -> uiData  - The PCI cfg sp CLASS CODE and REV ID register value.
		// |  <IN> -> bDrawSeparator - 'true' to include a line separator within the
		// |                            bit list strings.
		// |
		// |  Throws std::runtime_error on error
		// +----------------------------------------------------------------------------
		std::unique_ptr<CArcStringList> CArcPCI::getHSTRBitList( const std::uint32_t uiData, bool bDrawSeparator )
		{
			std::unique_ptr<CArcStringList> pBitList( new CArcStringList() );

			if ( bDrawSeparator )
			{
				*pBitList << "____________________________________________________";
			}

			auto uiHSTR = ( ( uiData & HTF_BIT_MASK ) >> 3U );

			auto eHSTR = static_cast<ePCIStatus>( uiHSTR );

			if (eHSTR == ePCIStatus::DONE_STATUS )
			{
				#ifdef __cpp_lib_format
					*pBitList << std::format( "Status: 0x{:X} [ DON ]"sv, uiHSTR );
				#else
					*pBitList << CArcBase::formatString( "Status: 0x%X [ DON ]", uiHSTR );
				#endif
			}

			else if ( eHSTR == ePCIStatus::READ_REPLY_STATUS )
			{
				#ifdef __cpp_lib_format
					*pBitList << std::format( "Status: 0x{:X} [ READ REPLY ]"sv, uiHSTR );
				#else
					*pBitList << CArcBase::formatString( "Status: 0x%X [ READ REPLY ]", uiHSTR );
				#endif
			}

			else if ( eHSTR == ePCIStatus::ERROR_STATUS )
			{
				#ifdef __cpp_lib_format
					*pBitList << std::format( "Status: 0x{:X} [ ERR ]"sv, uiHSTR );
				#else
					*pBitList << CArcBase::formatString( "Status: 0x%X [ ERR ]", uiHSTR );
				#endif
			}
	
			else if ( eHSTR == ePCIStatus::SYSTEM_RESET_STATUS )
			{
				#ifdef __cpp_lib_format
					*pBitList << std::format( "Status: 0x{:X} [ SYR ]"sv, uiHSTR );
				#else
					*pBitList << CArcBase::formatString( "Status: 0x%X [ SYR ]", uiHSTR );
				#endif
			}

			else if ( eHSTR == ePCIStatus::READOUT_STATUS )
			{
				#ifdef __cpp_lib_format
					*pBitList << std::format( "Status: 0x{:X} [ READOUT ]"sv, uiHSTR );
				#else
					*pBitList << CArcBase::formatString( "Status: 0x%X [ READOUT ]", uiHSTR );
				#endif
			}

			else if ( eHSTR == ePCIStatus::BUSY_STATUS )
			{
				#ifdef __cpp_lib_format
					*pBitList << std::format( "Status: 0x{:X} [ BUSY ]"sv, uiHSTR );
				#else
					*pBitList << CArcBase::formatString( "Status: 0x%X [ BUSY ]", uiHSTR );
				#endif
			}

			else if ( eHSTR == ePCIStatus::TIMEOUT_STATUS )
			{
				#ifdef __cpp_lib_format
					*pBitList << std::format( "Status: 0x{:X} [ IDLE / TIMEOUT ]"sv, uiHSTR );
				#else
					*pBitList << CArcBase::formatString( "Status: 0x%X [ IDLE / TIMEOUT ]", uiHSTR );
				#endif
			}

			else
			{
				#ifdef __cpp_lib_format
					*pBitList << std::format( "Status: 0x{:X} [ UNKNOWN ]"sv, uiHSTR );
				#else
					*pBitList << CArcBase::formatString( "Status: 0x%X [ UNKNOWN ]", uiHSTR );
				#endif
			}

			return pBitList;
		}

	}	// end gen3 namespace
}	// end arc namespace