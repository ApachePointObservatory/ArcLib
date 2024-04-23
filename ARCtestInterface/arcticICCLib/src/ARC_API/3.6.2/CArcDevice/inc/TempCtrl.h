// +------------------------------------------------------------------------------------------------------------------+
// |  FILE:  TempCtrl.h                                                                                               |
// +------------------------------------------------------------------------------------------------------------------+
// |  PURPOSE: This file defines the standard ARC temperature control constants and functions.                        |
// |                                                                                                                  |
// |  AUTHOR:  Scott Streit			DATE: ?                                                                           |
// |                                                                                                                  |
// |  Copyright Astronomical Research Cameras, Inc. All rights reserved.                                              |
// +------------------------------------------------------------------------------------------------------------------+
/*! \file TempCtrl.h */

#pragma once


namespace arc
{
	namespace gen3
	{

		// + ----------------------------------------------------------
		// | Define temperature conversions
		// + ----------------------------------------------------------
		constexpr auto C2K( double c )	{ return ( ( double )( c + 273.15 ) ); }						// Celcius To Kelvin
		constexpr auto K2C( double k )	{ return ( ( double )( k - 273.15 ) ); }						// Kelvin To Celcius
		constexpr auto F2K( double f )	{ return ( ( double )( f * ( 9.0 / 5.0 ) - 459.67 ) ); }		// Fahrenheit To Kelvin
		constexpr auto K2F( double k )	{ return ( ( double )( ( k + 459.67 ) * ( 5.0 / 9.0 ) ) ); }	// Kelvin To Fahrenheit

		// + ----------------------------------------------------------
		// | Define temperature constants
		// + ----------------------------------------------------------
		constexpr auto TMPCTRL_SD_NUM_OF_READS				= 30;
		constexpr auto TMPCTRL_SD_VOLT_TOLERANCE_TRIALS		= 30;
		constexpr auto TMPCTRL_SD_VOLT_TOLERANCE			= 0.005;
		constexpr auto TMPCTRL_SD_DEG_TOLERANCE				= 3.0;

		// +-----------------------------------------------------------
		// | Define Temperature Coeffients for DT-670 Sensor
		// +-----------------------------------------------------------
		constexpr auto TMPCTRL_DT670_COEFF_1				= 0.03;
		constexpr auto TMPCTRL_DT670_COEFF_2				= 0.0000251;


		// +-----------------------------------------------------------
		// | Define Temperature Coeffients for CY7 Sensor
		// +-----------------------------------------------------------
		constexpr auto TMPCTRL_SD_ADU_OFFSET				= 2045.0;
		constexpr auto TMPCTRL_SD_ADU_PER_VOLT				= 1366.98;


		// +-----------------------------------------------------------
		// | Define Temperature Coeffients for High Gain Utility Board
		// +-----------------------------------------------------------
		constexpr auto TMPCTRL_HG_ADU_OFFSET				= -3108.0;
		constexpr auto TMPCTRL_HG_ADU_PER_VOLT				= 7321.0;


		// + ----------------------------------------------------------
		// | Define temperature coefficient data structure
		// + ----------------------------------------------------------
		typedef struct TmpCtrlData
		{
			double vu;
			double vl;
			int    count;
			double coeff[ 12 ];
		} TmpCtrlCoeff_t;


		//// +-----------------------------------------------------------
		//// | Define Temperature Coeffients for Non-linear Silicone
		//// | Diode ( SD )
		//// +-----------------------------------------------------------
		constexpr auto TMPCTRL_SD_2_12K_VU			= 1.680000;
		constexpr auto TMPCTRL_SD_2_12K_VL			= 1.294390;
		constexpr auto TMPCTRL_SD_2_12K_COUNT		= 10;
		extern double TMPCTRL_SD_2_12K_COEFF[ TMPCTRL_SD_2_12K_COUNT ];


		constexpr auto TMPCTRL_SD_12_24K_VU			= 1.38373;
		constexpr auto TMPCTRL_SD_12_24K_VL			= 1.11230;
		constexpr auto TMPCTRL_SD_12_24K_COUNT		= 11;
		extern double TMPCTRL_SD_12_24K_COEFF[ TMPCTRL_SD_12_24K_COUNT ];


		constexpr auto TMPCTRL_SD_24_100K_VU		= 1.122751;
		constexpr auto TMPCTRL_SD_24_100K_VL		= 0.909416;
		constexpr auto TMPCTRL_SD_24_100K_COUNT		= 12;
		extern double TMPCTRL_SD_24_100K_COEFF[ TMPCTRL_SD_24_100K_COUNT ];


		constexpr auto TMPCTRL_SD_100_475K_VU		= 0.999614;
		constexpr auto TMPCTRL_SD_100_475K_VL		= 0.079767;
		constexpr auto TMPCTRL_SD_100_475K_COUNT	= 11;
		extern double TMPCTRL_SD_100_475K_COEFF[ TMPCTRL_SD_100_475K_COUNT ];


		// +-----------------------------------------------------------
		// | Define temperature control keys for input ini files
		// +-----------------------------------------------------------
		constexpr auto TMPCTRL_DT670_COEFF_1_KEY			= "[TMPCTRL_DT670_COEFF_1]";
		constexpr auto TMPCTRL_DT670_COEFF_2_KEY			= "[TMPCTRL_DT670_COEFF_2]";
		constexpr auto TMPCTRL_SDADU_OFFSET_KEY				= "[TMPCTRL_SDADU_OFFSET]";
		constexpr auto TMPCTRL_SDADU_PER_VOLT_KEY			= "[TMPCTRL_SDADU_PER_VOLT]";
		constexpr auto TMPCTRL_HGADU_OFFSET_KEY				= "[TMPCTRL_HGADU_OFFSET]";
		constexpr auto TMPCTRL_HGADU_PER_VOLT_KEY			= "[TMPCTRL_HGADU_PER_VOLT]";
		constexpr auto TMPCTRL_SDNUMBER_OF_READS_KEY		= "[TMPCTRL_SDNUMBER_OF_READS]";
		constexpr auto TMPCTRL_SDVOLT_TOLERANCE_TRIALS_KEY	= "[TMPCTRL_SDVOLT_TOLERANCE_TRIALS]";
		constexpr auto TMPCTRL_SDVOLT_TOLERANCE_KEY			= "[TMPCTRL_SDVOLT_TOLERANCE]";
		constexpr auto TMPCTRL_SDDEG_TOLERANCE_KEY			= "[TMPCTRL_SDDEG_TOLERANCE]";
		constexpr auto TMPCTRL_SD2_12K_COEFF_KEY			= "[TMPCTRL_SD2_12K_COEFF]";
		constexpr auto TMPCTRL_SD12_24K_COEFF_KEY			= "[TMPCTRL_SD12_24K_COEFF]";
		constexpr auto TMPCTRL_SD24_100K_COEFF_KEY			= "[TMPCTRL_SD24_100K_COEFF]";
		constexpr auto TMPCTRL_SD100_475K_COEFF_KEY			= "[TMPCTRL_SD100_475K_COEFF]";

	}	// end gen3 namespace
}	// end arc namespace
