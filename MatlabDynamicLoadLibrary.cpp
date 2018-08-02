#include "MatlabDynamicLoadLibrary.h"

namespace Matlab{

MatlabDynamicLoadLibray::MatlabDynamicLoadLibray() {
}

MatlabDynamicLoadLibray::~MatlabDynamicLoadLibray() {
}

MatlabDynamicLoadLibray& MatlabDynamicLoadLibray::instance() {
    static MatlabDynamicLoadLibray instance;
    return instance;
}

MatlabEngineDll::MatlabEngineDll() {
    _lib.setDllName(libName());
}

MatlabMatrixDll::MatlabMatrixDll() {
    _lib.setDllName(libName());
}

MatlabMatDll::MatlabMatDll() {
    _lib.setDllName(libName());
}

}
