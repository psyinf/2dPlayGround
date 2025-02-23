#pragma once

#include <physfs.h>
#include <cstdint>

namespace physfspp {
class FileHandle
{
public:
    explicit FileHandle(PHYSFS_File* file)
      : m_file(file)
    {
    }

    // only allow move
    FileHandle(const FileHandle&) = default;
    FileHandle& operator=(const FileHandle&) = default;
    FileHandle(FileHandle&&) = delete;
    FileHandle& operator=(FileHandle&&) = delete;

    ~FileHandle()
    {
        // if (m_file != nullptr) { PHYSFS_close(m_file); }
    }

    // length of file
    int64_t length() const { return PHYSFS_fileLength(m_file); }

    // seek to a position
    void seek(size_t pos) { PHYSFS_seek(m_file, pos); }

    // tell the current position
    int64_t tell() const { return PHYSFS_tell(m_file); }

    // close
    void close()
    {
        if (m_file != nullptr) { PHYSFS_close(m_file); }
        m_file = nullptr;
    }

    bool eof() const { return PHYSFS_eof(m_file) != 0; }

    // read
    int64_t read(void* buffer, size_t objSize) const { return PHYSFS_readBytes(m_file, buffer, objSize); }

    // write
    int64_t write(const void* buffer, size_t objSize) const { return PHYSFS_writeBytes(m_file, buffer, objSize); }

private:
    PHYSFS_File* m_file{nullptr};
};
} // namespace physfspp
