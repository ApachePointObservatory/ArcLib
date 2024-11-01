// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcPCIe.h                                                                                               |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the ARC-66 PCIe device class.                                                        |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file CArcPCIe.h */

#pragma once

#ifdef _WINDOWS
	#pragma warning( disable: 4251 )
#endif

#include <string_view>
#include <filesystem>
#include <charconv>
#include <string>
#include <vector>
#include <memory>
#include <list>

#include <CArcDeviceDllMain.h>
#include <CArcBase.h>
#include <CArcPCIBase.h>
#include <ArcDefs.h>


namespace arc
{
	namespace gen3
	{

		namespace device
		{

			/** @enum arc::gen3::device::PCIeRegs
			 *  Convenience names for base addr registers (BAR)
			 *  @var arc::gen3::device::PCIeRegs::LCL_CFG_BAR
			 *  Local configuration registers
			 *  @var arc::gen3::device::PCIeRegs::DEV_REG_BAR
			 *  Device registers
			 */
			typedef enum class PCIeRegs : std::uint32_t
			{
				LCL_CFG_BAR = 0x00,
				DEV_REG_BAR = 0x02
			} ePCIeRegs;


			/** @enum arc::gen3::device::PCIeRegOffsets
			 *  Device register offsets (BAR2)
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CMD_HEADER
			 *  Register to write the command header into
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CMD_COMMAND
			 *  Register to write the command into
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CMD_ARG0
			 *  Register to write the command argument 0 into
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CMD_ARG1
			 *  Register to write the command argument 1 into
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CMD_ARG2
			 *  Register to write the command argument 2 into
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CMD_ARG3
			 *  Register to write the command argument 3 into
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CMD_ARG4
			 *  Register to write the command argument 4 into
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CTLR_SPECIAL_CMD
			 *  Special command register to read the controller id or reset the controller
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_RESET
			 *  Reset the PCIe board. All registers will be cleared to their initial state.
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_INIT_IMG_ADDR
			 *  Inform the PCIe board to reset its internal image buffer address
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_FIBER_2X_CTRL
			 *  Enables or disables dual fiber optic receivers
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_STATUS
			 *  Read the PCI board status register
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CMD_REPLY
			 *  Read the current command reply
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CMD_REPLY
			 *  Read the current command reply
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CTLR_ARG1
			 *  Read the first reply argument for the current command
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_CTLR_ARG2
			 *  Read the second reply argument for the current command
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_PIXEL_COUNT
			 *  Read the current image read pixel count
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_FRAME_COUNT
			 *  Read the current continuous read frame count
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_ID_LO
			 *  Read the lower 32-bits of the PCIe board id. Example: 0x36523141 ('6R1A')
			 *  @var arc::gen3::device::PCIeRegOffsets::REG_ID_HI
			 *  Read the upper 32-bits of the PCIe board id. Defined as: 0x41524336 ('ARC6')
			 */
			typedef enum class PCIeRegOffsets : std::uint32_t
			{
				REG_CMD_HEADER			= 0x00,
				REG_CMD_COMMAND			= 0x04,
				REG_CMD_ARG0			= 0x08,
				REG_CMD_ARG1			= 0x0C,
				REG_CMD_ARG2			= 0x10,
				REG_CMD_ARG3			= 0x14,
				REG_CMD_ARG4			= 0x18,
				REG_CTLR_SPECIAL_CMD	= 0x1C,
				REG_RESET				= 0x20,
				REG_INIT_IMG_ADDR		= 0x38,
				REG_FIBER_2X_CTRL		= 0x5C,
				REG_STATUS				= 0x60,
				REG_CMD_REPLY			= 0x64,
				REG_CTLR_ARG1			= 0x68,
				REG_CTLR_ARG2			= 0x6C,
				REG_PIXEL_COUNT			= 0x70,
				REG_FRAME_COUNT			= 0x74,
				REG_ID_LO				= 0x78,
				REG_ID_HI				= 0x7C
			} ePCIeRegOffsets;


			 /** @enum arc::gen3::device::RegCmds
			  *  Special command Register Commands
			  *  @var arc::gen3::device::RegCmds::CONTROLLER_GET_ID
			  *  Read the controller id
			  *  @var arc::gen3::device::PCIeRegs::CONTROLLER_RESET
			  *  Reset the controller
			  */
			typedef enum class RegCmds : std::uint32_t
			{
				CONTROLLER_GET_ID = 0x09,
				CONTROLLER_RESET  = 0x0B
			} eRegCmds;


			/** @enum arc::gen3::device::Fiber2x
			 *  Fiber Optic 2x Selector
			 *  @var arc::gen3::device::RegCmds::FIBER_2X_DISABLE
			 *  Disable the use of dual fiber optic receivers
			 *  @var arc::gen3::device::PCIeRegs::FIBER_2X_ENABLE
			 *  Enable the use of dual fiber optic receivers
			 */
			typedef enum class Fiber2x : std::uint32_t
			{
				FIBER_2X_DISABLE,
				FIBER_2X_ENABLE
			} eFiber2x;


			/**  Fiber Optic Selector
			 */

			/** @enum arc::gen3::device::Fiber
			 *  Fiber optic selector
			 *  @var arc::gen3::device::RegCmds::FIBER_A
			 *  Select receive fiber optic A
			 *  @var arc::gen3::device::PCIeRegs::FIBER_B
			 *  Select receive fiber optic B
			 */
			typedef enum class Fiber : std::uint32_t
			{
				FIBER_A,
				FIBER_B
			} eFiber;

		}	// end device namespace


		/** @class CArcPCIe
		 *
		 *  ARC-66 PCIe device class
		 *
		 *  @see arc::gen3::CArcPCIe
		 */
		class GEN3_CARCDEVICE_API CArcPCIe : public CArcPCIBase
		{
			public:

				/** Default constructor
				 */
				CArcPCIe( void );

				/** Default destructor
				 */
				virtual ~CArcPCIe( void );

				/** Returns a textual representation of the class.
				 *  @return A string representation of the class.
				 */
				constexpr std::string_view toString( void );

				//  PCI(e) configuration space access
				// +-------------------------------------------------+

				/** Returns a configuration space byte (8-bit) value
				 *  @param uiOffset - The PCIe configuration space byte offset.
				 *  @return The PCIe configuration space byte value read from offset address.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCfgSpByte( const std::uint32_t uiOffset );
				
				/** Returns a configuration space word (16-bit) value
				 *  @param uiOffset - The PCIe configuration space word offset.
				 *  @return The PCIe configuration space word value read from offset address.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCfgSpWord( const std::uint32_t uiOffset );

				/** Returns a configuration space dword (32-bit) value
				 *  @param uiOffset - The PCIe configuration space dword offset.
				 *  @return The PCIe configuration space dword value read from offset address.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCfgSpDWord( const std::uint32_t uiOffset );

				/** Set a configuration space byte (8-bit) value
				 *  @param uiOffset - The PCIe configuration space byte offset.
				 *  @param uiValue  - The byte (8-bit) value to write.
				 *  @throws std::runtime_error
				 */
				void setCfgSpByte( const std::uint32_t uiOffset, const std::uint32_t uiValue );

				/** Set a configuration space word (16-bit) value
				 *  @param uiOffset - The PCIe configuration space word offset.
				 *  @param uiValue  - The word (16-bit) value to write.
				 *  @throws std::runtime_error
				 */
				void setCfgSpWord( const std::uint32_t uiOffset, const std::uint32_t uiValue );
				
				/** Set a configuration space dword (32-bit) value
				 *  @param uiOffset - The PCIe configuration space dword offset.
				 *  @param uiValue  - The dword (32-bit) value to write.
				 *  @throws std::runtime_error
				 */
				void setCfgSpDWord( const std::uint32_t uiOffset, const std::uint32_t uiValue );

				/** Reads and parses the entire PCIe configuration space header into readable text and bit definitions 
				 *  that are stored in a member list variable. The public methods of this class allow access to this list. 
				 *  This method will create the member list if it doesn't already exist and clears it if it does.
				 *  @throws std::runtime_error
				 */
				void getCfgSp( void );
				
				/** Reads and parses the entire PCIe Base Address Registers (BAR) into readable text and bit definitions 
				 *  that are stored in a member list variable. The public methods of this class allow access to this list.
				 *  This method will create the member list if it doesn't already exist and clears it if it does.
				 *  NOTE: Not all BARS or PCIe boards have data.
				 *  @throws std::runtime_error
				 */
				void getBarSp( void );


				//  Device access
				// +-------------------------------------------------+

				/** Searches for available ARC-66 PCIe devices and stores the list.
				 *  @throws std::runtime_error
				 */
				static void findDevices( void );

				/** Returns the number of items in the PCIe device list. Must be called after findDevices().
				 *  @return The number of ARC-66 PCIe boards found in the system.
				 */
				static std::uint32_t deviceCount( void ) noexcept;

				/** Returns a text representation of the ARC-66 PCIe device list.
				 *  @return A list of device strings
				 */
				static const std::weak_ptr<std::string[]> getDeviceStringList( void ) noexcept;

				/** Returns whether or not an ARC-66 PCIe device has been opened ( i.e. if the open() method has been called ).
				 *  @return <i>true</i> if the open() method has been successfully called; <i>false</i> otherwise.
				 */
				bool isOpen( void ) noexcept;
				
				/** Open a connection to the specified ARC-66 PCIe device. This method does not map the kernel image buffer into user space.
				 *  @param uiDeviceNumber - The device driver number 0 to N ( default = 0 ). Use 0 for a single ARC PCIe board.
				 *  @throws std::runtime_error
				 */
				void open( const std::uint32_t uiDeviceNumber = 0 );
				
				/** Open a connection to the specified ARC-66 PCIe device. This method maps all or part of the kernel image buffer into user space.
				 *  @param uiDeviceNumber - The device driver number 0 to N ( default = 0 ). Use 0 for a single ARC PCIe board.
				 *  @param uiBytes        - The number of bytes to map from the kernel image buffer. This value must not exceed the actual size of
				 *                          the buffer, which is 4200x4200x2 bytes by default.
				 *  @throws std::runtime_error
				 */
				void open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiBytes );
				
				/** Open a connection to the specified ARC-66 PCIe device. This method maps all or part of the kernel image buffer into user space.
				 *  @param uiDeviceNumber - The device driver number 0 to N ( default = 0 ). Use 0 for a single ARC PCIe board.
				 *  @param uiRows         - The number of rows to map from the kernel image buffer. This value must not exceed the default row size of
				 *                          the default kernel buffer row size of 4200 pixels.
				 *  @param uiCols         - The number of columns to map from the kernel image buffer. This value must not exceed the default column size
				 *                          of the default kernel buffer column size of 4200 pixels.
				 *  @throws std::runtime_error
				 */
				void open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiRows, const std::uint32_t uiCols );
				
				/** Closes the ARC-66 PCIe device connection
				 */
				void close( void );

				/** Reset the ARC-66 PCIe device
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

				/** Returns the PCIe board id.
				 *  @return Returns a unique identifier for the PCIe board.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getId( void );
				
				/** Returns the PCIe board status. The content and meaning of the returned value depends upon
				 *  which board is being used.
				 *  @return The PCIe board status
				 *  @throws std::runtime_error
				 */
				std::uint32_t getStatus( void );
				
				/** Clears the PCIe board status register.
				 *  @throws std::runtime_error
				 */
				void clearStatus( void );

				/** Enables or disables the second fiber optic transmitter on supporting timing boards.
				 *  @param bOnOff - <i>true</i> to tnable the second transmitter; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				void set2xFOTransmitter( bool bOnOff );

				/** Load a PCIe DSP (.lod) firmware file.
				 *  @param tFile - A valid PCIe DSP (.lod) firmware file.
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

				/** Returns whether or not the specified fiber optic cable is connected correctly.
				 *  @param eFiberId - One of arc::gen3::device::eFiber::FIBER_A or FIBER_B (default = FIBER_A).
				 *  @return <i>true</i> if the specified fiber is connected; <i>false</i> otherwise.
				 */
				bool isFiberConnected( arc::gen3::device::eFiber eFiberId = arc::gen3::device::eFiber::FIBER_A );

				//  Expose commands
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

				//  PCIe only methods
				// +-------------------------------------------------+

				/** Write to a PCIe base address register.
				 *  @param eBar		- One of the PCIe base address registers. Should typically be DEV_REG_BAR.
				 *  @param uiOffset	- The offset address into the base address register.
				 *  @param uiValue	- The value to write.
				 *  @throws std::runtime_error
				 */
				void writeBar( arc::gen3::device::ePCIeRegs eBar, const std::uint32_t uiOffset, const std::uint32_t uiValue );

				/** Read a PCIe base address register.
				 *  @param eBar		- One of the PCIe base address register. Should typically be DEV_REG_BAR.
				 *  @param uiOffset	- The offset address into the base address register.
				 *  @return The value read
				 *  throws std::runtime_error
				 */
				std::uint32_t readBar( arc::gen3::device::ePCIeRegs eBar, const std::uint32_t uiOffset );

				/** Read a controller command reply.
				 *  @param fTimeOutSecs - The number of seconds to wait for a reply from the controller before timing out (default = 1.5 seconds).
				 *  @return The command reply.
				 *  @throws std::runtime_error
				 */
				std::uint32_t readReply( double fTimeOutSecs = 1.5 );

				//  PCIe Board ID Constant
				// +-------------------------------------------------+

				/** ARC-66 PCIe board ascii identifier ('ARC6') */
				static constexpr auto ID				= static_cast< std::uint32_t >( 0x41524336 );

				//  Driver ioctl commands                                                      
				// +-----------------------------------------------------------------------------+

				/** Read PCIe base address register */
				static constexpr auto ARC_READ_BAR		= static_cast< std::uint32_t >( 0x01 );

				/** Write PCIe base address register */
				static constexpr auto ARC_WRITE_BAR		= static_cast< std::uint32_t >( 0x02 );
	
				/** Get PCIe base address register size */
				static constexpr auto ARC_BAR_SIZE		= static_cast< std::uint32_t >( 0x03 );

				/** Read 8-bits of PCIe config space */
				static constexpr auto ARC_READ_CFG_8	= static_cast< std::uint32_t >( 0x04 );
				
				/** Read 16-bits of PCIe config space */
				static constexpr auto ARC_READ_CFG_16	= static_cast< std::uint32_t >( 0x05 );
				
				/** Read 32-bits of PCIe config space */
				static constexpr auto ARC_READ_CFG_32	= static_cast< std::uint32_t >( 0x06 );

				/** Write 8-bits to PCIe config space */
				static constexpr auto ARC_WRITE_CFG_8	= static_cast< std::uint32_t >( 0x07 );
				
				/** Write 16-bits to PCIe config space */
				static constexpr auto ARC_WRITE_CFG_16	= static_cast< std::uint32_t >( 0x08 );
				
				/** Write 32-bits to PCIe config space */
				static constexpr auto ARC_WRITE_CFG_32	= static_cast< std::uint32_t >( 0x09 );

				/** Get common buffer properties */
				static constexpr auto ARC_BUFFER_PROP	= static_cast< std::uint32_t >( 0x0A );

				/** Maps the kernel image buffer */
				static constexpr auto ARC_MEM_MAP		= static_cast< std::uint32_t >( 0x0C );

				/** Un-maps the kernel image buffer */
				static constexpr auto ARC_MEM_UNMAP		= static_cast< std::uint32_t >( 0x0D );

				// +----------------------------------------------------------------------------
				// |  Status register definitions and functions
				// +----------------------------------------------------------------------------

				/** Clear the PCIe board status register */
				static constexpr auto PCIe_STATUS_CLEAR_ALL = static_cast< std::uint32_t >( 0x7F );

				/** Function that returns whether or not a controller command reply has been received.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that a controller command reply has been received; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusReplyRecvd;

				/** Function that returns whether or not the controller has been reset.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that a controller has been reset; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusControllerReset;

				/** Function that returns whether or not the controller is currently reading out image data.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that the controller is currently reading out image data; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusReadout;

				/** Function that returns whether or not the controller is currently idle.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that the controller is currently idle; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusIdle;

				/** Function that returns whether or not a command was successfully sent.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that the last command was successfully sent; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusCmdSent;

				/** Function that returns whether or not the PCIe timed-out waiting for image data from the controller.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that the PCIe time-out waiting for image data; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusImageReadTimeOut;

				/** Function that returns whether or not an incorrect command/reply header was received.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that an incorrect header was received; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusHeaderError;

				/** Function that returns whether or not dual receive fibers are enabled.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that dual receive fibers are enabled; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusFiber2xReceiver;

				/** Function that returns whether or not PCIe receive fiber A is properly connected to a powered-on controller.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that the PCIe receive fiber A is properly connected; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusFiberAConnected;

				/** Function that returns whether or not PCIe receive fiber B is properly connected to a powered-on controller.
				 *  @param - The status register value
				 *  @return <i>true</i> if the status register indicates that the PCIe receive fiber B is properly connected; <i>false</i> otherwise.
				 */
				std::function<bool( const std::uint32_t )> fnPCIeStatusFiberBConnected;

			protected:

				/** Minimum PCIe base address register address */
				static constexpr auto ARC_MIN_BAR = static_cast< std::uint32_t >( 0 );
				
				/** Maximum PCIe base address register address */
				static constexpr auto ARC_MAX_BAR = static_cast< std::uint32_t >( 5 );

				/**
				 * Returns the boundary adjusted image size for continuous readout. The PCIe card ( ARC-66/67 ) requires no boundary adjustments 
				 * and writes data continuously.
				 * @param uiImageSize - The non-adjusted image size ( in bytes ).
				 * @return The current image size (in bytes) with no boundary adjustments.
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

				/** Reads and parses the entire PLX PCIe local configuration registers located within base address register 0 (BAR 0).
				 *  @throws std::runtime_error
				 */
				void getLocalConfiguration( void );

				/** Returns whether or not the PCIe device/vendor id value matches the expected id for the PCIe board.
				 *  @param parameter - The 16-bit device/vendor id as read from the PCIe board configurations space registers.
				 *  @return 1 if the id's match; 0 if they don't
				 */
				std::function<std::uint32_t( const std::uint16_t )>	m_fnValidDeviceVendorId;

				/** Checks whether or not the specified value is contained within 24-bits. An exception is thrown if the specified data 
				 *  value is greater than 24-bits in length.
				 *  @param parameter - The value to check
				 *  @throws std::runtime_error
				 */
				std::function<void( const std::uint32_t )>	m_fnVerify24Bits;

				/** PCIe device list pointer */
				static std::unique_ptr<std::vector<arc::gen3::arcDev_t>> m_pvDevList;

				/** PCIe device string list pointer */
				static std::shared_ptr<std::string[]> m_psDevList;
		};

	}	// end gen3 namespace
}	// end arc namespace
