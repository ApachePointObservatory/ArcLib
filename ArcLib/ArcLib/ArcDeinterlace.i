%define ModuleDocStr
"Python interface to CArcDeinterlace C++ code."
%enddef

%feature("autodoc", "1");
%module(package="ArcLib", docstring=ModuleDocStr) ArcDeinterlace

%{
#include <initializer_list>
#include <cstdint>
#include <cstdarg>
#include <memory>
#include <string>
#include <vector>
#include <CArcDeinterlace.h>
#include <CArcDeinterlaceDllMain.h>
#include <CArcPluginManager.h>
#include <CArcBase.h>

%}

%init %{
%}

#define __attribute__(x)
//#define GEN3_CARCBASE_API __attribute__((visibility( "default" )))
#define GEN3_CARCDEINTERLACE_API __attribute__((visibility( "default" )))

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

/* cant do this because SWIG can't handle requires expressions from c++20 yet
%import "CArcBase.h" */
%include "CArcDeinterlace.h"

//%template(arcDeinterlaceUint8) arc::gen3::CArcDeinterlace<uint8_t>;
%template(arcDeinterlaceUint16) arc::gen3::CArcDeinterlace<arc::gen3::dlace::BPP_16>;
%template(arcDeinterlaceUint32) arc::gen3::CArcDeinterlace<arc::gen3::dlace::BPP_32>;

//%extend arcticICC::CameraConfig {
//    std::string __repr__() const {
//        std::ostringstream os;
//        os << (*self);
//        return os.str();
//    }
//}
