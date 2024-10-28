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

/* Extensions to the arc::gen3::CArcPCI */
%extend arc::gen3::CArcPCI {
        /* Overload return/params for arc::gen3::CArcPCI::getDeviceStringList() */
        static std::vector<std::string> getDeviceStringList(void) {
                std::shared_ptr<std::string[]> deviceStringList = arc::gen3::CArcPCI::getDeviceStringList().lock();
                std::vector<std::string> devList(arc::gen3::CArcPCI::deviceCount(), *deviceStringList.get());
                return devList;
        }

        /* 
         * The following will modify return types or include class methods in
         * arc::gen3::CArcDevice and make them available in the wrapper around
         * the arc::gen3::CArcPCI class.
         */

        /* Overload return for arc::gen3::CArcDevice::commonBufferVA()
         * Returns a uint8. This will modify the API. 
         */
        std::uint8_t commonBufferVA( void ) {
                return *($self->CArcDevice::commonBufferVA());
        }
        /* Overload return for arc::gen3::CArcDevice::commonBufferVA()
         * Returns a uint16. This modifies the API.
         */
        std::uint16_t commonBufferVA_uint16( void ) {
                return static_cast<std::uint16_t>( *($self->CArcDevice::commonBufferVA()) );
        }
        /* Extend to use CArcDevice::fillCommonBuffer() */
        void fillCommonBuffer( const std::uint16_t uwValue=0 ) {
                $self->fillCommonBuffer( uwValue );
        }
        /* Extend to use CArcDevice::setupController() */
        void setupController(   bool bReset,
                                bool bTdl,
                                bool bPower,
                                const std::uint32_t uiRows,
                                const std::uint32_t uiCols,
                                const std::filesystem::path &tTimFile,
                                const std::filesystem::path &tUtilFile=std::filesystem::path(),
                                const std::filesystem::path &tPciFile=std::filesystem::path(),
                                bool *pAbort=nullptr
                                ) {
                $self->setupController( bReset,
                                        bTdl,
                                        bPower,
                                        uiRows,
                                        uiCols,
                                        tTimFile,
                                        tUtilFile,
                                        tPciFile,
                                        pAbort
                                        );
        }
        /* Extend to use CArcDevice::loadControllerFile() */
        void loadControllerFile(        const std::filesystem::path &tFilename,
                                        bool bValidate=true,
                                        bool *pAbort=nullptr
                                        ) {
                $self->loadControllerFile( tFilename, bValidate, pAbort );
        }
        /* Extend to use CArcDevice::setImageSize() */
        void setImageSize(      const std::uint32_t uiRows,
                                const std::uint32_t uiCols
                                ) {
                $self->setImageSize( uiRows, uiCols );
        }
        /* Extend to use CArcDevice::getImageRows() */
        std::uint32_t getImageRows( void ) {
                return $self->getImageRows();
        }
        /* Extend to use CArcDevice::getImageCols() */
        std::uint32_t getImageCols( void ) {
                return $self->getImageCols();
        }
        /* Extend to use CArcDevice::getCCParams() */
        std::uint32_t getCCParams( void ) {
                return $self->getCCParams();
        }
        /* Extend to use CArcDevice::isBinningSet() */
        bool isBinningSet( void ) {
                return $self->isBinningSet();
        }
        /* Extend to use CArcDevice::setBinning() */
        void setBinning(        const std::uint32_t uiRows, 
                                const std::uint32_t uiCols, 
                                const std::uint32_t uiRowFactor, 
                                const std::uint32_t uiColFactor, 
                                std::uint32_t *pBinRows=nullptr, 
                                std::uint32_t *pBinCols=nullptr
                                ) {
                $self->setBinning(      uiRows,
                                        uiCols,
                                        uiRowFactor,
                                        uiColFactor,
                                        pBinRows,
                                        pBinCols
                                        );
        }
        /* Extend to use CArcDevice::unSetBinning */
        void unSetBinning(      const std::uint32_t uiRows,
                                const std::uint32_t uiCols
                                ) {
                $self->unSetBinning( uiRows, uiCols );
        }
        /* Extend to use CArcDevice::setSubArray() */
        void setSubArray(       std::uint32_t &uiOldRows,
                                std::uint32_t &uiOldCols, 
                                const std::uint32_t uiRow, 
                                const std::uint32_t uiCol, 
                                const std::uint32_t uiSubRows, 
                                const std::uint32_t uiSubCols, 
                                const std::uint32_t uiBiasOffset, 
                                const std::uint32_t uiBiasWidth
                                ) {
                $self->setSubArray(     uiOldRows,
                                        uiOldCols,
                                        uiRow,
                                        uiCol,
                                        uiSubRows,
                                        uiSubCols,
                                        uiBiasOffset,
                                        uiBiasWidth
                                        );
        }
        /* Extend to use CArcDevice::unSetSubArray() */
        void unSetSubArray(const std::uint32_t uiRows, const std::uint32_t uiCols) {
                $self->unSetSubArray(uiRows, uiCols);
        }
        /* Extend to use CArcDevice::isSyntheticImageMode() */
        bool isSyntheticImageMode( void ) {
                return $self->isSyntheticImageMode();
        }
        /* Extend to use CArcDevice::setSyntheticImageMode() */
        void setSyntheticImageMode( bool bMode ) {
                $self->setSyntheticImageMode( bMode );
        }
        /* Extend to use CArcDevice::setOpenShutter() */
        void setOpenShutter( bool bShouldOpen ) {
                $self->setOpenShutter( bShouldOpen );
        }
        /* Extend to use CArcDevice::expose() */
        void expose(    const float fExpTime, 
                        const std::uint32_t uiRows, 
                        const std::uint32_t uiCols, 
                        const bool *pAbort=nullptr, 
                        arc::gen3::CExpIFace *pExpIFace=nullptr, 
                        bool bOpenShutter=true
                        ) {
                $self->expose(  fExpTime,
                                uiRows,
                                uiCols,
                                pAbort,
                                pExpIFace,
                                bOpenShutter
                                );
        }
        /* Extend to use CArcDevice::continuous() */
        void continuous(        const std::uint32_t uiRows, 
                                const std::uint32_t uiCols, 
                                const std::uint32_t uiNumOfFrames, 
                                const float fExpTime, 
                                const bool *pAbort=nullptr, 
                                arc::gen3::CConIFace *pConIFace=nullptr, 
                                bool bOpenShutter=true
                                ) {
                $self->continuous(      uiRows,
                                        uiCols,
                                        uiNumOfFrames,
                                        fExpTime,
                                        pAbort,
                                        pConIFace,
                                        bOpenShutter
                                        );
        }
        /* Extend to use CArcDevice::stopContinuous() */
        void stopContinuous( void ) {
                $self->stopContinuous();
        }
        /* Extend to use CArcDevice::getCRPixelCount() */
        std::uint32_t getCRPixelCount( void ) {
                $self->getCRPixelCount();
        }
        /* Extend to use CArcDevice::getArrayTemperature() */
        double getArrayTemperature( void ) {
                return $self->getArrayTemperature();
        }
        /* Extend to use CArcDevice::getArrayTemperatureDN() */
        double getArrayTemperatureDN( void ) {
                return $self->getArrayTemperatureDN();
        }
        /* Extend to use CArcDevice::setArrayTemperature() */
        void setArrayTemperature( double gTempVal ) {
                $self->setArrayTemperature( gTempVal );
        }
}
/* Ignore the original prototype of arc::gen3::CArcPCI::getDeviceStringList() */
%ignore arc::gen3::CArcPCI::getDeviceStringList();

/*
 * Ignore this member function because it takes std::initializer_list<T> as
 * input, which is hard to wrap. It could be extended later to take
 * std::vector<T> as input however its not really intended to be used in this
 * way in the original API.
 */
%ignore arc::gen3::CArcPCI::command(const std::initializer_list<const std::uint32_t>&);

%import "CArcDevice.h"
%import "CArcPCIBase.h"
%include "CArcPCI.h"

