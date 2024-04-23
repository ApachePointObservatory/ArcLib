// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  ArcBase.h  ( Gen3 )                                                                                      |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the ARC base class.                                                                  |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: March 25, 2020                                                              |
// |                                                                                                                  |
// |  Copyright 2013 Astronomical Research Cameras, Inc. All rights reserved.                                         |
// +------------------------------------------------------------------------------------------------------------------+

#ifndef _CARCBASE_H_
#define _CARCBASE_H_

#ifdef _WINDOWS
	#include <windows.h>
#else
	#include <cstring>
#endif

#include <type_traits>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cstdint>
#include <cstdarg>

#include <CArcBaseDllMain.h>
#include <CArcStringList.h>



namespace arc
{
	namespace gen3
	{

		// +----------------------------------------------------------------------------------------------------------+
		// |  Macro - throwArcGen3Error                                                                                           |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Convenience to auto fill class::method name                                                             |
		// +----------------------------------------------------------------------------------------------------------+

		/** Convenience macro to throw an exception that contains the class and function name as part of the message.
		 *  @see arc::gen3::CArcBase::throwException() for parameter options/details
		 */
		#define throwArcGen3Error( ... )			arc::gen3::CArcBase::throwException<std::runtime_error>( __FUNCTION__, __LINE__, __VA_ARGS__ )

		#define throwArcGen3InvalidArgument( ... )	arc::gen3::CArcBase::throwException<std::invalid_argument>( __FUNCTION__, __LINE__, __VA_ARGS__ )		

		#define throwArcGen3OutOfRange( ... )		arc::gen3::CArcBase::throwOutOfRange( __FUNCTION__, __LINE__, __VA_ARGS__ )

		#define throwArcGen3LengthError( ... )		arc::gen3::CArcBase::throwException<std::length_error>( __FUNCTION__, __LINE__, __VA_ARGS__ )

		#define throwArcGen3NoDeviceError()			arc::gen3::CArcBase::throwNoDeviceError( __FUNCTION__, __LINE__ )


		// +----------------------------------------------------------------------------------------------------------+
		// |  CArcBase Class                                                                                          |
		// +----------------------------------------------------------------------------------------------------------+

		/** @class CArcBase
		 *  Standard ARC device base class that makes a set of useful methods available to all subclasses.
		 *  Class methods include throwing formatted exceptions, ascii to string command conversion, error
		 *  uiCode formatting and character conversion ( unicode to ascii and vice versa ).
		 */
		class GEN3_CARCBASE_API CArcBase
		{
			public:

				/** Returns a textual representation of the library version.
				 *  @return A string representation of the library version.
				 */
				static const std::string version( void );


				/** Zero out the specified buffer.
				 *  @param pDest  - Pointer to the buffer to zero.
				 *  @param uiSize - The size of the buffer ( in bytes ).
				 *  @throws std::invalid_argument if arguments are invalid.
				 */
				static void zeroMemory( void* pDest, const std::size_t uiSize );


				/** Copies the source buffer into the destination buffer.
				 *  @param pDest  - Pointer to the destination buffer to copy into.
				 *  @param pSrc   - Pointer to the source buffer to copy out of.
				 *  @param uiSize  - The size of the buffers ( in bytes ).
				 *  @throws std::invalid_argument if arguments are invalid.
				 */
				static void copyMemory( void* pDest, void* pSrc, const std::size_t uiSize );


				/** Throws a std::out_of_range exception.
				 *  @param sMethodName	- The name of the method that's the source of the exception.
				 *  @param iLine		- The line number that's the source of the exception.
				 *  @param uiElement    - The element that's out of range.
				 *  @param range        - A pair that defines the valid range.
				 *  @see std::out_of_range()
				 */
				static void throwOutOfRange( const std::string& sMethodName, const std::int32_t iLine, const std::uint32_t uiElement, std::pair<std::uint32_t, std::uint32_t> range );


				/** Throws a std::out_of_range exception.
				 *  @param sMethodName	- The name of the method that's the source of the exception.
				 *  @param iLine		- The line number that's the source of the exception.
				 *  @param sElement     - The element that's out of range.
				 *  @param uiSize       - The elements max size.
				 *  @see std::out_of_range()
				 */
				static void throwOutOfRange( const std::string& sMethodName, const std::int32_t iLine, const std::string& sElement, const std::size_t uiSize );


				/** Throws a std::runtime_error exception.
				 *  @param sMethodName	- The name of the method that's the source of the exception.
				 *  @param iLine		- The line number that's the source of the exception.
				 *  @param sMsg			- The exception message.
				 *  @see std::runtime_error()
				 */
				static void throwNoDeviceError( const std::string& sMethodName, const std::int32_t iLine, const std::string& sMsg = "" );


				/** Throws a std::exception based on the template parameter (default = std::runtime_error).
				 *  @param sMethodName	- The name of the method that's the source of the exception.
				 *  @param iLine		- The line number that's the source of the exception.
				 *  @param sMsg			- The exception message.
				 *  @see std::runtime_error()
				 */
				template <typename T = std::runtime_error>
				static void throwException( const std::string& sMethodName, const std::int32_t iLine, const std::string& sMsg )
				{
					std::ostringstream oss;

					oss << sMsg << "\nTrace: ( "	<< sMethodName << "() line: " << iLine << " )" << std::ends;

					throw T( oss.str() );
				}


				/** Throws a std::exception based on the template parameter (default = std::runtime_error). 
				 *  Uses printf-style formatting. Acceptable format string specifiers:
				 *
				 *                Acceptable format string specifiers:
				 *                %u   -> unsigned integer
				 *                %d   -> integer
				 *                %f   -> double
				 *				  %l   -> long
				 *                %j   -> unsigned long
				 *                %J   -> unsigned long long
				 *                %s   -> char * string
				 *                %e   -> system message (as returned by errno or GetLastError)
				 *                %x/X -> lower/upper case hexadecimal integer.
				 *
				 *  @param sMethodName	- The name of the method that's the source of the exception.
				 *  @param iLine		- The line number that's the source of the exception.
				 *  @param pszFmt		- A printf-style format string. See above for specifiers.
				 *  @param ...			- Arguments to fill in format string specifiers.
				 *  @see std::runtime_error()
			 	 */
				template <typename T = std::runtime_error>
				static void throwException( const std::string& sMethodName, const std::int32_t iLine, const char* pszFmt, ... )
				{
					std::ostringstream oss;
					char* pzChar;
					char* pszVal;

					va_list ap;

					va_start( ap, pszFmt );

					for ( pzChar = (char* )pszFmt; *pzChar; pzChar++ )
					{
						if ( *pzChar != '%' )
						{
							oss << *pzChar;
							continue;
						}

						switch ( *++pzChar )
						{
							case 'd':
							{
								oss << va_arg( ap, int );
							}
							break;

							case 'u':
							{
								oss << va_arg( ap, unsigned int );
							}
							break;

							case 'l':
							{
								oss << va_arg( ap, long );
							}
							break;

							case 'j':
							{
								oss << va_arg( ap, unsigned long );
							}
							break;

							case 'J':
							{
								oss << va_arg( ap, unsigned long long );
							}
							break;

							case 'f':
							{
								oss << va_arg( ap, double );
							}
							break;

							case 's':
							{
								for ( pszVal = va_arg( ap, char* ); *pszVal; pszVal++ )
								{
									oss << *pszVal;
								}
							}
							break;

							case 'e':
							{
								#ifdef _WINDOWS
									oss << CArcBase::getSystemMessage( va_arg( ap, std::uint32_t ) );
								#else
									oss << CArcBase::getSystemMessage( va_arg( ap, std::int32_t ) );
								#endif
							}
							break;

							case 'X':
							case 'x':
							{
								oss << std::uppercase << std::hex << va_arg( ap, unsigned int ) << std::dec;
							}
							break;

							case 'b':
							{
								oss << std::boolalpha << ( va_arg( ap, int ) > 0 ? true : false );
							}
							break;

							case 'p':
							{
								oss << std::uppercase << std::showbase << std::hex << va_arg( ap, std::uint64_t ) << std::dec;
							}
							break;

							default:
							{
								oss << *pzChar;
							}
							break;
						}
					}
					va_end( ap );

					throwException<T>( sMethodName, iLine, oss.str() );
				}


				/** Returns a string that represents the specified system error uiCode.
				 *  @param uiCode/iCode - The system specific error uiCode ( GetLastError() on windows, errno on linux/mac ).
				 *  @return A string that represents the error message assoctiated with the specified system error uiCode.
				 */
				template<typename ErrCode>
				static const std::string getSystemMessage( ErrCode iCode ) requires std::is_integral_v<ErrCode>
				{
					#ifdef _WINDOWS
						std::ostringstream	oss;
						std::string			sMsg;

						LPTSTR lpBuffer;

						FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
										NULL,
										( DWORD )iCode,
										MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
										( LPTSTR )&lpBuffer,
										0,
										NULL );

						char szBuffer[ 1024 ] = { '\0' };

						std::int32_t iBytes = WideCharToMultiByte( CP_ACP,
																	0,
																	reinterpret_cast< LPWSTR >( lpBuffer ),
																	-1,
																	szBuffer,
																	sizeof( szBuffer ),
																	NULL,
																	NULL );

						if (iBytes > 0)
						{
							oss << szBuffer;
						}

						LocalFree( lpBuffer );

					#else

						std::ostringstream	oss;
						std::string			sMsg;

						if ( iCode != -1 )
						{
							oss << "( errno: " << iCode << " ) - " << strerror( iCode );
						}

						//
						// Remove and return characters '\r' or '\n' at the end of the string.
						//
						sMsg = oss.str();

						if (sMsg.back() == '\n' || sMsg.back() == '\r')
						{
							sMsg.erase( sMsg.end() - 1, sMsg.end() );
						}

					#endif

					//
					// Remove and return characters '\r' or '\n' at the end of the string.
					//
					sMsg = oss.str();

					if ( sMsg.back() == '\n' || sMsg.back() == '\r' )
					{
						sMsg.erase( sMsg.end() - 1, sMsg.end() );
					}

					return sMsg;
				}


				/** Returns the current system error code.
				 *  @return The current system error code (GetLastError() on windows, errno on linux).
				 */
				#ifdef _WINDOWS
					static std::uint32_t getSystemError( void ) noexcept
					{
						return static_cast< std::uint32_t >( ::GetLastError() );
					}
				#else
					static std::int32_t getSystemError( void ) noexcept
					{
						return static_cast< std::int32_t >( errno );
					}
				#endif


				/** Splits a string based on the specified deliminator.
				 *  @param sString - The string to split.
				 *  @param zDelim  - The character used to split the string ( default = ' ' ).
				 *  @return A pointer to a CArcStringList object that contains the string tokens.
				 *  @throws std::runtime_error()
				 */
				static std::unique_ptr<CArcStringList> splitString( const std::string& sString, const char& zDelim = ' ' );


				/** Formats a std::string using printf-style formatting. Acceptable format string specifiers:
				 *
				 *                Acceptable format string specifiers:
				 *                %u   -> unsigned integer
				 *                %d   -> integer
				 *                %f   -> double
				 *				  %l   -> long
				 *                %j   -> unsigned long
				 *                %J   -> unsigned long long
				 *                %s   -> char * string
				 *                %e   -> system message (as returned by errno or GetLastError)
				 *                %x/X -> lower/upper case hexadecimal integer.
				 *
				 *  @param pszFmt	- A printf-style format string. See above for specifiers.
				 *  @param ...		- Arguments to fill in format string specifiers.
				 *  @return A string formatted using the specified parameters.
				 */
				static std::string formatString( const char* pszFmt, ... );


				/** Converts a unicode character array to an ansi string.
				 *  @param wzString - The unicode character array to convert.
				 *  @return An ansi string version of the unicode array parameter.
				 */
				static std::string convertWideToAnsi( wchar_t wzString[] ) noexcept;


				/** Converts a unicode string to an ansi string.
				 *  @param wsString - The unicode string to convert.
				 *  @return An ansi string version of the unicode string parameter.
				 */
				static std::string convertWideToAnsi( const std::wstring& wsString );


				/** Converts an ansi string to a unicode string.
				 *  @param pszString - The ansi string to convert.
				 *  @return A unicode string version of the ansi string parameter.
				 */
				static std::wstring convertAnsiToWide( const char* pszString );


				/** Converts a device or controller ascii command to a string represented by changing each
				 *  each byte to its ascii letter equivalent. This is useful for logging human readable
				 *  versions of a command. For example, 0x54444C would be converted to "TDL". If the command
				 *  fails to be converted, then the hexadecimal value is returned as a string. For example,
				 *  0x212223 would be returned as "0x212223" and not "!*#".
				 *  @param uiCmd - A device or controller ascii command.
				 *  @return A string version of the command parameter.
				 */
				static std::string cmdToString( const std::uint32_t uiCmd );


				/** Converts a device or controller ascii command to a string represented by changing each
				 *  each byte to its ascii letter equivalent. This is useful for logging human readable
				 *  versions of a command. For example, 0x54444C would be converted to "TDL". If the command
				 *  fails to be converted, then the hexadecimal value is returned as a string. For example,
				 *  0x212223 would be returned as "0x212223" and not "!*#".
				 *  @param uiCmd - A controller ascii command reply value.
				 *  @param tCmdList - The controller ascii command list.
				 *  @return A string version of the command parameter.
				 */
				static std::string cmdToString( const std::uint32_t uiReply, const std::initializer_list<std::uint32_t>& tCmdList );


				/** Appends dots to the end of the specified string to show a connection to a component.
				 *  @param svText 		- The text to append the dots too.
				 *  @param uiMaxLength	- The maximum length of the returned string.
				 *  @param szDot		- The dot character (default = '.').
				 *  @return A string of length iMaxLength with dots appended.
				 *  @throws std::ios_base::failure
				 *  @throws std::length_error
				 */
				static std::string setDots( std::string_view svText, const std::size_t uiMaxLength, const char szDot = '.' );


				/** Moves through the specified iterator positions and places each value into a return
				 *  string that is seperated by the specified zSeperator.
				 *
				 *  Example:
				 *           std::initializer_list<std::uint32_t> list = { 1, 2, 3, 4 };
				 *           produces the following output: "1 2 3 4 "
				 *
				 *  @param pBegin		- Pointer to iterator to the first element.
				 *  @param pEnd			- Pointer to iterator to the last element.
				 *  @param zSeperator	- Seperator character ( default = ' ' ).
				 *  @return A string containing all iterator elements seperated by the zSeperator parameter.
				 *  @throws std::invalid_argument if iterator pointers are invalid.
				 */
				template <typename T> static std::string iterToString( T* pBegin, T* pEnd, char zSeperator = ' ' )
				{
					std::ostringstream oss;

					if ( pBegin != nullptr )
					{
						if ( pEnd != nullptr )
						{
							while ( pBegin != pEnd )
							{
								oss << cmdToString( *pBegin++ ) << zSeperator;
							}
						}

						else
						{
							throwArcGen3Error( "Invalid \"end\" parameter ( nullptr )." );
						}
					}

					else
					{
						throwArcGen3Error( "Invalid \"begin\" parameter ( nullptr )." );
					}

					return oss.str();
				}


				/** Destructor */
				virtual ~CArcBase( void );

			protected:

				/** Default constructor */
				CArcBase( void ) = default;

				/** Delete copy constructor */
				CArcBase( const CArcBase& ) = delete;

				/** Delete move constructor */
				CArcBase( CArcBase&& ) = delete;

				/** Delete copy assignment constructor */
				CArcBase& operator=( const CArcBase& ) = delete;

				/** Delete move assignment constructor */
				CArcBase& operator=( CArcBase&& ) = delete;

				/** version() text holder */
				static const std::string m_sVersion;

			};


	}		// pEnd gen3 namespace
}			// pEnd arc namespace


#endif		// _CARCBASE_H_
