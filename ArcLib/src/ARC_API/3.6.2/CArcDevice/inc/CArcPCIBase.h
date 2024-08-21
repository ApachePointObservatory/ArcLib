// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcPCIBase.h                                                                                            |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the PCI/PCIe device base class.                                                      |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file CArcPCIBase.h */

#pragma once


#ifdef _WINDOWS
	#pragma warning( disable: 4251 )
#endif

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include <CArcDeviceDllMain.h>
#include <CArcStringList.h>
#include <CArcDevice.h>
#include <CArcBase.h>



namespace arc
{
	namespace gen3
	{

		/** Convenience ( compact ) definition for a pointer to a CArcStringList
		 */
		using pCStrList_t = std::unique_ptr<arc::gen3::CArcStringList>;

		/** @struct PCIRegData
		 *  PCI configuration space register data
		 */
		struct PCIRegData
		{
			pCStrList_t		pBitList;	/**< The PCI/e bit list definitions */
			std::string		sName;		/**< The PCI/e register name */
			std::uint32_t	uiValue;	/**< The PCI/e register value */
			std::uint32_t	uiAddr;		/**< The PCI/e register address */
		};

		/** Convenience ( compact ) definition for a pointer to the PCIRegData type
		 */
		using pPCIRegData_t = std::shared_ptr<PCIRegData>;

		/** Convenience ( compact ) definition for a vector to a pointer to the PCIRegData type
		 */
		using PCIRegList_t	= std::vector<pPCIRegData_t>;


		// +-------------------------------------------------------------------+
		// |  PCI configuration space register bar item ( used to name and     |
		// |  contain a set of local gen3 configuration registers; if they     |
		// |  exist )                                                          |
		// +-------------------------------------------------------------------+

		/** Convenience ( compact ) definition for a pointer to the PCIRegList type
		 */
		using pPCIRegList_t = std::unique_ptr<PCIRegList_t>;

		/** @struct PCIBarData
		 *  PCI configuration space register bar item (used to name and contain a set of local gen3 configuration registers; if they exist).
		 */
		struct PCIBarData
		{
			std::string		sName;		/**< The PCI/e base address register (BAR) name */
			pPCIRegList_t	pList;		/**< The PCI/e base address register list details */
		};

		/** Convenience ( compact ) definition for a pointer to the PCIBarData type
		 */
		using pPCIBarData_t = std::shared_ptr<PCIBarData>;

		/** Convenience ( compact ) definition for a vector to a pointer to PCIBarData type
		 */
		using PCIBarList_t	= std::vector<pPCIBarData_t>;


		/** @class CArcPCIBase
		 *
		 *  PCI device base class
		 *
		 *  @see arc::gen3::CArcPCIBase
		 */
		class GEN3_CARCDEVICE_API CArcPCIBase : public CArcDevice
		{
			public:

				/** Default constructor
				 */
				CArcPCIBase( void ) = default;

				/** Default destructor
				 */
				virtual ~CArcPCIBase( void );

				/** Returns a configuration space byte (8-bit) value
				 *  @param uiOffset - The PCI/e configuration space byte offset.
				 *  @return The PCI/e configuration space byte value read from offset address.
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getCfgSpByte( const std::uint32_t uiOffset )  = 0;

				/** Returns a configuration space word (16-bit) value
				 *  @param uiOffset - The PCI/e configuration space word offset.
				 *  @return The PCI/e configuration space word value read from offset address.
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getCfgSpWord( const std::uint32_t uiOffset )  = 0;

				/** Returns a configuration space dword (32-bit) value
				 *  @param uiOffset - The PCI/e configuration space dword offset.
				 *  @return The PCI/e configuration space dword value read from offset address.
				 *  @throws std::runtime_error
				 */
				virtual std::uint32_t getCfgSpDWord( const std::uint32_t uiOffset ) = 0;

				/** Set a configuration space byte (8-bit) value
				 *  @param uiOffset - The PCI/e configuration space byte offset.
				 *  @param uiValue  - The byte (8-bit) value to write.
				 *  @throws std::runtime_error
				 */
				virtual void setCfgSpByte( const std::uint32_t uiOffset, const std::uint32_t uiValue )  = 0;

				/** Set a configuration space word (16-bit) value
				 *  @param uiOffset - The PCI/e configuration space word offset.
				 *  @param uiValue  - The word (16-bit) value to write.
				 *  @throws std::runtime_error
				 */
				virtual void setCfgSpWord( const std::uint32_t uiOffset, const std::uint32_t uiValue )  = 0;

				/** Set a configuration space dword (32-bit) value
				 *  @param uiOffset - The PCI/e configuration space dword offset.
				 *  @param uiValue  - The dword (32-bit) value to write.
				 *  @throws std::runtime_error
				 */
				virtual void setCfgSpDWord( const std::uint32_t uiOffset, const std::uint32_t uiValue ) = 0;

				/** Reads and parses the entire PCI/e configuration space header into readable text and bit definitions
				 *  that are stored in a member list variable. The public methods of this class allow access to this list.
				 *  This method will create the member list if it doesn't already exist and clears it if it does.
				 *  @throws std::runtime_error
				 */
				virtual void getCfgSp( void );

				/** Reads and parses the entire PCI/e Base Address Registers (BAR) into readable text and bit definitions
				 *  that are stored in a member list variable. The public methods of this class allow access to this list.
				 *  This method will create the member list if it doesn't already exist and clears it if it does.
				 *  NOTE: Not all BARS or PCI/e boards have data.
				 *  @throws std::runtime_error
				 */
				virtual void getBarSp( void );

				/** Returns the number of elements in the PCI/e configuration space. Call getCfgSpCount() first.
				 *  @param The number of configuration space elements.
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCfgSpCount( void );

				/** Returns the address for the specified configuration space element. Call getCfgSpCount() first.
				 *  @param uiIndex - Index into the PCI/e configuration space element list.
				 *  @return The PCI/e configuration space address
				 *  @throws std::out-of-range
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCfgSpAddr( const std::uint32_t uiIndex );

				/** Returns the register value for the specified configuration space element. Call getCfgSpCount() first.
				 *  @param uiIndex - Index into the PCI/e configuration space element list.
				 *  @return The PCI/e configuration space value at the specified address 
				 *  @throws std::out-of-range
				 *  @throws std::runtime_error
				 */
				std::uint32_t getCfgSpValue( const std::uint32_t uiIndex );

				/** Returns the register name for the specified configuration space element. Call getCfgSpCount() first.
				 *  @param uiIndex - Index into the PCI/e configuration space element list.
				 *  @return The PCI/e configuration space name at the specified address
				 *  @throws std::out-of-range
				 *  @throws std::runtime_error
				 */
				const std::string getCfgSpName( const std::uint32_t uiIndex );

				/** Returns the register bit descriptions for the specified configuration space element. Call getCfgSpCount() first.
				 *  @param uiIndex - Index into the PCI/e configuration space element list.
				 *  @param pCount  - Reference to a local variable that will receive the total list count.
				 *  @return The PCI/e configuration space list at the specified address. Use the return value of pCount to know the number of elements in the list.
				 *  @throws std::out-of-range
				 */
				const std::string* getCfgSpBitList( const std::uint32_t uiIndex, std::uint32_t& pCount );

				/** Returns the number of valid PCI/e base address registers (BARS). Call getCfgSpCount() first.
				 *  @return The number of (BARS) base address registers
				 *  @throws std::runtime_error
				 */
				std::uint32_t getBarCount( void );

				/** Returns the PCI/e base address register (BAR) name at the specified BAR index. Call getCfgSpCount() first.
				 *  @return The name for the base address register at the specified BAR index.
				 *  @throws std::runtime_error
				 */
				const std::string getBarName( const std::uint32_t dIndex );

				/** Returns the PCI/e base address register (BAR) sub-register count at the specified BAR index. Call getCfgSpCount() first.
				 *  @return The sub-register count for the base address register at the specified BAR index.
				 *  @throws std::out-of-range-error
				 *  @throws std::runtime_error
				 */
				std::uint32_t getBarRegCount( const std::uint32_t dIndex );

				/** Returns the PCI/e base address register (BAR) sub-register address at the specified BAR index. Call getCfgSpCount() first.
				 *  @return The sub-register address for the base address register at the specified BAR index.
				 *  @throws std::out-of-range-error
				 *  @throws std::runtime_error
				 */
				std::uint32_t getBarRegAddr( const std::uint32_t dIndex, const std::uint32_t dRegIndex );

				/** Returns the PCI/e base address register (BAR) sub-register value at the specified BAR index. Call getCfgSpCount() first.
				 *  @return The sub-register value for the base address register at the specified BAR index.
				 *  @throws std::out-of-range-error
				 *  @throws std::runtime_error
				 */
				std::uint32_t getBarRegValue( const std::uint32_t dIndex, const std::uint32_t dRegIndex );

				/** Returns the PCI/e base address register (BAR) sub-register name at the specified BAR index. Call getCfgSpCount() first.
				 *  @return The sub-register name for the base address register at the specified BAR index.
				 *  @throws std::out-of-range-error
				 *  @throws std::runtime_error
				 */
				const std::string getBarRegName( const std::uint32_t dIndex, const std::uint32_t dRegIndex );

				/** Returns the PCI/e base address register (BAR) sub-register bit list count at the specified BAR index. Call getCfgSpCount() first.
				 *  @return The sub-register bit list for the base address register at the specified BAR index.
				 *  @throws std::out-of-range-error
				 *  @throws std::runtime_error
				 */
				std::uint32_t getBarRegBitListCount( const std::uint32_t dIndex, const std::uint32_t dRegIndex );

				/** Returns the PCI/e base address register (BAR) sub-register bit list definition at the specified BAR index. Call getCfgSpCount() first.
				 *  @return The sub-register bit list definition for the base address register at the specified BAR index. Returns an empty string if no name exists.
				 *  @throws std::out-of-range-error
				 *  @throws std::runtime_error
				 */
				const std::string getBarRegBitListDef( const std::uint32_t dIndex, const std::uint32_t dRegIndex, const std::uint32_t dBitListIndex );

				/** Prints the stored PCI/e configuration space to the specified output stream. Call getCfgSpCount() first.
				 *  @param os - The output stream used to print (default = std::cout).
				 *  @throws std::runtime_error
				 */
				void printCfgSp( std::ostream& os = std::cout );

				/** Prints the stored PCI/e configuration space base addresses (BARS) to the specified output stream. Call getCfgSpCount() first.
				 *  @param os - The output stream used to print (default = std::cout).
				 *  @throws std::runtime_error
				 */
				void printBars( std::ostream& os = std::cout );

			protected:

				/** Adds the specified parameters to the specified register data list.
				 *  @param pvDataList	- The register list to add
				 *  @param uiAddr		- The register address
				 *  @param sName		- The register name
				 *  @param uiValue		- The register value
				 *  @param pBitList		- A pointer to a list of bit definitions for the register (default = nullptr)
				 *  @throws std::runtime_error
				 */
				void addRegItem( arc::gen3::PCIRegList_t* pvDataList, const std::uint32_t uiAddr, const std::string& sName, const std::uint32_t uiValue, arc::gen3::CArcStringList* pBitList = nullptr );

				/** Adds the specified parameters to the specified base address (BAR) list.
				 *  @param sName - The register name
				 *  @param pList - A pointer to a register list
				 *  @throws std::runtime_error
				 */
				void addBarItem( const std::string& sName, PCIRegList_t* pList );

				/** Return the bit definitions for the PCI/e configuration space device/vedor id register.
				 *  @param uiData - The device/vendor id register value
				 *  @param bDrawSeperator - <i>true</i> to draw a seperator before the bit definition string; <i>false</i> otherwise.
				 *  @throws std::exception if std::vector::push_back() throws
				 */
				std::unique_ptr<arc::gen3::CArcStringList> getDevVenBitList( const std::uint32_t uiData, bool bDrawSeparator = false );

				/** Return the bit definitions for the PCI/e configuration space command register.
				 *  @param uiData - The command register value
				 *  @param bDrawSeperator - <i>true</i> to draw a seperator before the bit definition string; <i>false</i> otherwise.
				 *  @throws std::exception if std::vector::push_back() throws
				 */
				std::unique_ptr<arc::gen3::CArcStringList> getCommandBitList( const std::uint32_t uiData, bool bDrawSeparator = false );

				/** Return the bit definitions for the PCI/e configuration space status register.
				 *  @param uiData - The status register value
				 *  @param bDrawSeperator - <i>true</i> to draw a seperator before the bit definition string; <i>false</i> otherwise.
				 *  @throws std::exception if std::vector::push_back() throws
				 */
				std::unique_ptr<arc::gen3::CArcStringList> getStatusBitList( const std::uint32_t uiData, bool bDrawSeparator = false );

				/** Return the bit definitions for the PCI/e configuration space class/revision register.
				 *  @param uiData - The class/revision register value
				 *  @param bDrawSeperator - <i>true</i> to draw a seperator before the bit definition string; <i>false</i> otherwise.
				 *  @throws std::exception if std::vector::push_back() throws
				 */
				std::unique_ptr<arc::gen3::CArcStringList> getClassRevBitList( const std::uint32_t uiData, bool bDrawSeparator = false );

				/** Return the bit definitions for the PCI/e configuration space BIST/header/latency register.
				 *  @param uiData - The BIST/header/latency register value
				 *  @param bDrawSeperator - <i>true</i> to draw a seperator before the bit definition string; <i>false</i> otherwise.
				 *  @throws std::exception if std::vector::push_back() throws
				 */
				std::unique_ptr<arc::gen3::CArcStringList> getBistHeaderLatencyCache( const std::uint32_t uiData, bool bDrawSeparator = false );

				/** Return the bit definitions for the PCI/e configuration space base address registers.
				 *  @param uiData - The base address register value
				 *  @param bDrawSeperator - <i>true</i> to draw a seperator before the bit definition string; <i>false</i> otherwise.
				 *  @throws std::exception if std::vector::push_back() throws
				 */
				std::unique_ptr<arc::gen3::CArcStringList> getBaseAddressBitList( const std::uint32_t uiData, bool bDrawSeparator = false );

				/** Return the bit definitions for the PCI/e configuration space sub-system register.
				 *  @param uiData - The sub-system register value
				 *  @param bDrawSeperator - <i>true</i> to draw a seperator before the bit definition string; <i>false</i> otherwise.
				 *  @throws std::exception if std::vector::push_back() throws
				 */
				std::unique_ptr<arc::gen3::CArcStringList> getSubSysBitList( const std::uint32_t uiData, bool bDrawSeparator = false );

				/** Return the bit definitions for the PCI/e configuration space max latency register.
				 *  @param uiData - The max latency register value
				 *  @param bDrawSeperator - <i>true</i> to draw a seperator before the bit definition string; <i>false</i> otherwise.
				 *  @throws std::exception if std::vector::push_back() throws
				 */
				std::unique_ptr<arc::gen3::CArcStringList> getMaxLatGntIntBitList( const std::uint32_t uiData, bool bDrawSeparator = false );

				/** struct VectorDeleter
				 *  Custom deleter to remove all elements from a PCI/e register list
				 */
				struct VectorDeleter
				{
					/** The deleter function
					 *  @param p - The PCI/e register list to clear and delete
					 */
					void operator()( PCIRegList_t* p ) const
					{
						if ( p != nullptr )
						{
							p->clear();

							delete p;
						}
					}
				};

				/** The PCI/e configuration space register list
				 */
				std::shared_ptr<PCIRegList_t> m_pCfgSpList;

				/** The PCI/e configuration space base address register (BAR) list
				 */
				std::shared_ptr<PCIBarList_t> m_pBarList;

				/** Temporary PCI/e configuration space register bit list
				 */
				std::shared_ptr<std::string> m_pTmpCfgBitList;

				/** Temporary PCI/e configuration space base address register (BAR) bit list
				 */
				std::shared_ptr<std::string> m_pTmpBarBitList;
		};

	}	// end gen3 namespace
}	// end arc namespace
