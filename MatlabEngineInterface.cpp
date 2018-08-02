#include "MatlabEngineInterface.h"

namespace Matlab {

MatlabEngineInterface::MatlabEngineInterface() {
    updateBufferPointerSize(_bufferSize);
    open();    
}

MatlabEngineInterface::MatlabEngineInterface(bool show) {
    updateBufferPointerSize(_bufferSize);
    open(show);
}

MatlabEngineInterface::MatlabEngineInterface(size_t bSize) {
    updateBufferPointerSize(bSize);
    open();
}

MatlabEngineInterface::MatlabEngineInterface(size_t bSize, bool show) {
    updateBufferPointerSize(bSize);
    open(show);
}

MatlabEngineInterface::~MatlabEngineInterface() {
    close(true);
}

bool MatlabEngineInterface::open(bool show) {    
    if (MLibs::instance().mxEngine().isLoaded()) {
#if defined _WIN32 || defined _WIN64
        GET_FP(MLibs::instance().mxEngine().lib(),engOpenSingleUse,engOpenSingleUse)
        _engine = engOpenSingleUse(nullptr,nullptr,nullptr);
#else
        GET_FP(MLibs::instance().mxEngine().lib(),engOpen,engOpen);
        _engine = engOpen("");
#endif
        setOutputBuffer();
        setVisable(show);
    }
    return isConnected();
}

bool MatlabEngineInterface::close(bool forceClose) {
    if (MLibs::instance().mxEngine().isLoaded()) {
        if (forceClose){ // Send quit command
            if (isConnected()) {
                runCommand("exit");
            }
        }
        else {
            GET_FP(MLibs::instance().mxEngine().lib(),engClose,engClose)
            engClose(_engine);
        }
    }
    return isConnected();
}

bool MatlabEngineInterface::runCommand(const std::string &c) {
    if (MLibs::instance().mxEngine().isLoaded()) {
        GET_FP(MLibs::instance().mxEngine().lib(),engEvalString,engEvalString)
        return !engEvalString(_engine,c.c_str());
    }
    else {
        return false;
    }
}

mxArray *MatlabEngineInterface::getVar(const char *varName) const {
    if (MLibs::instance().mxEngine().isLoaded()) {
        GET_FP(MLibs::instance().mxEngine().lib(),engGetVariable,engGetVariable)
        return engGetVariable(_engine,varName);
    }
    else {
        return nullptr;
    }
}

mxArray *MatlabEngineInterface::getVar(const std::string &varName) const {
    return getVar(varName.c_str());
}

bool MatlabEngineInterface::putVar(const char *varName, const mxArray *pm) {
    if (MLibs::instance().mxEngine().isLoaded()) {
        GET_FP(MLibs::instance().mxEngine().lib(),engPutVariable,engPutVariable)
        return !engPutVariable(_engine,varName, pm);
    }
    else {
        return false;
    }
}

bool MatlabEngineInterface::putVar(const std::string &varName, const mxArray *pm) {
    return putVar(varName.c_str() ,pm);
}

#ifdef QT_CORE_LIB
mxArray *MatlabEngineInterface::getVar(const QString &varName) const {
    return getVar(varName.toUtf8().constData());
}

bool MatlabEngineInterface::putVar(const QString &varName, const mxArray *pm) {
    return putVar(varName.toUtf8().constData(), pm);
}
#endif

double MatlabEngineInterface::getScalarFromStruct(
        const mxArray *matStruct, const std::string &fieldName) {
    GET_FP(MLibs::instance().mxMatrix().lib(),mxGetField,mxGetField)
    GET_FP(MLibs::instance().mxMatrix().lib(),mxIsNumeric,mxIsNumeric)
    GET_FP(MLibs::instance().mxMatrix().lib(),mxGetPr,mxGetPr)
    mxArray* val = mxGetField(matStruct,0,fieldName.c_str());
    mxAssert(mxIsNumeric(val),"MatOperator::getScalarFromStruct() does not report value appropriately");
    return *mxGetPr(val);
}

std::string MatlabEngineInterface::getStringFromStruct(
        const mxArray *matStruct, const std::string &fieldName) {
    GET_FP(MLibs::instance().mxMatrix().lib(),mxGetField,mxGetField)
    GET_FP(MLibs::instance().mxMatrix().lib(),mxArrayToString,mxArrayToString)
    mxArray* val = mxGetField(matStruct,0,fieldName.c_str());
    std::string str(mxArrayToString(val));
    return str;
}

std::vector<std::string> MatlabEngineInterface::getStringVecFromStruct(const mxArray *matStruct, const std::string &fieldName) {
    GET_FP(MLibs::instance().mxMatrix().lib(),mxGetField,mxGetField)
    GET_FP(MLibs::instance().mxMatrix().lib(),mxGetNumberOfElements,mxGetNumberOfElements)
    GET_FP(MLibs::instance().mxMatrix().lib(),mxGetCell,mxGetCell)
    GET_FP(MLibs::instance().mxMatrix().lib(),mxArrayToString,mxArrayToString)
    GET_FP(MLibs::instance().mxMatrix().lib(),mxFree,mxFree)
    std::vector<std::string> strVec;
    mxArray* cellPtr = mxGetField(matStruct,0,fieldName.c_str());
    size_t nCell = mxGetNumberOfElements(cellPtr);
    for (size_t idx = 0 ; idx < nCell ; ++idx) {
        mxArray* iCellPtr = mxGetCell(cellPtr, idx);
        char* bufStr = mxArrayToString(iCellPtr);
        strVec.push_back(bufStr);
        mxFree(bufStr);
    }
    return strVec;
}

bool MatlabEngineInterface::getBoolFromStruct(const mxArray *matStruct, const std::string &fieldName) {
    GET_FP(MLibs::instance().mxMatrix().lib(),mxGetField,mxGetField)
    GET_FP(MLibs::instance().mxMatrix().lib(),mxIsLogical,mxIsLogical)
    GET_FP(MLibs::instance().mxMatrix().lib(),mxGetLogicals,mxGetLogicals)
    mxArray* val = mxGetField(matStruct,0,fieldName.c_str());
    mxAssert(mxIsLogical(val),"MatOperator::getBoolFromStruct() does not report value appropriately");
    return *mxGetLogicals(val);
}

void MatlabEngineInterface::transformStrvecToChars(const std::vector<std::string> &sFieldNames, std::vector<const char *> &cFieldNames) {
    for (const auto &str:sFieldNames) {
        cFieldNames.push_back(const_cast<const char*>(str.c_str()));
    }
}

void MatlabEngineInterface::updateBufferPointerSize(size_t bSize) {
    if (bSize == _bufferSize) { // no update
        return;
    }
    else {
        if (_hasOutputBuffer) {
            delete[] _outputBuffer;
        }
        _bufferSize = bSize;
    }
    if (bSize == 0) {
        _outputBuffer = NULL; // reset to original state
        _hasOutputBuffer = false;
    }
    else {
        char* tempBuffer;
        tempBuffer = new char[bSize+1];
        _outputBuffer = tempBuffer;
        _outputBuffer[0] = '\0';
    }
}

std::string MatlabEngineInterface::getEngineOutput() const {
    return std::string(_outputBuffer);
}

void MatlabEngineInterface::cclean() {
    runCommand("clear functions;clear;");
}

void MatlabEngineInterface::setOutputBuffer() {
    setOutputBuffer(_bufferSize);
}

void MatlabEngineInterface::setOutputBuffer(size_t bSize) {
    if (MLibs::instance().mxEngine().isLoaded()) {
        updateBufferPointerSize(bSize);
        GET_FP(MLibs::instance().mxEngine().lib(),engOutputBuffer,engOutputBuffer)
        engOutputBuffer(_engine, _outputBuffer, _bufferSize);
    }
}

bool MatlabEngineInterface::isVisible() {
    if (MLibs::instance().mxEngine().isLoaded()) {
        bool isVisable;
        GET_FP(MLibs::instance().mxEngine().lib(),engGetVisible,engGetVisible)
        engGetVisible(_engine, &isVisable);
        return isVisable;
    }
    else {
        return false;
    }
}

bool MatlabEngineInterface::setVisable(bool val) {
    if (MLibs::instance().mxEngine().isLoaded()) {
        GET_FP(MLibs::instance().mxEngine().lib(),engSetVisible,engSetVisible)
        return !engSetVisible(_engine, val);
    }
    else {
        return false;
    }
}

bool MatlabEngineInterface::isConnected() {
    if (_engine) {
        return runCommand(""); // Send a dummy command to test if the engine is alive
    }
    else {
        return false;
    }
}

}
