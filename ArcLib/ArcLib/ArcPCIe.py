# This file was automatically generated by SWIG (https://www.swig.org).
# Version 4.2.1
#
# Do not make changes to this file unless you know what you are doing - modify
# the SWIG interface file instead.

"""Python interface to CArcDeinterlace C++ code."""

from sys import version_info as _swig_python_version_info
# Import the low-level C/C++ module
if __package__ or "." in __name__:
    from . import _ArcPCIe
else:
    import _ArcPCIe

try:
    import builtins as __builtin__
except ImportError:
    import __builtin__

def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)


def _swig_setattr_nondynamic_instance_variable(set):
    def set_instance_attr(self, name, value):
        if name == "this":
            set(self, name, value)
        elif name == "thisown":
            self.this.own(value)
        elif hasattr(self, name) and isinstance(getattr(type(self), name), property):
            set(self, name, value)
        else:
            raise AttributeError("You cannot add instance attributes to %s" % self)
    return set_instance_attr


def _swig_setattr_nondynamic_class_variable(set):
    def set_class_attr(cls, name, value):
        if hasattr(cls, name) and not isinstance(getattr(cls, name), property):
            set(cls, name, value)
        else:
            raise AttributeError("You cannot add class attributes to %s" % cls)
    return set_class_attr


def _swig_add_metaclass(metaclass):
    """Class decorator for adding a metaclass to a SWIG wrapped class - a slimmed down version of six.add_metaclass"""
    def wrapper(cls):
        return metaclass(cls.__name__, cls.__bases__, cls.__dict__.copy())
    return wrapper


class _SwigNonDynamicMeta(type):
    """Meta class to enforce nondynamic attributes (no new attributes) for a class"""
    __setattr__ = _swig_setattr_nondynamic_class_variable(type.__setattr__)


SHARED_PTR_DISOWN = _ArcPCIe.SHARED_PTR_DISOWN

class SwigPyIterator(object):
    r"""Proxy of C++ swig::SwigPyIterator class."""

    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _ArcPCIe.delete_SwigPyIterator

    def value(self):
        r"""value(SwigPyIterator self) -> PyObject *"""
        return _ArcPCIe.SwigPyIterator_value(self)

    def incr(self, n=1):
        r"""incr(SwigPyIterator self, size_t n=1) -> SwigPyIterator"""
        return _ArcPCIe.SwigPyIterator_incr(self, n)

    def decr(self, n=1):
        r"""decr(SwigPyIterator self, size_t n=1) -> SwigPyIterator"""
        return _ArcPCIe.SwigPyIterator_decr(self, n)

    def distance(self, x):
        r"""distance(SwigPyIterator self, SwigPyIterator x) -> ptrdiff_t"""
        return _ArcPCIe.SwigPyIterator_distance(self, x)

    def equal(self, x):
        r"""equal(SwigPyIterator self, SwigPyIterator x) -> bool"""
        return _ArcPCIe.SwigPyIterator_equal(self, x)

    def copy(self):
        r"""copy(SwigPyIterator self) -> SwigPyIterator"""
        return _ArcPCIe.SwigPyIterator_copy(self)

    def next(self):
        r"""next(SwigPyIterator self) -> PyObject *"""
        return _ArcPCIe.SwigPyIterator_next(self)

    def __next__(self):
        r"""__next__(SwigPyIterator self) -> PyObject *"""
        return _ArcPCIe.SwigPyIterator___next__(self)

    def previous(self):
        r"""previous(SwigPyIterator self) -> PyObject *"""
        return _ArcPCIe.SwigPyIterator_previous(self)

    def advance(self, n):
        r"""advance(SwigPyIterator self, ptrdiff_t n) -> SwigPyIterator"""
        return _ArcPCIe.SwigPyIterator_advance(self, n)

    def __eq__(self, x):
        r"""__eq__(SwigPyIterator self, SwigPyIterator x) -> bool"""
        return _ArcPCIe.SwigPyIterator___eq__(self, x)

    def __ne__(self, x):
        r"""__ne__(SwigPyIterator self, SwigPyIterator x) -> bool"""
        return _ArcPCIe.SwigPyIterator___ne__(self, x)

    def __iadd__(self, n):
        r"""__iadd__(SwigPyIterator self, ptrdiff_t n) -> SwigPyIterator"""
        return _ArcPCIe.SwigPyIterator___iadd__(self, n)

    def __isub__(self, n):
        r"""__isub__(SwigPyIterator self, ptrdiff_t n) -> SwigPyIterator"""
        return _ArcPCIe.SwigPyIterator___isub__(self, n)

    def __add__(self, n):
        r"""__add__(SwigPyIterator self, ptrdiff_t n) -> SwigPyIterator"""
        return _ArcPCIe.SwigPyIterator___add__(self, n)

    def __sub__(self, *args):
        r"""
        __sub__(SwigPyIterator self, ptrdiff_t n) -> SwigPyIterator
        __sub__(SwigPyIterator self, SwigPyIterator x) -> ptrdiff_t
        """
        return _ArcPCIe.SwigPyIterator___sub__(self, *args)
    def __iter__(self):
        return self

# Register SwigPyIterator in _ArcPCIe:
_ArcPCIe.SwigPyIterator_swigregister(SwigPyIterator)
class vectorstr(object):
    r"""Proxy of C++ std::vector< std::string > class."""

    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")
    __repr__ = _swig_repr

    def iterator(self):
        r"""iterator(vectorstr self) -> SwigPyIterator"""
        return _ArcPCIe.vectorstr_iterator(self)
    def __iter__(self):
        return self.iterator()

    def __nonzero__(self):
        r"""__nonzero__(vectorstr self) -> bool"""
        return _ArcPCIe.vectorstr___nonzero__(self)

    def __bool__(self):
        r"""__bool__(vectorstr self) -> bool"""
        return _ArcPCIe.vectorstr___bool__(self)

    def __len__(self):
        r"""__len__(vectorstr self) -> std::vector< std::string >::size_type"""
        return _ArcPCIe.vectorstr___len__(self)

    def __getslice__(self, i, j):
        r"""__getslice__(vectorstr self, std::vector< std::string >::difference_type i, std::vector< std::string >::difference_type j) -> vectorstr"""
        return _ArcPCIe.vectorstr___getslice__(self, i, j)

    def __setslice__(self, *args):
        r"""
        __setslice__(vectorstr self, std::vector< std::string >::difference_type i, std::vector< std::string >::difference_type j)
        __setslice__(vectorstr self, std::vector< std::string >::difference_type i, std::vector< std::string >::difference_type j, vectorstr v)
        """
        return _ArcPCIe.vectorstr___setslice__(self, *args)

    def __delslice__(self, i, j):
        r"""__delslice__(vectorstr self, std::vector< std::string >::difference_type i, std::vector< std::string >::difference_type j)"""
        return _ArcPCIe.vectorstr___delslice__(self, i, j)

    def __delitem__(self, *args):
        r"""
        __delitem__(vectorstr self, std::vector< std::string >::difference_type i)
        __delitem__(vectorstr self, SWIGPY_SLICEOBJECT * slice)
        """
        return _ArcPCIe.vectorstr___delitem__(self, *args)

    def __getitem__(self, *args):
        r"""
        __getitem__(vectorstr self, SWIGPY_SLICEOBJECT * slice) -> vectorstr
        __getitem__(vectorstr self, std::vector< std::string >::difference_type i) -> std::vector< std::string >::value_type const &
        """
        return _ArcPCIe.vectorstr___getitem__(self, *args)

    def __setitem__(self, *args):
        r"""
        __setitem__(vectorstr self, SWIGPY_SLICEOBJECT * slice, vectorstr v)
        __setitem__(vectorstr self, SWIGPY_SLICEOBJECT * slice)
        __setitem__(vectorstr self, std::vector< std::string >::difference_type i, std::vector< std::string >::value_type const & x)
        """
        return _ArcPCIe.vectorstr___setitem__(self, *args)

    def pop(self):
        r"""pop(vectorstr self) -> std::vector< std::string >::value_type"""
        return _ArcPCIe.vectorstr_pop(self)

    def append(self, x):
        r"""append(vectorstr self, std::vector< std::string >::value_type const & x)"""
        return _ArcPCIe.vectorstr_append(self, x)

    def empty(self):
        r"""empty(vectorstr self) -> bool"""
        return _ArcPCIe.vectorstr_empty(self)

    def size(self):
        r"""size(vectorstr self) -> std::vector< std::string >::size_type"""
        return _ArcPCIe.vectorstr_size(self)

    def swap(self, v):
        r"""swap(vectorstr self, vectorstr v)"""
        return _ArcPCIe.vectorstr_swap(self, v)

    def begin(self):
        r"""begin(vectorstr self) -> std::vector< std::string >::iterator"""
        return _ArcPCIe.vectorstr_begin(self)

    def end(self):
        r"""end(vectorstr self) -> std::vector< std::string >::iterator"""
        return _ArcPCIe.vectorstr_end(self)

    def rbegin(self):
        r"""rbegin(vectorstr self) -> std::vector< std::string >::reverse_iterator"""
        return _ArcPCIe.vectorstr_rbegin(self)

    def rend(self):
        r"""rend(vectorstr self) -> std::vector< std::string >::reverse_iterator"""
        return _ArcPCIe.vectorstr_rend(self)

    def clear(self):
        r"""clear(vectorstr self)"""
        return _ArcPCIe.vectorstr_clear(self)

    def get_allocator(self):
        r"""get_allocator(vectorstr self) -> std::vector< std::string >::allocator_type"""
        return _ArcPCIe.vectorstr_get_allocator(self)

    def pop_back(self):
        r"""pop_back(vectorstr self)"""
        return _ArcPCIe.vectorstr_pop_back(self)

    def erase(self, *args):
        r"""
        erase(vectorstr self, std::vector< std::string >::iterator pos) -> std::vector< std::string >::iterator
        erase(vectorstr self, std::vector< std::string >::iterator first, std::vector< std::string >::iterator last) -> std::vector< std::string >::iterator
        """
        return _ArcPCIe.vectorstr_erase(self, *args)

    def __init__(self, *args):
        r"""
        __init__(vectorstr self) -> vectorstr
        __init__(vectorstr self, vectorstr other) -> vectorstr
        __init__(vectorstr self, std::vector< std::string >::size_type size) -> vectorstr
        __init__(vectorstr self, std::vector< std::string >::size_type size, std::vector< std::string >::value_type const & value) -> vectorstr
        """
        _ArcPCIe.vectorstr_swiginit(self, _ArcPCIe.new_vectorstr(*args))

    def push_back(self, x):
        r"""push_back(vectorstr self, std::vector< std::string >::value_type const & x)"""
        return _ArcPCIe.vectorstr_push_back(self, x)

    def front(self):
        r"""front(vectorstr self) -> std::vector< std::string >::value_type const &"""
        return _ArcPCIe.vectorstr_front(self)

    def back(self):
        r"""back(vectorstr self) -> std::vector< std::string >::value_type const &"""
        return _ArcPCIe.vectorstr_back(self)

    def assign(self, n, x):
        r"""assign(vectorstr self, std::vector< std::string >::size_type n, std::vector< std::string >::value_type const & x)"""
        return _ArcPCIe.vectorstr_assign(self, n, x)

    def resize(self, *args):
        r"""
        resize(vectorstr self, std::vector< std::string >::size_type new_size)
        resize(vectorstr self, std::vector< std::string >::size_type new_size, std::vector< std::string >::value_type const & x)
        """
        return _ArcPCIe.vectorstr_resize(self, *args)

    def insert(self, *args):
        r"""
        insert(vectorstr self, std::vector< std::string >::iterator pos, std::vector< std::string >::value_type const & x) -> std::vector< std::string >::iterator
        insert(vectorstr self, std::vector< std::string >::iterator pos, std::vector< std::string >::size_type n, std::vector< std::string >::value_type const & x)
        """
        return _ArcPCIe.vectorstr_insert(self, *args)

    def reserve(self, n):
        r"""reserve(vectorstr self, std::vector< std::string >::size_type n)"""
        return _ArcPCIe.vectorstr_reserve(self, n)

    def capacity(self):
        r"""capacity(vectorstr self) -> std::vector< std::string >::size_type"""
        return _ArcPCIe.vectorstr_capacity(self)
    __swig_destroy__ = _ArcPCIe.delete_vectorstr

# Register vectorstr in _ArcPCIe:
_ArcPCIe.vectorstr_swigregister(vectorstr)
class ImgBuf_t(object):
    r"""Proxy of C++ arc::gen3::device::ImgBuf_t class."""

    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")
    __repr__ = _swig_repr
    pUserAddr = property(_ArcPCIe.ImgBuf_t_pUserAddr_get, _ArcPCIe.ImgBuf_t_pUserAddr_set, doc=r"""pUserAddr : p.std::uint16_t""")
    ulPhysicalAddr = property(_ArcPCIe.ImgBuf_t_ulPhysicalAddr_get, _ArcPCIe.ImgBuf_t_ulPhysicalAddr_set, doc=r"""ulPhysicalAddr : std::uint64_t""")
    ulSize = property(_ArcPCIe.ImgBuf_t_ulSize_get, _ArcPCIe.ImgBuf_t_ulSize_set, doc=r"""ulSize : std::uint64_t""")

    def __init__(self):
        r"""__init__(ImgBuf_t self) -> ImgBuf_t"""
        _ArcPCIe.ImgBuf_t_swiginit(self, _ArcPCIe.new_ImgBuf_t())
    __swig_destroy__ = _ArcPCIe.delete_ImgBuf_t

# Register ImgBuf_t in _ArcPCIe:
_ArcPCIe.ImgBuf_t_swigregister(ImgBuf_t)
class CArcDevice(object):
    r"""Proxy of C++ arc::gen3::CArcDevice class."""

    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _ArcPCIe.delete_CArcDevice

    def toString(self):
        r"""toString(CArcDevice self) -> std::string_view"""
        return _ArcPCIe.CArcDevice_toString(self)

    def isOpen(self):
        r"""isOpen(CArcDevice self) -> bool"""
        return _ArcPCIe.CArcDevice_isOpen(self)

    def open(self, *args):
        r"""
        open(CArcDevice self, std::uint32_t const uiDeviceNumber=0)
        open(CArcDevice self, std::uint32_t const uiDeviceNumber, std::uint32_t const uiBytes)
        open(CArcDevice self, std::uint32_t const uiDeviceNumber, std::uint32_t const uiRows, std::uint32_t const uiCols)
        """
        return _ArcPCIe.CArcDevice_open(self, *args)

    def close(self):
        r"""close(CArcDevice self)"""
        return _ArcPCIe.CArcDevice_close(self)

    def reset(self):
        r"""reset(CArcDevice self)"""
        return _ArcPCIe.CArcDevice_reset(self)

    def mapCommonBuffer(self, uiBytes=0):
        r"""mapCommonBuffer(CArcDevice self, std::size_t const uiBytes=0)"""
        return _ArcPCIe.CArcDevice_mapCommonBuffer(self, uiBytes)

    def unMapCommonBuffer(self):
        r"""unMapCommonBuffer(CArcDevice self)"""
        return _ArcPCIe.CArcDevice_unMapCommonBuffer(self)

    def reMapCommonBuffer(self, uiBytes=0):
        r"""reMapCommonBuffer(CArcDevice self, std::uint32_t const uiBytes=0)"""
        return _ArcPCIe.CArcDevice_reMapCommonBuffer(self, uiBytes)

    def fillCommonBuffer(self, uwValue=0):
        r"""fillCommonBuffer(CArcDevice self, std::uint16_t const uwValue=0)"""
        return _ArcPCIe.CArcDevice_fillCommonBuffer(self, uwValue)

    def commonBufferVA(self):
        r"""commonBufferVA(CArcDevice self) -> std::uint8_t *"""
        return _ArcPCIe.CArcDevice_commonBufferVA(self)

    def commonBufferPA(self):
        r"""commonBufferPA(CArcDevice self) -> std::uint64_t"""
        return _ArcPCIe.CArcDevice_commonBufferPA(self)

    def commonBufferSize(self):
        r"""commonBufferSize(CArcDevice self) -> std::uint64_t"""
        return _ArcPCIe.CArcDevice_commonBufferSize(self)

    def getId(self):
        r"""getId(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getId(self)

    def getStatus(self):
        r"""getStatus(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getStatus(self)

    def clearStatus(self):
        r"""clearStatus(CArcDevice self)"""
        return _ArcPCIe.CArcDevice_clearStatus(self)

    def set2xFOTransmitter(self, bOnOff):
        r"""set2xFOTransmitter(CArcDevice self, bool bOnOff)"""
        return _ArcPCIe.CArcDevice_set2xFOTransmitter(self, bOnOff)

    def loadDeviceFile(self, tFile):
        r"""loadDeviceFile(CArcDevice self, std::filesystem::path const & tFile)"""
        return _ArcPCIe.CArcDevice_loadDeviceFile(self, tFile)

    def command(self, *args):
        r"""
        command(CArcDevice self, std::initializer_list< std::uint32_t const > const & tCmdList) -> std::uint32_t
        command(CArcDevice self, std::initializer_list< std::uint32_t const > const & tCmdList, std::uint32_t uiExpectedReply)
        """
        return _ArcPCIe.CArcDevice_command(self, *args)

    def getControllerId(self):
        r"""getControllerId(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getControllerId(self)

    def resetController(self):
        r"""resetController(CArcDevice self)"""
        return _ArcPCIe.CArcDevice_resetController(self)

    def isControllerConnected(self):
        r"""isControllerConnected(CArcDevice self) -> bool"""
        return _ArcPCIe.CArcDevice_isControllerConnected(self)

    def setupController(self, *args):
        r"""setupController(CArcDevice self, bool bReset, bool bTdl, bool bPower, std::uint32_t const uiRows, std::uint32_t const uiCols, std::filesystem::path const & tTimFile, std::filesystem::path const & tUtilFile=std::filesystem::path(), std::filesystem::path const & tPciFile=std::filesystem::path(), bool * pAbort=None)"""
        return _ArcPCIe.CArcDevice_setupController(self, *args)

    def loadControllerFile(self, tFilename, bValidate=True, pAbort=None):
        r"""loadControllerFile(CArcDevice self, std::filesystem::path const & tFilename, bool bValidate=True, bool * pAbort=None)"""
        return _ArcPCIe.CArcDevice_loadControllerFile(self, tFilename, bValidate, pAbort)

    def setImageSize(self, uiRows, uiCols):
        r"""setImageSize(CArcDevice self, std::uint32_t const uiRows, std::uint32_t const uiCols)"""
        return _ArcPCIe.CArcDevice_setImageSize(self, uiRows, uiCols)

    def getImageRows(self):
        r"""getImageRows(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getImageRows(self)

    def getImageCols(self):
        r"""getImageCols(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getImageCols(self)

    def getCCParams(self):
        r"""getCCParams(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getCCParams(self)

    def isCCParamSupported(self, uiParameter):
        r"""isCCParamSupported(CArcDevice self, std::uint32_t const uiParameter) -> bool"""
        return _ArcPCIe.CArcDevice_isCCParamSupported(self, uiParameter)

    def isCCD(self):
        r"""isCCD(CArcDevice self) -> bool"""
        return _ArcPCIe.CArcDevice_isCCD(self)

    def isBinningSet(self):
        r"""isBinningSet(CArcDevice self) -> bool"""
        return _ArcPCIe.CArcDevice_isBinningSet(self)

    def setBinning(self, uiRows, uiCols, uiRowFactor, uiColFactor, pBinRows=None, pBinCols=None):
        r"""setBinning(CArcDevice self, std::uint32_t const uiRows, std::uint32_t const uiCols, std::uint32_t const uiRowFactor, std::uint32_t const uiColFactor, std::uint32_t * pBinRows=None, std::uint32_t * pBinCols=None)"""
        return _ArcPCIe.CArcDevice_setBinning(self, uiRows, uiCols, uiRowFactor, uiColFactor, pBinRows, pBinCols)

    def unSetBinning(self, uiRows, uiCols):
        r"""unSetBinning(CArcDevice self, std::uint32_t const uiRows, std::uint32_t const uiCols)"""
        return _ArcPCIe.CArcDevice_unSetBinning(self, uiRows, uiCols)

    def setSubArray(self, uiOldRows, uiOldCols, uiRow, uiCol, uiSubRows, uiSubCols, uiBiasOffset, uiBiasWidth):
        r"""setSubArray(CArcDevice self, std::uint32_t & uiOldRows, std::uint32_t & uiOldCols, std::uint32_t const uiRow, std::uint32_t const uiCol, std::uint32_t const uiSubRows, std::uint32_t const uiSubCols, std::uint32_t const uiBiasOffset, std::uint32_t const uiBiasWidth)"""
        return _ArcPCIe.CArcDevice_setSubArray(self, uiOldRows, uiOldCols, uiRow, uiCol, uiSubRows, uiSubCols, uiBiasOffset, uiBiasWidth)

    def unSetSubArray(self, uiRows, uiCols):
        r"""unSetSubArray(CArcDevice self, std::uint32_t const uiRows, std::uint32_t const uiCols)"""
        return _ArcPCIe.CArcDevice_unSetSubArray(self, uiRows, uiCols)

    def isSyntheticImageMode(self):
        r"""isSyntheticImageMode(CArcDevice self) -> bool"""
        return _ArcPCIe.CArcDevice_isSyntheticImageMode(self)

    def setSyntheticImageMode(self, bMode):
        r"""setSyntheticImageMode(CArcDevice self, bool bMode)"""
        return _ArcPCIe.CArcDevice_setSyntheticImageMode(self, bMode)

    def setOpenShutter(self, bShouldOpen):
        r"""setOpenShutter(CArcDevice self, bool bShouldOpen)"""
        return _ArcPCIe.CArcDevice_setOpenShutter(self, bShouldOpen)

    def expose(self, fExpTime, uiRows, uiCols, pAbort=None, pExpIFace=None, bOpenShutter=True):
        r"""expose(CArcDevice self, float const fExpTime, std::uint32_t const uiRows, std::uint32_t const uiCols, bool const * pAbort=None, arc::gen3::CExpIFace * pExpIFace=None, bool bOpenShutter=True)"""
        return _ArcPCIe.CArcDevice_expose(self, fExpTime, uiRows, uiCols, pAbort, pExpIFace, bOpenShutter)

    def stopExposure(self):
        r"""stopExposure(CArcDevice self)"""
        return _ArcPCIe.CArcDevice_stopExposure(self)

    def continuous(self, uiRows, uiCols, uiNumOfFrames, fExpTime, pAbort=None, pConIFace=None, bOpenShutter=True):
        r"""continuous(CArcDevice self, std::uint32_t const uiRows, std::uint32_t const uiCols, std::uint32_t const uiNumOfFrames, float const fExpTime, bool const * pAbort=None, arc::gen3::CConIFace * pConIFace=None, bool bOpenShutter=True)"""
        return _ArcPCIe.CArcDevice_continuous(self, uiRows, uiCols, uiNumOfFrames, fExpTime, pAbort, pConIFace, bOpenShutter)

    def stopContinuous(self):
        r"""stopContinuous(CArcDevice self)"""
        return _ArcPCIe.CArcDevice_stopContinuous(self)

    def isReadout(self):
        r"""isReadout(CArcDevice self) -> bool"""
        return _ArcPCIe.CArcDevice_isReadout(self)

    def getPixelCount(self):
        r"""getPixelCount(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getPixelCount(self)

    def getCRPixelCount(self):
        r"""getCRPixelCount(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getCRPixelCount(self)

    def getFrameCount(self):
        r"""getFrameCount(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getFrameCount(self)

    def containsError(self, *args):
        r"""
        containsError(CArcDevice self, std::uint32_t const uiWord) -> bool
        containsError(CArcDevice self, std::uint32_t const uiWord, std::uint32_t const uiWordMin, std::uint32_t const uiWordMax) -> bool
        """
        return _ArcPCIe.CArcDevice_containsError(self, *args)

    def getNextLoggedCmd(self):
        r"""getNextLoggedCmd(CArcDevice self) -> std::string const"""
        return _ArcPCIe.CArcDevice_getNextLoggedCmd(self)

    def getLoggedCmdCount(self):
        r"""getLoggedCmdCount(CArcDevice self) -> std::uint32_t"""
        return _ArcPCIe.CArcDevice_getLoggedCmdCount(self)

    def setLogCmds(self, bOnOff):
        r"""setLogCmds(CArcDevice self, bool bOnOff)"""
        return _ArcPCIe.CArcDevice_setLogCmds(self, bOnOff)

    def getArrayTemperature(self):
        r"""getArrayTemperature(CArcDevice self) -> double"""
        return _ArcPCIe.CArcDevice_getArrayTemperature(self)

    def getArrayTemperatureDN(self):
        r"""getArrayTemperatureDN(CArcDevice self) -> double"""
        return _ArcPCIe.CArcDevice_getArrayTemperatureDN(self)

    def setArrayTemperature(self, gTempVal):
        r"""setArrayTemperature(CArcDevice self, double gTempVal)"""
        return _ArcPCIe.CArcDevice_setArrayTemperature(self, gTempVal)

    def loadTemperatureCtrlData(self, tFilename):
        r"""loadTemperatureCtrlData(CArcDevice self, std::filesystem::path const & tFilename)"""
        return _ArcPCIe.CArcDevice_loadTemperatureCtrlData(self, tFilename)

    def saveTemperatureCtrlData(self, tFilename):
        r"""saveTemperatureCtrlData(CArcDevice self, std::filesystem::path const & tFilename)"""
        return _ArcPCIe.CArcDevice_saveTemperatureCtrlData(self, tFilename)

# Register CArcDevice in _ArcPCIe:
_ArcPCIe.CArcDevice_swigregister(CArcDevice)
ePCIeRegs_LCL_CFG_BAR = _ArcPCIe.ePCIeRegs_LCL_CFG_BAR

ePCIeRegs_DEV_REG_BAR = _ArcPCIe.ePCIeRegs_DEV_REG_BAR

ePCIeRegOffsets_REG_CMD_HEADER = _ArcPCIe.ePCIeRegOffsets_REG_CMD_HEADER

ePCIeRegOffsets_REG_CMD_COMMAND = _ArcPCIe.ePCIeRegOffsets_REG_CMD_COMMAND

ePCIeRegOffsets_REG_CMD_ARG0 = _ArcPCIe.ePCIeRegOffsets_REG_CMD_ARG0

ePCIeRegOffsets_REG_CMD_ARG1 = _ArcPCIe.ePCIeRegOffsets_REG_CMD_ARG1

ePCIeRegOffsets_REG_CMD_ARG2 = _ArcPCIe.ePCIeRegOffsets_REG_CMD_ARG2

ePCIeRegOffsets_REG_CMD_ARG3 = _ArcPCIe.ePCIeRegOffsets_REG_CMD_ARG3

ePCIeRegOffsets_REG_CMD_ARG4 = _ArcPCIe.ePCIeRegOffsets_REG_CMD_ARG4

ePCIeRegOffsets_REG_CTLR_SPECIAL_CMD = _ArcPCIe.ePCIeRegOffsets_REG_CTLR_SPECIAL_CMD

ePCIeRegOffsets_REG_RESET = _ArcPCIe.ePCIeRegOffsets_REG_RESET

ePCIeRegOffsets_REG_INIT_IMG_ADDR = _ArcPCIe.ePCIeRegOffsets_REG_INIT_IMG_ADDR

ePCIeRegOffsets_REG_FIBER_2X_CTRL = _ArcPCIe.ePCIeRegOffsets_REG_FIBER_2X_CTRL

ePCIeRegOffsets_REG_STATUS = _ArcPCIe.ePCIeRegOffsets_REG_STATUS

ePCIeRegOffsets_REG_CMD_REPLY = _ArcPCIe.ePCIeRegOffsets_REG_CMD_REPLY

ePCIeRegOffsets_REG_CTLR_ARG1 = _ArcPCIe.ePCIeRegOffsets_REG_CTLR_ARG1

ePCIeRegOffsets_REG_CTLR_ARG2 = _ArcPCIe.ePCIeRegOffsets_REG_CTLR_ARG2

ePCIeRegOffsets_REG_PIXEL_COUNT = _ArcPCIe.ePCIeRegOffsets_REG_PIXEL_COUNT

ePCIeRegOffsets_REG_FRAME_COUNT = _ArcPCIe.ePCIeRegOffsets_REG_FRAME_COUNT

ePCIeRegOffsets_REG_ID_LO = _ArcPCIe.ePCIeRegOffsets_REG_ID_LO

ePCIeRegOffsets_REG_ID_HI = _ArcPCIe.ePCIeRegOffsets_REG_ID_HI

eRegCmds_CONTROLLER_GET_ID = _ArcPCIe.eRegCmds_CONTROLLER_GET_ID

eRegCmds_CONTROLLER_RESET = _ArcPCIe.eRegCmds_CONTROLLER_RESET

eFiber2x_FIBER_2X_DISABLE = _ArcPCIe.eFiber2x_FIBER_2X_DISABLE

eFiber2x_FIBER_2X_ENABLE = _ArcPCIe.eFiber2x_FIBER_2X_ENABLE

eFiber_FIBER_A = _ArcPCIe.eFiber_FIBER_A

eFiber_FIBER_B = _ArcPCIe.eFiber_FIBER_B

class CArcPCIe(object):
    r"""Proxy of C++ arc::gen3::CArcPCIe class."""

    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")
    __repr__ = _swig_repr

    def __init__(self):
        r"""__init__(CArcPCIe self) -> CArcPCIe"""
        _ArcPCIe.CArcPCIe_swiginit(self, _ArcPCIe.new_CArcPCIe())
    __swig_destroy__ = _ArcPCIe.delete_CArcPCIe

    def toString(self):
        r"""toString(CArcPCIe self) -> std::string_view"""
        return _ArcPCIe.CArcPCIe_toString(self)

    def getCfgSpByte(self, uiOffset):
        r"""getCfgSpByte(CArcPCIe self, std::uint32_t const uiOffset) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_getCfgSpByte(self, uiOffset)

    def getCfgSpWord(self, uiOffset):
        r"""getCfgSpWord(CArcPCIe self, std::uint32_t const uiOffset) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_getCfgSpWord(self, uiOffset)

    def getCfgSpDWord(self, uiOffset):
        r"""getCfgSpDWord(CArcPCIe self, std::uint32_t const uiOffset) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_getCfgSpDWord(self, uiOffset)

    def setCfgSpByte(self, uiOffset, uiValue):
        r"""setCfgSpByte(CArcPCIe self, std::uint32_t const uiOffset, std::uint32_t const uiValue)"""
        return _ArcPCIe.CArcPCIe_setCfgSpByte(self, uiOffset, uiValue)

    def setCfgSpWord(self, uiOffset, uiValue):
        r"""setCfgSpWord(CArcPCIe self, std::uint32_t const uiOffset, std::uint32_t const uiValue)"""
        return _ArcPCIe.CArcPCIe_setCfgSpWord(self, uiOffset, uiValue)

    def setCfgSpDWord(self, uiOffset, uiValue):
        r"""setCfgSpDWord(CArcPCIe self, std::uint32_t const uiOffset, std::uint32_t const uiValue)"""
        return _ArcPCIe.CArcPCIe_setCfgSpDWord(self, uiOffset, uiValue)

    def getCfgSp(self):
        r"""getCfgSp(CArcPCIe self)"""
        return _ArcPCIe.CArcPCIe_getCfgSp(self)

    def getBarSp(self):
        r"""getBarSp(CArcPCIe self)"""
        return _ArcPCIe.CArcPCIe_getBarSp(self)

    @staticmethod
    def findDevices():
        r"""findDevices()"""
        return _ArcPCIe.CArcPCIe_findDevices()

    @staticmethod
    def deviceCount():
        r"""deviceCount() -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_deviceCount()

    def isOpen(self):
        r"""isOpen(CArcPCIe self) -> bool"""
        return _ArcPCIe.CArcPCIe_isOpen(self)

    def open(self, *args):
        r"""
        open(CArcPCIe self, std::uint32_t const uiDeviceNumber=0)
        open(CArcPCIe self, std::uint32_t const uiDeviceNumber, std::uint32_t const uiBytes)
        open(CArcPCIe self, std::uint32_t const uiDeviceNumber, std::uint32_t const uiRows, std::uint32_t const uiCols)
        """
        return _ArcPCIe.CArcPCIe_open(self, *args)

    def close(self):
        r"""close(CArcPCIe self)"""
        return _ArcPCIe.CArcPCIe_close(self)

    def reset(self):
        r"""reset(CArcPCIe self)"""
        return _ArcPCIe.CArcPCIe_reset(self)

    def getCommonBufferProperties(self):
        r"""getCommonBufferProperties(CArcPCIe self) -> bool"""
        return _ArcPCIe.CArcPCIe_getCommonBufferProperties(self)

    def mapCommonBuffer(self, uiBytes=0):
        r"""mapCommonBuffer(CArcPCIe self, std::size_t uiBytes=0)"""
        return _ArcPCIe.CArcPCIe_mapCommonBuffer(self, uiBytes)

    def unMapCommonBuffer(self):
        r"""unMapCommonBuffer(CArcPCIe self)"""
        return _ArcPCIe.CArcPCIe_unMapCommonBuffer(self)

    def getId(self):
        r"""getId(CArcPCIe self) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_getId(self)

    def getStatus(self):
        r"""getStatus(CArcPCIe self) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_getStatus(self)

    def clearStatus(self):
        r"""clearStatus(CArcPCIe self)"""
        return _ArcPCIe.CArcPCIe_clearStatus(self)

    def set2xFOTransmitter(self, bOnOff):
        r"""set2xFOTransmitter(CArcPCIe self, bool bOnOff)"""
        return _ArcPCIe.CArcPCIe_set2xFOTransmitter(self, bOnOff)

    def loadDeviceFile(self, tFile):
        r"""loadDeviceFile(CArcPCIe self, std::filesystem::path const & tFile)"""
        return _ArcPCIe.CArcPCIe_loadDeviceFile(self, tFile)

    def command(self, tCmdList):
        r"""command(CArcPCIe self, std::initializer_list< std::uint32_t const > const & tCmdList) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_command(self, tCmdList)

    def getControllerId(self):
        r"""getControllerId(CArcPCIe self) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_getControllerId(self)

    def resetController(self):
        r"""resetController(CArcPCIe self)"""
        return _ArcPCIe.CArcPCIe_resetController(self)

    def isControllerConnected(self):
        r"""isControllerConnected(CArcPCIe self) -> bool"""
        return _ArcPCIe.CArcPCIe_isControllerConnected(self)

    def isFiberConnected(self, *args):
        r"""isFiberConnected(CArcPCIe self, arc::gen3::device::eFiber eFiberId=arc::gen3::device::eFiber::FIBER_A) -> bool"""
        return _ArcPCIe.CArcPCIe_isFiberConnected(self, *args)

    def stopExposure(self):
        r"""stopExposure(CArcPCIe self)"""
        return _ArcPCIe.CArcPCIe_stopExposure(self)

    def isReadout(self):
        r"""isReadout(CArcPCIe self) -> bool"""
        return _ArcPCIe.CArcPCIe_isReadout(self)

    def getPixelCount(self):
        r"""getPixelCount(CArcPCIe self) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_getPixelCount(self)

    def getCRPixelCount(self):
        r"""getCRPixelCount(CArcPCIe self) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_getCRPixelCount(self)

    def getFrameCount(self):
        r"""getFrameCount(CArcPCIe self) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_getFrameCount(self)

    def writeBar(self, eBar, uiOffset, uiValue):
        r"""writeBar(CArcPCIe self, arc::gen3::device::ePCIeRegs eBar, std::uint32_t const uiOffset, std::uint32_t const uiValue)"""
        return _ArcPCIe.CArcPCIe_writeBar(self, eBar, uiOffset, uiValue)

    def readBar(self, eBar, uiOffset):
        r"""readBar(CArcPCIe self, arc::gen3::device::ePCIeRegs eBar, std::uint32_t const uiOffset) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_readBar(self, eBar, uiOffset)

    def readReply(self, fTimeOutSecs=1.5):
        r"""readReply(CArcPCIe self, double fTimeOutSecs=1.5) -> std::uint32_t"""
        return _ArcPCIe.CArcPCIe_readReply(self, fTimeOutSecs)
    fnPCIeStatusReplyRecvd = property(_ArcPCIe.CArcPCIe_fnPCIeStatusReplyRecvd_get, _ArcPCIe.CArcPCIe_fnPCIeStatusReplyRecvd_set, doc=r"""fnPCIeStatusReplyRecvd : std::function<(f(q(const).std::uint32_t).bool)>""")
    fnPCIeStatusControllerReset = property(_ArcPCIe.CArcPCIe_fnPCIeStatusControllerReset_get, _ArcPCIe.CArcPCIe_fnPCIeStatusControllerReset_set, doc=r"""fnPCIeStatusControllerReset : std::function<(f(q(const).std::uint32_t).bool)>""")
    fnPCIeStatusReadout = property(_ArcPCIe.CArcPCIe_fnPCIeStatusReadout_get, _ArcPCIe.CArcPCIe_fnPCIeStatusReadout_set, doc=r"""fnPCIeStatusReadout : std::function<(f(q(const).std::uint32_t).bool)>""")
    fnPCIeStatusIdle = property(_ArcPCIe.CArcPCIe_fnPCIeStatusIdle_get, _ArcPCIe.CArcPCIe_fnPCIeStatusIdle_set, doc=r"""fnPCIeStatusIdle : std::function<(f(q(const).std::uint32_t).bool)>""")
    fnPCIeStatusCmdSent = property(_ArcPCIe.CArcPCIe_fnPCIeStatusCmdSent_get, _ArcPCIe.CArcPCIe_fnPCIeStatusCmdSent_set, doc=r"""fnPCIeStatusCmdSent : std::function<(f(q(const).std::uint32_t).bool)>""")
    fnPCIeStatusImageReadTimeOut = property(_ArcPCIe.CArcPCIe_fnPCIeStatusImageReadTimeOut_get, _ArcPCIe.CArcPCIe_fnPCIeStatusImageReadTimeOut_set, doc=r"""fnPCIeStatusImageReadTimeOut : std::function<(f(q(const).std::uint32_t).bool)>""")
    fnPCIeStatusHeaderError = property(_ArcPCIe.CArcPCIe_fnPCIeStatusHeaderError_get, _ArcPCIe.CArcPCIe_fnPCIeStatusHeaderError_set, doc=r"""fnPCIeStatusHeaderError : std::function<(f(q(const).std::uint32_t).bool)>""")
    fnPCIeStatusFiber2xReceiver = property(_ArcPCIe.CArcPCIe_fnPCIeStatusFiber2xReceiver_get, _ArcPCIe.CArcPCIe_fnPCIeStatusFiber2xReceiver_set, doc=r"""fnPCIeStatusFiber2xReceiver : std::function<(f(q(const).std::uint32_t).bool)>""")
    fnPCIeStatusFiberAConnected = property(_ArcPCIe.CArcPCIe_fnPCIeStatusFiberAConnected_get, _ArcPCIe.CArcPCIe_fnPCIeStatusFiberAConnected_set, doc=r"""fnPCIeStatusFiberAConnected : std::function<(f(q(const).std::uint32_t).bool)>""")
    fnPCIeStatusFiberBConnected = property(_ArcPCIe.CArcPCIe_fnPCIeStatusFiberBConnected_get, _ArcPCIe.CArcPCIe_fnPCIeStatusFiberBConnected_set, doc=r"""fnPCIeStatusFiberBConnected : std::function<(f(q(const).std::uint32_t).bool)>""")

    def commonBufferVA(self):
        r"""commonBufferVA(CArcPCIe self) -> std::uint8_t"""
        return _ArcPCIe.CArcPCIe_commonBufferVA(self)

    def commonBufferVA_uint16(self):
        r"""commonBufferVA_uint16(CArcPCIe self) -> std::uint16_t"""
        return _ArcPCIe.CArcPCIe_commonBufferVA_uint16(self)

    def getDeviceStringList(self):
        r"""getDeviceStringList(CArcPCIe self) -> vectorstr"""
        return _ArcPCIe.CArcPCIe_getDeviceStringList(self)

# Register CArcPCIe in _ArcPCIe:
_ArcPCIe.CArcPCIe_swigregister(CArcPCIe)

