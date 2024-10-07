// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcDevice.h                                                                                             |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the standard ARC device class. This is the class all user applications should use.   |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file CArcDevice.h */

#pragma once

#include <string_view>
#include <filesystem>

#include <CArcDeviceDllMain.h>
#include <CArcSystem.h>
#include <CExpIFace.h>
#include <CConIFace.h>
#include <TempCtrl.h>
#include <CArcLog.h>

#if defined( linux ) || defined( __linux )
	#include <sys/types.h>
#endif

//using namespace std::string_literals;


namespace arc
{
	namespace gen3
	{
		namespace device
		{

			/** @struct ImgBuf_t
			 *  Image buffer data type. Stores image buffer addresses and size.
			 */
			struct ImgBuf_t
			{
				std::uint16_t*	pUserAddr;			/**< Image buffer virtual address  */
				std::uint64_t	ulPhysicalAddr;		/**< Image buffer physical address */
				std::uint64_t	ulSize;				/**< Image buffer size (in bytes)  */
			};


		}	// end device namespace


		/** @class CArcDevice
		 *
		 *  ARC PCI/PCIe device class. This is the class for all controller and device driver communications.
		 *
		 *  @see arc::gen3::device::CArcDevice
		 */
		class GEN3_CARCDEVICE_API CArcDevice
		{
			public:

				/**
				 * Default constructor
				 */
				CArcDevice( void );

				/**
				 * Default destructor
				 */
				virtual ~CArcDevice( void ) = default;

				/** Returns a textual representation of the class.
				 *  @return A string representation of the class.
				 */
				virtual constexpr std::string_view toString( void ) = 0;

				//  Device access
				// +---------------------------------------------------------------+

				/** Returns whether or not an ARC PCI/PCIe device has been opened ( i.e. if the open() method has been called ).
				 *  @return <i>true</i> if the open() method has been successfully called; <i>false</i> otherwise.
				 */
				virtual bool isOpen( void ) noexcept;

				/** Open a connection to the specified ARC PCI/PCIe device. This method does not map the kernel image buffer into user space.
				 *  @param uiDeviceNumber - The device driver number 0 to N ( default = 0 ). Use 0 for a single ARC PCI/PCIe board.
				 *  @throws std::runtime_error
				 */
				virtual void open( const std::uint32_t uiDeviceNumber = 0 ) = 0;

				/** Open a connection to the specified ARC PCI/PCIe device. This method maps all or part of the kernel image buffer into user space.
				 *  @param uiDeviceNumber - The device driver number 0 to N ( default = 0 ). Use 0 for a single ARC PCI/PCIe board.
				 *  @param uiBytes        - The number of bytes to map from the kernel image buffer. This value must not exceed the actual size of 
				 *                          the buffer, which is 4200x4200x2 bytes by default.
				 *  @throws std::runtime_error
				 */
				virtual void open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiBytes ) = 0;

				/** Open a connection to the specified ARC PCI/PCIe device. This method maps all or part of the kernel image buffer into user space.
				 *  @param uiDeviceNumber - The device driver number 0 to N ( default = 0 ). Use 0 for a single ARC PCI/PCIe board.
				 *  @param uiRows         - The number of rows to map from the kernel image buffer. This value must not exceed the default row size of 
				 *                          the default kernel buffer row size of 4200 pixels.
				 *  @param uiCols         - The number of columns to map from the kernel image buffer. This value must not exceed the default column size 
				 *                          of the default kernel buffer column size of 4200 pixels.
				 *  @throws std::runtime_error
				 */
				virtual void open( const std::uint32_t uiDeviceNumber, const std::uint32_t uiRows, const std::uint32_t uiCols ) = 0;

				/** Closes the ARC PCI/PCIe device connection
				 */
				virtual void close( void ) = 0;

				/** Reset the ARC PCI/PCIe device
				 *  @throws std::runtime_error
				 */
				virtual void reset( void ) = 0;

				/** Maps the specified number of bytes from the kernel image buffer into user space.
				 *  uiBytes - The number of bytes to map from the kernel image buffer. This value must not exceed the actual size of 
				 *            the buffer, which is 4200x4200x2 bytes by default (default = 0).
				 *  @throws std::runtime_error
				 */
				virtual void mapCommonBuffer( const std::size_t uiBytes = 0 ) = 0;

				/** Un-maps the kernel image buffer from user space. The buffer should not be accessed once it's un-mapped.
				 *  @throws std::invalid_argument
				 */
				virtual void unMapCommonBuffer( void ) = 0;

				/** Re-maps the kernel image buffer into user space by first calling unMapCommonBuffer(), then calling mapCommonBuffer().
				 *  This is purely a convenience method to change the user space mapped size of the kernel image buffer.
				 *  uiBytes - The number of bytes to map from the kernel image buffer. This value must not exceed the actual size of
				 *            the buffer, which is 4200x4200x2 bytes by default (default = 0).
				 *  @see mapCommonBuffer
				 *  @see unMapCommonBuffer
				 *  @throws std::runtime_error
				 */
				virtual void reMapCommonBuffer( const std::uint32_t uiBytes = 0 );

				/** Fill the kernel image buffer with the specified value.
				 *  uwValue - The unsigned 16-bit value used to fill the image buffer (default = 0 ).
				 *  throws std::invalid-argument
				 */
				virtual void fillCommonBuffer( const std::uint16_t uwValue = 0 );

				/** Returns the kernel image buffer virtual address. The returned value can be used to access the image data.
				 *  @return A pointer to the kernel image buffer.
				 */
				virtual std::uint8_t* commonBufferVA( void );

				/** Returns the kernel image buffer physical address. The returned value should never be accessed by the user.
				 *  This value is for informational purposes only.
				 *  @return The kernel image buffer physical address.
				 */
				virtual std::uint64_t commonBufferPA( void );

				/** Returns the kernel image buffer size (in bytes).
				 *  return The kernel image buffer size (in bytes).
				 */
				virtual std::uint64_t commonBufferSize( void );

				/** Returns the PCI/PCIe board id.
				 *  @return Returns a unique identifier for the PCI/PCIe board.
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getId( void ) = 0;

				/** Returns the PCI/PCIe board status. The content and meaning of the returned value depends upon
				 *  which board is being used.
				 *  @return The PCI/PCIe board status
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getStatus( void ) = 0;

				/** Clears the PCI/PCIe board status register.
				 *  @throws std::runtime_error
				 */ 
				virtual void clearStatus( void ) = 0;

				/** Enables or disables the second fiber optic transmitter on supporting timing boards.
				 *  @param bOnOff - <i>true</i> to tnable the second transmitter; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				virtual void set2xFOTransmitter( bool bOnOff ) = 0;

				/** Load a PCI/PCIe DSP firmware file.
				 *  @param tFile - A valid PCI/PCIe DSP firmware file.
				 *  @throws std::runtime_error
				 */
				virtual void loadDeviceFile( const std::filesystem::path& tFile ) = 0;

				//  Setup & General commands
				// +---------------------------------------------------------------+

				/** Send a command to the controller.
				 *  @param tCmdList - The command and any arguments to be sent. The command should be the first value, followed by any arguments. e.g. cmd arg0 arg1 etc.
				 *  @return The controller status reply. Will return the ascii value 'DON' on success, 'ERR' on error, or 'TOUT' on 
				 *          timeout. The timeout value derives from the device driver, while done and error derive from the controller.
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t command( const std::initializer_list<const std::uint32_t>& tCmdList ) = 0;

				/** Send a command to the controller and verify the reply.
				 *  @param tCmdList			- The command and any arguments to be sent. The command should be the first value, followed by any arguments. e.g. cmd arg0 arg1 etc.
				 *  @param uiExpectedReply	- The expected controller reply
				 *  @throws std::runtime_error
				 */
				virtual void command( const std::initializer_list<const std::uint32_t>& tCmdList, std::uint32_t uiExpectedReply );

				/** Returns the controller id. The returned value will identify the controller as GenII, GenIII or SmallCam.
				 *  @return Returns ascii 'SC0' for SmallCam or the controller id or ascii 'ERR' if no id exists.
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getControllerId( void ) = 0;

				/** Resets the controller by disabling power to the boards and reloading the DSP boot code from ROM.
				 *  @throws std::runtime_error
				 */
				virtual void resetController( void ) = 0;

				/** Returns whether or not a controller is connected and powered-on.
				 *  @return <i>true</i> if a controller is connected; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				virtual bool isControllerConnected( void ) = 0;

				/** Initializes the controller, which must be attached to an ARC PCI/PCIe and powered on. An optional data link test,
				 *  which verifies that the communications link between the host computer and the controller is also available. Any test
				 *  command sent after the file load will be processed by the controller timing board on-board processor. Prior to this, the
				 *  tests will be processed by the controller timing board hardware itself. With a test data link value of zero this method
				 *  is identical to calling loadFile.
				 *  @param bReset    - <i>true</i> to reset the controller; <i>false</i> otherwise.
				 *  @param bTdl      - <i>true</i> to test the PCI/PCIe-controller data link; <i>false</i> otherwise. Which board are tested 
				 *                     depends on which board DSP files are uploaded. For example, if a timing file is uploaded, then the 
				 *                     data link to the timing board is tested.
				 *  @param bPower    - <i>true</i> to power-on the controller; <i>false</i> otherwise.
				 *  @param uiRows    - The number of image rows to be used by the controller.
				 *  @param uiCold    - The number of image columns to be used by the controller.
				 *  @param tTimFile  - The DSP firmware timing board file to upload to the controller.
				 *  @param tUtilFile - The DSP firmware utility board file to upload to the controller (optional, not used by default).
				 *  @param tPciFile  - The DSP firmware ARC-64 PCI board file to upload (optional, not used by default).
				 *  @param pAbort    - A pointer to a boolean that can be used to cancel execution (default = nullptr ).
				 *  @throws std::runtime_error
				 */
				virtual void setupController( bool bReset, bool bTdl, bool bPower, const std::uint32_t uiRows, const std::uint32_t uiCols, const std::filesystem::path& tTimFile,
											  const std::filesystem::path& tUtilFile = std::filesystem::path(), const std::filesystem::path& tPciFile = std::filesystem::path(), 
											  bool* pAbort = nullptr );

				/** Loads the specified timing, utility or smallCam firmware file onto the controller.
				 *  @param tFilename	- The DSP firmware timing file to load.
				 *  @param bValidate	- <i>truw</i> to validate each write by following it with a read and compare; <i>false</i> to not validate ( default = true ).
				 *  @param pAbort		- Pointer used to cancel execution ( default = nullptr ).
				 *  @throws std::runtime_error
				 */
				virtual void loadControllerFile( const std::filesystem::path& tFilename, bool bValidate = true, bool* pAbort = nullptr );

				/** Set the image dimensions on the controller. This method will auto re-map the image buffer if the new dimensions
				 *  exceed the exising map size but are less than the maximum buffer size.
				 *  @param uiRows	- The number of pixel rows in the image.
				 *  @param uiCols	- The number of pixel columns in the image.
				 *  @throws std::runtime_error
				 */
				virtual void setImageSize( const std::uint32_t uiRows, const std::uint32_t uiCols );

				/** Returns the image row dimension in pixels from the controller.
				 *  @return The image row size in pixels
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getImageRows( void );

				/** Returns the image column dimension in pixels from the controller.
				 *  @return The image column size in pixels
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getImageCols( void );

				/** Returns the controller configuration parameters enable bits. Each bit or set of bits represents a feature 
				 *  available in the controller. The ArcDefs.h file contains the bit definitions.
				 *  The bit definitions are as follows:
				 * 
				 *  BIT #'s		FUNCTION
				 *  -------     ------------------------------------------------
				 *  2,1,0		Video Processor
				 *  			000	ARC-41 Dual Readout CCD
				 *  			001	CCD Gen I
				 *  			010	ARC-42 Dual Readout IR
				 *  			011	ARC-44 Four Readout IR Coadder ( obsolete )
				 *  			100	ARC-45 Dual Readout CCD
				 *  			101	ARC-46 8-Channel IR
				 *  			110 ARC-48 8-Channel CCD
				 *  			111 ARC-47 4-Channel CCD
				 *  
				 *  4,3			Timing Board
				 *  			00	Rev. 4, Gen II
				 *  			01	Gen I
				 *  			10	Rev. 5, Gen III, 250 MHz
				 *  
				 *  6,5			Utility Board
				 *  			00	No utility board
				 *  			01	Utility Rev. 3
				 *  
				 *  7			Shutter
				 *  			0	No shutter support
				 *  			1	Yes shutter support
				 *  
				 *  9,8			Temperature readout
				 *  			00	No temperature readout
				 *  			01	Polynomial Diode calibration
				 *  			10	Linear temperature sensor calibration
				 *  
				 *  10			Subarray readout
				 *  			0	Not supported
				 *  			1	Yes supported
				 *  
				 *  11			Binning
				 *  			0	Not supported
				 *  			1	Yes supported
				 *  
				 *  12			Split-Serial readout
				 *  			0	Not supported
				 *  			1	Yes supported
				 *  
				 *  13			Split-Parallel readout
				 *  			0	Not supported
				 *  			1	Yes supported
				 *  
				 *  14			MPP = Inverted parallel clocks
				 *  			0	Not supported
				 *  			1	Yes supported
				 *  
				 *  16,15		Clock Driver Board
				 *  			00	Rev. 3
				 *  			11	No clock driver board (Gen I)
				 *  
				 *  19,18,17	Special implementations
				 *  			000 	Somewhere else
				 *  			001	Mount Laguna Observatory
				 *  			010	NGST Aladdin
				 *              011 2x FO Transmitter
				 *  			xxx	Other
				 * 
				 *  20			Continuous readout
				 *				0 Not supported
				 *				1 Yes supported
				 * 
				 *	21			Selectable readout speeds
				 * 
				 *  @see ArcDefs.h
				 *  @return The controller configuration parameter dword value
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getCCParams( void );

				/** Returns whether or not a specific controller parameter is supported.
				 *  @param uiParameter - A valid contoller parameter as defined in the ArcDefs.h file. For example, use SUBARRAY from the
				 *                       ArcDefs.h file to check if the controller supports the sub-array feature.
				 *  @return <i>true</i> if the specified parameter value is supported by the controller; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				virtual bool isCCParamSupported( const std::uint32_t uiParameter );

				/** Returns whether or not the controller controller contains a CCD or infrared video processor.
				 *  @return <i>true</i> if the controller contains a CCD video processor; <i>false</i> for infrared.
				 *  @throws std::runtime_error
				 */
				virtual bool isCCD( void );

				/** Returns whether or not binning is enalbed on the controller
				 *  @return <i>true</i> if binning is enabled on the controller; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				virtual bool isBinningSet( void );

				/** Enable binning on the controller by setting the binning factors. Binning causes each axis to combine 
				 *  xxxFactor number of pixels. For example, if rows = 1000, cols = 1000, rowFactor = 2 and colFactor = 4, then binRows = 500 
				 *  and binCols = 250.
				 *  @param uiRows		- The pre-binning number of rows.
				 *  @param uiCols		- The pre-binning number of columns.
				 *  @param uiRowFactor	- The row binning factor. i.e. the number of row pixels to combine.
				 *  @param uiColFactor	- The column binning factor. i.e. the number of column pixels to combine.
				 *  @param pBinRows		- An optional pointer to receive the new binned image row pixel dimension (default = nullptr).
				 *  @param pBinCols		- An optional pointer to receive the new binned image column pixel dimension (default = nullptr).
				 *  @throws std::runtime_error
				 */
				virtual void setBinning( const std::uint32_t uiRows, const std::uint32_t uiCols, const std::uint32_t uiRowFactor, const std::uint32_t uiColFactor, std::uint32_t* pBinRows = nullptr, std::uint32_t* pBinCols = nullptr );

				/** Disable binning on the controller and set the image dimensions to the specified size, which is typically
				 *  a previously saved image dimension.
				 *  @param uiRows - The new image row size
				 *  @param uiCols - The new image column size
				 *  @throws std::runtime_error
				 */
				virtual void unSetBinning( const std::uint32_t uiRows, const std::uint32_t uiCols );

				/** Sets the controller to sub-array mode, which allows the user to read a sub-section of the array 
				 *  and bias regions.
				 *
				 *	+-----------------------+------------+
				 *	|						|            |
				 *	|          BOX	        |    BIAS    |
				 *  |<----------------5------->          |
				 *  |        <--4-->        |  |         |
				 *	|       +-------+^      |  +-------+ |
				 *	|       |       ||      |  |       | |
				 *	|<------|-->2   |3      |  |<--6-->| |
				 *	|       |   ^   ||		|  |	   | |
				 *	|       +---|---+v      |  +-------+ |
				 *	|			|	        |            |
				 *	|			|	        |            |
				 *	|			1	        |            |
				 *	|			|	        |            |
				 *	|			v	        |            |
				 *	+-----------------------+------------+
				 *
				 *  @param uiOldRows		- Returns the original row size currently set on the controller
				 *  @param uiOldCols		- Returns the original col size currently set on the controller
				 *  @param uiRow			- The row number of the center of the sub-array in pixels (see #1 in the above figure)
				 *  @param uiCol			- The col number of the center of the sub-array in pixels (see #2 in the above figure)
				 *  @param uiSubRows		- The sub-array row count in pixels (see #3 in the above figure)
				 *  @param uiSubCols		- The sub-array col count in pixels (see #4 in the above figure)
				 *  @param uiBiasOffset		- The offset of the bias region in pixels (see #5 in the above figure)
				 *  @param uiBiasCols		- The col count of the bias region in pixels (see #6 in the above figure)
				 *  @param throws std::runtime_error
				 */
				virtual void setSubArray( std::uint32_t& uiOldRows, std::uint32_t& uiOldCols, const std::uint32_t uiRow, const std::uint32_t uiCol, const std::uint32_t uiSubRows, const std::uint32_t uiSubCols, const std::uint32_t uiBiasOffset, const std::uint32_t uiBiasWidth );

				/** Disable sub-array mode on the controller
				 *  @param uiRows - The new image row dimension to set on the controller (typically a previously saved value)
				 *  @param uiCols - The new image column dimension to set on the controller (typically a previously saved value)
				 *  @throws std::runtimer_error
				 */
				virtual void unSetSubArray( const std::uint32_t uiRows, const std::uint32_t uiCols );

				/** Returns whether or not the controller is in synthetic mode. A synthetic image is one that is generated by the timing 
				 *  board DSP, not the video board, and has the following pattern: 0, 1, 2, ..., 65535, 0, 1, 2, ..., 65353, etc.
				 *  @return <i>true</i> if the controller is in synthetic mode; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				virtual bool isSyntheticImageMode( void );

				/** Enables or disable synthetic image mode on the controller. A synthetic image is one that is generated by the timing 
				 *  board DSP, not the video board, and has the following pattern: 0, 1, 2, ..., 65535, 0, 1, 2, ..., 65353, etc.
				 *  @param bMode - <i>true</i> to enable synthetic image mode; <i>false</i> to disable.
				 *  @throws std::runtime_error
				 */
				virtual void setSyntheticImageMode( bool bMode );


				//  expose commands
				// +-------------------------------------------------+

				/** Enable or disable the shutter on expose.
				 *  @param bShouldOpen - <i>true</i> if the shutter should open during the exposure; <i>false</i> to keep the shutter closed (i.e. dark frame).
				 *  throws std::runtime_error
				 */
				virtual void setOpenShutter( bool bShouldOpen );

				/** Start image aquisition
				 *  @param fExpTime		- The exposure time ( in seconds ).
				 *  @param uiRows		- The image row size ( in pixels ).
				 *  @param uiCols		- The image column size ( in pixels ).
				 *  @param pAbort		- Pointer to a boolean value that can be used to cancel the exposure/readout (default = nullptr).
                 *  @param pExpIFace	- Function pointer to a CExpIFace object, whose methods are called during exposure and readout to provide exposure time and pixel count updates (default = nullptr).
				 *  @param bOpenShutter	- Set to <i>true</i> if the shutter should open during the exposure. Set to <i>false</i> to keep the shutter closed (default = true).
				 *  @throws std::runtime_error
				 */
				virtual void expose( const float fExpTime, const std::uint32_t uiRows, const std::uint32_t uiCols, const bool* pAbort = nullptr, arc::gen3::CExpIFace* pExpIFace = nullptr, bool bOpenShutter = true );

				/** Attempts to stop the current exposure
				 *  @throws std::runtime_error
				 */
				virtual void stopExposure( void ) = 0;

				/** Start continuous image aquisition.
				 *  @param uiRows		 - The image row size ( in pixels ).
				 *  @param uiCols		 - The image column size ( in pixels ).
				 *  @param uiNumOfFrames - The number of image frames to read.
				 *  @param fExpTime		 - The exposure time ( in seconds ).
				 *  @param pAbort		 - Pointer to a boolean value that can be used to cancel the exposure/readout (default = nullptr).
				 *  @param pConIFace	 - Function pointer to a CConIFace object, whose methods are called to provide frame completion updates (default = nullptr).
				 *  @param bOpenShutter	 - Set to <i>true</i> if the shutter should open during the exposure. Set to <i>false</i> to keep the shutter closed (default = true).
				 *  @throws std::runtime_error
				 */
				virtual void continuous( const std::uint32_t uiRows, const std::uint32_t uiCols, const std::uint32_t uiNumOfFrames, const float fExpTime, const bool* pAbort = nullptr, arc::gen3::CConIFace* pConIFace = nullptr, bool bOpenShutter = true );

				/** Attempts to stop the current frame aquisition and set the controller back to single image aquisition.
				 *  @throws std::runtime_error
				 */
				virtual void stopContinuous( void );

				/** Returns whether or not image readout is in progress.
				 *  @return <i>true</i> if the controller is currently reading out image pixels; <i>false</i> otherwise.
				 *  @throws std::runtime_error
				 */
				virtual bool isReadout( void ) = 0;

				/** Returns the current pixel count during image aquisition. Note: this method will log the value if logging is enabled.
				 *  @return The current pixel count
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getPixelCount( void ) = 0;
	
				/** Returns the cumulative pixel count across all frames read while in continuous readout mode.
				 *  @return The cumulative continuous readout pixel count.
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getCRPixelCount( void ) = 0;

				/** Returns the current continuous readout frame count. The controller must be in continuous readout mode Note: this method 
				 *  will log the value if logging is enabled.
				 *  @return The current frame count.
				 */
				virtual std::uint32_t getFrameCount( void ) = 0;


				//  Error & Degug message access
				// +--------------------------------------------------------------------------------------+

				/** Returns whether or not the specified parameter contains one of the error status codes (i.e. 'ERR', 'TOUT', etc).
				 *  @param uiWord - The value to check for error status codes. This is typically a command reply value.
				 *  @return <i>true</i> if an error status code was found; <i>false</i> otherwise.
				 */
				virtual bool containsError( const std::uint32_t uiWord ) noexcept;

				/** Returns whether or not the specified value falls within the specified range.
				 *  @param uiWord    - The value to range check
				 *  @param uiWordMin - The lower range value (inclusive)
				 *  @param uiWordMax - The upper range value (inclusive)
				 *  @return <i>true</i> if the specified value falls outside the specified range; <i>false</i> otherwise.
				 */
				virtual bool containsError( const std::uint32_t uiWord, const std::uint32_t uiWordMin, const std::uint32_t uiWordMax ) noexcept;

				/** Pops and returns the next logged message from the command logger.
				 *  @return The next message in the command logger or an empty string if there are no more messages.
				 */
				virtual const std::string getNextLoggedCmd( void );

				/** Returns the number of messages remaining in the command logger.
				 *  @return The command logger message count.
				 */
				virtual std::uint32_t getLoggedCmdCount( void );

				/** Turns command logging on/off. Command logging will slow the execution of any method implementing it.
				 *  This logging can be used for debugging to see command details in the following form:
				 *  <header> <cmd> <arg1> ... <arg4> -> <controller reply>
				 * 
				 *  Example: 0x203 TDL 0x112233 -> 0x444E4F
				 * 
				 *  @param bOnOff - <i>true</i> to enable command logging; <i>false</i> to disable.
				 */
				virtual void setLogCmds( bool bOnOff ) noexcept;


				//  Temperature control
				// +----------------------------------------------------------------------------------------+

				/** Returns the current array temperature in degrees celcuis. The temperature is read m_gTmpCtrl_SDNumberOfReads times and averaged. 
				 *  Also, for a read to be included in the average, the difference between the target temperature and the actual temperature must be 
				 *  less than the tolerance specified by gTmpCtrl_SDTolerance.
				 *  @return The current array temperature in celcius.
				 *  @throws std::runtime_error
				 */
				virtual double getArrayTemperature( void );

				/** Returns the digital number associated with the current array temperature value.
				 *  @return The current array temperature digital number.
				 *  @throws std::runtime_error
				 */
				virtual double getArrayTemperatureDN( void );

				/** Set the temperature that the array should be regulated around.
				 *  @param gTempVal - The temperature, in celcius, that the array should be regulated around.
				 *  @throws std::runtime_error
				 */
				virtual void setArrayTemperature( double gTempVal );

				/** Loads temperature control constants from the specified file. The default constants are stored in TempCtrl.h and cannot be permanently overwritten.
				 *  @param tFilename - The file that contains the temperature control constant values.
				 *  @throws std::runtime_error
				 *  @throws std::invalid_argument
				 */
				virtual void loadTemperatureCtrlData( const std::filesystem::path& tFilename );

				/** Saves the current temperature control constants to the specified file. These may be different from the values in TempCtrl.h if another
				 *  temperature control file has been previously loaded.
				 *  @param tFilename - The file in which to write the temperature control data.
				 *  @throws std::runtime_error
				 */
				virtual void saveTemperatureCtrlData( const std::filesystem::path& tFilename );


				/** Maximum number of command parameters the controller will accept
				 */
				static constexpr auto CTLR_CMD_MAX = static_cast< std::uint32_t >( 6 );


				/** Timeout loop count for image readout
				 */
				static constexpr auto READ_TIMEOUT = static_cast< std::uint32_t >( 200 );


				/** Invalid parameter value
				 */
				static constexpr auto NOPARAM = static_cast< std::uint32_t >( 0xFF000000 );

			protected:

				/** Internally fills the kernel image buffer physical address and size from the device driver.
				 *  @return <i>true</i> if the properties were successfully read from the driver; <i>false</i> on error.
				 *  @throws std::runtime_error
				 */
				virtual bool getCommonBufferProperties( void ) = 0;

				/** Initializes the default array temperature settings. Called from class constructor.
				 */
				virtual void setDefaultTemperatureValues( void ) noexcept;

				/** Calculates the voltage associated with the specified digital number (adu). There is a different calculation for SmallCam, 
				 *  which can be specified via the parameters.
				 *  @param uiADU     - The digital number to use for the voltage calculation.
				 *  @param bArc12    - <i>true</i> to use the SmallCam calculation (default = false).
				 *  @param bHighGain - <i>true</i> if High Gain is used (default = false).
				 */
				virtual double ADUToVoltage( const std::uint32_t uiAdu, bool bArc12 = false, bool bHighGain = false ) noexcept;

				/** Calculates the digital number (adu) associated with the specified voltage. There is a different calculation for SmallCam, 
				 *  which can be specified via the parameters.
				 *  @param gVoltage  - The voltage to use for the calculation.
				 *  @param bArc12    - <i>true</i> to use the SmallCam calculation (default = false).
				 *  @param bHighGain - <i>true</i> if High Gain is used (default = false).
				 */
				virtual double voltageToADU( const double gVoltage, bool bArc12 = false, bool bHighGain = false ) noexcept;

				/** Calculates the average array temperature.
				 *  @return The average temperature value ( in Celcius ). The temperature is read m_gTmpCtrl_SDNumberOfReads 
				 *  times and averaged. Also, for a read to be included in the average the difference between the target 
				 *  temperature and the actual temperature must be less than the tolerance specified by m_gTmpCtrl_SDDegTolerance 
				 *  (default = 3.0 degrees celcius).
				 *  @throws std::runtime_error
				 */
				virtual double calculateAverageTemperature( void );

				/** Calculates the voltage using the specified temperature. The voltage is determined using the number of 
				 *  trials specified by m_gTmpCtrl_SDVoltToleranceTrials.  Also, the difference between the target temperature 
				 *  and the actual must be less than the tolerance specified by m_gTmpCtrl_SDVoltTolerance.
				 *  @param gTemperature - The temperature to use for the calculation.
				 *  @return The voltage as calculated from the specified temperature.
				 *  @throws std::runtime_error
				 */
				virtual double calculateVoltage( const double gTemperature );

				/** Calculates the silicon diode temperature using a voltage ( converted digital number read from the controller ) 
				 *  and a Chebychev polynomial series.
				 *  @param gVoltage - The voltage to use for the temperature calculation.
				 *  @return The temperature, in Celcius, as calculated from the specified voltage.
				 *  @throws std::rumtime_error
				 */
				virtual double calculateTemperature( const double gVoltage );

				/** Returns the boundary adjusted image size for continuous readout. Some interface cards require the 
				 *  data to be written on the start of a specific byte boundary. See the PCI/PCIe headers for specific 
				 *  information on that device.
				 *  @param uiImageSize - The image size, in bytes, that will be adjusted to meet the needs of the specific device.
				 *  @return The adjusted image size for any device specific boundary conditions.
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getContinuousImageSize( const std::uint32_t uiImageSize ) = 0;

				/** Sends a DSP firmware (.lod) file data stream of up to six values to the SmallCam controller.
				 *  @param uiBoardId - Must be SMALLCAM_DLOAD_ID
				 *  @param pvData    - Data vector of up to six download values
				 *  @return The controller reply value. Typically 'DON' or 'ERR'.
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t smallCamDLoad( const std::uint32_t uiBoardId, const std::vector<std::uint32_t>* pvData ) = 0;

				/** Loads a timing or utility file (.lod) into a SmallCam controller.
				 *  @param tFilename - The TIM or UTIL lod file to load.
				 *  @param bValidate - <i>true</i> if the download should be read back and checked after every write; <i>false</i> to not check.
				 *  @param pAbort    - <i>true</i> to cancel execution; <i>false</i> otherwise (default = nullptr).
				 *  @throws std::runtime_error
				 */
				virtual void loadSmallCamControllerFile( const std::filesystem::path& tFilename, [[maybe_unused]] bool bValidate, bool* pAbort = nullptr );

				/** Loads a timing or utility file (.lod) into a GenII or GenIII controller.
				 *  @param tFilename - The TIM or UTIL lod file to load.
				 *  @param bValidate - <i>true</i> if the download should be read back and checked after every write; <i>false</i> to not check.
				 *  @param pAbort    - <i>true</i> to cancel execution; <i>false</i> otherwise (default = nullptr).
				 *  @throws std::runtime_error
				 */
				virtual void loadGen23ControllerFile( const std::filesystem::path& tFilename, bool bValidate, bool* pAbort = nullptr ) = 0;

				/** This is an outdated method purely left in for compatability. This turns the PCI hardware byte-swapping 
				 *  on if system architecture is solaris. Otherwise, does nothing; compiles to empty method and will likely 
				 *  be optimized out by the compiler.
				 *  @throws std::runtime_error
				 */
				virtual void setByteSwapping( void ) = 0;

				/** Bundle command values into a std::string that can be used to throw a std::runtime_error exception.
				 *  @param uiReply   - The controller command reply
				 *  @param uiBoardId - The board id
				 *  @param pvData    - The controller command data in the format: command, argument 0, argument 1, etc.
				 *  @return A command and reply string formatted as follows: "[0xheader, 0xcommand, 0xargument, ... -> 0xreply]"
				 *  @throws std::runtime_error
				 */
				virtual const std::string formatDLoadString( const std::uint32_t uiReply, const std::uint32_t uiBoardId, const std::vector<std::uint32_t>* pvData );

				//  Temperature control variables
				// +-------------------------------------------------------------+
				double									m_gTmpCtrl_DT670Coeff1;				/**< Temperature Coeffient for DT-670 Sensor */
				double									m_gTmpCtrl_DT670Coeff2;				/**< Temperature Coeffient for DT-670 Sensor */
				double									m_gTmpCtrl_SDAduOffset;				/**< ADU offset for CY7 Sensor */
				double									m_gTmpCtrl_SDAduPerVolt;			/**< ADU per volt for CY7 Sensor */
				double									m_gTmpCtrl_HGAduOffset;				/**< ADU offset for high gain utility board */
				double									m_gTmpCtrl_HGAduPerVolt;			/**< ADU per volt for high gain utility board */
				double									m_gTmpCtrl_SDVoltTolerance;			/**< Voltage tolerance for non-linear silicone diode */
				double									m_gTmpCtrl_SDDegTolerance;			/**< Temerature tolerance for non-linear silicone diode */
				std::uint32_t							m_gTmpCtrl_SDNumberOfReads;			/**< Number of temperature reads to average for non-linear silicone diode */
				std::uint32_t							m_gTmpCtrl_SDVoltToleranceTrials;	/**< Number of voltage tolerance trials for non-linear silicone diode */

				TmpCtrlCoeff_t							m_tTmpCtrl_SD_2_12K;				/**< Non-linear silicon diode temperature coefficient */
				TmpCtrlCoeff_t							m_tTmpCtrl_SD_12_24K;				/**< Non-linear silicon diode temperature coefficient */
				TmpCtrlCoeff_t							m_tTmpCtrl_SD_24_100K;				/**< Non-linear silicon diode temperature coefficient */
				TmpCtrlCoeff_t							m_tTmpCtrl_SD_100_475K;				/**< Non-linear silicon diode temperature coefficient */

				arc::gen3::arcDevHandle_t				m_hDevice;							/**< Device driver file descriptor */
				std::unique_ptr<arc::gen3::CArcLog>		m_pCLog;							/**< Command logger for debug purposes */
				arc::gen3::device::ImgBuf_t				m_tImgBuffer;						/**< Kernel image buffer properties */
				std::uint32_t							m_uiCCParam;						/**< Controller configuration parameters value */
				bool	 								m_bStoreCmds;						/**< <i>true</i> to store commanmd strings in logger */
		};

	}	// end gen3 namespace
}	// end arc namespace
