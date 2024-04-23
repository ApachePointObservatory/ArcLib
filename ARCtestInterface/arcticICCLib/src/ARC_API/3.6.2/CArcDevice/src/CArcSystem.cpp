#include <CArcSystem.h>

#ifdef __linux

	#include <sys/stat.h>
	#include <unistd.h>
	#include <fcntl.h>

#endif


namespace arc
{
	namespace gen3
	{

		// +----------------------------------------------------------------------------------------------------------+
		// |  open                                                                                                    |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Opens a connection to the specified system device.                                                      |
		// |                                                                                                          |
		// |  <OUT> tDevice - A reference to a device handle storage location.                                        |
		// |  <IN>  tDevice - A reference to the arcDev_t entry.                                                      |
		// |  <IN>  uiFlags - Any device specific flags.                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		bool CArcSystem::open( arc::gen3::arcDevHandle_t& hDevice, const arcDev_t& tDevice, [[maybe_unused]] const std::uint64_t uiFlags ) noexcept
		{
			#ifdef _WINDOWS

				std::wstring wsDevice = CArcBase::convertAnsiToWide( tDevice.sName.c_str() ).c_str();

				hDevice = CreateFile( ( LPCTSTR )( wsDevice.c_str() ),
									  ( GENERIC_READ | GENERIC_WRITE ),
									  ( FILE_SHARE_READ | FILE_SHARE_WRITE ),
									  nullptr,
									  OPEN_EXISTING,
									  static_cast< DWORD >( uiFlags ),
									  nullptr );

				return ( hDevice != INVALID_HANDLE_VALUE ? true : false );

			#elif defined( __APPLE__ )

				//
				// This call will cause the user client to be instantiated. It
				// returns an io_connect_t handle that is used for all subsequent
				// calls to the user client.
				//
				kern_return_t kernResult = IOServiceOpen( *( ( io_service_t* )&tDevice.tService ), mach_task_self(), 0, &hDevice );

				if ( kernResult == KERN_SUCCESS )
				{
					kernResult = IOConnectCallScalarMethod( hDevice, kArcOpenUserClient, nullptr, 0, nullptr, nullptr );
				}

				return ( kernResult == KERN_SUCCESS ? true : false );

			#else

				hDevice = ::open( tDevice.sName.c_str(), O_RDWR );

				return ( hDevice > 0 ? true : false );

			#endif
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  close                                                                                                   |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Closes a connection to the specified system device.                                                     |
		// |                                                                                                          |
		// |  <IN> hDevice - A handle reference to the device.                                                        |
		// +----------------------------------------------------------------------------------------------------------+
		bool CArcSystem::close( arc::gen3::arcDevHandle_t& hDevice ) noexcept
		{
			#ifdef _WINDOWS

				return static_cast< bool >( CloseHandle( hDevice ) );

			#elif defined( __APPLE__ )

				kern_return_t kernResult = IOConnectCallScalarMethod( hDevice, kArcCloseUserClient, nullptr, 0, nullptr, nullptr );

				kernResult = IOServiceClose( hDevice );

				return ( kernResult == KERN_SUCCESS ? 1 : 0 );

			#else

				return ( ::close( hDevice ) < 0 ? false : true );

			#endif
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  ioctl                                                                                                   |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Sends a control code directly to a specified device driver, causing the corresponding device to         |
		// |  perform the corresponding operation.                                                                    |
		// |                                                                                                          |
		// |  <IN>  hDevice   - A handle reference to the device.                                                     |
		// |  <IN>  uiCommand - The device driver control code.                                                       |
		// |  <IN>  pIn       - A pointer to an input buffer.                                                         |
		// |  <IN>  uiInSize  - The size of the input buffer, in bytes.                                               |
		// |  <OUT> pOut      - A pointer to an output buffer.                                                        |
		// |  <OUT> uiOutSize - The size of the output buffer, in bytes.                                              |
		// +----------------------------------------------------------------------------------------------------------+
		bool CArcSystem::ioctl( const arcDevHandle_t hDevice, const std::uint32_t uiCommand, void* pIn, [[maybe_unused]] const std::uint32_t uiInSize, void* pOut, const std::uint32_t uiOutSize ) noexcept
		{
			#ifdef _WINDOWS

				DWORD dwBytesReturned = 0;

				DWORD dwCtrlCode = CTL_CODE( ARC_CTRL_ID, ( 0x800 | uiCommand ), METHOD_BUFFERED, FILE_ANY_ACCESS );

				BOOL bSuccess = DeviceIoControl( hDevice, dwCtrlCode, pIn, uiInSize, pOut, uiOutSize, &dwBytesReturned, nullptr );

				return ( bSuccess == TRUE ? true : false );

			#elif defined( __APPLE__ )

				kern_return_t kernResult = kBadArgsErr;
				uint32_t      uiInCount = ( uiInSize / sizeof( std::uint32_t ) );
				uint32_t      uiOutCount = ( uiOutSize / sizeof( std::uint32_t ) );
				uint32_t	  uiMaxOutCount = MAX_IOCTL_OUT_COUNT;

				uint64_t      u64Data[ MAX_IOCTL_IN_COUNT ];

				//
				// Zero in/out data buffers
				//
				zeroMemory( u64Data, static_cast< std::uint64_t >( MAX_IOCTL_IN_COUNT * sizeof( uint64_t ) ) );

				//
				// Initialize data with command
				//
				u64Data[ 0 ] = MKCMD( uiCommand );

				//
				// Initialize argument(s)
				//
				if ( pIn != nullptr )
				{
					std::uint32_t* pInReader = reinterpret_cast< std::uint32_t* >( pIn );

					for ( auto i = 0; i < uiInCount; i++ )
					{
						u64Data[ i + 1 ] = pInReader[ i ];
					}
				}

				kernResult = IOConnectCallScalarMethod( hDevice, kArcIOCtlUserClient, u64Data, MAX_IOCTL_IN_COUNT, u64Data, &uiMaxOutCount );

				if ( pOut != nullptr )
				{
					std::uint32_t* pOutInserter = reinterpret_cast< std::uint32_t* >( pOut );

					if ( kernResult == KERN_SUCCESS )
					{
						//
						// Set the return value(s)
						//
						for ( auto i = 0; i < uiOutCount; i++ )
						{
							pOutInserter[ i ] = std::uint32_t( u64Data[ i ] & 0xFFFFFFFF );
						}
					}

					else
					{
						*pOutInserter = 0;
					}
				}

				return ( kernResult == KERN_SUCCESS ? true : false );

			#else

				std::int32_t iSuccess = 0;

				if ( pIn != nullptr )
				{
					iSuccess = ::ioctl( hDevice, MKCMD( uiCommand ), pIn );

					if ( pOut != nullptr )
					{
						arc::gen3::CArcBase::copyMemory( pOut, pIn, uiOutSize );
					}
				}

				else if ( pOut != nullptr )
				{
					iSuccess = ::ioctl( hDevice, MKCMD( uiCommand ), pOut );
				}

				else
				{
					iSuccess = ::ioctl( hDevice, MKCMD( uiCommand ), nullptr );
				}

				return ( iSuccess < 0 ? false : true );

			#endif

		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  mmap                                                                                                    |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Map a device into memory.                                                                               |
		// |                                                                                                          |
		// |  <IN>  hDevice     - A handle reference to the device.                                                   |
		// |  <IN>  iMapCommand - The device driver map code.                                                         |
		// |  <IN>  uiSize      - The length of the mapping ( which must be greater than 0 ).                         |
		// +----------------------------------------------------------------------------------------------------------+
		void* CArcSystem::mmap( const arcDevHandle_t hDevice, [[maybe_unused]] const std::int32_t iMapCommand, [[maybe_unused]] const std::uint32_t uiSize ) noexcept
		{
			#ifdef _WINDOWS

				ULONG64 u64VirtAddr = 0;

				if ( !ioctl( hDevice, iMapCommand, nullptr, 0, &u64VirtAddr, sizeof( u64VirtAddr ) ) )
				{
					return nullptr;
				}

				return reinterpret_cast< PVOID >( u64VirtAddr );

			#elif defined( __APPLE__ )

				#if __LP64__
					mach_vm_address_t   addr;
					mach_vm_size_t      size;
				#else
					vm_address_t        addr;
					vm_size_t           size;
				#endif

				kern_return_t kernResult = IOConnectMapMemory( hDevice, kIODefaultMemoryType, mach_task_self(), &addr, &size, ( kIOMapAnywhere ) );

				if ( kernResult != kIOReturnSuccess )
				{
					addr = 0;
				}

				return reinterpret_cast< void* >( addr );

			#else

				return ::mmap( 0, uiSize, ( PROT_READ | PROT_WRITE ), MAP_SHARED, hDevice, 0 );

			#endif
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  munmap                                                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Un-map a device from memory.                                                                            |
		// |                                                                                                          |
		// |  <IN>  hDevice     - A handle reference to the device.                                                   |
		// |  <IN>  iMapCommand - The device driver map code.                                                         |
		// |  <IN>  pAddress    - A pointer to the mapped area.                                                       |
		// |  <IN>  uiSize      - The length of the mapping ( which must be greater than 0 ).                         |
		// +----------------------------------------------------------------------------------------------------------+
		bool CArcSystem::munmap( [[maybe_unused]] const arcDevHandle_t hDevice, [[maybe_unused]] const std::int32_t iMapCommand, void* pAddress, const std::size_t uiSize ) noexcept
		{
			#ifdef _WINDOWS

				return ioctl( hDevice, iMapCommand, &pAddress, sizeof( ULONG64 ) );

			#elif defined( __APPLE__ )

				#if __LP64__
					mach_vm_address_t   addr = ( mach_vm_address_t )pAddress;
				#else
					vm_address_t        addr = ( vm_address_t )pAddress;
				#endif

				return ( IOConnectUnmapMemory( hDevice, kIODefaultMemoryType, mach_task_self(), addr ) != kIOReturnSuccess ? false : true );

			#else

				return ( ::munmap( pAddress, static_cast<size_t>( uiSize ) ) == 0 ? true : false );

			#endif
		}


	}
}