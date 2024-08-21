// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcLog.h                                                                                                |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the message logging class.                                                           |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file CArcLog.h */

#pragma once


#ifdef _WINDOWS
#pragma warning( disable: 4251 )
#endif

#include <queue>
#include <string>
#include <sstream>
#include <cstdarg>
#include <memory>

#include <CArcDeviceDllMain.h>


namespace arc
{
	namespace gen3
	{

		/** @class CArcLog
		 *
		 *  ARC message logging class
		 *
		 *  @see arc::gen3::CArcPCI
		 */
		class GEN3_CARCDEVICE_API CArcLog
		{
			public:

				/** Default constructor
				 */
				CArcLog( void );

				/** Default destructor
				 */
				~CArcLog( void ) = default;

				/** Sets the maximum number of messages that the log can hold.
				 *  @param ulSize - The maximum number of messages the log can hold. Must be > 0.
				 */
				void setMaxSize( const std::uint32_t ulSize ) noexcept;

				/** Inserts a message into the log. It dumps the oldest message if the log size is greater than or equal to the max size.
				 *  @param szFmt - C-printf style format (sort of):
				 *				%d   -> Integer
				 *				%f   -> Double
				 *				%s   -> Char *, std::string not accepted
				 *				%x,X -> Integer as hex, produces uppercase only
				 *				%e   -> Special case that produces a std::string message from
				 *				        one of the system functions ::GetLastError or
				 *				        strerror, depending on the OS.
				 *  @throws Any exception thrown by std::queue
				 */
				void put( const char* szFmt, ... );

				/** Returns and removes the oldest string from the log. Applications should call empty() to check if more messages are available.
				 *  @return The oldest message in the log
				 */
				const std::string getNext( void ) noexcept;

				/** Returns and removes the newest string from the log. Applications should call empty() to check if more messages are available.
				 *  @return The newest message in the log
				 */
				const std::string getLast( void ) noexcept;

				/** Returns the number of messages in the log
				 *  @return The number of available messages
				 */
				std::uint32_t getLogCount( void ) noexcept;

				/** Returns whether or not the log contains any messages
				 *  @return <i>true</i> if the log is empty; <i>false</i> otherwise.
				 */
				bool empty( void ) noexcept;

				/** Performs a self-test. Output goes to a message box on windows and to the terminal on linux systems.
				 *  @throws Any exception thrown by std::stringstream
				 */
				void selfTest( void );

			private:

				/** The maximum number of messages the log can hold */
				static std::queue<std::string>::size_type Q_MAX;

				/** The log message queue */
				std::unique_ptr<std::queue<std::string>> m_sQueue;
		};

	}	// end gen3 namespace
}	// end arc namespace
