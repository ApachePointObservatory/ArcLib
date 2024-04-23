// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcImage.h  ( Gen3 )                                                                                    |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the standard ARC image operations interface.                                         |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: March 26, 2020                                                              |
// |                                                                                                                  |
// |  Copyright 2014 Astronomical Research Cameras, Inc. All rights reserved.                                         |
// +------------------------------------------------------------------------------------------------------------------+
/**< @file CArcImage.h */

#ifndef _GEN3_CARCIMAGE_H_
#define _GEN3_CARCIMAGE_H_

#ifdef _WINDOWS
#pragma warning( disable: 4251 )
#endif

#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <memory>
#include <functional>
#include <cmath>

#include <CArcImageDllMain.h>
#include <CArcBase.h>



namespace arc
{
	namespace gen3
	{
		namespace image
		{

			/** \typedef BPP_16
			 *  16 bits-per-pixel image data
			 */
			using BPP_16 = std::uint16_t;


			/** \typedef BPP_32
			 *  32 bits-per-pixel image data
			 */
			using BPP_32 = std::uint32_t;


			/** @class CAvgStats
			 *  Average image statistics info class
			 */
			class GEN3_CARCIMAGE_API CAvgStats
			{
				public:

					/** Constructor
					 *  @param uiChannelCount - The number of channels in the image(s)
					 */
					CAvgStats( const std::uint32_t uiChannelCount )
					{
						this->uiChannelCount = uiChannelCount;

						gMeanOfMeans = gStdDevOfMeans = gAvgAvgStdDev = 0;

						pAverageMin = new double[ uiChannelCount ];
						pAverageMax = new double[ uiChannelCount ];
						pAverageMean = new double[ uiChannelCount ];
						pAverageStdDev = new double[ uiChannelCount ];

						std::memset( pAverageMin, 0, ( uiChannelCount * sizeof( double ) ) );
						std::memset( pAverageMax, 0, ( uiChannelCount * sizeof( double ) ) );
						std::memset( pAverageMean, 0, ( uiChannelCount * sizeof( double ) ) );
						std::memset( pAverageStdDev, 0, ( uiChannelCount * sizeof( double ) ) );
					}

					/** Destructor
					 */
					~CAvgStats( void )
					{
						if ( pAverageMin != nullptr ) { delete[] pAverageMin; }
						if ( pAverageMax != nullptr ) { delete[] pAverageMax; }
						if ( pAverageMean != nullptr ) { delete[] pAverageMean; }
						if ( pAverageStdDev != nullptr ) { delete[] pAverageStdDev; }
					}

					double* pAverageMin;				/**< Average of the minimum value */
					double* pAverageMax;				/**< Average of the maximum value */
					double* pAverageMean;				/**< Average of the mean value */
					double* pAverageStdDev;				/**< Average of the standard deviation */

					double gMeanOfMeans;				/**< Average of the average of the mean values */
					double gStdDevOfMeans;				/**< Standard deviation of the mean values */
					double gAvgAvgStdDev;				/**< Average of the average standard deviation value */

					std::uint32_t uiChannelCount;		/**< Number of channels in the image(s) */
					std::uint32_t uiImageCount;			/**< Number of images used for the averaging */
			};


			/** @class CStats
			 *  Image statistics info class
			 */
			class GEN3_CARCIMAGE_API CStats
			{
				public:

					/** Default constructor
					 */
					CStats( void )
					{
						gMin = gMax = gMean = gVariance = gStdDev = gTotalPixels = gSaturatedCount = 0;
					}

					/** Copy constructor
					 *  @param rCStats - The CStats object to copy
					 */
					CStats( const CStats& rCStats )
					{
						gTotalPixels = rCStats.gTotalPixels;
						gMin = rCStats.gMin;
						gMax = rCStats.gMax;
						gMean = rCStats.gMean;
						gVariance = rCStats.gVariance;
						gStdDev = rCStats.gStdDev;
						gSaturatedCount = rCStats.gSaturatedCount;
					}

					/** Default destructor
					 */
					~CStats( void ) = default;

					/** Copies another instance of this class to this one.
					 *  @param rCStats - The CStats object to copy
					 */
					void set( const CStats& rCStats )
					{
						gTotalPixels = rCStats.gTotalPixels;
						gMin = rCStats.gMin;
						gMax = rCStats.gMax;
						gMean = rCStats.gMean;
						gVariance = rCStats.gVariance;
						gStdDev = rCStats.gStdDev;
						gSaturatedCount = rCStats.gSaturatedCount;
					}

					double gTotalPixels;		/**< The total number of pixels in the image */
					double gMin;				/**< The minimum value found in the image */
					double gMax;				/**< The maximum value found in the image */
					double gMean;				/**< The mean value of the pixels in the image */
					double gVariance;			/**< The variance of the pixels in the image. Measures how far spread out the pixel values are from their mean. */
					double gStdDev;				/**< The standard deviation of the pixels in the image. Measures the dispersion of the pixel values. */
					double gSaturatedCount;		/**< The number of saturated pixels in the iamge. Saturation pixel value is 65535 for 16-bit images */
			};


			/** @class CDifStats
			 *  Image difference statistics info class
			 */
			class GEN3_CARCIMAGE_API CDifStats
			{
				public:

					/** Default constructor
					 */
					CDifStats( void ) = default;

					/** Default destructor
					 */
					~CDifStats( void ) = default;

					CStats cStats1;			/**< Statistics values for image #1 */
					CStats cStats2;			/**< Statistics values for image #2 */
					CStats cDiffStats;		/**< Difference values for the cStats1 and cStats2 members of this class */
			};


			/** @struct ArrayDeleter
			 *  Returned array deleter
			 */
			template <typename T>
			struct GEN3_CARCIMAGE_API ArrayDeleter
			{
				void operator()( T* p ) const
				{
					if ( p != nullptr )
					{
						delete[] p;
					}
				}
			};

		};	// end image namespace



		/** @class CArcImage
		 *  ARC image processing class. WARNING - All methods within this class perform destructive operations
		 *  on the original image buffer. i.e. Original buffer contents will be modified in place.
		 *  @see arc::gen3::CArcBase
		 */
		template <typename T = arc::gen3::image::BPP_16>
		class GEN3_CARCIMAGE_API CArcImage : public arc::gen3::CArcBase
		{
		public:

			/** Constructor
			 *  Creates an image operations object.
			 */
			CArcImage( void ) = default;

			/** Destructor
			 */
			virtual ~CArcImage( void );

			/** Returns a textual representation of the library version.
			 *  @return A string representation of the library version.
			 */
			static const std::string version( void );

			/** Fills the specified buffer with the specified value.
			 *  @param pBuf		- Pointer to the image buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @param uiValue	- The value to fill the buffer with.
			 *  @throws std::invalid_argument
			 *  @throws std::out_of_range
			 */
			static void fill( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows, const T uiValue );

			/** Fills the specified buffer with the specified value.
			 *  @param pBuf		- Pointer to the image buffer.
			 *  @param uiBytes	- The number of bytes in the image data buffer.
			 *  @param uiValue	- The value to fill the buffer with.
			 *  @throws std::invalid_argument
			 *  @throws std::out_of_range
			 */
			static void fill( T* pBuf, const std::uint32_t uiBytes, const T uiValue );

			/** Fills the specified buffer with a gradient pattern.
			 *  @param pBuf		- Pointer to the image buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @throws std::invalid_argument
			 */
			static void fillWithGradient( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Fills the specified buffer with zeroes and puts a smiley face at the center.
			 *  @param pBuf		- Pointer to the image buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @throws std::invalid_argument
			 */
			static void fillWithSmiley( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Fills the specified buffer with a ramp image. Data has the form 0, 1, 2, ..., 65535, 0, 1, ....
			 *  @param pBuf		- Pointer to the image buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @throws std::invalid_argument
			 */
			static void fillWithRamp( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Verify a ramp synthetic image. Data has the form 0, 1, 2, ..., 65535, 0, 1, ....
			 *  @param pBuf		- Pointer to the image data buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @throws std::runtime_error
			 *  @throws std::invalid_argument
			 */
			static void containsValidRamp( const T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Count the number of pixels having the specified value.
			 *  @param pBuf		- Pointer to the image data buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @param uwValue	- The pixel value to include in the count.
			 *  @throws std::runtime_error
			 */
			static std::uint32_t countPixels( const T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows, const std::uint16_t uwValue );

			/** Count the number of pixels having the specified value.
			 *  @param pBuf			- Pointer to the image data buffer.
			 *  @param uiBufSize	- The image buffer size ( in pixels ).
			 *  @param uwValue		- The pixel value to include in the count.
			 *  @throws std::runtime_error
			 */
			static std::uint32_t countPixels( const T* pBuf, const std::uint32_t uiBufSize, const std::uint16_t uwValue );

			/** Returns the value of a pixel at the specified row and column.
			 *  @param pBuf		- Pointer to the image buffer.
			 *  @param uiCol	- The pixel column number.
			 *  @param uiRow	- The pixel row number.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @return The pixel value at the specified row and column.
			 *  @throws std::runtime_error
			 */
			static T getPixel( const T* pBuf, const std::uint32_t uiCol, const std::uint32_t uiRow, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Returns a region of an image.
			 *  @param pBuf		- Pointer to the image data buffer.
			 *  @param uiCol1	- The start column.
			 *  @param uiCol2	- The end column.
			 *  @param uiRow1	- The start row.
			 *  @param uiRow2	- The end row.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @param uiCount	- The pixel count of the returned array.
			 *  @return A std::unique_ptr to an array of type T, where CArcImage<T>.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>>
			getRegion( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1, const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount );

			/** Returns all or part of an image row.
			 *  @param pBuf		- Pointer to the image data buffer.
			 *  @param uiCol1	- The start column.
			 *  @param uiCol2	- The end column.
			 *  @param uiRow		- The row to read from.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @param uiCount	- The pixel count of the returned array.
			 *  @return A std::unique_ptr to an array of type T, where CArcImage<T>.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>>
			getRow( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount );

			/** Returns all or part of an image column.
			 *  @param pBuf		- Pointer to the image data buffer.
			 *  @param uiCol		- The column to read from.
			 *  @param uiRow1	- The start row.
			 *  @param uiRow2	- The end row.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiCount	- The pixel count of the returned array.
			 *  @return A std::unique_ptr to an array of type T, where CArcImage<T>.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>>
			getCol( const T* pBuf, const std::uint32_t uiCol, const std::uint32_t uiRow1, const std::uint32_t uiRow2, const std::uint32_t uiCols, std::uint32_t& uiCount );

			/** Returns a row of pixel data where each value is the average over the specified range of columns.
			 *  @param pBuf		- Pointer to the image data buffer.
			 *  @param uiCol1	- The start column.
			 *  @param uiCol2	- The end column.
			 *  @param uiRow1	- The start row.
			 *  @param uiRow2	- The end row.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @param uiCount	- The pixel count of the returned array.
			 *  @return A std::unique_ptr to an array of doubles.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<double[], arc::gen3::image::ArrayDeleter<double>>
			getRowArea( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1, const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount );

			/** Returns a column of pixel data where each value is the average over the specified range of rows.
			 *  @param pBuf		- Pointer to the image data buffer.
			 *  @param uiCol1	- The start column.
			 *  @param uiCol2	- The end column.
			 *  @param uiRow1	- The start row.
			 *  @param uiRow2	- The end row.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @param uiCount	- The pixel count of the returned array.
			 *  @return A std::unique_ptr to an array of doubles.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<double[], arc::gen3::image::ArrayDeleter<double>> getColArea( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2,
																								const std::uint32_t uiRow1, const std::uint32_t uiRow2,
																								const std::uint32_t uiCols, const std::uint32_t uiRows,
																								 std::uint32_t& uiCount );

			/** Calculates the image min, max, mean, variance, standard deviation, total pixel count and saturated
			 *  pixel count over the specified image buffer cols and rows.
			 *  @param pBuf		- Pointer to the image data buffer.
			 *  @param uiCol1	- The start column.
			 *  @param uiCol2	- The end column.
			 *  @param uiRow1	- The start row.
			 *  @param uiRow2	- The end row.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @return A std::unique_ptr to an arc::gen3::image::CStats object.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<arc::gen3::image::CStats>
			getStats( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1, const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Calculates the image min, max, mean, variance, standard deviation, total pixel count and
			 *  saturated pixel count over the entire image.
			 *  @param pBuf		- Pointer to the image data buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @return A std::unique_ptr to an arc::gen3::image::CStats object.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<arc::gen3::image::CStats> getStats( const T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Calculates the min, max, mean, variance, standard deviation, total pixel count and saturated pixel count for
			 *  each image as well as the difference mean, variance and standard deviation over the specified image buffer
			 *  cols and rows. This is used for photon transfer curves ( PTC ). The two images MUST be the same size or the
			 *  methods behavior is undefined as this cannot be verified using the given parameters.
			 *  @param pBuf1	- Pointer to the first image buffer.
			 *  @param pBuf2	- Pointer to the second image buffer.
			 *  @param uiCol1	- The start column.
			 *  @param uiCol2	- The end column.
			 *  @param uiRow1	- The start row.
			 *  @param uiRow2	- The end row.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @return A std::unique_ptr to an arc::gen3::image::CDifStats object.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<arc::gen3::image::CDifStats>
			getDiffStats( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1, const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Calculates the min, max, mean, variance, standard deviation, total pixel count and saturated pixel count for
			 *  each image as well as the difference mean, variance and standard deviation for the entire image. This is used
			 *  for photon transfer curves ( PTC ). The two images MUST be the same size or the methods behavior is undefined
			 *  as this cannot be verified using the given parameters.
			 *  @param pBuf1	- Pointer to the first image buffer.
			 *  @param pBuf2	- Pointer to the second image buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @return A std::unique_ptr to an arc::gen3::image::CDifStats object.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<arc::gen3::image::CDifStats> getDiffStats( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Calculates the histogram over the specified image buffer columns and rows.
			 *  @param pBuf		- Pointer to the image buffer.
			 *  @param uiCol1	- The start column.
			 *  @param uiCol2	- The end column.
			 *  @param uiRow1	- The start row.
			 *  @param uiRow2	- The end row.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @param uiCount	- The element count of the returned array.
			 *  @return A std::unique_ptr to an array of unsigned integers. The size of the array depends on the image data type.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<std::uint32_t[], arc::gen3::image::ArrayDeleter<std::uint32_t>>
			histogram( const T* pBuf, const std::uint32_t uiCol1, const std::uint32_t uiCol2, const std::uint32_t uiRow1, const std::uint32_t uiRow2, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount );

			/** Calculates the histogram over the entire image buffer.
			 *  @param pBuf		- Pointer to the image buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @param uiCount	- The element count of the returned array.
			 *  @return A std::unique_ptr to an array of unsigned integers. The size of the array depends on the image data type.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<std::uint32_t[], arc::gen3::image::ArrayDeleter<std::uint32_t>> histogram( const T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows, std::uint32_t& uiCount );

			/** Adds two buffers together pixel by pixel.
			 *  @param pBuf1	- Pointer to the first image buffer.
			 *  @param pBuf2	- Pointer to the second image buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @return A pointer to a 64-bit buffer.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<std::uint64_t[], arc::gen3::image::ArrayDeleter<std::uint64_t>> add( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Subtracts two image buffers pixel by pixel. Buffer two is subtracted from buffer one.
			 *  @param pBuf1	- Pointer to the first image buffer. Result is placed in this buffer.
			 *  @param pBuf2	- Pointer to the second image buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @return A pointer to a buffer with the same data type as the two images being subtracted.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>> subtract( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Subtracts one half of an image from the other. The first half of the image buffer is replaced with the new image.
			 *  @param pBuf		- Pointer to the image buffer. Result is placed in this buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @throws std::runtime_error
			 *  @throws std::invalid_argument
			 */
			static void subtractHalves( T* pBuf, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Divide two image buffers pixel by pixel.
			 *  @param pBuf1	- Pointer to the first image buffer.
			 *  @param pBuf2	- Pointer to the second image buffer.
			 *  @param uiCols	- The image column size ( in pixels ).
			 *  @param uiRows	- The image row size ( in pixels ).
			 *  @return A pointer to a buffer with the same data type as the two images being divided.
			 *  @throws std::runtime_error
			 */
			static std::unique_ptr<T[], arc::gen3::image::ArrayDeleter<T>> divide( const T* pBuf1, const T* pBuf2, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Copies the source image buffer to the destination image buffer. The source buffer must be less than or equal
			 *  in dimensions to the destination buffer.
			 *  @param pDstBuf	- Pointer to the destination image buffer. Result is placed in this buffer.
			 *  @param pSrcBuf	- Pointer to the source image buffer.
			 *  @param uiCols	- The number of image column pixels to copy.
			 *  @param uiRows	- The number of image row pixels to copy.
			 *  @throws std::runtime_error
			 */
			static void copy( T* pDstBuf, const T* pSrcBuf, const std::uint32_t uiCols, const std::uint32_t uiRows );

			/** Copies the source image buffer to the destination image buffer. The source buffer must be less than or equal
			 *  in dimensions to the destination buffer.
			 *  @param pDstBuf	- Pointer to the destination image buffer. Result is placed in this buffer.
			 *  @param pSrcBuf	- Pointer to the source image buffer.
			 *  @param uiSize	- The number of image bytes to copy.
			 *  @throws std::runtime_error
			 */
			static void copy( T* pDstBuf, const T* pSrcBuf, const std::uint32_t uiSize );

			/** Determines the maximum value for a specific data type. Example, for std::uint16_t: 2^16 = 65536.
			 *  @return The maximum value for the data type currently in use.
			 */
			static std::uint32_t maxTVal( void );

		private:

			/** Verifies that the specified buffer is not equal to nullptr.
			 *  @param pBuf - Pointer to the buffer to check.
			 *  @throws std::runtime_error
			 */
			static constexpr void verifyBuffer( const T* pBuffer );

			/** Verifies that the specified row value is less than the total number of rows.
			 *  @param row  - The row to check.
			 *  @param rows	- The total row length ( i.e. image row count ).
			 *  @throws std::runtime_error
			 */
			static constexpr void verifyRow( const std::uint32_t uiRow, const std::uint32_t uiRows );

			/** Verifies that the specified number of rows is not equal to zero.
			 *  @param rows - The total row length ( i.e. image row count ).
			 *  @throws std::runtime_error
			 */
			static constexpr void verifyRows( const std::uint32_t uiRows );

			/** Verifies that the specified number of cols is not equal to zero.
			 *  @param cols - The total column length ( i.e. image column count ).
			 *  @throws std::runtime_error
			 */
			static constexpr void verifyColumns( const std::uint32_t uiCols );

			/** Verifies that the specified column value is less than the total number of columns.
			 *  @param col	- The column to check.
			 *  @param cols	- The total column length ( i.e. image column count ).
			 *  @throws std::runtime_error
			 */
			static constexpr void verifyColumn( const std::uint32_t uiCol, const std::uint32_t uiCols );

			/** Verifies that value1 is less than value2. Throws exception on error.
			 *  @param value1 - The first ( lesser ) range value.
			 *  @param value2 - The second ( higher ) range value.
			 *  @throws std::runtime_error
			 */
			static constexpr void verifyRangeOrder( const std::uint32_t uiValue1, const std::uint32_t uiValue2 );

			/** Draws a semi-circle on the specified buffer.
			 *  @param uiXCenter	- x position of circle center point.
			 *  @param uiYCenter	- y position of circle center point.
			 *  @param uiRadius		- The radius of the circle.
			 *  @param gStartAngle	- The start angle of the semi-circle.
			 *  @param gEndAngle	- The end angle of the semi-circle.
			 *  @param uiCols		- The image column size ( in pixels ).
			 *  @param pBuf			- Pointer to the image data buffer.
			 *  @param uiColor		- Color to draw circle ( default = 0 ).
			 *  @throws std::invalid_argument
			 */
			static void drawSemiCircle( const std::uint32_t uiXCenter, const std::uint32_t uiYCenter, const std::uint32_t uiRadius, const double gStartAngle,
										const double gEndAngle, const std::uint32_t uiCols, T* pBuf, const T uiColor = 0 );

			/** Draws a filled circle on the specified buffer.
			 *  @param uiXCenter	- x position of circle center point.
			 *  @param uiYCenter	- y position of circle center point.
			 *  @param uiRadius		- The radius of the circle.
			 *  @param uiCols		- The image column size ( in pixels ).
			 *  @param pBuf			- Pointer to the image data buffer.
			 *  @param uiColor		- Color to draw circle ( default = 0 ).
			 *  @throws std::invalid_argument
			 */
			static void drawFillCircle( const std::uint32_t uiXCenter, const std::uint32_t uiYCenter, const std::uint32_t uiRadius, const std::uint32_t uiCols, T* pBuf, const T uiColor = 0 );

			/** Draws a gradient filled circle on the specified buffer.
			 *  @param uiRadius		- The radius of the circle.
			 *  @param uiCols		- The image column size ( in pixels ).
			 *  @param uiRows		- The image row size ( in pixels ).
			 *  @param pBuf			- Pointer to the image data buffer.
			 *  @param uiColor		- Color to draw circle ( default = 0 ).
			 *  @throws std::invalid_argument
			 */
			static void drawGradientFillCircle( const std::uint32_t uiRadius, const std::uint32_t uiCols, const std::uint32_t uiRows, T* pBuf );

			/** Draws a gradient filled circle on the specified buffer.
			 *  @param uiXCenter	- x position of circle center point.
			 *  @param uiYCenter	- y position of circle center point.
			 *  @param uiRadius		- The radius of the circle.
			 *  @param uiCols		- The image column size ( in pixels ).
			 *  @param pBuf			- Pointer to the image data buffer.
			 *  @param uiColor		- Color to draw circle ( default = 0 ).
			 *  @throws std::invalid_argument
			 */
			static void drawCircle( const std::uint32_t uiXCenter, const std::uint32_t uiYCenter, const std::uint32_t uiRadius, const std::uint32_t uiCols, T* pBuf, const T uiColor = 0 );

			/** version() text holder */
			static const std::string m_sVersion;
		};


	}	// end gen3 namespace
}		// end arc namespace



//
// Creates a modified version of the std::default_delete class for use by
// all std::unique_ptr's that wrap a CStats/CDifStats object.
//
namespace std
{

	/**
	 *  Creates a modified version of the std::default_delete class for use by
	 *  all std::unique_ptr's returned from CArcImage to delete CStats objects.
	 */
	template<>
	class GEN3_CARCIMAGE_API default_delete< arc::gen3::image::CStats >
	{
	public:

		/** Deletes the specified CStats object
		 *  @param pObj - The object to be deleted/destroyed.
		 */
		void operator()( arc::gen3::image::CStats* pObj );
	};

	/**
	 *  Creates a modified version of the std::default_delete class for use by
	 *  all std::unique_ptr's returned from CArcImage to delete CDifStats objects.
	 */
	template<>
	class GEN3_CARCIMAGE_API default_delete< arc::gen3::image::CDifStats >
	{
	public:

		/** Deletes the specified CDifStats object
		 *  @param pObj - The object to be deleted/destroyed.
		 */
		void operator()( arc::gen3::image::CDifStats* pObj );
	};
}



#endif		// _GEN3_CARCIMAGE_H_


















//#ifndef _ARC_CIMAGE_H_
//#define _ARC_CIMAGE_H_
//
//#ifdef _WINDOWS
//#pragma warning( disable: 4251 )
//#endif
//
//#include <string>
//#include <memory>
//#include <cmath>
//#include <cstdint>
//#include <unordered_map>
//
//#include <CArcImageDllMain.h>
//
//
//namespace arc
//{
//	namespace image
//	{
//		#define T_SIZE( t )	pow( ( double )2, ( double )( sizeof( t ) * 8 ) )
//
//		typedef std::shared_ptr<std::uint8_t>	tSharedU8Ptr;	// 16 & 32-bit data are stored as bytes
//		typedef std::shared_ptr<float>			tSharedFltPtr;
//		typedef std::shared_ptr<int>			tSharedS32Ptr;
//
//
//		class DLLARCIMAGE_API CArcImage
//		{
//			public:
//				//  Constructors/destructors
//				// +---------------------------------------------------------------------------------+
//				CArcImage();
//				virtual ~CArcImage();
//
//				void Free( std::uint8_t* ptr );
//				void Free( float* ptr );
//				void Free( int* ptr );
//
//
//				//  Image data
//				// +---------------------------------------------------------------------------------+
//				unsigned char* GetRow( void* pBuf, int dRow, int dCol1, int dCol2, int dRows, int dCols, int& rdCount, int dBpp = CArcImage::BPP16 );
//				unsigned char* GetCol( void* pBuf, int dCol, int dRow1, int dRow2, int dRows, int dCols, int& rdCount, int dBpp = CArcImage::BPP16 );
//
//				float* GetRowArea( void* pBuf, int dRow1, int dRow2, int dCol1, int dCol2, int dRows, int dCols, int& rdCount, int dBpp = CArcImage::BPP16 );
//				float* GetColArea( void* pBuf, int dRow1, int dRow2, int dCol1, int dCol2, int dRows, int dCols, int& rdCount, int dBpp = CArcImage::BPP16 );
//
//
//				//  Statistics
//				// +---------------------------------------------------------------------------------+
//				class CImgStats
//				{
//				public:
//					CImgStats()
//					{
//						gMin = gMax = gMean = gVariance = gStdDev = gTotalPixels = gSaturatedPixCnt = 0;
//					}
//
//					double gTotalPixels;
//					double gMin;
//					double gMax;
//					double gMean;
//					double gVariance;
//					double gStdDev;
//					double gSaturatedPixCnt;
//				};
//
//
//				class CImgDifStats
//				{
//				public:
//					CImgStats cImg1Stats;
//					CImgStats cImg2Stats;
//					CImgStats cImgDiffStats;
//				};
//
//
//				CImgDifStats GetDiffStats( void* pBuf1, void* pBuf2, int dRow1, int dRow2, int dCol1, int dCol2, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//				CImgDifStats GetDiffStats( void* pBuf1, void* pBuf2, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//
//				CImgStats GetStats( void* pBuf, int dRow1, int dRow2, int dCol1, int dCol2, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//				CImgStats GetStats( void* pBuf, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//
//
//				//  Histogram
//				// +---------------------------------------------------------------------------------+
//				int* Histogram( int& rdCount, void* pBuf, int dRow1, int dRow2, int dCol1, int dCol2, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//				int* Histogram( int& rdCount, void* pBuf, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//
//
//				//  Image Buffer Manipulation
//				// +---------------------------------------------------------------------------------+
//				void Add( unsigned int* pU32Buf, unsigned short* pU16Buf1, unsigned short* pU16Buf2, int dRows, int dCols );
//				void Subtract( void* pBuf1, void* pBuf2, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//				void SubtractHalves( void* pBuf, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//				void Divide( void* pBuf1, void* pBuf2, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//
//
//				//  Image Copy
//				// +---------------------------------------------------------------------------------+
//				void Copy( void* pDstBuf, int dDstRows, int dDstCols, void* pSrcBuf, int dSrcRows, int dSrcCols, int dBpp = CArcImage::BPP16 );
//				void Copy( void* pDstBuf, int dDstSize, void* pSrcBuf, int dSrcSize );
//
//
//				//  Image Buffer Fill
//				// +---------------------------------------------------------------------------------+
//				void Fill( void* pBuf, int dRows, int dCols, int dValue, int dBpp = CArcImage::BPP16 );
//				void GradientFill( void* pBuf, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//				void SmileyFill( void* pBuf, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//	//			void LogoFill( void* pBuf, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//
//
//				//  Synthetic Image
//				// +---------------------------------------------------------------------------------+
//				void VerifyAsSynthetic( void* pBuf, int dRows, int dCols, int dBpp = CArcImage::BPP16 );
//
//
//				//  Constants
//				// +---------------------------------------------------------------------------------+
//				const static int BPP16 = 16;
//				const static int BPP32 = 32;
//
//			private:
//				void DrawSemiCircle( int xCenter, int yCenter, int radius, double gStartAngle, double gEndAngle, int dCols, void* pBuf, int dColor = 0, int dBpp = CArcImage::BPP16 );
//				void DrawFillCircle( int xCenter, int yCenter, int radius, int dCols, void* pBuf, int dColor = 0, int dBpp = CArcImage::BPP16 );
//				void DrawGradientFillCircle( int xCenter, int yCenter, int radius, int dRows, int dCols, void* pBuf, int dBpp = CArcImage::BPP16 );
//				void DrawCircle( int xCenter, int yCenter, int radius, int dCols, void* pBuf, int dColor = 0, int dBpp = CArcImage::BPP16 );
//				void ThrowException( std::string sMethodName, std::string sMsg );
//
//				//  Smart pointer array deleter
//				// +--------------------------------------------------------------------+
//				template<typename T> static void ArrayDeleter( T* p );
//
//				//  Stores pointers returned to the user
//				// +--------------------------------------------------------------------+
//				static std::unordered_map<std::uint64_t,tSharedU8Ptr>  m_u8PtrMap;
//				static std::unordered_map<std::uint64_t,tSharedFltPtr> m_fltPtrMap;
//				static std::unordered_map<std::uint64_t,tSharedS32Ptr> m_s32PtrMap;
//		};
//
//	}	// end image namespace
//}	// end arc namespace
//
//#endif
