// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  CArcDisplay.cpp  ( Gen3 )                                                                                |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file implements the ARC image display interface.                                                  |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: March 26, 2020                                                              |
// |                                                                                                                  |
// |  Copyright 2013 Astronomical Research Cameras, Inc. All rights reserved.                                         |
// +------------------------------------------------------------------------------------------------------------------+
#include <sstream>
#include <cstdlib>
#include <cstring>

#ifdef _WINDOWS
	#include <tchar.h>
#endif

#if defined( linux ) || defined( __linux )
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <unistd.h>
	#include <link.h>
	#include <dlfcn.h>
#endif

#ifdef __APPLE__
	#include <csignal>
	#include <unistd.h>
	#include <dlfcn.h>
#endif

#include <CArcDisplay.h>


using namespace std::string_literals;



namespace arc
{
	namespace gen3
	{

		// +----------------------------------------------------------------------------------------------------------+
		// | Library build and version info                                                                           |
		// +----------------------------------------------------------------------------------------------------------+
		const std::string CArcDisplay::m_sVersion = "ARC Gen III Display API Library.  "s +

		#ifdef _WINDOWS
			CArcBase::formatString( "[ Compiler Version: %d, Built: %s ]", _MSC_VER, __TIMESTAMP__ );
		#else
			arc::gen3::CArcBase::formatString( "[ Compiler Version: %d.%d.%d, Built: %s %s ]", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, __DATE__, __TIME__ );
		#endif


		// +----------------------------------------------------------------------------------------------------------+
		// |  Default constructor                                                                                     |
		// +----------------------------------------------------------------------------------------------------------+
		CArcDisplay::CArcDisplay( void )
		{
			#ifdef _WINDOWS
				ZeroMemory( &m_processInfo, sizeof( m_processInfo ) );
			#else
				m_pid = 0;
			#endif
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  Destructor                                                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		CArcDisplay::~CArcDisplay( void )
		{
			#ifdef _WINDOWS

				CloseHandle( m_processInfo.hProcess );
				CloseHandle( m_processInfo.hThread );

			#elif defined( __APPLE__ ) || defined( linux ) || defined( __linux )

				kill( m_pid, 1 );

			#endif
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  version                                                                                                 |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns a textual representation of the library version.                                                |
		// +----------------------------------------------------------------------------------------------------------+
		const std::string CArcDisplay::version( void ) noexcept
		{
			return m_sVersion;
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  launch                                                                                                  |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Attempts to launch the DS9 application as a new process.                                                |
		// |                                                                                                          |
		// |  <IN> uiMSDelay - The amount of time to delay before attempting to access the program ( Default = 1000 ) |
		// |                                                                                                          |
		// |  Throws std::runtime_error if DS9 cannot be started or found.                                            |
		// +----------------------------------------------------------------------------------------------------------+
		void CArcDisplay::launch( const std::uint32_t uiMSDelay )
		{
			#ifdef _WINDOWS

				//
				// Make sure the process isn't already running.
				//
				DWORD dwExitCode = 0;

				GetExitCodeProcess( m_processInfo.hProcess, &dwExitCode );

				if ( dwExitCode != STILL_ACTIVE )
				{
					runAndSaveProcess( "ds9.exe"s );
				}

			#else

				char* pszEnv = getenv( "ARC_DS9_PATH" );

				if ( pszEnv == nullptr )
				{
					std::ostringstream oss;

					oss << "Failed to locate DS9 path environment variable. "
						<< "Set \"ARC_DS9_PATH\" to location of ds9 executable."
						<< std::ends;

					throwArcGen3Error( oss.str() );
				}

				std::string sPath = std::string( pszEnv ) + "/ds9"s;

				runAndSaveProcess( sPath.c_str() );

				//
				// LINUX/APPLE ONLY: Sleep for 1 sec to allow DS9 to start
				//
				usleep( uiMSDelay * 1000 );

			#endif

			//
			// Set the default region to "box"
			//
			//setBoxShape();
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  show                                                                                                    |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Displays the specified FITS image data in DS9.                                                          |
		// |                                                                                                          |
		// |  <IN> sFitsFile - FITS file to display.                                                                  |
		// |                                                                                                          |
		// |  Throws std::runtime_error                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		void CArcDisplay::show( const std::string& sFitsFile )
		{
			if ( !verifyPID() ) { return; }

			std::ostringstream oss;

			#ifdef _WINDOWS
				oss << "xpaset -p ds9 file \"{"s << sFitsFile << "}\""s << std::ends;
			#else
				oss << "xpaset -p ds9 fits "s << sFitsFile << std::ends;
			#endif

			runProcess( oss.str() );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  clear                                                                                                   |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Clears the specified frame or all if no frame number is specified.                                      |
		// |                                                                                                          |
		// |  <IN> iFrame - Frame number to clear or -1 to clear all ( default = -1 ).                                |
		// |                                                                                                          |
		// |  Throws std::runtime_error                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		void CArcDisplay::clear( const std::int32_t iFrame )
		{
			if ( !verifyPID() ) { return; }

			std::ostringstream oss;

			oss << "xpaset -p ds9 frame clear "s << ( ( iFrame > 0 ) ? std::to_string( iFrame ) : "all"s ) << std::ends;

			runProcess( oss.str() );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  terminate                                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Forces the termination of the display process.                                                          |
		// |                                                                                                          |
		// |  Throws std::runtime_error                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		void CArcDisplay::terminate( void )
		{
			if ( !verifyPID() ) { return; }

			//			std::cout << "terminating DS9" << '\n';

			#ifdef _WINDOWS

				runProcess( "xpaset -p ds9 exit"s );

			#else

				kill( m_pid, 1 );

			#endif
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  setBoxShape                                                                                             |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Sets the "region" shape to box [].                                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		void CArcDisplay::setBoxShape( void )
		{
			if ( !verifyPID() ) { return; }

			runProcess( "xpaset -p ds9 regions shape box"s );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getVersion                                                                                              |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Returns the DS9 version number.                                                                         |
		// |                                                                                                          |
		// |  Throws std::runtime_error                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		void CArcDisplay::getVersion( void )
		{
			if ( !verifyPID() ) { return; }

			runProcess( "xpaget -p ds9 version"s );
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  verifyPID                                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Verify that the DS9 process is running and valid.                                                       |
		// |                                                                                                          |
		// |  Returns true if the DS9 process is running and valid; false otherwise.                                  |
		// +----------------------------------------------------------------------------------------------------------+
		constexpr bool CArcDisplay::verifyPID( void ) noexcept
		{
			#ifdef _WINDOWS

				if ( m_processInfo.dwProcessId == 0 )
				{
					return false;
				}
	
				else
				{
					return true;
				}

			#else

				if ( m_pid == 0 )
				{
					return false;
				}

				else
				{
					return true;
				}

			#endif
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  runProcess ( _WINDOWS ONLY )                                                                            |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Creates a new _WINDOWS process.                                                                         |
		// |                                                                                                          |
		// |  <IN> sBuf - Process string to execute.                                                                  |
		// |                                                                                                          |
		// |  Throws std::runtime_error                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		void CArcDisplay::runProcess( const std::string& sBuf )
		{
			#ifdef _WINDOWS

				STARTUPINFOA si;
				PROCESS_INFORMATION pi;

				ZeroMemory( &si, sizeof( si ) );
				si.cb = sizeof( si );
				ZeroMemory( &pi, sizeof( pi ) );

				BOOL bOK = CreateProcessA(  nullptr,
											( LPSTR )sBuf.c_str(),
											nullptr,
											nullptr,
											FALSE,
											( NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW ),
											nullptr,
											nullptr,
											&si,
											&pi );

				if ( !bOK )
				{
					throwArcGen3Error( "Failed to run \"%s\"! Verify path!", sBuf.c_str() );
				}

				CloseHandle( pi.hProcess );
				CloseHandle( pi.hThread );

			#else

				FILE* handle = popen( sBuf.c_str(), "r" );

				if ( handle == NULL )
				{
					throwArcGen3Error( "%e", errno );
				}

				char buf[ 64 ];

				size_t readn = 0;

				while ( ( readn = fread( buf, 1, sizeof( buf ), handle ) ) > 0 )
				{
					fwrite( buf, 1, readn, stdout );
				}

				if ( readn > 0 )
				{
					throwArcGen3Error( "%s", buf );
				}

				pclose( handle );

			#endif
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  runAndSaveProcess                                                                                       |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Creates a new process and saves PROCESS_INFORMATION ( _WINDOWS ) or pid_t.                                 |
		// |                                                                                                          |
		// |  <IN> sBuf - Process string to execute.                                                                  |
		// |                                                                                                          |
		// |  Throws std::runtime_error                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		void CArcDisplay::runAndSaveProcess( const std::string& sBuf )
		{
			#ifdef _WINDOWS

				STARTUPINFOA si;
				ZeroMemory( &si, sizeof( si ) );
				si.cb = sizeof( si );
				si.wShowWindow = SW_HIDE;

				ZeroMemory( &m_processInfo, sizeof( m_processInfo ) );

				BOOL bOK = CreateProcessA(  nullptr,
											( LPSTR )sBuf.c_str(),
											nullptr,
											nullptr,
											FALSE,
											( NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW ),
											nullptr,
											nullptr,
											&si,
											&m_processInfo );

				if ( !bOK )
				{
					ZeroMemory( &m_processInfo, sizeof( m_processInfo ) );

					throwArcGen3Error( "Failed to run \"%s\"! Verify path!", sBuf.c_str() );
				}

				// Wait until child process exits.
				WaitForSingleObject( m_processInfo.hProcess, 5000 );

			#else

				m_pid = fork();

				switch ( m_pid )
				{
					// +------------------------+
					// |  Error                 |
					// +------------------------+
				case -1:
				{
					throwArcGen3Error( "Failed to create process for: \"%s\"! Verify path!", sBuf.c_str() );
				}
				break;

				// +------------------------+
				// |  Child Process         |
				// +------------------------+
				case 0:
				{
					execl( ( char* )sBuf.c_str(), ( char* )sBuf.c_str(), ( char* )nullptr );
				}
				break;

				// +------------------------+
				// |  Parent Process        |
				// +------------------------+
				default:
				{
					std::int32_t iStatus = 0;

					if ( waitpid( m_pid, &iStatus, WNOHANG ) == -1 )
					{
						throwArcGen3Error( "Failed to launch DS9 process! %e", CArcBase::getSystemError() );
					}
				}
				break;
				}

			#endif
		}


		// +----------------------------------------------------------------------------------------------------------+
		// |  getModuleDirectory                                                                                      |
		// +----------------------------------------------------------------------------------------------------------+
		// |  Gets the path of THIS module. Returns 'true' on success; 'false' othewise.                              |
		// |                                                                                                          |
		// |  <OUT> pszBuf   - Buffer to hold module path. Set to '\0' on failure.                                    |
		// |  <IN > tBufSize - The byte size of the obuf buffer.                                                      |
		// |                                                                                                          |
		// |  Throws std::runtime_error                                                                               |
		// +----------------------------------------------------------------------------------------------------------+
		#ifdef _WINDOWS
		bool CArcDisplay::getModuleDirectory( TCHAR* pszBuf, const size_t tBufSize )
		{
			if ( !GetModuleFileName( 0, pszBuf, static_cast< DWORD >( tBufSize ) ) )
			{
				*pszBuf = '\0';	// Ensure it's nullptr terminated

				return false;
			}

			// Run through looking for the *last* slash in this path.
			// if we find it, NUL it out to truncate the following
			// filename part.
			TCHAR* lastslash = 0;

			for ( ; *pszBuf; pszBuf++ )
			{
				if ( *pszBuf == '\\' || *pszBuf == '/' )
				{
					lastslash = pszBuf;
				}
			}

			if ( lastslash ) { *lastslash = '\0'; }

			return true;
		}

		#else
		bool CArcDisplay::getModuleDirectory( char* pszBuf, const size_t tBufSize )
		{
			Dl_info	rInfo;

			*pszBuf = 0;

			memset( &rInfo, 0, sizeof( rInfo ) );

			if ( !dladdr( ( void* )&getModuleStub, &rInfo ) || !rInfo.dli_fname )
			{
				return false;
			}

			strncpy( pszBuf, rInfo.dli_fname, tBufSize );

			char* pLastSlash = 0;

			for ( ; *pszBuf; pszBuf++ )
			{
				if ( *pszBuf == '\\' || *pszBuf == '/' )
				{
					pLastSlash = pszBuf;
				}
			}

			if ( pLastSlash ) { *pLastSlash = '\0'; }

			return true;
		}

		#endif

	}	// end gen3 namespace
}		// end arc namespace
