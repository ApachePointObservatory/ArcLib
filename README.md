# ArcLib

This is a Python wrapper around the Astronomical Research Cameras GenIII C++ API. The wrapper allows users to control and interface with ARC interfaces and devices from the Python interpreter. Wrapping is achieved using the Simplified Wrapper and Interface Generator (SWIG).

The idea is to allow calls directly to the ARC API from the Python interpreter using an interface similar to ARC's own API.

For more information about the ARC GenIII API, *c.f.* [The Gen III software and Documents Reference](http://www.astro-cam.com/Gen3Software.php) and the [Gen III Developer Docs](http://www.astro-cam.com/DOCUMENTS/GenIII/arc_api/v3.6.2/).

## Authors

Python Wrapper: Gordon A. MacDonald, Ph.D, Apache Point Observatory, Sunspot NM.

ARC GenIII API: Scott Streit, Astronomical Research Cameras.

## Provided Libraries

The following functionalities of the ARC API are provided:

+ **ArcDeinterlace**: an interface to the CArcDeinterlace class used to deinterlace images.
+ **ArcFitsFile**: an interface to the CArcFitsFile class used to manipulate FITS files.
+ **ArcPCI**: an interface to the CArcPCI class with extended functionality inherited from the CArcDevice base class.
+ **ArcPCIe**: an interface to the CArcPCIe class with extended functionality inherited from the CArcDevice base class.
+ **ArcDefs**: a collection of global variables and constants needed for the above wrappers to work. 

## Dependencies

This wrapper is *tested* with the following dependencies:

+ Python v3.10
+ SWIG > v1.4.2
+ gcc v11.4.0
+ ARC API v3.6.2
+ libcfitsio (what version???)

It will likely work with other versions of Python 3 and gcc. However, because ARC GenIII is a C++ codebase using the C++20 standard, it needs SWIG > v1.4.2 and gcc > v8.

### The GenIII ARC API

Slight modification of the ARC API might be needed to work with SWIG. For example, global variable declarations of the type ```constexpr auto VAR = static_cast<std::uint32_t>(1)``` need to be changed to read ```constexpr std::uint32_t VAR = static_cast<std::uint32_t>(1)```. This is because SWIG has a difficult time auto-typing static-cast constants.

Another modification needed is to move ```using namespace``` directives from header files to their respective implementation. Not doing so pollutes the wrapper namespace and causes errors when compiling the SWIG wrapper code.

## Building

From the top-level directory of this repository, execute ```python3 setup.py build_ext```.

The resulting shared libs will be located in ```./build/lib.<distro>-cpython-<python-version>/```.

## How to Use

Open a python interpreter and do

```import _ArcDefs, _ArcDeinterlace, _ArcFitsFile, _ArcPCI, _ArcPCIe```

Running ```dir()``` using any of the above modules as an argument will reveal the API.

### A Limited Example

A limited example illustrating how to use this wrapper is given below.

```
>>> import _ArcPCIe, _ArcDefs
>>> _ArcPCIe.CArcPCIe_findDevices()	# Find the devices.
>>> _ArcPCIe.CArcPCIe_deviceCount()	# Return the number devices found.
1
>>> _ArcPCIe.CArcPCIe_getDeviceStringList()	# Print a list of devices found.
('PCIe Device 0/dev/Arc66PCIe0\x00',)
>>> arcDev=_ArcPCIe.new_CArcPCIe()		# Initialize a new PCIe device.
>>> _ArcPCIe.CArcPCIe_open(arcDev, 0, 2048, 2048)	# Open device 0, assuming 2048x2048 pixels.
>>> _ArcPCIe.CArcPCIe_commonBufferVA_uint16(arcDev)	# Get the kernel buffer virtual address.
239
>>> _ArcPCIe.CArcPCIe_getStatus(arcDev)		# Return device status.
0
>>> _ArcPCIe.CArcPCIe_isControllerConnected(arcDev)	# See if it is connected to the controller.
False
>>> _ArcDefs.cvar.TDL	# Show the TDL global constant.
5522508
>>> _ArcDefs.cvar.RDM	# SHow the RDM global constant.
5391437
>>> _ArcPCIe.CArcPCIe_close(arcDev)		# Close the device.
>>> _ArcPCIe.delete_CArcPCIe(arcDev)		# Destroy the PCIe device.
```

