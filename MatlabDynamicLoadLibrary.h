#ifndef MATLABDYNAMICLOADLIBRARY_H
#define MATLABDYNAMICLOADLIBRARY_H

#include "Util/DynamicLinkLib/ILibrary.h"
#include "Util/DynamicLinkLib/DynamicLoadLibrary.h"

#include "engine.h"
#include "matrix.h"
#include "mat.h"

namespace Matlab{

typedef ::Util::DynamicLoadLibrary DymLib;

/*
 * Load dll for MATLAB engine library and provide methods to its original functions
 * Example:
 *      MatlabEngineDll libEng;
 *      libEng.loadLib();
 *      Engine* (*EngineOpen) (const char*, void*, int*);
 *      EngineOpen engOpen = libEng.lib().getFunction<EngineOpen>("engOpenSingleUse");
 *      engOpen(NULL,NULL,NULL);
 */
class MatlabEngineDll : public Util::ILibrary {

public:
    MatlabEngineDll();
    // ILibrary interface
    const char * libName() const override { return "libeng.dll"; }
    bool loadLib() override { return _lib.load(); }
    bool unloadLib() override { return _lib.unload(); }
    // Public methods
    DymLib &lib() { return _lib; }
    bool isLoaded() { return _lib.isLoaded(); }

private:
    DymLib _lib;

};

/* Load dll for MATLAB matrix library and provide methods to its original functions */
class MatlabMatrixDll : public Util::ILibrary {

public:
    MatlabMatrixDll();
    // ILibrary interface
    const char * libName() const override { return "libmx.dll"; }
    bool loadLib() override { return _lib.load(); }
    bool unloadLib() override { return _lib.unload(); }
    // Public methods
    DymLib &lib() { return _lib; }
    bool isLoaded() { return _lib.isLoaded(); }

private:
    DymLib _lib;
};

/* Load dll for MATLAB mat library and provide methods to its original functions */
class MatlabMatDll : public Util::ILibrary {

public:
    MatlabMatDll() ;
    // ILibrary interface
    const char * libName() const override { return "libmat.dll"; }
    bool loadLib() override { return _lib.load(); }
    bool unloadLib() override { return _lib.unload(); }
    // Public methods
    DymLib &lib() { return _lib; }
    bool isLoaded() { return _lib.isLoaded(); }

private:
    DymLib _lib;
};


/* A singleton class that provides the access to the library necessary to run the Matlab plugin */
class MatlabDynamicLoadLibray {

public:
    ~MatlabDynamicLoadLibray();
    static MatlabDynamicLoadLibray& instance();
    MatlabDynamicLoadLibray(MatlabDynamicLoadLibray const&) = delete;
    void operator=(MatlabDynamicLoadLibray const&) = delete;

    MatlabEngineDll& mxEngine() { return _libEng; }
    MatlabMatrixDll& mxMatrix() {return _libMx;}
    MatlabMatDll& mxMat() {return _libMat;}

private:
    MatlabDynamicLoadLibray();
    MatlabEngineDll _libEng;
    MatlabMatrixDll _libMx;
    MatlabMatDll _libMat;
};

}

typedef ::Matlab::MatlabDynamicLoadLibray MLibs;

#endif // MATLABDYNAMICLOADLIBRARY_H
