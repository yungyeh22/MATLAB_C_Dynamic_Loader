#ifndef MATLABENGINEINTERFACE_H
#define MATLABENGINEINTERFACE_H
#ifdef QT_CORE_LIB
#include <QString>
#endif
#include "MatlabDynamicLoadLibrary.h"

namespace Matlab{

/*
 * An interface to open/close/operate on a Matlab engine
 *  MatlabEngineInterface provides mothod to open, close, configure a Matlab engine, as well as perform operations to the engine.
 *  It also provides helper functions as static methods to assist parsing the result from which the Matlab computational engine
 *  calculated.
 */
class MatlabEngineInterface
{
public:
    MatlabEngineInterface();
    MatlabEngineInterface(bool show);
    MatlabEngineInterface(size_t bSize);
    MatlabEngineInterface(size_t bSize, bool show);
    ~MatlabEngineInterface();
    // open/close
    bool open(bool show = true);
    bool close(bool forceClose = false);
    // Query engine
    bool runCommand(const std::string &c); // Run commands with output
    // Retrive variable from engine's workspace
    mxArray *getVar(const char* varName) const; // Get result (from runCommand()) pointer
    mxArray *getVar(const std::string &varName) const;
    // Pass variable to engine's workspace
    bool putVar(const char* varName, const mxArray* pm);
    bool putVar(const std::string &varName, const mxArray* pm);
#ifdef QT_CORE_LIB
    bool putVar(const QString &varName, const mxArray* pm);
    mxArray *getVar(const QString &varName) const;
#endif
    // Parsers to the mxArray pointer
    static double getScalarFromStruct(const mxArray* matStruct, const std::string &fieldName);
    static std::string getStringFromStruct(
            const mxArray *matStruct, const std::string &fieldName);
    static std::vector<std::string> getStringVecFromStruct(const mxArray* matStruct,const std::string &fieldName);
    static bool getBoolFromStruct(const mxArray* matStruct,const std::string &fieldName);
    static void transformStrvecToChars(const std::vector<std::string> &sFieldNames, std::vector<const char*> &cFieldNames); // Transform string

    // Utilities
    std::string getEngineOutput() const; // Get output string from the engine's buffer
    void cclean(); // Clean Cache
    void setOutputBuffer(); // Assign the string buffer to engine with default size.
    void setOutputBuffer(size_t bSize); // Assign the string buffer to engine with specific size
    bool isSingleUse() {return _isSingleUse;} // Single use type of engine? Only valid in window system.
    bool isVisible(); // Check if the engline console window is visible.
    bool setVisable(bool val); // Set the visibility for engine cosole window.
    bool isConnected(); // Check connection status between engine pointer and the engine console.

private: // properties
    Engine* _engine = nullptr;
    size_t _bufferSize = 512; // Default buffer size
    char* _outputBuffer = nullptr; // Buffer string for engine output
    bool _hasOutputBuffer = false; // Check if the engine has output
    bool _isSingleUse = false; // Windows only, to distinguish single use type of engine.

private: // methods
    void updateBufferPointerSize(size_t bSize); // Resizing array size to char pointer. Original buffer will be erased.    
};

}

typedef Matlab::MatlabEngineInterface MInterface;


#endif // MATLABENGINEINTERFACE_H
