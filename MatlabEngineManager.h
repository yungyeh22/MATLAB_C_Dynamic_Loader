#ifndef MATLABENGINEMANAGER_H
#define MATLABENGINEMANAGER_H

#include <QDir>
#include "MatlabEngineInterface.h"

namespace Matlab {

/*
 * A singleton manager class for Matlab engine interface
 *  This class manages Matlab interface to Matlab api engine. This class uses sigleton design patten.
 *  An interface including a Matlab engine will be create when a unique id is requested.
 *  The interface can be deleted at anytime. The associated Matlab engine window will be closed when instructed.
 */
class MatlabEngineManager
{

private:
    MatlabEngineManager();

public:
    enum class ConsoleWindowStatus {Hide,Show,Auto};
    ~MatlabEngineManager();
    static MatlabEngineManager& instance();
    MatlabEngineManager(MatlabEngineManager const&) = delete;
    void operator=(MatlabEngineManager const&) = delete;
    MatlabEngineInterface *getEngineById(const int &id = 0); // Returns the interface by its id. if the id does not exist, create one.
    void deleteEngineById(const int &id); // deletes an existing engine
    void deleteAll(); // Delete all available engines
    bool exists(const int &id); // Check if an ID exist.
    size_t count() {return _engsMap.size();} // return current number of Matlab engines holding in the pool
    const QDir &establishMatlabLibrayDirectory();
    void setMatlabDir(const QDir &matlabDir) { _matlabDir = matlabDir; }
    QDir matlabDir(){ return _matlabDir; }
    bool isMatlabAvailable () { return _isMatlabAvailable; }
    void setShowMatlabConsole(ConsoleWindowStatus status = ConsoleWindowStatus::Auto){ _showMatlabConsole = status; }
    bool showMatlabConsole();

private: // Private functions
    QDir searchMatlabDir();

private: // Private properties
    std::map<const int, MatlabEngineInterface*> _engsMap;
    QDir _matlabDir;
    bool _isMatlabAvailable = false;
    ConsoleWindowStatus _showMatlabConsole = ConsoleWindowStatus::Auto;
};

}

typedef Matlab::MatlabEngineManager MManager;

#endif // MATLABENGINEMANAGER_H
