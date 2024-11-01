%define ModuleDocStr
"Python interface to CArcDeinterlace C++ code."
%enddef

%feature("autodoc", "1");
%module(package="ArcLib", docstring=ModuleDocStr) ArcFitsFile

%{
#include <filesystem>
#include <version>
#include <stdexcept>
#include <cstdint>
#include <utility>
#include <memory>
#include <string>

/*
Use the following preprocessor directives to force the
type alias arc::gen3::fits::keywordValue_t =std::tuple<>
*/
#ifdef __cpp_lib_variant
        #undef __cpp_lib_variant
#endif
#include <tuple>

#include <CArcFitsFileDllMain.h>
#include <CArcStringList.h>
#include <CArcBase.h>
#include <fitsio.h>
#include <CArcFitsFile.h>
%}

%init %{
%}

#define __attribute__(x)
#define GEN3_CARCFITSFILE_API __attribute__((visibility("default")))

// Specifies the default C++ to python exception handling interface
%exception {
    try {
        $action
    } catch (std::domain_error & e) {
        PyErr_SetString(PyExc_ArithmeticError, e.what());
        SWIG_fail;
    } catch (std::invalid_argument & e) {
        PyErr_SetString(PyExc_ValueError, e.what());
        SWIG_fail;
    } catch (std::length_error & e) {
        PyErr_SetString(PyExc_IndexError, e.what());
        SWIG_fail;
    } catch (std::out_of_range & e) {
        PyErr_SetString(PyExc_ValueError, e.what());
        SWIG_fail;
    } catch (std::logic_error & e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        SWIG_fail;
    } catch (std::range_error & e) {
        PyErr_SetString(PyExc_ValueError, e.what());
        SWIG_fail;
    } catch (std::underflow_error & e) {
        PyErr_SetString(PyExc_ArithmeticError, e.what());
        SWIG_fail;
    } catch (std::overflow_error & e) {
        PyErr_SetString(PyExc_OverflowError, e.what());
        SWIG_fail;
    } catch (std::runtime_error & e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        SWIG_fail;
    } catch (std::exception & e) {
        PyErr_SetString(PyExc_Exception, e.what());
        SWIG_fail;
    } catch (...) {
        SWIG_fail;
    }
}
%include "std_unique_ptr.i"
%include "typemaps.i"
%include "std_string.i"
%include "CArcFitsFile.h"

%template(arcFitsFileUint16) arc::gen3::CArcFitsFile<arc::gen3::fits::BPP_16>;
%template(arcFitsFileUint32) arc::gen3::CArcFitsFile<arc::gen3::fits::BPP_32>;

//%extend arcticICC::CameraConfig {
//    std::string __repr__() const {
//        std::ostringstream os;
//        os << (*self);
//        return os.str();
//    }
//}
