// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CExpIFace.h                                                                                              |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the standard ARC image readout feedback interface class.                             |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file CExpIFace.h */

#pragma once


#include <cstdint>

#include <CArcDeviceDllMain.h>


namespace arc
{
	namespace gen3
	{

		/** @class CExpIFace
		 *
		 *  ARC image readout interface. Implement this class to receive image readout feedback during execution.
		 *
		 *  @see arc::gen3::CExpIFace
		 */
		class GEN3_CARCDEVICE_API CExpIFace
		{
			public:

				/** Default destructor
				 */
				virtual ~CExpIFace( void ) = default;

				/** The method called during the current exposure countdown.
				 *  @param fElapsedTime - The current elapsed exposure time, in seconds.
				 */
				virtual void exposeCallback( float fElapsedTime ) = 0;

				/** The method called during the current image readout.
				 *  @param uiPixelCount - The current image read pixel count.
				 */
				virtual void readCallback( const std::uint32_t uiPixelCount ) = 0;

			protected:

				/** Default constructor
				 */
				CExpIFace( void ) = default;
		};

	}	// end gen3 namespace
}	// end arc namespace
