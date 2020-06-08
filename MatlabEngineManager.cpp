#include "MatlabEngineManager.h"
#include <QSettings>
#include <QStandardPaths>

namespace Matlab {

MatlabEngineManager::MatlabEngineManager() {
    establishMatlabLibrayDirectory();
}

MatlabEngineManager::~MatlabEngineManager() {    
}

MatlabEngineManager& MatlabEngineManager::instance() {
    static MatlabEngineManager instance;
    return instance;
}

MatlabEngineInterface *MatlabEngineManager::getEngineById(const int &id) {
    MatlabEngineInterface* engine;
    if (!(_engsMap.find(id) == _engsMap.end())) {
        engine = _engsMap.at(id); // out_of_range exception will throw if key does not exist
    }
    else { // Add a new engine
        if (!MLibs::instance().mxEngine().isLoaded()) {
            MLibs::instance().mxEngine().loadLib();
        }
        engine = new MatlabEngineInterface(showMatlabConsole());
        _engsMap.insert({id,engine});
    }
    return engine;
}

void MatlabEngineManager::deleteEngineById(const int &id)
{
    if (!(_engsMap.find(id) == _engsMap.end())) {
        // Delete pointer
        delete _engsMap[id];
        // Remove key
        _engsMap.erase(id);
    }
}

void MatlabEngineManager::deleteAll()
{
    for (auto& eng : _engsMap) {
        delete eng.second;
    }
    _engsMap.clear();
}

bool MatlabEngineManager::exists(const int &id) {
    bool found = false;
    if (!(_engsMap.find(id) == _engsMap.end())) {
        found = true;
    }
    return found;
}

const QDir &MatlabEngineManager::establishMatlabLibrayDirectory() {
    QString matlabDirStr;
    // Validate settings
    bool isDirEmpty = true;
    _matlabDir = searchMatlabDir();
    if (_matlabDir == QDir("")) { // No active matlab
        isDirEmpty = true;
        matlabDirStr = "";
    }
    else { // Found valid path
        isDirEmpty = false;
        matlabDirStr = _matlabDir.canonicalPath();
    }
#ifdef _WIN32
    // Add to environment path
    QByteArray envPathOrg;
    if (!isDirEmpty) { // Valid string found. Let's check dlls.
        envPathOrg = qgetenv("PATH");
        QByteArray envPath = envPathOrg;
        if (envPathOrg.right(1).at(0)==';') {
            envPath.append(matlabDirStr);
        }
        else {
            envPath.append(";" + matlabDirStr);
        }
        qputenv("PATH",envPath);
        // Quick sanity test        
        if (MLibs::instance().mxEngine().loadLib()) {
            _isMatlabAvailable = true;
            MLibs::instance().mxEngine().unloadLib();
        }
        else {
            qputenv("PATH",envPathOrg);
            _isMatlabAvailable = false;
        }
    }
    else {
        _isMatlabAvailable = false;
    }
#else

#endif
    return _matlabDir;
}

bool MatlabEngineManager::showMatlabConsole() {
    bool status;
    if (_showMatlabConsole != ConsoleWindowStatus::Auto) {
#ifdef QT_DEBUG
        status = true;
#else
        status = false;
#endif
    }
    else {
        status = (_showMatlabConsole==ConsoleWindowStatus::Show) ? true : false;
    }
    return status;
}

QDir MatlabEngineManager::searchMatlabDir() {
    QDir matlabPath = QDir("");
    QSettings regClsid("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
    if(regClsid.childGroups().contains("Matlab.AutoServer", Qt::CaseInsensitive)) {
        QString mClsid;
        QSettings mDefaultClsid("HKEY_CLASSES_ROOT\\MATLAB.AutoServer", QSettings::NativeFormat);
        if (!mDefaultClsid.childGroups().isEmpty()) {
            mClsid = mDefaultClsid.value("CLSID/.").toString();
            QString defaultPath = regClsid.value("CLSID/" + mClsid + "/LocalServer32/.").toString().section("MATLAB.exe",0,0);
            // Possible output: "C:\\Program Files\\MATLAB\\R2017a\\bin\\win64";
            matlabPath.setPath(defaultPath);
        }
    }
    return matlabPath;
}

}
