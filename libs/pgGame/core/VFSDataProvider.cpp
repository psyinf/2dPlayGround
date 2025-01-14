#include "VFSDataProvider.hpp"

#include <pgFoundation/MemStream.hpp>

pg::VFSDataProvider::~VFSDataProvider() {}

void pg::VFSDataProvider::open() {}

void pg::VFSDataProvider::close() {}

std::istream& pg::VFSDataProvider::asStream()
{
    auto file = _vfs->OpenFile({getUri()}, {vfspp::IFile::FileMode::Read});
    if (!file) { throw std::runtime_error("File not found: " + getUri().uri); }
    auto buffer = std::vector<uint8_t>(file->Size());
    file->Read(buffer, buffer.size());

    _stream = std::make_shared<pg::foundation::OwningMemStream>(std::move(buffer));
    return *_stream;
}

std::vector<char> pg::VFSDataProvider::asBuffer()
{
    auto file = _vfs->OpenFile({getUri()}, {vfspp::IFile::FileMode::Read});
    if (!file) { throw std::runtime_error("Virtual Filesystem: File not found: " + getUri().uri); }
    auto buffer = std::vector<uint8_t>(file->Size());
    file->Read(buffer, buffer.size());
    return std::vector<char>(buffer.begin(), buffer.end());
}

void pg::VFSDataProvider::toBuffer(std::vector<char>& buffer, size_t max_ch)
{
    auto file = _vfs->OpenFile({getUri()}, {vfspp::IFile::FileMode::Read});
    if (max_ch == 0) { max_ch = file->Size(); }
    auto buffer_ = std::vector<uint8_t>(std::min(file->Size(), max_ch));

    file->Read(buffer_, buffer_.size());
    buffer.resize(buffer_.size());
    std::copy(buffer_.begin(), buffer_.end(), buffer.begin());
}

char pg::VFSDataProvider::readNext()
{
    return 0;
}

std::span<char> pg::VFSDataProvider::asSpan()
{
    thread_local auto buffer = std::vector<uint8_t>();
    buffer.resize(size());

    _file->Read(buffer, buffer.size());
    return std::span<char>(reinterpret_cast<char*>(buffer.data()), buffer.size());
}

size_t pg::VFSDataProvider::size()
{
    return _file->Size();
}
