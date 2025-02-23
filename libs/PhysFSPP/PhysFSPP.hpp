#pragma once

#include <PhysFSPP/PhysFSPP_Exceptions.hpp>
#include <PhysFSPP/PhysFSPP_File.hpp>
#include <physfs.h>
#include <string>
#include <vector>

namespace physfspp {

class VirtualFileSystem
{
public:
    enum class AppendMode
    {
        Append,
        Prepend,
    };
    enum class FileOpenMode
    {
        Read,
        Write,
        Append,
    };

    VirtualFileSystem(const std::string& arg0) { PHYSFS_init(arg0.c_str()); }

    ~VirtualFileSystem() { PHYSFS_deinit(); }

    void mount(const std::string& path, const std::string& mountPoint, AppendMode appendMode)
    {
        handleError(PHYSFS_mount(path.c_str(), mountPoint.c_str(), appendMode == AppendMode::Append ? 1 : 0));
    }

    void setWriteDir(const std::string& path) { handleError(PHYSFS_setWriteDir(path.c_str())); }

    FileHandle open(const std::string& path, FileOpenMode fileOpenMode = FileOpenMode::Read)
    {
        PHYSFS_File* f = nullptr;
        switch (fileOpenMode)
        {
        case FileOpenMode::Read:
            f = PHYSFS_openRead(path.c_str());
            break;
        case FileOpenMode::Write:
            f = PHYSFS_openWrite(path.c_str());
            break;
        case FileOpenMode::Append:
            f = PHYSFS_openAppend(path.c_str());
            break;
        default:
            throw VirtualFileSystemException("Invalid file open mode");
        }

        if (f == nullptr) { throw VirtualFileSystemException(PHYSFS_getLastError()); }
        return FileHandle{f};
    }

private:
    void handleError(int res)
    {
        if (res == 0) { throw VirtualFileSystemException(PHYSFS_getLastError()); }
    }
};
} // namespace physfspp
