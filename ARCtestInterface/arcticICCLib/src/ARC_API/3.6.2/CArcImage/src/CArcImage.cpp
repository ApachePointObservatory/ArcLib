// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcImage.cpp  ( Gen3 )                                                                                  |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE:  Defines the exported functions for the CArcImage DLL.                                                 |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: March 26, 2020                                                              |
// |                                                                                                                  |
// |  Copyright 2014 Astronomical Research Cameras, Inc. All rights reserved.                                         |
// +------------------------------------------------------------------------------------------------------------------+

#include <iostream>
#include <type_traits>
#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>
#include <cstdlib>

#include <CArcImage.h>

using namespace std::string_literals;



namespace arc
{
	namespace gen3
	{


		// +----------------------------------------------------------------------------------------------------------+
		// |  verifyBuffer                                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Verifies that the specified buffer is not equal to nullptr. Throws exception on error.                  |
		// |                                                                                                          |
		// |  <IN> -> pBuf	- Pointer to the buffer to check.                                                         |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> constexpr void CArcImage<T>::verifyBuffer( const T* pBuffer )
		{
			if ( pBuffer == nullptr )
			{
				throwArcGen3Error( "Invalid buffer parameter ( nullptr )!"s );
			}
		}

		// +----------------------------------------------------------------------------------------------------------+
		// |  verifyRow                                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Verifies that the specified row value is less than the total number of rows. Throws exception on error. |
		// |                                                                                                          |
		// |  <IN> -> row	- The row to check.                                                                       |
		// |  <IN> -> rows	- The total row length ( i.e. image row count )                                           |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> constexpr void CArcImage<T>::verifyRow( const std::uint32_t uiRow, const std::uint32_t uiRows )
		{
			if ( uiRow >= uiRows )
			{
				throwArcGen3Error( "Invalid row [ %u ]! Must be between 0 and %u!", uiRow, uiRows - 1 );
			}
		}

		// +----------------------------------------------------------------------------------------------------------+
		// |  verifyRows                                                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Verifies that the specified number of rows is not equal to zero. Throws exception on error.             |
		// |                                                                                                          |
		// |  <IN> -> rows - The total row length ( i.e. image row count )                                            |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> constexpr void CArcImage<T>::verifyRows( const std::uint32_t uiRows )
		{
			if ( uiRows == 0 )
			{
				throwArcGen3Error( "Invalid row count [ %u ]! Cannot be zero!", uiRows );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  verifyColumns                                                                                           |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Verifies that the specified number of cols is not equal to zero. Throws exception on error.             |
		// |                                                                                                          |
		// |  <IN> -> cols - The total column length ( i.e. image column count )                                      |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> constexpr void CArcImage<T>::verifyColumns( const std::uint32_t uiCols )
		{
			if ( uiCols == 0 )
			{
				throwArcGen3Error( "Invalid row count [ %u ]! Cannot be zero!", uiCols );
			}
		}

		// +----------------------------------------------------------------------------------------------------------+
		// |  verifyColumn                                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Verifies that the specified column value is less than the total number of columns. Throws exception on  |
		// |  error.                                                                                                  |
		// |                                                                                                          |
		// |  <IN> -> col	- The column to check.                                                                    |
		// |  <IN> -> cols	- The total column length ( i.e. image column count )                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> constexpr void CArcImage<T>::verifyColumn( const std::uint32_t uiCol, const std::uint32_t uiCols )
		{
			if ( uiCol >= uiCols )
			{
				throwArcGen3Error( "Invalid column [ %u ]! Must be between 0 and %u!", uiCol, uiCols - 1 );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  verifyRangeOrder                                                                                        |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Verifies that value1 is less than value2. Throws exception on error.                                    |
		// |                                                                                                          |
		// |  <IN> -> value1 - The first ( lesser ) range value.                                                      |
		// |  <IN> -> value2 - The second ( higher ) range value.                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> constexpr void CArcImage<T>::verifyRangeOrder( const std::uint32_t uiValue1, const std::uint32_t uiValue2 )
		{
			if ( uiValue2 < uiValue1 )
			{
				throwArcGen3Error( "Invalid range order [ %u < %u ]! Values must be reversed!", uiValue2, uiValue1 );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  Constant - DEG2RAD                                                                                      |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Degree to radian conversion factor.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		constexpr auto DEG2RAD = ( 3.14159 / 180.0 );


		// +----------------------------------------------------------------------------------------------------------+
		// |  GenIII image channel type                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Image channel insertion pointers.                                                                       |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		struct imageChannel_t
		{
			T* pStart;
			T* pEnd;
			T* pInserter;
		};


		// +----------------------------------------------------------------------------------------------------------+
		// | Library build and version info                                                                           |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> const std::string CArcImage<T>::m_sVersion = "ARC Gen III Image API Library.    "s +

		#ifdef _WINDOWS
			CArcBase::formatString( "[ Compiler Version: %d, Built: %s ]", _MSC_VER, __TIMESTAMP__ );
		#else
			arc::gen3::CArcBase::formatString( "[ Compiler Version: %d.%d.%d, Built: %s %s ]", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, __DATE__, __TIME__ );
		#endif


		// +----------------------------------------------------------------------------------------------------------+
		// |  Destructor                                                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Destroys the class.                                                                                     |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> CArcImage<T>::~CArcImage( void )
		{
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  version                                                                                                 |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns a textual representation of the library version.                                                |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> const std::string CArcImage<T>::version( void )
		{
			return m_sVersion;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  fill                                                                                                    |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Fills the specified buffer with the specified value.                                                    |
		// |                                                                                                          |
		// |  <IN> -> pBuf	  - Pointer to the image data buffer.                                                     |
		// |  <IN> -> uiCols  - The image column size ( in pixels ).                                                  |
		// |  <IN> -> uiRows  - The image row size ( in pixels ).                                                     |
		// |  <IN> -> uiValue - The value to fill the buffer with.                                                    |
		// |                                                                                                          |
		// |  Throws std::invalid_argument, std::invalid_argument                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::fill( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows, const T uiValue )
		{
			if ( uiValue >= maxTVal() )
			{
				throwArcGen3OutOfRange( uiValue, std::make_pair( 0, ( maxTVal() - 1 ) ) );
			}

			if ( pBuf != nullptr )
			{
				for ( std::uint32_t i = 0; i < ( uiCols * uiRows ); i++ )
				{
					pBuf[ i ] = uiValue;
				}
			}

			else
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  fill                                                                                                    |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Fills the specified buffer with the specified value.                                                    |
		// |                                                                                                          |
		// |  <IN> -> pBuf	  - Pointer to the image data buffer.                                                     |
		// |  <IN> -> uiBytes - The number of bytes in the image data buffer.                                         |
		// |  <IN> -> uiValue - The value to fill the buffer with.                                                    |
		// |                                                                                                          |
		// |  Throws std::invalid_argument, std::invalid_argument                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::fill( T* pBuf, const std::uint32_t uiBytes, const T uiValue )
		{
			if ( uiValue >= maxTVal() )
			{
				throwArcGen3OutOfRange( uiValue, std::make_pair( 0, ( maxTVal() - 1 ) ) );
			}

			if ( pBuf != nullptr )
			{
				for ( std::uint32_t i = 0; i < ( uiBytes / sizeof( T ) ); i++ )
				{
					pBuf[ i ] = uiValue;
				}
			}

			else
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  fillWithGradient                                                                                        |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Fills the specified buffer with a gradient pattern.                                                     |
		// |                                                                                                          |
		// |  <IN> -> pBuf	  - Pointer to the image data buffer.                                                     |
		// |  <IN> -> uiCols  - The image column size ( in pixels ).                                                  |
		// |  <IN> -> uiRows  - The image row size ( in pixels ).                                                     |
		// |                                                                                                          |
		// |  Throws std::invalid_argument on error.                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::fillWithGradient( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			if ( pBuf != nullptr )
			{
				zeroMemory( pBuf, ( static_cast< std::size_t >( uiCols ) * static_cast< std::size_t >( uiRows ) * sizeof( T ) ) );

				T uiValue = 0;

				for ( std::remove_const_t<decltype( uiRows )> r = 0; r < uiRows; r++ )
				{
					for ( std::remove_const_t<decltype( uiCols )> c = 0; c < uiCols; c++ )
					{
						pBuf[ c + r * uiCols ] = uiValue;
					}

					uiValue += static_cast< T >( ( maxTVal() - 1 ) / uiRows );
				}
			}

			else
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  fillWithSmiley                                                                                          |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Fills the specified buffer with zeroes and puts a smiley face at the center.                            |
		// |                                                                                                          |
		// |  <IN> -> pBuf	  - Pointer to the image data buffer.                                                     |
		// |  <IN> -> uiCols  - The image column size ( in pixels ).                                                  |
		// |  <IN> -> uiRows  - The image row size ( in pixels ).                                                     |
		// |                                                                                                          |
		// |  Throws std::invalid_argument on error.                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::fillWithSmiley( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			if ( pBuf != nullptr )
			{
				zeroMemory( pBuf, ( static_cast< std::size_t >( uiCols ) * static_cast< std::size_t >( uiRows ) * sizeof( T ) ) );

				std::uint32_t uiRadius = static_cast<std::uint32_t>( std::fmin( ( uiRows / 2.0 ), ( uiCols / 2.0 ) ) - 10.0 );

				//  Draw Head
				// +---------------------------------------------------------------------------- +
				drawGradientFillCircle(	uiRadius,
										uiCols,
										uiRows,
										pBuf );

				//  Draw Left Eye
				// +---------------------------------------------------------------------------- +
				std::uint32_t uiRowFactor = static_cast< std::uint32_t >( uiRadius / 2.5 );

				drawFillCircle( ( uiCols / 2 ) - uiRowFactor,
								( uiRows / 2 ) + uiRowFactor,
								( uiRadius / 5 ),
								uiCols,
								pBuf );

				//  Draw Right Eye
				// +---------------------------------------------------------------------------- +
				drawFillCircle( ( uiCols / 2 ) + uiRowFactor,
								( uiRows / 2 ) + uiRowFactor,
								( uiRadius / 5 ),
								uiCols,
								pBuf );

				//  Draw Mouth
				// +---------------------------------------------------------------------------- +
				for ( decltype( uiRadius ) i = 0; i < ( uiRadius / 2 ); i++ )
				{
					drawSemiCircle( ( uiCols / 2 ),
									( uiRows / 2 ) - ( uiRowFactor / 2 ),
									i,
									180,
									360,
									uiCols,
									pBuf );
				}
			}

			else
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  fillWithRamp                                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Fills the specified buffer with a ramp image. Data has the form 0, 1, 2, ..., 65535, 0, 1, ....         |
		// |                                                                                                          |
		// |  <IN> -> pBuf	 - Pointer to the image data buffer.                                                      |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::invalid_argument on error.                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::fillWithRamp( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			if ( pBuf != nullptr )
			{
				T uiValue = 0;

				for ( std::uint32_t i = 0; i < ( uiCols * uiRows ); i++ )
				{
					pBuf[ i ] = uiValue;

					uiValue++;

					if ( uiValue >= maxTVal() ) { uiValue = 0; }
				}

				containsValidRamp( pBuf, uiCols, uiRows );
			}

			else
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  containsValidRamp                                                                                       |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Verify a ramp synthetic image. Data has the form 0, 1, 2, ..., 65535, 0, 1, ....                        |
		// |                                                                                                          |
		// |  <IN> -> pBuf	 - Pointer to the image data buffer.                                                      |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::containsValidRamp( const T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			if ( pBuf != nullptr )
			{
				T uiValue = pBuf[ 0 ];

				std::uint32_t uiPixel = 0;

				for ( std::uint32_t r = 0; r < uiRows; r++ )
				{
					for ( std::uint32_t c = 0; c < uiCols; c++ )
					{
						if ( pBuf[ uiPixel ] != uiValue )
						{
							throwArcGen3Error( "Invalid ramp image. Expected %u at col %u row %u, found %u",
									uiValue,
									c,
									r,
									pBuf[ uiPixel ] );
						}

						uiPixel++;
						uiValue++;

						if ( uiValue >= maxTVal() )
						{
							uiValue = pBuf[ c + r * uiCols + 1 ];
						}
					}
				}
			}

			else
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  countPixels                                                                                             |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Count the number of pixels having the specified value.                                                  |
		// |                                                                                                          |
		// |  <IN> -> pBuf	   - Pointer to the image data buffer.                                                    |
		// |  <IN> -> uiCols  - The image column size ( in pixels ).                                                  |
		// |  <IN> -> uiRows  - The image row size ( in pixels ).                                                     |
		// |  <IN> -> uwValue - The pixel value to include in the count.                                              |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		std::uint32_t CArcImage<T>::countPixels( const T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows, const std::uint16_t uwValue )
		{
			return countPixels( pBuf, ( uiCols * uiRows ), uwValue );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  countPixels                                                                                             |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Count the number of pixels having the specified value.                                                  |
		// |                                                                                                          |
		// |  <IN> -> pBuf	    - Pointer to the image data buffer.                                                   |
		// |  <IN> -> uiBufSize - The image buffer size ( in pixels ).                                                |
		// |  <IN> -> uwValue   - The pixel value to include in the count.                                            |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		std::uint32_t CArcImage<T>::countPixels( const T* pBuf, const std::uint32_t uiBufSize, const std::uint16_t uwValue )
		{
			std::uint32_t uiCount = 0;

			verifyBuffer( pBuf );

			if ( pBuf != nullptr )
			{
				for ( std::remove_const_t<decltype( uiBufSize )> i = 0; i < uiBufSize; i++ )
				{
					if ( pBuf[ i ] == uwValue )
					{
						uiCount++;
					}
				}
			}
			
			return uiCount;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getPixel                                                                                                |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns the value of a pixel at the specified row and column.                                           |
		// |                                                                                                          |
		// |  <IN> -> pBuf		- Pointer to the image data buffer.                                                   |
		// |  <IN> -> uiCol		- The pixel column number.                                                            |
		// |  <IN> -> uiRow		- The pixel row number.                                                               |
		// |  <IN> -> uiCols	- The image column size ( in pixels ).                                                |
		// |  <IN> -> uiRows	- The image row size ( in pixels ).                                                   |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> T
		CArcImage<T>::getPixel( const T* pBuf, const std::uint32_t uiCol, const std::uint32_t uiRow, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			verifyBuffer( pBuf );

			verifyColumn( uiCol, uiCols );

			verifyRow( uiRow, uiRows );

			return pBuf[ uiCol + ( uiRow * uiCols ) ];
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getRegion                                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		// |   Returns a region of an image.                                                                          |
		// |                                                                                                          |
		// |  <IN> -> pBuf		- Pointer to the image data buffer.                                                   |
		// |  <IN> -> uiCol1	- The start column.                                                                   |
		// |  <IN> -> uiCol2	- The end column.                                                                     |
		// |  <IN> -> uiRow1	- The start row.                                                                      |
		// |  <IN> -> uiRow2	- The end row.                                                                        |
		// |  <IN> -> uiCols	- The image column size ( in pixels ).                                                |
		// |  <IN> -> uiRows	- The image row size ( in pixels ).                                                   |
		// |  <OUT>-> uiCount	- The pixel count of the returned array.                                              |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>  std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>>
		CArcImage<T>::getRegion( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1,
								 const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount )
		{
			verifyBuffer( pBuf );

			verifyColumn( uiCol1, uiCols );

			verifyColumn( uiCol2, uiCols );

			verifyRow( uiRow1, uiRows );

			verifyRow( uiRow2, uiRows );

			verifyRangeOrder( uiCol1, uiCol2 );

			verifyRangeOrder( uiRow1, uiRow2 );

			uiCount = ( ( uiCol2 - uiCol1 ) * ( uiRow2 - uiRow1 ) );

			std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>> pRegion( new T[ uiCount ], arc::gen3::image::ArrayDeleter<T>() );

			if ( pRegion == nullptr )
			{
				throwArcGen3Error( "Failed to allocate region data buffer!"s );
			}

			auto uiRegionIndex = 0U;

			for ( auto r = uiRow1; r < uiRow2; r++ )
			{
				for ( auto c = uiCol1; c < uiCol2; c++ )
				{
					pRegion[ uiRegionIndex ] = pBuf[ c + r * uiCols ];

					uiRegionIndex++;
				}
			}

			return pRegion;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getRow                                                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns all or part of a image row.                                                                     |
		// |                                                                                                          |
		// |  <IN> -> pBuf	  - Pointer to the image data buffer.                                                     |
		// |  <IN> -> uiCol1  - The start column.                                                                     |
		// |  <IN> -> uiCol2  - The end column.                                                                       |
		// |  <IN> -> uiRow   - The row to read from.                                                                 |
		// |  <IN> -> uiCols  - The image column size ( in pixels ).                                                  |
		// |  <IN> -> uiRows  - The image row size ( in pixels ).                                                     |
		// |  <IN> -> uiCount - The pixel count of the returned array.                                                |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>>
		CArcImage<T>::getRow( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount )
		{
			verifyBuffer( pBuf );

			verifyRangeOrder( uiCol1, uiCol2 );

			verifyRow( uiRow, uiRows );

			uiCount = ( ( uiCol2 - uiCol1 ) == 0 ? 1 : ( uiCol2 - uiCol1 ) );

			std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>> pRow( new T[ uiCount ], arc::gen3::image::ArrayDeleter<T>() );

			if ( pRow == nullptr )
			{
				throwArcGen3Error( "Failed to allocate row data buffer!"s );
			}

			copyMemory( reinterpret_cast< void* >( pRow.get() ),
						reinterpret_cast< void* >( const_cast< T* >( &pBuf[ uiCol1 + uiRow * uiCols ] ) ),
						( uiCount * sizeof( T ) ) );

			return pRow;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getCol                                                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns all or part of a image row.                                                                     |
		// |                                                                                                          |
		// |  <IN> -> pBuf	  - Pointer to the image data buffer.                                                     |
		// |  <IN> -> uiCol   - The column to read from                                                               |
		// |  <IN> -> uiRow1  - The start row.                                                                        |
		// |  <IN> -> uiRow2  - The end row.                                                                          |
		// |  <IN> -> uiCols  - The image column size ( in pixels ).                                                  |
		// |  <IN> -> uiCount - The pixel count of the returned array.                                                |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>>
		CArcImage<T>::getCol( const T* pBuf, const std::uint32_t uiCol, const std::uint32_t uiRow1, const std::uint32_t uiRow2, const std::uint32_t uiCols, std::uint32_t& uiCount )
		{
			verifyBuffer( pBuf );

			verifyRangeOrder( uiRow1, uiRow2 );

			verifyColumn( uiCol, uiCols );

			uiCount = ( ( uiRow2 - uiRow1 ) == 0 ? 1 : ( uiRow2 - uiRow1 ) );

			std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>> pCol( new T[ uiCount ], arc::gen3::image::ArrayDeleter<T>() );

			if ( pCol == nullptr )
			{
				throwArcGen3Error( "Failed to allocate row data buffer!"s );
			}

			for ( std::uint32_t row = uiRow1, i = 0; row < uiRow2; row++, i++ )
			{
				pCol.get()[ i ] = pBuf[ uiCol + row * uiCols ];
			}
			
			return pCol;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getRowArea                                                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns a row of pixel data where each value is the average over the specified range of columns.        |
		// |                                                                                                          |
		// |  <IN> -> pBuf	  - Pointer to the image data buffer.                                                     |
		// |  <IN> -> uiCol1  - The start column.                                                                     |
		// |  <IN> -> uiCol2  - The end column.                                                                       |
		// |  <IN> -> uiRow1  - The start row.                                                                        |
		// |  <IN> -> uiRow2  - The end row.                                                                          |
		// |  <IN> -> uiCols  - The image column size ( in pixels ).                                                 |
		// |  <IN> -> uiRows  - The image row size ( in pixels ).                                                     |
		// |  <IN> -> uiCount - The pixel count of the returned array.                                                |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<double[], arc::gen3::image::ArrayDeleter<double>>
		CArcImage<T>::getRowArea( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1,
								  const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount )
		{
			double  gRowSum = 0.0;

			verifyRow( uiRow1, uiRows );

			verifyRow( uiRow2, uiRows );

			verifyColumn( uiCol1, uiCols );

			verifyColumn( uiCol2, uiCols );

			verifyRangeOrder( uiCol1, uiCol2 );

			verifyRangeOrder( uiRow1, uiRow2 );

			verifyBuffer( pBuf );

			uiCount = ( ( uiRow2 - uiRow1 ) == 0 ? 1 : ( uiRow2 - uiRow1 ) );
			
			std::unique_ptr<double[], arc::gen3::image::ArrayDeleter<double>> pAreaBuf( new double[ uiCount ], arc::gen3::image::ArrayDeleter<double>() );

			if ( pAreaBuf == nullptr )
			{
				throwArcGen3Error( "Failed to allocate row data buffer!"s );
			}

			for ( std::uint32_t row = uiRow1, i = 0; row < uiRow2; row++, i++ )
			{
				gRowSum = 0;

				for ( std::uint32_t col = uiCol1; col < uiCol2; col++ )
				{
					gRowSum += pBuf[ col + row * uiCols ];
				}

				pAreaBuf.get()[ i ] = gRowSum / ( static_cast< double >( uiCol2 - uiCol1 ) );
			}

			return pAreaBuf;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getColArea                                                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns a column of pixel data where each value is the average over the specified range of rows.        |
		// |                                                                                                          |
		// |  <IN> -> pBuf	  - Pointer to the image data buffer.                                                     |
		// |  <IN> -> uiCol1  - The start column.                                                                     |
		// |  <IN> -> uiCol2  - The end column.                                                                       |
		// |  <IN> -> uiRow1  - The start row.                                                                        |
		// |  <IN> -> uiRow2  - The end row.                                                                          |
		// |  <IN> -> uiCols  - The image column size ( in pixels ).                                                 |
		// |  <IN> -> uiRows  - The image row size ( in pixels ).                                                     |
		// |  <IN> -> uiCount - The pixel count of the returned array.                                                |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<double[], arc::gen3::image::ArrayDeleter<double>>
		CArcImage<T>::getColArea( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1,
								  const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount )
		{
			double  gColSum = 0.0;

			verifyRow( uiRow1, uiRows );
				
			verifyRow( uiRow2, uiRows );

			verifyColumn( uiCol1, uiCols );

			verifyColumn( uiCol2, uiCols );

			verifyRangeOrder( uiCol1, uiCol2 );

			verifyRangeOrder( uiRow1, uiRow2 );

			verifyBuffer( pBuf );

			uiCount = ( ( uiCol2 - uiCol1 ) == 0 ? 1 : ( uiCol2 - uiCol1 ) );

			std::unique_ptr<double[], arc::gen3::image::ArrayDeleter<double>> pAreaBuf( new double[ uiCount ], arc::gen3::image::ArrayDeleter<double>() );

			if ( pAreaBuf == nullptr )
			{
				throwArcGen3Error( "Failed to allocate column data buffer!"s );
			}

			for ( std::uint32_t col = uiCol1, i = 0; col < uiCol2; col++, i++ )
			{
				gColSum = 0;

				for ( std::uint32_t row = uiRow1; row < uiRow2; row++ )
				{
					gColSum += pBuf[ col + row * uiCols ];
				}

				pAreaBuf.get()[ i ] = gColSum / ( static_cast< double >( uiRow2 - uiRow1 ) );
			}

			return pAreaBuf;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getStats                                                                                                |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Calculates the image min, max, mean, variance, standard deviation, total pixel count and saturated      |
		// |  pixel count over the specified image buffer cols and rows.                                              |
		// |                                                                                                          |
		// |  <IN> -> pBuf	 - Pointer to the image data buffer.                                                      |
		// |  <IN> -> uiCol1 - The start column.                                                                      |
		// |  <IN> -> uiCol2 - The end column.                                                                        |
		// |  <IN> -> uiRow1 - The start row.                                                                         |
		// |  <IN> -> uiRow2 - The end row.                                                                           |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<arc::gen3::image::CStats>
		CArcImage<T>::getStats( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1, const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			double gDevSqrdSum = 0.0;
			double gVal = 0.0;
			double gSum = 0.0;

			std::remove_const_t<decltype( uiRow2 )> uiLocalRow2 = uiRow2;
			std::remove_const_t<decltype( uiCol2 )> uiLocalCol2 = uiCol2;

			verifyRow( uiRow1, uiRows );
				
			verifyRow( uiRow2, uiRows );

			verifyColumn( uiCol1, uiCols );

			verifyColumn( uiCol2, uiCols );
				
			verifyRangeOrder( uiCol1, uiCol2 );

			verifyRangeOrder( uiRow1, uiRow2 );

			verifyBuffer( pBuf );

			double gMaxBpp = maxTVal();

			std::unique_ptr<arc::gen3::image::CStats> pStats( new arc::gen3::image::CStats() );

			if ( pStats == nullptr )
			{
				throwArcGen3Error( "Failed to allocate stats data buffer!"s );
			}

			pStats->gMin = gMaxBpp;
			
			if ( uiRow1 == uiRow2 ) { uiLocalRow2++; }
			if ( uiCol1 == uiCol2 ) { uiLocalCol2++; }

			double gTotalPixelCount = ( static_cast< double >( uiLocalRow2 - uiRow1 ) * static_cast< double >( uiLocalCol2 - uiCol1 ) );

			pStats->gTotalPixels = gTotalPixelCount;

			for ( std::uint32_t i = uiRow1; i < uiLocalRow2; i++ )
			{
				for ( std::uint32_t j = uiCol1; j < uiLocalCol2; j++ )
				{
					gVal = static_cast< double >( pBuf[ j + i * uiCols ] );

					//
					// Determine min/max values
					//
					if ( gVal < pStats->gMin )
					{
						pStats->gMin = gVal;
					}

					else if ( gVal > pStats->gMax )
					{
						pStats->gMax = gVal;
					}

					//
					// Monitor for saturated pixels
					//
					//if ( gVal >= ( T_SIZE( T ) - 1 ) )
					if ( gVal >= ( maxTVal() - 1 ) )
					{
						pStats->gSaturatedCount++;
					}

					gSum += gVal;
				}
			}

			// Calculate mean
			pStats->gMean = gSum / gTotalPixelCount;

			for ( std::uint32_t i = uiRow1; i < uiLocalRow2; i++ )
			{
				for ( std::uint32_t j = uiCol1; j < uiLocalCol2; j++ )
				{
					double gPixVal = static_cast< double >( pBuf[ j + i * uiCols ] );
					
					gDevSqrdSum += std::pow( ( gPixVal - pStats->gMean ), 2 );
				}
			}

			pStats->gVariance = gDevSqrdSum / gTotalPixelCount;
			pStats->gStdDev = std::sqrt( pStats->gVariance );

			return pStats;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getStats                                                                                                |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Calculates the image min, max, mean, variance, standard deviation, total pixel count and saturated      |
		// |  pixel count over the entire image.                                                                      |
		// |                                                                                                          |
		// |  <IN> -> pBuf	 - Pointer to the image data buffer.                                                      |
		// |  <IN> -> uiCol1 - The start column.                                                                      |
		// |  <IN> -> uiCol2 - The end column.                                                                        |
		// |  <IN> -> uiRow1 - The start row.                                                                         |
		// |  <IN> -> uiRow2 - The end row.                                                                           |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		std::unique_ptr<arc::gen3::image::CStats> CArcImage<T>::getStats( const T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			return getStats( pBuf, 0, uiCols, 0, uiRows, uiCols, uiRows );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getDiffStats                                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Calculates the min, max, mean, variance, standard deviation, total pixel count and saturated pixel      |
		// |  count for each image as well as the difference mean, variance and standard deviation over the specified |
		// |  image buffer cols and rows. This is used for photon transfer curves( PTC ).The two images MUST be the   |
		// |  same size or the methods behavior is undefined as this cannot be verified using the given parameters.   |
		// |                                                                                                          |
		// |  <IN> -> pBuf1	 - Pointer to the first image buffer.                                                     |
		// |  <IN> -> pBuf2	 - Pointer to the second image buffer.                                                    |
		// |  <IN> -> uiCol1 - The start column.                                                                      |
		// |  <IN> -> uiCol2 - The end column.                                                                        |
		// |  <IN> -> uiRow1 - The start row.                                                                         |
		// |  <IN> -> uiRow2 - The end row.                                                                           |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<arc::gen3::image::CDifStats>
		CArcImage<T>::getDiffStats( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1, const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			double gSum		= 0.0;
			double gDifSum	= 0.0;
			double gVal1	= 0.0;
			double gVal2	= 0.0;

			std::remove_const_t<decltype( uiRow2 )> uiLocalRow2 = uiRow2;
			std::remove_const_t<decltype( uiCol2 )> uiLocalCol2 = uiCol2;

			verifyRow( uiRow1, uiRows );

			verifyRow( uiRow2, uiRows );

			verifyColumn( uiCol1, uiCols );

			verifyColumn( uiCol2, uiCols );

			verifyRangeOrder( uiCol1, uiCol2 );

			verifyRangeOrder( uiRow1, uiRow2 );

			verifyBuffer( pBuf1 );

			verifyBuffer( pBuf2 );

			if ( uiRow1 == uiRow2 ) { uiLocalRow2++; }
			if ( uiCol1 == uiCol2 ) { uiLocalCol2++; }

			double gTotalPixelCount = ( static_cast< double >( uiLocalRow2 - uiRow1 ) * static_cast<double>( uiLocalCol2 - uiCol1 ) );

			std::unique_ptr<arc::gen3::image::CDifStats> pDifStats( new arc::gen3::image::CDifStats() );

			pDifStats->cStats1.set( *getStats( pBuf1, uiCol1, uiLocalCol2, uiRow1, uiLocalRow2, uiCols, uiRows ).get() );

			pDifStats->cStats2.set( *getStats( pBuf2, uiCol1, uiLocalCol2, uiRow1, uiLocalRow2, uiCols, uiRows ).get() );

			for ( std::uint32_t i = uiRow1; i < uiLocalRow2; i++ )
			{
				for ( std::uint32_t j = uiCol1; j < uiLocalCol2; j++ )
				{
					gVal1 = static_cast< double >( pBuf1[ j + i * uiCols ] );

					gVal2 = static_cast< double >( pBuf2[ j + i * uiCols ] );

					gSum += ( gVal1 - gVal2 );

					gDifSum += ( std::pow( ( pDifStats->cStats2.gMean - gVal2 ) - ( pDifStats->cStats1.gMean - gVal1 ), 2 ) );
				}
			}

			pDifStats->cDiffStats.gMean = std::fabs( gSum / gTotalPixelCount );
			pDifStats->cDiffStats.gVariance = gDifSum / gTotalPixelCount;
			pDifStats->cDiffStats.gStdDev = std::sqrt( pDifStats->cDiffStats.gVariance );

			return pDifStats;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getDiffStats                                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Calculates the min, max, mean, variance, standard deviation, total pixel count and saturated pixel      |
		// |  count for each image as well as the difference mean, variance and standard deviation over the entire    |
		// |  image buffer. This is used for photon transfer curves( PTC ).The two images MUST be the same size or    |
		// |  the methods behavior is undefined as this cannot be verified using the given parameters.                |
		// |                                                                                                          |
		// |  <IN> -> pBuf1	 - Pointer to the first image buffer.                                                     |
		// |  <IN> -> pBuf2	 - Pointer to the second image buffer.                                                    |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<arc::gen3::image::CDifStats>
		CArcImage<T>::getDiffStats( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			return getDiffStats( pBuf1, pBuf2, 0, uiCols, 0, uiRows, uiCols, uiRows );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  histogram                                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Calculates the histogram over the specified image buffer columns and rows.                              |
		// |                                                                                                          |
		// |  <IN> -> pBuf		- Pointer to the image buffer.                                                        |
		// |  <IN> -> uiCol1	- The start column.                                                                   |
		// |  <IN> -> uiCol2	- The end column.                                                                     |
		// |  <IN> -> uiRow1	- The start row.                                                                      |
		// |  <IN> -> uiRow2	- The end row.                                                                        |
		// |  <IN> -> uiCols	- The image column size ( in pixels ).                                                |
		// |  <IN> -> uiRows	- The image row size ( in pixels ).                                                   |
		// |  <IN> -> uiCount	- The element count of the returned array. The size of the array depends on the       |
		// |                      image data type.                                                                    |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<std::uint32_t[], arc::gen3::image::ArrayDeleter<std::uint32_t>>
		CArcImage<T>::histogram( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1,
								 const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount )
		{
			std::remove_const_t<decltype( uiRow2 )> uiLocalRow2 = uiRow2;
			std::remove_const_t<decltype( uiCol2 )> uiLocalCol2 = uiCol2;

			verifyRow( uiRow1, uiRows );

			verifyRow( uiRow2, uiRows );

			verifyColumn( uiCol1, uiCols );

			verifyColumn( uiCol2, uiCols );

			verifyRangeOrder( uiCol1, uiCol2 );

			verifyRangeOrder( uiRow1, uiRow2 );

			verifyBuffer( pBuf );

			std::unique_ptr<std::uint32_t[], arc::gen3::image::ArrayDeleter<std::uint32_t>>
			pHist( new std::uint32_t[ maxTVal() ], arc::gen3::image::ArrayDeleter<std::uint32_t>() );

			if ( pHist == nullptr )
			{
				throwArcGen3Error( "Failed to allocate histogram data buffer!"s );
			}

			uiCount = maxTVal();

			zeroMemory( pHist.get(), uiCount * sizeof( std::uint32_t ) );

			if ( uiRow1 == uiRow2 ) { uiLocalRow2++; }
			if ( uiCol1 == uiCol2 ) { uiLocalCol2++; }

			for ( std::uint32_t i = uiRow1; i < uiLocalRow2; i++ )
			{
				for ( std::uint32_t j = uiCol1; j < uiLocalCol2; j++ )
				{
					pHist.get()[ pBuf[ j + i * uiCols ] ]++;
				}
			}
			
			return pHist;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  histogram                                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Calculates the histogram over the entire image buffer.                                                  |
		// |                                                                                                          |
		// |  <IN> -> pBuf		- Pointer to the image buffer.                                                        |
		// |  <IN> -> uiCols	- The image column size ( in pixels ).                                                |
		// |  <IN> -> uiRows	- The image row size ( in pixels ).                                                   |
		// |  <IN> -> uiCount	- The element count of the returned array. The size of the array depends on the       |
		// |                      image data type.                                                                    |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<std::uint32_t[], arc::gen3::image::ArrayDeleter<std::uint32_t>>
		CArcImage<T>::histogram( const T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount )
		{
			return histogram( pBuf, 0, uiCols, 0, uiRows, uiCols, uiRows, uiCount );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  add                                                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Adds two image buffers together pixel by pixel. The result is a pointer to a 64-bit buffer.             |
		// |                                                                                                          |
		// |  <IN> -> pBuf1	 - Pointer to the first image buffer.                                                     |
		// |  <IN> -> pBuf2	 - Pointer to the second image buffer.                                                    |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<std::uint64_t[], arc::gen3::image::ArrayDeleter<std::uint64_t>>
		CArcImage<T>::add( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			verifyBuffer( pBuf1 );
	
			verifyBuffer( pBuf2 );

			std::uint32_t uiLength = ( uiCols * uiRows );

			std::unique_ptr< std::uint64_t[], arc::gen3::image::ArrayDeleter< std::uint64_t > > pAdd( new std::uint64_t[ uiLength ], arc::gen3::image::ArrayDeleter< std::uint64_t >() );

			if ( pAdd == nullptr )
			{
				throwArcGen3Error( "Failed to allocate addition data buffer!"s );
			}

			for ( decltype( uiLength ) i = 0; i < uiLength; i++ )
			{
				pAdd.get()[ i ] = static_cast< std::uint64_t >( pBuf1[ i ] ) + static_cast< std::uint64_t >( pBuf2[ i ] );
			}

			return pAdd;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  subtract                                                                                                |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Subtracts two image buffers pixel by pixel. The result is a pointer to a buffer with the same data      |
		// |  type as the two images being subtracted. Buffer two is subtracted from buffer one.                      |
		// |                                                                                                          |
		// |  <IN> -> pBuf1	 - Pointer to the first image buffer.                                                     |
		// |  <IN> -> pBuf2	 - Pointer to the second image buffer.                                                    |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>>
		CArcImage<T>::subtract( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			verifyBuffer( pBuf1 );
	
			verifyBuffer( pBuf2 );

			std::uint32_t uiLength = ( uiCols * uiRows );

			std::unique_ptr< T[], arc::gen3::image::ArrayDeleter< T > > pSub( new T[ uiLength ], arc::gen3::image::ArrayDeleter< T >() );

			if ( pSub == nullptr )
			{
				throwArcGen3Error( "Failed to allocate subtraction data buffer!"s );
			}

			for ( decltype( uiLength ) i = 0; i < uiLength; i++ )
			{
				pSub.get()[ i ] = pBuf1[ i ] - pBuf2[ i ];
			}

			return pSub;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  subtractHalves                                                                                          |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Subtracts one half of an image from the other. The first half of the image buffer is replaced with      |
		// |  the new image.                                                                                          |
		// |                                                                                                          |
		// |  <IN> -> pBuf	 - Pointer to the image buffer. Result is placed in this buffer.                          |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error, std::invalid_argument                                                        |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::subtractHalves( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			verifyBuffer( pBuf );

			if ( ( uiRows % 2 ) != 0 )
			{
				throwArcGen3InvalidArgument( "Image must have an even number of rows [ %u ]", uiRows );
			}

			T* pBuf1 = pBuf;

			T* pBuf2 = pBuf + ( static_cast< std::uint64_t >( ( uiRows / 2 ) ) * static_cast<std::uint64_t>( uiCols ) );

			for ( std::uint32_t i = 0; i < ( ( uiRows / 2 ) * uiCols ); i++ )
			{
				pBuf1[ i ] -= pBuf2[ i ];
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  divide                                                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Divides two image buffers pixel by pixel.                                                               |
		// |                                                                                                          |
		// |  <IN> -> pBuf1	 - Pointer to the first image buffer. Result is placed in this buffer.                    |
		// |  <IN> -> pBuf2	 - Pointer to the second image buffer.                                                    |
		// |  <IN> -> uiCols - The image column size ( in pixels ).                                                   |
		// |  <IN> -> uiRows - The image row size ( in pixels ).                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>>
		CArcImage<T>::divide( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			verifyBuffer( pBuf1 );
	
			verifyBuffer( pBuf2 );

			std::uint32_t uiLength = ( uiCols * uiRows );

			std::unique_ptr< T[], arc::gen3::image::ArrayDeleter< T > > pDiv( new T[ uiLength ], arc::gen3::image::ArrayDeleter< T >() );

			if ( pDiv == nullptr )
			{
				throwArcGen3Error( "Failed to allocate division data buffer!"s );
			}

			for ( decltype( uiLength ) i = 0; i < uiLength; i++ )
			{
				if ( pBuf2[ i ] != 0 )
				{
					pDiv.get()[ i ] = pBuf1[ i ] / pBuf2[ i ];
				}

				else
				{
					pDiv.get()[ i ] = 0;
				}
			}

			return pDiv;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  copy                                                                                                    |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Copies the source image buffer to the destination image buffer. The source buffer must be less than or  |
		// |  equal in dimensions to the destination buffer.                                                          |
		// |                                                                                                          |
		// |  <IN> -> pDstBuf - Pointer to the destination image buffer. Result is placed in this buffer.             |
		// |  <IN> -> pSrcBuf - Pointer to the source image buffer.                                                   |
		// |  <IN> -> uiCols  - The number of image columns to copy ( in pixels ).                                    |
		// |  <IN> -> uiRows  - The number of image rows to copy ( in pixels ).                                       |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::copy( T* pDstBuf, const T* pSrcBuf, const std::uint32_t uiCols, const std::uint32_t uiRows )
		{
			verifyBuffer( pDstBuf );

			verifyBuffer( pSrcBuf );

			copy( pDstBuf, pSrcBuf, ( uiCols * uiRows * static_cast< std::uint32_t >( sizeof( T ) ) ) );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  copy                                                                                                    |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Copies the source image buffer to the destination image buffer. The source buffer must be less than or  |
		// |  equal in dimensions to the destination buffer.                                                          |
		// |                                                                                                          |
		// |  <IN> -> pDstBuf	- Pointer to the destination image buffer. Result is placed in this buffer.           |
		// |  <IN> -> pSrcBuf	- Pointer to the source image buffer.                                                 |
		// |  <IN> -> uiSize	- The number of image bytes to copy.                                                  |
		// |                                                                                                          |
		// |  Throws std::runtime_error on error.                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::copy( T* pDstBuf, const T* pSrcBuf, const std::uint32_t uiSize )
		{
			verifyBuffer( pDstBuf );
	
			verifyBuffer( pSrcBuf );

			copyMemory( pDstBuf, const_cast< T* >( pSrcBuf ), uiSize );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  maxTVal                                                                                                 |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Determines the maximum value for the data type currently in use.                                        |
		// |                                                                                                          |
		// |  Example, for std::uint16_t: 2^16 = 65536.                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> std::uint32_t CArcImage<T>::maxTVal()
		{
			auto gExponent = ( ( sizeof( T ) == sizeof( arc::gen3::image::BPP_32 ) ) ? 20 : ( sizeof( T ) * 8 ) );

			return static_cast< std::uint32_t >( std::pow( 2.0, gExponent ) );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  drawSemiCircle                                                                                          |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Draws a semi-circle on the specified buffer.                                                            |
		// |                                                                                                          |
		// |  <IN> -> uiXCenter		- uiX position of circle center point.                                            |
		// |  <IN> -> uiYCenter		- uiY position of circle center point.                                            |
		// |  <IN> -> uiRadius		- The radius of the circle.                                                       |
		// |  <IN> -> gStartAngle	- The start angle of the semi-circle.                                             |
		// |  <IN> -> gEndAngle		- The end angle of the semi-circle.                                               |
		// |  <IN> -> uiCols		- The image column size ( in pixels ).                                            |
		// |  <IN> -> pBuf			- Pointer to the image data buffer.                                               |
		// |  <IN> -> uiColor		- Color to draw circle ( default = 0 ).                                           |
		// |                                                                                                          |
		// |  Throws std::invalid_argument                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T> void CArcImage<T>::drawSemiCircle( const std::uint32_t uiXCenter, const std::uint32_t uiYCenter,
																 const std::uint32_t uiRadius, const double gStartAngle, const double gEndAngle,
																 const std::uint32_t uiCols, T* pBuf, const T uiColor )
		{
			if ( pBuf != nullptr )
			{
				for ( double angle = gStartAngle; angle < gEndAngle; angle += 0.1 )
				{
					std::uint32_t uiX = static_cast< std::uint32_t >( uiRadius * std::cos( angle * DEG2RAD ) + uiXCenter );
					std::uint32_t uiY = static_cast< std::uint32_t >( uiRadius * std::sin( angle * DEG2RAD ) + uiYCenter );

					pBuf[ uiX + uiY * uiCols ] = static_cast< T >( uiColor );
				}
			}

			else
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  drawFillCircle                                                                                          |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Draws a filled circle on the specified buffer.                                                          |
		// |                                                                                                          |
		// |  <IN> -> uiXCenter	- uiX position of circle center point.                                                |
		// |  <IN> -> uiYCenter	- uiY position of circle center point.                                                |
		// |  <IN> -> uiRadius	- The radius of the circle.                                                           |
		// |  <IN> -> uiCols	- The image column size ( in pixels ).                                                |
		// |  <IN> -> pBuf		- Pointer to the image data buffer.                                                   |
		// |  <IN> -> uiRows	- The image row size ( in pixels ).                                                   |
		// |  <IN> -> uiColor	- Color to draw circle ( default = 0 ).                                               |
		// |                                                                                                          |
		// |  Throws std::invalid_argument                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		void CArcImage<T>::drawFillCircle( const std::uint32_t uiXCenter, const std::uint32_t uiYCenter, const std::uint32_t uiRadius, const std::uint32_t uiCols, T* pBuf, const T uiColor )
		{
			if ( pBuf != nullptr )
			{
				for ( std::remove_const_t<decltype( uiRadius )> r = 0; r < uiRadius; r++ )
				{
					for ( double angle = 0; angle < 360; angle += 0.1 )
					{
						std::uint32_t uiX = static_cast< std::uint32_t >( r * std::cos( angle * DEG2RAD ) + uiXCenter );
						std::uint32_t uiY = static_cast< std::uint32_t >( r * std::sin( angle * DEG2RAD ) + uiYCenter );

						pBuf[ uiX + uiY * uiCols ] = static_cast< T >( uiColor );
					}
				}
			}

			else
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  drawGradientFillCircle                                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Draws a gradient filled circle on the specified buffer.                                                 |
		// |                                                                                                          |
		// |  <IN> -> uiRadius	- The radius of the circle.                                                           |
		// |  <IN> -> uiCols	- The image column size ( in pixels ).                                                |
		// |  <IN> -> uiRows	- The image row size ( in pixels ).                                                   |
		// |  <IN> -> pBuf		- Pointer to the image data buffer.                                                   |
		// |                                                                                                          |
		// |  Throws std::invalid_argument                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		void CArcImage<T>::drawGradientFillCircle( const std::uint32_t uiRadius, const std::uint32_t uiCols, const std::uint32_t uiRows, T* pBuf )
		{
			if ( pBuf == nullptr )
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}

			for ( std::remove_const_t<decltype( uiRadius )> r = 0; r < uiRadius; r++ )
			{
				drawCircle( ( uiCols / 2 ),
							( uiRows / 2 ),
							( uiRadius - r ),
							uiCols,
							pBuf,
							static_cast< std::uint32_t >( r + ( ( maxTVal() - 1 ) / uiRadius ) ) );
			}
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  drawCircle                                                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Draws a circle on the specified buffer.                                                                 |
		// |                                                                                                          |
		// |  <IN> -> uiXCenter	- uiX position of circle center point.                                                |
		// |  <IN> -> uiYCenter	- uiY position of circle center point.                                                |
		// |  <IN> -> uiRadius	- The radius of the circle.                                                           |
		// |  <IN> -> uiCols	- The image column size ( in pixels ).                                                |
		// |  <IN> -> pBuf		- Pointer to the image data buffer.                                                   |
		// |  <IN> -> uiColor	- Color to draw circle ( default = 0 ).                                               |
		// |                                                                                                          |
		// |  Throws std::invalid_argument                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		template <typename T>
		void CArcImage<T>::drawCircle( const std::uint32_t uiXCenter, const std::uint32_t uiYCenter, const std::uint32_t uiRadius, const std::uint32_t uiCols, T* pBuf, const T uiColor )
		{
			if ( pBuf != nullptr )
			{
				for ( double angle = 0; angle < 360; angle += 0.1 )
				{
					std::uint32_t uiX = static_cast< std::uint32_t >( uiRadius * std::cos( angle * DEG2RAD ) + uiXCenter );
					std::uint32_t uiY = static_cast< std::uint32_t >( uiRadius * std::sin( angle * DEG2RAD ) + uiYCenter );

					pBuf[ uiX + uiY * uiCols ] = static_cast< T >( uiColor );
				}
			}

			else
			{
				throwArcGen3InvalidArgument( "Invalid buffer reference ( nullptr )."s );
			}
		}


	}	// end gen3 namespace
}		// end arc namespace


/** Explicit instantiations - These are the only allowed instantiations of this class */
template class arc::gen3::CArcImage<arc::gen3::image::BPP_16>;
template class arc::gen3::CArcImage<arc::gen3::image::BPP_32>;



// +------------------------------------------------------------------------------------------------+
// |  default_delete definition                                                                     |
// +------------------------------------------------------------------------------------------------+
// |  Creates a modified version of the std::default_delete class for use by all                    |
// |  std::unique_ptr's that wrap a CStats/CDifStats object.                                        |
// +------------------------------------------------------------------------------------------------+
namespace std
{

	void default_delete<arc::gen3::image::CStats>::operator()( arc::gen3::image::CStats* pObj )
	{
		if ( pObj != nullptr )
		{
			delete pObj;
		}
	}


	void default_delete<arc::gen3::image::CDifStats>::operator()( arc::gen3::image::CDifStats* pObj )
	{
		if ( pObj != nullptr )
		{
			delete pObj;
		}
	}

}
