// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcDeinterlace.cpp  ( Gen3 )                                                                            |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file implements the ARC deinterlace interface.                                                    |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: March 27, 2020                                                              |
// |                                                                                                                  |
// |  Copyright 2013 Astronomical Research Cameras, Inc. All rights reserved.                                         |
// +------------------------------------------------------------------------------------------------------------------+

#ifdef _WINDOWS
	#include <windows.h>
#endif

#include <iomanip>
#include <type_traits>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>

#include <CArcDeinterlace.h>
#include <IArcPlugin.h>


using namespace std::string_literals;


namespace arc
{
	namespace gen3
	{

		// +----------------------------------------------------------------------------------------------------------+
		// | OS dependent macros                                                                                      |
		// +----------------------------------------------------------------------------------------------------------+
		#ifdef _WINDOWS
			#define ArcLoadLibrary( path, lib )			LoadLibraryA( ( LPCSTR )lib.c_str() )
			#define ArcFindLibrarySymbol( lib, name )	GetProcAddress( lib, name )
			#define ArcFreeLibrary( handle )			FreeLibrary( handle )
			#define ArcSysMsg( msg )					std::string( msg )
			#define ArcSysErrorCode()					GetLastError()
		#else
			#define ArcLoadLibrary( path, lib )			dlopen( std::string( path + "/" + lib ).c_str(), RTLD_LAZY )
			#define ArcFindLibrarySymbol( lib, name )	dlsym( lib, name )
			#define ArcFreeLibrary( handle )			dlclose( handle )
			#define ArcSysMsg( msg )					std::string( msg ) + std::string( " - " ) + dlerror()
			#define ArcSysErrorCode()					dlerror()
		#endif


// +----------------------------------------------------------------------------------------------------------+
// | Library build and version info                                                                           |
// +----------------------------------------------------------------------------------------------------------+
		template <typename T> const std::string CArcDeinterlace<T>::m_sVersion = "ARC Gen III DLace API Library.    "s +

		#ifdef _WINDOWS
			CArcBase::formatString( "[ Compiler Version: %d, Built: %s ]", _MSC_VER, __TIMESTAMP__ );
		#else
			arc::gen3::CArcBase::formatString( "[ Compiler Version: %d.%d.%d, Built: %s %s ]", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, __DATE__, __TIME__ );
		#endif


		// +----------------------------------------------------------------------------------------------------------+
		// | Constructor                                                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> CArcDeinterlace<T>::CArcDeinterlace( void ) : CArcBase()
		{
			m_pPluginManager.reset( new CArcPluginManager() );

			m_uiNewCols = 0;
			m_uiNewRows = 0;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  Destructor                                                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> CArcDeinterlace<T>::~CArcDeinterlace( void )
		{
			#ifdef _WINDOWS
				SetDllDirectoryA( nullptr );
			#endif

			m_pNewData.reset();

			m_uiNewCols = 0;
			m_uiNewRows = 0;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  version                                                                                                 |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns a textual representation of the library version.                                                |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> const std::string CArcDeinterlace<T>::version( void )
		{
			return m_sVersion;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// | run                                                                                                      |
		// +----------------------------------------------------------------------------------------------------------+
		// | The deinterlacing algorithms work on the principle that the CCD/IR array will read out the data in a     |
		// | predetermined order depending on the type of readout being implemented.  Here's how they look:     	  |
		// |                                                                                                          |
		// |  split-parallel          split-serial              quad CCD                quad IR          	          |
		// |  ----------------       ----------------        ----------------        ----------------     	          |
		// | |     1  ------->|     |        |       |      |<-----  |  ---->|      | -----> | ----> |    	          |
		// | |                |     |        |       |      |   3    |   2   |      |   0    |   1   |    	          |
		// | |                |     |        |       |      |        |       |      |        |       |    	          |
		// | |_______________ |     |        |       |      |________|_______|      |________|_______|    	          |
		// | |                |     |        |       |      |        |       |      |        |       |    	          |
		// | |                |     |        |       |      |        |       |      |        |       |    	          |
		// | |                |     |   0    |   1   |      |   0    |   1   |      |   3    |   2   |    	          |
		// | |<--------  0    |     |<------ |------>|      |<-----  |  ---->|      | -----> | ----> |    	          |
		// |  ----------------       ----------------        ----------------        ----------------     	          |
		// |                                                                                                          |
		// |  CDS quad IR          	                                                                                  |
		// |  ----------------     	     	                                                                          |
		// | | -----> | ----> |    	     	                                                                          |
		// | |   0    |   1   |    	     	                                                                          |
		// | |        |       |	               HawaiiRG                                                               |
		// | |________|_______|    ---------------------------------                                                  |
		// | |        |       |    |       |       |       |       |                                                  |
		// | |        |       |    |       |       |       |       |                                                  |
		// | |   3    |   2   |    |       |       |       |       |                                                  |
		// | | -----> | ----> |    |       |       |       |       |                                                  |
		// |  ----------------     |       |       |       |       |                                                  |
		// | | -----> | ----> |    |       |       |       |       |                                                  |
		// | |   0    |   1   |    |   0   |   1   |   2   |   3   |                                                  |
		// | |        |       |    | ----> | ----> | ----> | ----> |                                                  |
		// | |________|_______|    ---------------------------------                                                  |
		// | |        |       |    	                                                                                  |
		// | |        |       |    	                                                                                  |
		// | |   3    |   2   |    	                                                                                  |
		// | | -----> | ----> |    	                                                                                  |
		// | ----------------                                                                                         |
		// |                                                                                                          |
		// |                            STA1600                                                                       | 	
		// |                                                                                                          |
		// |                <-+     <-+     <-+           <-+                                                         |
		// |              +---|---+---|---+---|---+     |---|---+                                                     |
		// |              |   |   |   |   |   |   |     |   |   |                                                     |
		// |              |   8   |   9   |   10  | ... |  15   |                                                     |
		// |              |       |       |       |     |       |                                                     |
		// |              |       |       |       |     |       |                                                     |
		// |              +-------+-------+-------+ ... +-------+                                                     |
		// |              |       |       |       |     |       |                                                     |
		// |              |       |       |       |     |       |                                                     |
		// |              |   0   |   1   |   2   | ... |   7   |                                                     |
		// |              |   |   |   |   |   |   |     |   |   |                                                     |
		// |              +---|---+---|---+---|---+     |---|---+                                                     |
		// |                  |       |       |             |                                                         |
		// |                <-+     <-+     <-+           <-+                                                         |
		// |                                                                                                          |
		// |  <IN>  -> pBuf		- Pointer to the image pBuf to deinterlace                                            |
		// |  <IN>  -> uiCols	- Number of uiCols in image to deinterlace                                            |
		// |  <IN>  -> uiRows	- Number of rows in image to deinterlace                                              |
		// |  <IN>  -> eAlg		- Algorithm number that corresponds to deinterlacing method                           |
		// |  <IN>  -> tArgList - An optional argument list ( default = {}, empty list }.                             |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		void CArcDeinterlace<T>::run( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows, arc::gen3::dlace::e_Alg eAlg, const std::initializer_list<std::uint32_t>& tArgList )
		{
			T* pOldBuf = pBuf;	// Old image buffer pointer

			// NOTE ****** Instead of memcpy pOldBuf to m_pNewData, maybe just return the
			// buffer pointer.

			// Allocate a new buffer to hold the deinterlaced image
			// -------------------------------------------------------------------
			if ( uiCols > m_uiNewCols || uiRows > m_uiNewRows )
			{
				m_pNewData.reset( new T[ static_cast< std::uint64_t >( uiCols ) * static_cast< std::uint64_t >( uiRows ) ] );

				m_uiNewCols = uiCols;
				m_uiNewRows = uiRows;
			}

			if ( m_pNewData == nullptr || pOldBuf == nullptr )
			{
				throwArcGen3Error( "Error in allocating temporary image buffer for deinterlacing."s );
			}

			switch ( eAlg )
			{
				// +-------------------------------------------------------------------+
				// |                   NO DEINTERLACING                                |
				// +-------------------------------------------------------------------+
				case arc::gen3::dlace::e_Alg::NONE:
				{
					// Do nothing
					;
				}
				break;

				// +-------------------------------------------------------------------+
				// |                       PARALLEL READOUT                            |
				// +-------------------------------------------------------------------+
				case arc::gen3::dlace::e_Alg::PARALLEL:
				{
					parallel( pOldBuf, uiCols, uiRows );
				}
				break;

				// +-------------------------------------------------------------------+
				// |                        SERIAL READOUT                             |
				// +-------------------------------------------------------------------+
				case arc::gen3::dlace::e_Alg::SERIAL:
				{
					serial( pOldBuf, uiCols, uiRows );
				}
				break;

				// +-------------------------------------------------------------------+
				// |                         QUAD READOUT                              |
				// +-------------------------------------------------------------------+
				case arc::gen3::dlace::e_Alg::QUAD_CCD:
				{
					quadCCD( pOldBuf, uiCols, uiRows );
				}
				break;

				// +-------------------------------------------------------------------+
				// |                         IR QUAD READOUT                           |
				// +-------------------------------------------------------------------+
				case arc::gen3::dlace::e_Alg::QUAD_IR:
				{
					quadIR( pOldBuf, uiCols, uiRows );
				}
				break;

				// +-------------------------------------------------------------------+
				// |            CORRELATED DOUBLE SAMPLING IR QUAD READOUT             |
				// +-------------------------------------------------------------------+
				case arc::gen3::dlace::e_Alg::QUAD_IR_CDS:
				{
					quadIRCDS( pOldBuf, uiCols, uiRows );
				}
				break;

				// +-------------------------------------------------------------------+
				// |         HawaiiRG READOUT                                          |
				// +-------------------------------------------------------------------+
				case arc::gen3::dlace::e_Alg::HAWAII_RG:
				{
					if ( tArgList.size() != 1 )
					{
						throwArcGen3Error( "Invalid number of arguments. Expected 1, found: %d", tArgList.size() );
					}

					hawaiiRG( pOldBuf, uiCols, uiRows, *tArgList.begin() );
				}
				break;

				// +-------------------------------------------------------------------+
				// |         STA1600 READOUT                                           |
				// +-------------------------------------------------------------------+
				case arc::gen3::dlace::e_Alg::STA1600:
				{
					sta1600( pOldBuf, uiCols, uiRows );
				}
				break;

				// +-------------------------------------------------------------------+
				// |         Default                                                   |
				// +-------------------------------------------------------------------+
				default:
				{
					throwArcGen3Error( "Invalid deinterlace algorithm [ %d ]!", eAlg );
				}
				break;
			}	// End switch
		}


		// +----------------------------------------------------------------------------------------------------------+
		// | run                                                                                                      |
		// +----------------------------------------------------------------------------------------------------------+
		// | Calls a custom deinterlace routine that has been loaded through the deinterlace plugin manager.    	  |
		// |                                                                                                          |
		// |  <IN>  -> pBuf		- Pointer to the image pBuf to deinterlace                                            |
		// |  <IN>  -> uiCols	- Number of uiCols in image to deinterlace                                            |
		// |  <IN>  -> uiRows	- Number of rows in image to deinterlace                                              |
		// |  <IN>  -> sAlg		- Algorithm name that corresponds to deinterlacing method                             |
		// |  <IN>  -> tArgList - An optional argument list ( default = {}, empty list }.                             |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		void CArcDeinterlace<T>::run( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows, const std::string& sAlg, const std::initializer_list<std::uint32_t>& tArgList )
		{
			if ( m_pPluginManager->pluginLoaded() )
			{
				bool bSuccess = false;

				for ( std::uint32_t j = 0; j < m_pPluginManager->pluginCount(); j++ )
				{
					arc::gen3::IArcPlugin* pPlugin = m_pPluginManager->getPluginObject( j );

					if ( ( bSuccess = pPlugin->getNameList()->find( sAlg ) ) )
					{
						pPlugin->run( pBuf, uiCols, uiRows, ( 8 * sizeof( T ) ), sAlg, ( tArgList.begin() != tArgList.end() ? *tArgList.begin() : 0 ) );

						break;
					}
				}

				if ( !bSuccess )
				{
					throwArcGen3Error( "Algorithm [ \'%s\' ] not found!", sAlg.c_str() );
				}
			}
			else
			{
				throwArcGen3Error( "No deinterlace plugins loaded!"s );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  maxTVal                                                                                                 |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns the maximum value for a specific data type. Example, for an unsigned short: 2 ^ 16 = 65536.     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::uint32_t CArcDeinterlace<T>::maxTVal( void ) noexcept
		{
			auto gExponent = ( ( sizeof( T ) == sizeof( arc::gen3::dlace::BPP_32 ) ) ? 20 : ( sizeof( T ) * 8 ) );

			return static_cast< std::uint32_t >( std::pow( 2.0, gExponent ) );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// | parallel                                                                                                 |
		// +----------------------------------------------------------------------------------------------------------+
		// |                                                                                                          |
		// |                       Parallel                                                                           | 	
		// |                +---------------------+                                                                   |     	
		// |                |          1  ------->|                                                                   |  	
		// |                |                     |                                                                   |    	
		// |                |                     |                                                                   |
		// |                |_____________________|                                                                   |   	
		// |                |                     |                                                                   |   	
		// |                |                     |                                                                   |   	
		// |                |                     |                                                                   |   	
		// |                |<--------  0         |                                                                   |  	
		// |                +---------------------+                                                                   |   	
		// |                                                                                                          |
		// |  <IN>  -> pBuf   - Pointer to the image pixels to deinterlace                                            |
		// |  <IN>  -> uiCols - Number of columns in image to deinterlace                                             |
		// |  <IN>  -> uiRows - Number of rows in image to deinterlace                                                |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcDeinterlace<T>::parallel( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			if ( ( uiRows % 2 ) != 0 )
			{
				throwArcGen3Error( "Number of ROWS must be EVEN for PARALLEL deinterlace."s );
			}

			for ( std::uint64_t i = 0; i < ( ( uiCols * uiRows ) / 2 ); i++ )
			{
				*( m_pNewData.get() + i ) = *( pBuf + ( 2 * i ) );
				*( m_pNewData.get() + ( static_cast< std::uint64_t >( uiCols ) * static_cast< std::uint64_t >( uiRows ) ) - i - 1 ) = *( pBuf + ( 2 * i ) + 1 );
			}

			copyMemory( pBuf, m_pNewData.get(), ( static_cast<std::size_t>( uiCols ) * static_cast< std::size_t >( uiRows ) * sizeof( T ) ) );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// | serial                                                                                                   |
		// +----------------------------------------------------------------------------------------------------------+
		// |                                                                                                          |
		// |                         Serial                                                                           | 	
		// |                +----------+----------+                                                                   |     	
		// |                |          |          |                                                                   |  	
		// |                |          |          |                                                                   |    	
		// |                |          |          |                                                                   |
		// |                |          |          |                                                                   |
		// |                |          |          |                                                                   |
		// |                |          |          |                                                                   |
		// |                |     0    |    1     |                                                                   |
		// |                |<-------- | -------->|                                                                   |
		// |                +----------+----------+                                                                   |   	
		// |                                                                                                          |
		// |  <IN>  -> pBuf   - Pointer to the image pixels to deinterlace                                            |
		// |  <IN>  -> uiCols - Number of columns in image to deinterlace                                             |
		// |  <IN>  -> uiRows - Number of rows in image to deinterlace                                                |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcDeinterlace<T>::serial( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			std::uint32_t p1 = 0;
			std::uint32_t p2 = 0;
			std::uint32_t begin = 0;
			std::uint32_t end = 0;

			if ( ( uiCols % 2 ) != 0 )
			{
				throwArcGen3Error( "Number of COLS must be EVEN for SERIAL deinterlace."s );
			}

			for ( std::remove_const_t<decltype( uiRows )> i = 0; i < uiRows; i++ )
			{
				// Leave in +0 for clarity
				p1 = i * uiCols + 0;	// Position in raw image
				p2 = i * uiCols + 1;
				begin = i * uiCols + 0;	// Position in deinterlaced image
				end = i * uiCols + uiCols - 1;

				for ( std::remove_const_t<decltype( uiCols )> j = 0; j < uiCols; j += 2 )
				{
					*( m_pNewData.get() + begin ) = *( pBuf + p1 );
					*( m_pNewData.get() + end ) = *( pBuf + p2 );

					++begin;
					--end;

					p1 += 2;
					p2 += 2;
				}
			}

			copyMemory( pBuf, m_pNewData.get(), ( static_cast< std::size_t >( uiCols ) * static_cast< std::size_t >( uiRows ) * sizeof( T ) ) );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// | quadCCD                                                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		// |                                                                                                          |
		// |                        CCD Quad                                                                          | 	
		// |                +----------+----------+                                                                   |     	
		// |                | <--------|--------> |                                                                   |  	
		// |                |     3    |    2     |                                                                   |    	
		// |                |          |          |                                                                   |
		// |                |__________|__________|                                                                   |
		// |                |          |          |                                                                   |
		// |                |          |          |                                                                   |
		// |                |     0    |    1     |                                                                   |    	
		// |                | <--------|--------> |                                                                   |  	
		// |                +----------+----------+                                                                   |   	
		// |                                                                                                          |
		// |  <IN>  -> pBuf   - Pointer to the image pixels to deinterlace                                            |
		// |  <IN>  -> uiCols - Number of columns in image to deinterlace                                             |
		// |  <IN>  -> uiRows - Number of rows in image to deinterlace                                                |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcDeinterlace<T>::quadCCD( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			std::uint32_t i = 0;
			std::uint32_t j = 0;
			std::uint32_t counter = 0;
			std::uint32_t end = 0;
			std::uint32_t begin = 0;

			if ( ( uiCols % 2 ) != 0 || ( uiRows % 2 ) != 0 )
			{
				throwArcGen3Error( "Number of COLS and ROWS must be EVEN for QUAD CCD deinterlace."s );
			}

			while ( i < ( uiCols * uiRows ) )
			{
				if ( counter % ( uiCols / 2 ) == 0 )
				{
					end = ( uiCols * uiRows ) - ( uiCols * j ) - 1;
					begin = ( uiCols * j ) + 0;	// Left in 0 for clarity

					j++;							// Number of completed rows

					counter = 0;					// Reset for next convergece
				}

				*( m_pNewData.get() + begin + counter ) = *( pBuf + i++ );		// front_row--->
				*( m_pNewData.get() + begin + uiCols - 1 - counter ) = *( pBuf + i++ );		// front_row<--
				*( m_pNewData.get() + end - counter ) = *( pBuf + i++ );		// end_row<----
				*( m_pNewData.get() + end - uiCols + 1 + counter ) = *( pBuf + i++ );		// end_row---->

				counter++;
			}

			copyMemory( pBuf, m_pNewData.get(), ( static_cast< std::size_t >( uiCols ) * static_cast< std::size_t >( uiRows ) * sizeof( T ) ) );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// | quadIR                                                                                                   |
		// +----------------------------------------------------------------------------------------------------------+
		// |                                                                                                          |
		// |                        IR Quad                                                                           | 	
		// |                +----------+----------+                                                                   |     	
		// |                | -------> |--------> |                                                                   |  	
		// |                |    0     |    1     |                                                                   |    	
		// |                |          |          |                                                                   |
		// |                |__________|__________|                                                                   |
		// |                |          |          |                                                                   |
		// |                |          |          |                                                                   |
		// |                |    3     |    2     |                                                                   |    	
		// |                | -------> |--------> |                                                                   |  	
		// |                +----------+----------+                                                                   |   	
		// |                                                                                                          |
		// |  <IN>  -> pBuf   - Pointer to the image pixels to deinterlace                                            |
		// |  <IN>  -> uiCols - Number of columns in image to deinterlace                                             |
		// |  <IN>  -> uiRows - Number of rows in image to deinterlace                                                |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcDeinterlace<T>::quadIR( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			std::uint32_t i = 0;
			std::uint32_t j = uiRows - 1;
			std::uint32_t counter = 0;
			std::uint32_t end = 0;
			std::uint32_t begin = 0;

			if ( ( uiCols % 2 ) != 0 || ( uiRows % 2 ) != 0 )
			{
				throwArcGen3Error( "Number of COLS and ROWS must be EVEN for QUAD IR deinterlace."s );
			}

			while ( i < ( uiCols * uiRows ) )
			{
				if ( counter % ( uiCols / 2 ) == 0 )
				{
					end = ( j - ( uiRows / 2 ) ) * uiCols;
					begin = j * uiCols;

					j--; 						// Nnumber of completed rows

					counter = 0; 				// Reset for next convergece
				}

				*( m_pNewData.get() + begin + counter ) = *( pBuf + i++ );	// front_row--->
				*( m_pNewData.get() + begin + ( uiCols / 2 ) + counter ) = *( pBuf + i++ );	// front_row<--
				*( m_pNewData.get() + end + ( uiCols / 2 ) + counter ) = *( pBuf + i++ );	// end_row<----
				*( m_pNewData.get() + end + counter ) = *( pBuf + i++ );	// end_row---->

				counter++;
			}

			copyMemory( pBuf, m_pNewData.get(), ( static_cast< std::size_t >( uiCols ) * static_cast< std::size_t >( uiRows ) * sizeof( T ) ) );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// | quadIRCDS (Correlated Double Sampling )                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		// |                                                                                                          |
		// |                      IR Quad CDS                                                                         | 	
		// |                +----------+----------+                                                                   |     	
		// |                | -------> |--------> |                                                                   |  	
		// |                |    0     |    1     |                                                                   |    	
		// |                |          |          |                                                                   |
		// |                |__________|__________|                                                                   |
		// |                |          |          |                                                                   |
		// |                |          |          |                                                                   |
		// |                |    3     |    2     |                                                                   |    	
		// |                | -------> |--------> |                                                                   |  	
		// |                +----------+----------+                                                                   |   	
		// |                | -------> |--------> |                                                                   |  	
		// |                |    0     |    1     |                                                                   |    	
		// |                |          |          |                                                                   |
		// |                |__________|__________|                                                                   |
		// |                |          |          |                                                                   |
		// |                |          |          |                                                                   |
		// |                |    3     |    2     |                                                                   |    	
		// |                | -------> |--------> |                                                                   |  	
		// |                +----------+----------+                                                                   |   	
		// |                                                                                                          |
		// |  <IN>  -> pBuf   - Pointer to the image pixels to deinterlace                                            |
		// |  <IN>  -> uiCols - Number of columns in image to deinterlace                                             |
		// |  <IN>  -> uiRows - Number of rows in image to deinterlace                                                |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcDeinterlace<T>::quadIRCDS( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			std::uint32_t i			= 0;
			std::uint32_t j			= 0;
			std::uint32_t counter	= 0;
			std::uint32_t end		= 0;
			std::uint32_t begin		= 0;

			T* pOldStart = pBuf;
			T* pNewStart = m_pNewData.get();

			auto uiLocalRows = uiRows;

			if ( ( uiCols % 2 ) != 0 || ( uiLocalRows % 2 ) != 0 )
			{
				throwArcGen3Error( "Number of COLS and ROWS must be EVEN for QUAD IR CDS deinterlace."s );
			}

			// Set the the number of rows to half the image size.
			uiLocalRows	= ( uiLocalRows / 2U );

			// Deinterlace the two image halves separately.
			for ( int imageSection = 0; imageSection < 2; imageSection++ )
			{
				i = 0;
				j = uiLocalRows - 1;
				counter = 0;
				end = 0;
				begin = 0;

				while ( i < ( uiCols * uiLocalRows ) )
				{
					if ( ( counter % ( uiCols / 2 ) ) == 0 )
					{
						end = ( j - ( uiLocalRows / 2 ) ) * uiCols;
						begin = j * uiCols;

						j--; 			// Number of completed rows

						counter = 0; 	// Reset for next convergece
					}

					*( pNewStart + begin + counter ) = *( pOldStart + i++ );		// front_row--->
					*( pNewStart + begin + ( uiCols / 2 ) + counter ) = *( pOldStart + i++ );		// front_row<--
					*( pNewStart + end + ( uiCols / 2 ) + counter ) = *( pOldStart + i++ );		// end_row<----
					*( pNewStart + end + counter ) = *( pOldStart + i++ );		// end_row---->

					counter++;
				}

				pOldStart += ( static_cast< std::uint64_t >( uiLocalRows ) * static_cast< std::uint64_t >( uiCols ) );
				pNewStart += ( static_cast< std::uint64_t >( uiLocalRows ) * static_cast< std::uint64_t >( uiCols ) );
			}

			copyMemory( pBuf, m_pNewData.get(), ( static_cast< std::size_t >( uiCols ) * static_cast< std::size_t >( uiLocalRows ) * sizeof( T ) ) );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// | hawaiiRG                                                                                                 |
		// +----------------------------------------------------------------------------------------------------------+
		// |                                                                                                          |
		// |                          HawaiiRG                                                                        | 	
		// |              +-------+-------+-------+-------+                                                           |
		// |              |       |       |       |       |                                                           |
		// |              |       |       |       |       |                                                           |
		// |              |       |       |       |       |                                                           |
		// |              |       |       |       |       |                                                           |
		// |              |       |       |       |       |                                                           |
		// |              |       |       |       |       |                                                           |
		// |              |   0   |   1   |   2   |   3   |                                                           |
		// |              | ----> | ----> | ----> | ----> |                                                           |
		// |              +-------+-------+-------+-------+                                                           |
		// |                                                                                                          |
		// |  <IN>  -> pBuf      - Pointer to the image pixels to deinterlace                                         |
		// |  <IN>  -> uiCols    - Number of columns in image to deinterlace                                          |
		// |  <IN>  -> uiRows    - Number of rows in image to deinterlace                                             |
		// |  <IN>  -> uChannels - The number of channels in the image (16, 32, ..)                                   |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		void CArcDeinterlace<T>::hawaiiRG( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows, const std::uint32_t uChannels )
		{
			const std::uint32_t ERR = 0x00455252;

			if ( ( uiCols % 2 ) != 0 )
			{
				throwArcGen3Error( "Number of COLS must be EVEN for HAWAII RG deinterlace."s );
			}

			else if ( uChannels == 1 )
			{
				// Ignore and don't de-interlace. Bob requested this
				// action on March 30, 2012.
			}

			else if ( uChannels == ERR )
			{
				throwArcGen3Error( "The number of readout channels must be supplied for HAWAII RG deinterlace."s );
			}

			else if ( uChannels % 2 != 0 )
			{
				throwArcGen3Error( "The readout channel count must be EVEN for HAWAII RG deinterlace."s );
			}

			else
			{
				T* pRow = m_pNewData.get();

				std::uint32_t offset = uiCols / uChannels;
				std::uint32_t dataIndex = 0;

				for ( std::uint64_t r = 0; r < uiRows; r++ )
				{
					pRow = m_pNewData.get() + ( uiCols * r );

					for ( std::remove_const_t<decltype( uiCols )> c = 0; c < ( uiCols / uChannels ); c++ )
					{
						for ( std::remove_const_t<decltype( uChannels )> i = 0; i < uChannels; i++ )
						{
							pRow[ c + i * offset ] = pBuf[ dataIndex++ ];
						}
					}
				}

				copyMemory( pBuf, m_pNewData.get(), ( static_cast< std::size_t >( uiCols ) * static_cast< std::size_t >( uiRows ) * sizeof( T ) ) );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// | sta1600                                                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		// |                                                                                                          |
		// |                            STA1600                                                                       | 	
		// |                                                                                                          |
		// |                <-+     <-+     <-+           <-+                                                         |
		// |              +---|---+---|---+---|---+     |---|---+                                                     |
		// |              |   |   |   |   |   |   |     |   |   |                                                     |
		// |              |   8   |   9   |   10  | ... |  15   |                                                     |
		// |              |       |       |       |     |       |                                                     |
		// |              |       |       |       |     |       |                                                     |
		// |              +-------+-------+-------+ ... +-------+                                                     |
		// |              |       |       |       |     |       |                                                     |
		// |              |       |       |       |     |       |                                                     |
		// |              |   0   |   1   |   2   | ... |   7   |                                                     |
		// |              |   |   |   |   |   |   |     |   |   |                                                     |
		// |              +---|---+---|---+---|---+     |---|---+                                                     |
		// |                  |       |       |             |                                                         |
		// |                <-+     <-+     <-+           <-+                                                         |
		// |                                                                                                          |
		// |  <IN>  -> pBuf   - Pointer to the image pixels to deinterlace                                            |
		// |  <IN>  -> uiCols - Number of columns in image to deinterlace                                             |
		// |  <IN>  -> uiRows - Number of rows in image to deinterlace                                                |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcDeinterlace<T>::sta1600( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			if ( ( uiCols % 16 ) != 0 )
			{
				throwArcGen3Error( "Number of COLS must be a multiple of 16 for STA1600 deinterlace."s );
			}

			if ( ( uiRows % 2 ) != 0 )
			{
				throwArcGen3Error( "Number of ROWS must be a multiple of 2 for STA1600 deinterlace."s );
			}

			T* botPtr = m_pNewData.get();
			T* topPtr = m_pNewData.get() + ( static_cast< std::uint64_t >( uiCols ) * static_cast< std::uint64_t >( uiRows - 1 ) );

			std::uint32_t offset = uiCols / 8;
			std::uint32_t dataIndex = 0;

			for ( std::uint64_t r = 0; r < ( uiRows / 2 ); r++ )
			{
				topPtr = m_pNewData.get() + ( uiCols * ( uiRows - r - 1 ) );
				botPtr = m_pNewData.get() + ( uiCols * r );

				for ( std::remove_const_t<decltype( uiCols )> c = 0; c < ( uiCols / 8 ); c++ )
				{
					botPtr[ c + 7 * offset ] = pBuf[ dataIndex++ ];
					botPtr[ c + 6 * offset ] = pBuf[ dataIndex++ ];
					botPtr[ c + 5 * offset ] = pBuf[ dataIndex++ ];
					botPtr[ c + 4 * offset ] = pBuf[ dataIndex++ ];
					botPtr[ c + 3 * offset ] = pBuf[ dataIndex++ ];
					botPtr[ c + 2 * offset ] = pBuf[ dataIndex++ ];
					botPtr[ c + 1 * offset ] = pBuf[ dataIndex++ ];
					botPtr[ c + 0 * offset ] = pBuf[ dataIndex++ ];

					topPtr[ c + 7 * offset ] = pBuf[ dataIndex++ ];
					topPtr[ c + 6 * offset ] = pBuf[ dataIndex++ ];
					topPtr[ c + 5 * offset ] = pBuf[ dataIndex++ ];
					topPtr[ c + 4 * offset ] = pBuf[ dataIndex++ ];
					topPtr[ c + 3 * offset ] = pBuf[ dataIndex++ ];
					topPtr[ c + 2 * offset ] = pBuf[ dataIndex++ ];
					topPtr[ c + 1 * offset ] = pBuf[ dataIndex++ ];
					topPtr[ c + 0 * offset ] = pBuf[ dataIndex++ ];
				}
			}

			copyMemory( pBuf, m_pNewData.get(), ( static_cast< std::size_t >( uiCols ) * static_cast< std::size_t >( uiRows ) * sizeof( T ) ) );
		}


	}	// end gen3 namespace
}		// end arc namespace



// +----------------------------------------------------------------------------------------------------------+
// | getPluginManager                                                                                         |
// +----------------------------------------------------------------------------------------------------------+
// | Returns the deinterlace plugin manager instance.                                                         |
// +----------------------------------------------------------------------------------------------------------+
template <typename T> arc::gen3::CArcPluginManager* arc::gen3::CArcDeinterlace<T>::getPluginManager( void ) noexcept
{
	return m_pPluginManager.get();
}

template <typename T>
std::unique_ptr<arc::gen3::CArcPluginManager> arc::gen3::CArcDeinterlace<T>::m_pPluginManager;


/** Explicit instantiations - These are the only allowed instantiations of this class */
template class arc::gen3::CArcDeinterlace<arc::gen3::dlace::BPP_16>;
template class arc::gen3::CArcDeinterlace<arc::gen3::dlace::BPP_32>;
