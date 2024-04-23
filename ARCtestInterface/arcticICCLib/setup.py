from setuptools import Extension, setup
import glob

srcList = glob.glob("src/camera/*.cpp")
srcList.extend( glob.glob("camera/*.i") )
srcList.extend( glob.glob("src/ARC_API/3.6.2/CArcDevice/src/C*.cpp") )
srcList.extend( ["src/ARC_API/3.6.2/CArcDevice/src/TempCtrl.cpp",] )
srcList.extend( glob.glob("src/ARC_API/3.6.2/CArcBase/src/C*.cpp") )
srcList.extend( glob.glob("src/ARC_API/3.6.2/CArcFitsFile/src/C*.cpp") )
srcList.extend( glob.glob("src/ARC_API/3.6.2/CArcDeinterlace/src/C*.cpp") )

setup(
        name="arcticICCLib",
        ext_modules=[
            Extension(
                name="_arcticICCLib",
                sources=srcList,
                include_dirs=[
                    "include",
                    "src/ARC_API/3.6.2/CArcDevice/inc",
                    "src/ARC_API/3.6.2/CArcBase/inc",
                    "src/ARC_API/3.6.2/CArcFitsFile/inc",
                    "src/ARC_API/3.6.2/CArcDeinterlace/inc",
                    "src/ARC_API/3.6.2/cfitsio-3450/include",
                    ],
                library_dirs=[
                    "src/ARC_API/3.6.2/cfitsio-3450/lib",
                    ],
                libraries=[
                    "cfitsio",
                    ],
                extra_compile_args=[
                    "-std=c++20",
                    ],
                swig_opts=[
                    "-Iinclude",
                    "-c++",
                    ],
                ),
            ],
        )

