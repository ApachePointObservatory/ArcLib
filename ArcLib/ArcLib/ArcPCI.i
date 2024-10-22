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

/* Template for getDeviceStringList return */
%template(vectorString) std::vector<std::string>;
%template(vectorUint32) std::vector<std::uint32_t>;

%extend arc::gen3::CArcPCI {
        /* Overload return/params for arc::gen3::CArcPCI::commonBufferVA() */
        std::uint8_t commonBufferVA( void ) {
                return *($self->CArcDevice::commonBufferVA());
        }
        /* Overload return/params for arc::gen3::CArcPCI::commonBufferVA() */
        std::uint16_t commonBufferVA_uint16( void ) {
                return static_cast<std::uint16_t>( *($self->CArcDevice::commonBufferVA()) );
        }
        /* Overload return/params for arc::gen3::CArcPCI::getDeviceStringList() */
        static std::vector<std::string> getDeviceStringList(void) {
                std::shared_ptr<std::string[]> deviceStringList = arc::gen3::CArcPCI::getDeviceStringList().lock();
                std::vector<std::string> devList(arc::gen3::CArcPCI::deviceCount(), *deviceStringList.get());
                return devList;
        }
}
/* Ignore the original prototype of arc::gen3::CArcPCI::getDeviceStringList() */
%ignore arc::gen3::CArcPCI::getDeviceStringList();
/* Ignore this member function because it takes std::initializer_list<T> as
   input, which is hard to wrap. It could be extended later to take
   std::vector<T> as input. */
%ignore arc::gen3::CArcPCI::command(const std::initializer_list<const std::uint32_t>&);

%import "CArcDevice.h"
%import "CArcPCIBase.h"
%include "CArcPCI.h"

