#!/usr/bin/env python3

from setuptools import Extension, setup
import glob

srcList = glob.glob("src/ARC_API/3.6.2/CArcDeinterlace/src/*.cpp")
srcList.extend( glob.glob("ArcLib/*.i") )
srcList.append( "src/ARC_API/3.6.2/CArcBase/src/CArcBase.cpp")
srcList.append( "src/ARC_API/3.6.2/CArcBase/src/CArcStringList.cpp" )

incList = glob.glob("src/ARC_API/3.6.2/*/inc")

swigOpts = ["-I" + i for i in incList]
swigOpts.append("-Iinclude")
swigOpts.append("-c++")
swigOpts.append("-Wall")

setup(
        name="ArcLib",
        ext_modules=[
            Extension(
                name="_ArcDeinterlace",
                sources=srcList,
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
            ],
        )

