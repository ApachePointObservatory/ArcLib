%define ModuleDocStr
"Python interface to CArcDeinterlace C++ code."
%enddef

%feature("autodoc", "1");
%module(package="ArcLib", docstring=ModuleDocStr) ArcDefs

%{
#include <cstdint>

#include <ArcDefs.h>
%}

%init %{
%}

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

%include "stdint.i"

%include "ArcDefs.h"

