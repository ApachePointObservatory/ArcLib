// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CConIFace.h                                                                                              |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the standard ARC continuous readout feedback interface class.                        |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file CConIFace.h */

#pragma once


#include <CArcDeviceDllMain.h>


namespace arc
{
	namespace gen3
	{

		/** @class CConIFace
		 *
		 *  ARC continuous readout interface. Implement this class to receive continuous readout feedback during execution.
		 *
		 *  @see arc::gen3::CConIFace
		 */
		class GEN3_CARCDEVICE_API CConIFace
		{
			public:

				/** Default destructor
				 */
				virtual ~CConIFace( void ) = default;

				/** The method called after frame completion during continuous readout.
				 *  @param uiFramesPerBuffer	- The number of frames-per-buffer count
				 *  @param uiFrameCount			- The current PCI/e frame count
				 *  @param uiRows				- The number of rows, in pixels, in the frame
				 *  @param uiCols				- The number of columns, in pixels, in the frame
				 *  @param pBuffer				- A pointer to the start of the frame in the mapped kernel image bufer
				 */
				virtual void frameCallback( std::uint32_t   uiFramesPerBuffer,	// Frames-per-buffer count
											std::uint32_t   uiFrameCount,		// PCI frame count
											std::uint32_t   uiRows,				// # of rows in frame
											std::uint32_t   uiCols,				// # of cols in frame
											void* pBuffer ) = 0;				// Pointer to frame start in buffer

			protected:

				/** Default constructor
				 */
				CConIFace( void ) = default;

		};

	}	// end gen3 namespace
}	// end arc namespace
