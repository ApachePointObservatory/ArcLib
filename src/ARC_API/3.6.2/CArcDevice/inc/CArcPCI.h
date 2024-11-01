// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcPCI.h                                                                                                |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the ARC-64 PCI device class.                                                         |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file CArcPCI.h */

#pragma once

#ifdef _WINDOWS
#pragma warning( disable: 4251 )
#endif

#include <string_view>
#include <filesystem>
#include <vector>
#include <string>
#include <memory>

#include <CArcDeviceDllMain.h>
#include <CArcPCIBase.h>
#include <CArcStringList.h>
#include <ArcDefs.h>

//using namespace std::string_literals;


namespace arc
{
	namespace gen3
	{

		/** @class CArcPCI
		 *
		 *  ARC-64 PCI device class
		 *
		 *  @see arc::gen3::CArcPCI
		 */
		class GEN3_CARCDEVICE_API CArcPCI : public CArcPCIBase
		{
			public:

				/** Default constructor
				 */
				CArcPCI( void );
				
				/** Default destructor
				 */
				 ~CArcPCI( void );

				 /** Returns a textual representation of the class.
				  *  @return A string representation of the class.
				  */
				constexpr std::string_view toString( void );

				//  CArcPCIBase methods
				// +-------------------------------------------------+

				/** Returns a configuration space byte (8-bit) value
				 *  @param uiOffset - The PCI configuration space byte offset.
				 *  @return The PCI configuration space byte value read from offset address.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCfgSpByte( const std::uint32_t uiOffset );

				/** Returns a configuration space word (16-bit) value
				 *  @param uiOffset - The PCI configuration space word offset.
				 *  @return The PCI configuration space word value read from offset address.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCfgSpWord( const std::uint32_t uiOffset );

				/** Returns a configuration space dword (32-bit) value
				 *  @param uiOffset - The PCI configuration space dword offset.
				 *  @return The PCI configuration space dword value read from offset address.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCfgSpDWord( const std::uint32_t uiOffset );

				/** Set a configuration space byte (8-bit) value
				 *  @param uiOffset - The PCI configuration space byte offset.
				 *  @param uiValue  - The byte (8-bit) value to write.
				 *  @throws std::runtime_error
				 */
				void setCfgSpByte( const std::uint32_t uiOffset, const std::uint32_t uiValue );

				/** Set a configuration space word (16-bit) value
				 *  @param uiOffset - The PCI configuration space word offset.
				 *  @param uiValue  - The word (16-bit) value to write.
				 *  @throws std::runtime_error
				 */
				void setCfgSpWord( const std::uint32_t uiOffset, const std::uint32_t uiValue );

				/** Set a configuration space dword (32-bit) value
				 *  @param uiOffset - The PCI configuration space dword offset.
				 *  @param uiValue  - The dword (32-bit) value to write.
				 *  @throws std::runtime_error
				 */
				void setCfgSpDWord( const std::uint32_t uiOffset, const std::uint32_t uiValue );

				/** Reads and parses the entire PCI configuration space header into readable text and bit definitions
				 *  that are stored in a member list variable. The public methods of this class allow access to this list.
				 *  This method will create the member list if it doesn't already exist and clears it if it does.
				 *  @throws std::runtime_error
				 */
				void getCfgSp( void );

				/** Reads and parses the entire PCI Base Address Registers (BAR) into readable text and bit definitions
				 *  that are stored in a member list variable. The public methods of this class allow access to this list.
				 *  This method will create the member list if it doesn't already exist and clears it if it does.
				 *  NOTE: Not all BARS or PCI boards have data.
				 *  @throws std::runtime_error
				 */
				void getBarSp( void );


				//  Device access
				// +-------------------------------------------------+

				/** Searches for available ARC-64 PCI devices and stores the list.
				 *  @throws std::runtime_error
				 */
				static void findDevices( void );
	
				/** Returns the number of items in the PCI device list. Must be called after findDevices().
				 *  @return The number of ARC-64 PCI boards found in the system.
				 */
				static std::uint32_t  deviceCount( void ) noexcept;
	
				/** Returns a text representation of the ARC-64 PCI device list.
				 *  @return A list of device strings
				 *  @throws std::runtime_error
				 */
				static const std::weak_ptr<std::string[]> getDeviceStringList( void );

				/** Returns whether or not an ARC-64 PCI device has been opened ( i.e. if the open() method has been called ).
				 *  @return <i>true</i> if the open() method has been successfully called; <i>false</i> otherwise.
				 */
				bool isOpen( void ) noexcept;

				/** Open a connection to the specified ARC-64 PCI device. This method does not map the kernel image buffer into user space.
				 *  @param uiDeviceNumber - The device driver number 0 to N ( default = 0 ). Use 0 for a single ARC PCI board.
				 *  @throws std::runtime_error
				 */
				void open( const std::uint32_t uiDeviceNumber = 0 );

				/** Open a connection to the specified ARC-64 PCI device. This method maps all or part of the kernel image buffer into user space.
				 *  @param uiDeviceNumber - The device driver number 0 to N ( default = 0 ). Use 0 for a single ARC PCI board.
				 *  @param uiBytes        - The number of bytes to map from the kernel image buffer. This value must not exceed the actual size of
				 *                          the buffer, which is 4200x4200x2 bytes by default.
				 *  @throws std::runtime_error
				 */
				void open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiBytes );

				/** Open a connection to the specified ARC-64 PCI device. This method maps all or part of the kernel image buffer into user space.
				 *  @param uiDeviceNumber - The device driver number 0 to N ( default = 0 ). Use 0 for a single ARC PCI board.
				 *  @param uiRows         - The number of rows to map from the kernel image buffer. This value must not exceed the default row size of
				 *                          the default kernel buffer row size of 4200 pixels.
				 *  @param uiCols         - The number of columns to map from the kernel image buffer. This value must not exceed the default column size
				 *                          of the default kernel buffer column size of 4200 pixels.
				 *  @throws std::runtime_error
				 */
				void open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiRows, const std::uint32_t uiCols );

				/** Closes the ARC-64 PCI device connection
				 */
				void close( void );

				/** Reset the ARC-64 PCI device
				 *  @throws std::runtime_error
				 */
				void reset( void );

				/** Internally fills the kernel image buffer physical address and size from the device driver.
				 *  @return <i>true</i> if the properties were successfully read from the driver; <i>false</i> on error.
				 *  @throws std::runtime_error
				 */
				bool getCommonBufferProperties( void );

				/** Maps the specified number of bytes from the kernel image buffer into user space.
				 *  uiBytes - The number of bytes to map from the kernel image buffer. This value must not exceed the actual size of
				 *            the buffer, which is 4200x4200x2 bytes by default (default = 0).
				 *  @throws std::runtime_error
				 */
				void mapCommonBuffer( std::size_t uiBytes = 0 );

				 /** Un-maps the kernel image buffer from user space. The buffer should not be accessed once it's un-mapped.
				  *  @throws std::invalid_argument
				  */
				void unMapCommonBuffer( void );

				/** Returns zero, no id is used by the PCI board.
				 *  @return 0
				 */
				std::uint32_t getId( void );

				/** Returns the PCI board status register value, which are read from the PCI board HTF ( Host Transfer Flags ) bits in the HSTR register.
				 *  @return The PCI board status
				 *  @throws std::runtime_error
				 */
				std::uint32_t getStatus( void );
				
				/** Does nothing, not supported by the PCI board.
				 */
				void clearStatus( void );

				/** Enables or disables the second fiber optic transmitter on supporting timing boards.
				 *  @param bOnOff - <i>true</i> to tnable the second transmitter; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				void set2xFOTransmitter( bool bOnOff );

				/** Load a PCI DSP (.lod) firmware file.
				 *  @param tFile - A valid PCI DSP (.lod) firmware file.
				 *  @throws std::runtime_error
				 */
				void loadDeviceFile( const std::filesystem::path& tFile );
				
				//  Setup & General commands
				// +-------------------------------------------------+

				/** Send a command to the controller.
				 *  @param tCmdList - The command and any arguments to be sent. The board id should be the first value, followed by
				 *                    the command and any arguments. e.g. boardId cmd arg0 arg1 etc. There should be no more than five arguments.
				 *  @return The controller status reply. Will return the ascii value 'DON' on success, 'ERR' on error, or 'TOUT' on
				 *          timeout. The timeout value derives from the device driver, while done and error derive from the controller.
				 *  @throws std::runtime_error
				 */
				std::uint32_t command( const std::initializer_list<const std::uint32_t>& tCmdList );

				/** Returns the controller id. The returned value will identify the controller as GenII, GenIII or SmallCam.
				 *  @return Returns ascii 'SC0' for SmallCam or the controller id or ascii 'ERR' if no id exists.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getControllerId( void );

				/** Resets the controller by disabling power to the boards and reloading the DSP boot code from ROM.
				 *  @throws std::runtime_error
				 */
				void resetController( void );

				/** Returns whether or not a controller is connected and powered-on.
				 *  @return <i>true</i> if a controller is connected; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				bool isControllerConnected( void );


				//  expose commands
				// +-------------------------------------------------+

				/** Attempts to stop the current exposure
				 *  @throws std::runtime_error
				 */
				void stopExposure( void );

				/** Returns whether or not image readout is in progress.
				 *  @return <i>true</i> if the controller is currently reading out image pixels; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				bool isReadout( void );
				
				/** Returns the current pixel count during image aquisition. Note: this method will log the value if logging is enabled.
				 *  @return The current pixel count
				 *  @throws std::runtime_error
				 */
				std::uint32_t getPixelCount( void );

				/** Returns the cumulative pixel count across all frames read while in continuous readout mode.
				 *  @return The cumulative continuous readout pixel count.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCRPixelCount( void );

				/** Returns the current continuous readout frame count. The controller must be in continuous readout mode.
				 *  Note: this method will log the value if logging is enabled.
				 *  @return The current frame count.
				 */
				std::uint32_t getFrameCount( void );


				//  PCI only commands
				// +-------------------------------------------------+

				/** Set the current value of the PCI board DSP host control register (HCTR).
				 *  @param uiVal - The value to set on the PCI board HCTR register.
				 *                 bits  8-9 = 10 to enable 24-bit commands
				 *				   bits 11-12 = 10 to enable 32-bit command replies
				 *  @throws std::runtime_error
				 */
				void setHCTR( const std::uint32_t uiVal );
	
				/** Get the current value of the PCI board DSP host status register (HSTR).
				 *  @return The current HSTR value.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getHSTR( void );

				/** Get the current value of the PCI board DSP host control register (HCTR).
				 *  @return The current HCTR value.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getHCTR( void );

				/** Send a command to the PCI board
				 *  @param uiCommand - The PCI command
				 *  @return The command reply value. 'DON' on success; 'ERR' on error
				 *  @throw std::runtime_error
				 */
				std::uint32_t PCICommand( const std::uint32_t uiCommand );


				//  Driver ioctl commands **GMacD CHANGED auto to std::uint32_t for SWIG
				// +------------------------------------------------------------------------------

				/** Read the host control register (HSTR) */
				static constexpr std::uint32_t ASTROPCI_GET_HCTR		= static_cast< std::uint32_t >( 0x01 );

				/** Read the pixel count for the current image readout */
				static constexpr std::uint32_t ASTROPCI_GET_PROGRESS		= static_cast< std::uint32_t >( 0x02 );

				/** Read the kernel image buffer physical address */
				static constexpr std::uint32_t ASTROPCI_GET_DMA_ADDR		= static_cast< std::uint32_t >( 0x03 );

				/** Read the host status register (HSTR) */
				static constexpr std::uint32_t ASTROPCI_GET_HSTR		= static_cast< std::uint32_t >( 0x04 );

				/** Memory map the kernel image buffer into the user space for access by this library */
				static constexpr std::uint32_t ASTROPCI_MEM_MAP			= static_cast< std::uint32_t >( 0x05 );

				/** Read the kernel image buffer size (in bytes). This is the actual size; not the mapped size, which may be less */
				static constexpr std::uint32_t ASTROPCI_GET_DMA_SIZE		= static_cast< std::uint32_t >( 0x06 );

				/** Read the number of image frames during continuous readout */
				static constexpr std::uint32_t ASTROPCI_GET_FRAMES_READ		= static_cast< std::uint32_t >( 0x07 );

				/** Read or write to the host command vector data register. The command data is written to this resiter and 
				 *  the command reply data is in turn read from this register. */
				static constexpr std::uint32_t ASTROPCI_HCVR_DATA		= static_cast< std::uint32_t >( 0x10 );

				/** Write to the host control register (HCTR) */
				static constexpr std::uint32_t ASTROPCI_SET_HCTR		= static_cast< std::uint32_t >( 0x11 );

				/** Write to the host command vector register */
				static constexpr std::uint32_t ASTROPCI_SET_HCVR		= static_cast< std::uint32_t >( 0x12 );

				/** Inform the DSP that new PCI boot code will be downloaded */
				static constexpr std::uint32_t ASTROPCI_PCI_DOWNLOAD		= static_cast< std::uint32_t >( 0x13 );

				/** Waits for the PCI board to finish booting after a download. Returns 'DON' when finished; 'BSY' while busy. */
				static constexpr std::uint32_t ASTROPCI_PCI_DOWNLOAD_WAIT	= static_cast< std::uint32_t >( 0x14 );

				/** Inform the PCI board to send the command data written to the HCVR_DATA register to the controller. Waits for the command reply. */
				static constexpr std::uint32_t ASTROPCI_COMMAND			= static_cast< std::uint32_t >( 0x15 );

				/** Un-map the kernel image buffer from user space */
				static constexpr std::uint32_t ASTROPCI_MEM_UNMAP		= static_cast< std::uint32_t >( 0x16 );

				/** Unused */
				static constexpr std::uint32_t ASTROPCI_ABORT			= static_cast< std::uint32_t >( 0x17 );

				/** Unused */
				static constexpr std::uint32_t ASTROPCI_CONTROLLER_DOWNLOAD	= static_cast< std::uint32_t >( 0x19 );

				/** Read the current pixel count during continuous readout */
				static constexpr std::uint32_t ASTROPCI_GET_CR_PROGRESS		= static_cast< std::uint32_t >( 0x20 );

				/** Read the lower 32-bits of the kernel image buffer physical address */
				static constexpr std::uint32_t ASTROPCI_GET_DMA_LO_ADDR		= static_cast< std::uint32_t >( 0x21 );

				/** Read the upper 32-bits of the kernel image buffer physical address */
				static constexpr std::uint32_t ASTROPCI_GET_DMA_HI_ADDR		= static_cast< std::uint32_t >( 0x22 );

				/** Read a byte (8-bits) of data from a PCI configuration space register */
				static constexpr std::uint32_t ASTROPCI_GET_CONFIG_BYTE		= static_cast< std::uint32_t >( 0x30 );

				/** Read a word (16-bits) of data from a PCI configuration space register */
				static constexpr std::uint32_t ASTROPCI_GET_CONFIG_WORD		= static_cast< std::uint32_t >( 0x31 );

				/** Read a dword (32-bits) of data from a PCI configuration space register */
				static constexpr std::uint32_t ASTROPCI_GET_CONFIG_DWORD	= static_cast< std::uint32_t >( 0x32 );

				/** Write a byte (8-bits) of data to a PCI configuration space register */
				static constexpr std::uint32_t ASTROPCI_SET_CONFIG_BYTE		= static_cast< std::uint32_t >( 0x33 );

				/** Write a word (16-bits) of data to a PCI configuration space register */
				static constexpr std::uint32_t ASTROPCI_SET_CONFIG_WORD		= static_cast< std::uint32_t >( 0x34 );

				/** Write a dword (32-bits) of data to a PCI configuration space register */
				static constexpr std::uint32_t ASTROPCI_SET_CONFIG_DWORD	= static_cast< std::uint32_t >( 0x35 );


				//  Status register ( HSTR ) constants
				// +------------------------------------------------------------------------------

				/** Host status flag bit mask */
				static constexpr std::uint32_t HTF_BIT_MASK			= static_cast< std::uint32_t >( 0x00000038 );

				/** @enum arc::gen3::ePCIStatus
				 *  Defines the PCI status register bits
				 *  @var arc::gen3::ePCIStatus::TIMEOUT_STATUS
				 *  The current operation failed to complete in the expected time
				 *  @var arc::gen3::ePCIStatus::DONE_STATUS
				 *  The current operation completed successfully
				 *  @var arc::gen3::ePCIStatus::READ_REPLY_STATUS
				 *  The current operation resulted in data returned from the controller and is ready to read
				 *  @var arc::gen3::ePCIStatus::ERROR_STATUS
				 *  The current operation failed
				 *  @var arc::gen3::ePCIStatus::SYSTEM_RESET_STATUS
				 *  The controller was reset
				 *  @var arc::gen3::ePCIStatus::READOUT_STATUS
				 *  The controller is currently reading image data
				 *  @var arc::gen3::ePCIStatus::BUSY_STATUS
				 *  The controller or PCI board is busy with the current operation
				 */
				enum class ePCIStatus : std::uint32_t
				{
					TIMEOUT_STATUS = 0,
					DONE_STATUS,
					READ_REPLY_STATUS,
					ERROR_STATUS,
					SYSTEM_RESET_STATUS,
					READOUT_STATUS,
					BUSY_STATUS
				};


				//  PCI commands
				// +----------------------------------------------------------------------------

				/** Reset the PCI board */
				static constexpr std::uint32_t PCI_RESET			= static_cast< std::uint32_t >( 0x8077 );

				/** Attempt to cancel the current image read */
				static constexpr std::uint32_t ABORT_READOUT			= static_cast< std::uint32_t >( 0x8079 );

				/** Boot the PCI board using the firmware stored in the EEPROM */
				static constexpr std::uint32_t BOOT_EEPROM			= static_cast< std::uint32_t >( 0x807B );

				/** Read the header from the last received command reply */
				static constexpr std::uint32_t READ_HEADER			= static_cast< std::uint32_t >( 0x81 );

				/** Reset the controller */
				static constexpr std::uint32_t RESET_CONTROLLER			= static_cast< std::uint32_t >( 0x87 );

				/** Initialize the internal PCI image buffer address */
				static constexpr std::uint32_t INITIALIZE_IMAGE_ADDRESS		= static_cast< std::uint32_t >( 0x91 );

				/** Write the stored command over the fiber to the controller */
				static constexpr std::uint32_t WRITE_COMMAND			= static_cast< std::uint32_t >( 0xB1 );


				// +----------------------------------------------------------------------------
				// |  PCI File Download Constants
				// +----------------------------------------------------------------------------

				/** The PCI status register flag mask */
				static constexpr std::uint32_t HTF_MASK				= static_cast< std::uint32_t >( 0x200 );

				/** Clear the PCI status register flag bits */
				static constexpr std::uint32_t HTF_CLEAR_MASK			= static_cast< std::uint32_t >( 0xFFFFFCFF );	// Bits 8 and 9

				/** The maximum number of PCI test data links ('TDL') to perform */
				static constexpr std::uint32_t MAX_PCI_COMM_TEST		= static_cast< std::uint32_t >( 3 );

				/** The PCI test data link start value */
				static constexpr std::uint32_t PCI_COM_TEST_VALUE		= static_cast< std::uint32_t >( 0xABC123 );

			private:

				/** Returns the boundary adjusted image size for continuous readout. The PCI card ( ARC-63/64 ) requires each image 
				 *  to start on a 1024 byte boundary within the kernel image buffer.
				 * @param uiImageSize - The non-adjusted image size ( in bytes ).
				 * @return The boundary adjusted image size ( in bytes ).
				 * @throws std::runtime_error
				 */
				std::uint32_t getContinuousImageSize( const std::uint32_t uiImageSize );

				/**
				 * Sends a .lod download file data stream of up to 6 values to a SmallCam controller.
				 * @param uiBoardId  - Must be SMALLCAM_DLOAD_ID
				 * @param pvData     - Download data vector of up to 6 values
				 * @return The controller reply, which should be 'DON' on success.
				 * @throws std::runtime_error
				 */
				std::uint32_t smallCamDLoad( const std::uint32_t uiBoardId, const std::vector<std::uint32_t>* pvData );
				
				/** Loads a timing or utility file (.lod) into a GenII or GenIII controller.
				 *  @param tFilename - The timing or utility board .lod file to load.
				 *  @param bValidate - Set to 1 if the downloaded data should be read back and verified after every write.
				 *  @param pAbort    - <i>true</i> to cancel the file load; <i>false</i> otherwise (default = nullptr ).
				 *  @throws std::runtime_error
				 *  @throws std::invalid_argument
				 *  @throws std::length_error
				 */
				void loadGen23ControllerFile( const std::filesystem::path& tFilename, bool bValidate, bool* pAbort = nullptr );

				/** Sets the hardware byte - swapping if system architecture is solaris. Otherwise, does nothing; compiles to empty function.
				 */
				void setByteSwapping( void );

				/** Formats an IOCTL command into a std::string that can be passed into a throw exception method.
				 *  @param uiCmd      - The command value
				 *  @param uiReply    - The received command reply value
				 *  @param uiArg      - Any argument that may go with the specified command
				 *  @param bGetSysErr - A system error number, used to get system message
				 *  @return The formatted string
				 *  @throws Any exception thrown by std::ostringstream
				 */
				const std::string formatPCICommand( const std::uint32_t uiCmd, const std::uint64_t uiReply, const std::uint32_t uiArg = CArcDevice::NOPARAM, bool bGetSysErr = false );
				
				/** Formats an IOCTL command into a std::string that can be passed into a throw exception method.
				 *  @param uiCmd      - The command value
				 *  @param uiReply    - The received command reply value
				 *  @param tArgList   - The list of arguments that may go with the specified command
				 *  @param bGetSysErr - A system error number, used to get system message
				 *  @return The formatted string
				 *  @throws Any exception thrown by std::ostringstream
				 */
				const std::string formatPCICommand( const std::uint32_t uiCmd, const std::uint64_t uiReply, const std::initializer_list<const std::uint32_t>& tArgList, bool bGetSysErr = false );

				/** Sets the bit list strings for the PCI DSP HSTR register.
				 *  @param uiData			- The PCI cfg sp CLASS CODE and REV ID register value.
				 *  @param bDrawSeparator	- 'true' to include a line separator within the bit list strings (default = false).
				 *  @throws Any exception thrown by std::format()
				 */
				std::unique_ptr<CArcStringList> getHSTRBitList( const std::uint32_t uiData, bool bDrawSeparator = false );

				/** Returns whether or not the PCI device/vendor id value matches the expected id for the PCI board.
				 *  @param parameter - The 16-bit device/vendor id as read from the PCI board configurations space registers.
				 *  @return 1 if the id's match; 0 if they don't
				 */
				std::function<std::uint32_t( const std::uint16_t )>	m_fnValidDeviceVendorId;

				/** PCIe device list pointer */
				static std::unique_ptr<std::vector<arc::gen3::arcDev_t>> m_pvDevList;

				/** PCIe device string list pointer */
				static std::shared_ptr<std::string[]> m_psDevList;
		};

	}	// end gen3 namespace
}	// end arc namespace
