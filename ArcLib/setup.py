#!/usr/bin/env python3

from setuptools import Extension, setup
import glob

srcDict={}

#TODO: srcList needs to be a dict.
srcDict['ArcDeinterlace'] = glob.glob("src/ARC_API/3.6.2/CArcDeinterlace/src/*.cpp")
srcDict['ArcDeinterlace'].append( "ArcLib/ArcDeinterlace.i")
srcDict['ArcDeinterlace'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcBase.cpp")
srcDict['ArcDeinterlace'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcStringList.cpp")
srcDict['ArcFitsFile'] = glob.glob("src/ARC_API/3.6.2/CArcFitsFile/src/*.cpp")
srcDict['ArcFitsFile'].append( "ArcLib/ArcFitsFile.i")
srcDict['ArcFitsFile'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcStringList.cpp")
srcDict['ArcFitsFile'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcBase.cpp")
srcDict['ArcPCI'] = glob.glob("src/ARC_API/3.6.2/CArcDevice/src/*.cpp")
srcDict['ArcPCI'].append( "ArcLib/ArcPCI.i")
srcDict['ArcPCI'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcBase.cpp")
srcDict['ArcPCI'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcStringList.cpp")
srcDict['ArcPCIe'] = glob.glob("src/ARC_API/3.6.2/CArcDevice/src/*.cpp")
srcDict['ArcPCIe'].append( "ArcLib/ArcPCIe.i")
srcDict['ArcPCIe'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcBase.cpp")
srcDict['ArcPCIe'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcStringList.cpp")
srcDict['ArcDefs'] = ["ArcLib/ArcDefs.i"]
srcDict['PCI'] = glob.glob("src/ARC_API/3.6.2/CArcDevice/src/*.cpp")
srcDict['PCI'].append( "ArcLib/PCI.i" )
srcDict['PCI'].append( "src/PCI.cpp" )
srcDict['PCI'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcStringList.cpp" )
srcDict['PCI'].append( "src/ARC_API/3.6.2/CArcBase/src/CArcBase.cpp")


incList = glob.glob("src/ARC_API/3.6.2/*/inc")
incList.append("src/ARC_API/3.6.2/cfitsio-3450/include")
incList.append("include")

swigOpts = ["-I" + i for i in incList]
swigOpts.append("-Iinclude")
swigOpts.append("-c++")
swigOpts.append("-Wall")

setup(
        name="ArcLib",
        ext_modules=[
            Extension(
                name="_ArcDeinterlace",
                sources=srcDict['ArcDeinterlace'],
                include_dirs=incList,
                library_dirs=[
                    ],
                libraries=[
                    ],
                define_macros=[
                    ],
                extra_compile_args=[
                    "-std=c++20",
                    ],
                swig_opts=swigOpts,
                ),
            Extension(
                name="_ArcFitsFile",
                sources=srcDict['ArcFitsFile'],
                include_dirs=incList,
                library_dirs=[
                    "src/ARC_API/3.6.2/cfitsio-3450/lib/",
                    ],
                libraries=[
                    "cfitsio",
                    ],
                define_macros=[
                    ],
                extra_compile_args=[
                    "-std=c++20",
                    ],
                swig_opts=swigOpts,
                ),
            Extension(
                name="_ArcPCI",
                sources=srcDict['ArcPCI'],
                include_dirs=incList,
                library_dirs=[
                    ],
                libraries=[
                    ],
                define_macros=[
                    ],
                extra_compile_args=[
                    "-std=c++20",
                    ],
                swig_opts=swigOpts,
                ),
            Extension(
                name="_ArcPCIe",
                sources=srcDict['ArcPCIe'],
                include_dirs=incList,
                library_dirs=[
                    ],
                libraries=[
                    ],
                define_macros=[
                    ],
                extra_compile_args=[
                    "-std=c++20",
                    ],
                swig_opts=swigOpts,
                ),
            Extension(
                name="_ArcDefs",
                sources=srcDict['ArcDefs'],
                include_dirs=incList,
                library_dirs=[
                    ],
                libraries=[
                    ],
                define_macros=[
                    ],
                extra_compile_args=[
                    "-std=c++20",
                    ],
                swig_opts=swigOpts,
                ),
#            Extension(
#                name="_PCI",
#                sources=srcDict['PCI'],
#                include_dirs=incList,
#                library_dirs=[
#                    ],
#                libraries=[
#                    ],
#                define_macros=[
#                    ],
#                extra_compile_args=[
#                    "-std=c++20",
#                    ],
#                swig_opts=swigOpts,
#                ),

            ],
        )

