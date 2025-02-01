#include "VFSDataProvider.hpp"

#include <pgFoundation/MemStream.hpp>

pg::VFSDataProvider::~VFSDataProvider() {}

void pg::VFSDataProvider::open() {}

void pg::VFSDataProvider::close() {}

std::istream& pg::VFSDataProvider::asStream()
{
    auto buffer = std::vector<uint8_t>(_file->Size());
    _file->Read(buffer, buffer.size());

    _stream = std::make_shared<pg::foundation::OwningMemStream>(std::move(buffer));
    return *_stream;
}

std::vector<char> pg::VFSDataProvider::asBuffer()
{
    auto buffer = std::vector<uint8_t>(_file->Size());
    _file->Read(buffer, buffer.size());
    return std::vector<char>(buffer.begin(), buffer.end());
}

void pg::VFSDataProvider::toBuffer(std::vector<char>& buffer, size_t max_ch)
{
    if (max_ch == 0) { max_ch = _file->Size(); }
    auto buffer_ = std::vector<uint8_t>(std::min(_file->Size(), max_ch));

    _file->Read(buffer_, buffer_.size());
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
