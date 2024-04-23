// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcSystem.h                                                                                             |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the standard ARC operating system class and definitions.                             |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file CArcSystem.h */

#pragma once


#ifdef _WINDOWS

	#include <windows.h>

#elif defined( __APPLE__ )

	#include <IOKit/IOKitLib.h>
	#include <ApplicationServices/ApplicationServices.h>
	#include <IOKit/usb/IOUSBLib.h>

#elif defined( __linux )

	#include <sys/mman.h>
	#include <sys/ioctl.h>

#endif

#include <memory>

#include <CArcDeviceDllMain.h>
#include <CArcBase.h>



namespace arc
{
	namespace gen3
	{

		// +=========================================================================================================+
		// |  WINDOWS DEFINITIONS                                                                                    |
		// +=========================================================================================================+
		#ifdef _WINDOWS

			// +-----------------------------------------------------------------------------------------------------+
			// |  Data Types                                                                                         |
			// +-----------------------------------------------------------------------------------------------------+
			using arcDevHandle_t = HANDLE;


			// +-----------------------------------------------------------------------------------------------------+
			// |  Constants                                                                                          |
			// +-----------------------------------------------------------------------------------------------------+
			constexpr auto ARC_CTRL_ID		= 33000;	// Used as part of DeviceIoCtrl
			constexpr auto MAP_FAILED		= ( uint8_t* )0;
			constexpr auto ARC_MAX_PATH		= _MAX_PATH;


		// +=========================================================================================================+
		// |  MAC OS X DEFINITIONS                                                                                   |
		// +=========================================================================================================+
		#elif defined( __APPLE__ )

			// +-----------------------------------------------------------------------------------------------------+
			// |  Data Types                                                                                         |
			// +-----------------------------------------------------------------------------------------------------+
			using arcDevHandle_t = io_connect_t;


			// +-----------------------------------------------------------------------------------------------------+
			// |  Constants                                                                                          |
			// +-----------------------------------------------------------------------------------------------------+
			constexpr auto INVALID_HANDLE_VALUE		= IO_OBJECT_NULL;
			constexpr auto MAX_IOCTL_IN_COUNT		= 4;
			constexpr auto MAX_IOCTL_OUT_COUNT		= 2;
			constexpr auto ARC_MAX_PATH				= 512;

			/** User client method dispatch selectors. MAC OS X only. */
			enum
			{
				kArcOpenUserClient,
				kArcCloseUserClient,
				kArcIOCtlUserClient,
				kArcNumberOfMethods // Must be last 
			};


			// +-----------------------------------------------------------------------------------------------------+
			// |  Macros                                                                                             |
			// +-----------------------------------------------------------------------------------------------------+
			auto constexpr MKCMD( std::uint32_t uiCmd ) { return ( 0x41524300 | uiCmd ); }


	// +=========================================================================================================+
	// |  LINUX DEFINITIONS                                                                                      |
	// +=========================================================================================================+
		#else

			// +-----------------------------------------------------------------------------------------------------+
			// |  Data Types                                                                                         |
			// +-----------------------------------------------------------------------------------------------------+
			using arcDevHandle_t = int;


			// +-----------------------------------------------------------------------------------------------------+
			// |  Constants                                                                                          |
			// +-----------------------------------------------------------------------------------------------------+
			constexpr auto INVALID_HANDLE_VALUE		= int( -1 );
			constexpr auto ARC_MAX_PATH				= 512;


			// +-----------------------------------------------------------------------------------------------------+
			// |  Macros                                                                                             |
			// +-----------------------------------------------------------------------------------------------------+
			auto constexpr MKCMD( std::uint32_t uiCmd ) { return ( 0x41524300 | uiCmd ); }

		#endif


		// +----------------------------------------------------------------------------------------------------------+
		// |  Device Entry Type                                                                                       |
		// +----------------------------------------------------------------------------------------------------------+
		/** @struct arcDev_t
		 *  Device data type. Stores system specific device information.
		 */
		typedef struct ArcDevType
		{
			std::string			sName;			/**< The Windows/Linux dependent device name */

			#ifdef __APPLE__
				io_service_t	tService;		/**< The MAC OS X device name */
			#endif
		} arcDev_t;


		// +-------------------------------------------------------------------------------------------------------+
		// |  CArcSystem Class                                                                                     |
		// +-------------------------------------------------------------------------------------------------------+

		/** @class CArcSystem
		 *
		 *  ARC operating system class.
		 *
		 *  @see arc::gen3::CArcDevice
		 */
		class GEN3_CARCDEVICE_API CArcSystem : public CArcBase
		{
			public:

				/** Opens a connection to the specified system device.
				 *  @param hDevice - A reference to a device handle storage location.
				 *  @param tDevice - A string reference to the name of the device to open.
				 *  @param uiFlags - Any device specific flags.
				 *  @return Returns <i>true</> if the device closed successfully; <i>false</i> otherwise.
				 */
				static bool open( arc::gen3::arcDevHandle_t& hDevice, const arcDev_t& tDevice, const std::uint64_t uiFlags = 0 ) noexcept;

				/** Closes a connection to the specified system device.
				 *  @param hDevice - A handle reference to the device.
				 *  @return Returns <i>true</> if the device closed successfully; <i>false</i> otherwise.
				 */
				static bool close( arc::gen3::arcDevHandle_t& hDevice ) noexcept;

				/** Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.
				 *  @param hDevice   - A handle reference to the device.
				 *  @param uiCommand - The device driver control code.
				 *  @param pIn       - A pointer to an input buffer.
				 *  @param uiInSize  - The size of the input buffer, in bytes.
				 *  @param pOut      - A pointer to an output buffer.
				 *  @param uiOutSize - The size of the output buffer, in bytes.
				 *  @return Returns <i>true</> if the operation completed successfully; <i>false</i> otherwise.
				 */
				static bool ioctl( const arcDevHandle_t hDevice, const std::uint32_t uiCommand, void* pIn, const std::uint32_t uiInSize, void* pOut = nullptr, const std::uint32_t uiOutSize = 0 ) noexcept;

				/** Map a device into memory.
				 *  @param hDevice     - A handle reference to the device.
				 *  @param iMapCommand - The device driver map code.
				 *  @param uiSize      - The length of the mapping ( which must be greater than 0 ). Not used on Windows.
				 *  @return Returns a pointer to the mapped area.
				 */
				static void* mmap( const arcDevHandle_t hDevice, const std::int32_t iMapCommand, const std::uint32_t uiSize ) noexcept;

				/** Un-map a device from memory.
				 *  @param hDevice     - A handle reference to the device.
				 *  @param iMapCommand - The device driver map code.
				 *  @param pAddress    - A pointer to the mapped area.
				 *  @param uiSize      - The length of the mapping ( which must be greater than 0 ).
				 */
				static bool munmap( const arcDevHandle_t hDevice, const std::int32_t iMapCommand, void* pAddress, const std::size_t uiSize ) noexcept;

			protected:

				/** Default constructor
				 */
				CArcSystem( void ) = default;

				/** Default destructor
				 */
				~CArcSystem( void ) = default;
		};


	}		// end gen3
}			// end arc
