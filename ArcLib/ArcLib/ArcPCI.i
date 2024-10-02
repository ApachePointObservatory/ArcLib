%define ModuleDocStr
"Python interface to CArcDeinterlace C++ code."
%enddef

%feature("autodoc", "1");
%module(package="ArcLib", docstring=ModuleDocStr) ArcPCI

%{

#include <string_view>
#include <filesystem>
#include <vector>
#include <string>
#include <memory>

#include <CArcDeviceDllMain.h>
#include <CArcPCIBase.h>
#include <CArcStringList.h>
#include <ArcDefs.h>
#include <CArcDevice.h>
#include <CArcPCI.h>
%}

%init %{
%}

#define __attribute__(x)
#define GEN3_CARCDEVICE_API __attribute__((visibility("default")))

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
%include "std_shared_ptr.i"
%include "std_string.i"
%include "std_string_view.i"
%include "std_filesystem.i"
%include "std_vector.i"
%include "typemaps.i"
%include "stdint.i"

%feature("notabstract") arc::gen3::CArcPCI;

%include "CArcDevice.h"
%import "CArcPCIBase.h"
%include "CArcPCI.h"

//%extend arcticICC::CameraConfig {
//    std::string __repr__() const {
//        std::ostringstream os;
//        os << (*self);
//        return os.str();
//    }
//}
