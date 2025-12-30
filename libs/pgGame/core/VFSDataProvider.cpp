#include "VFSDataProvider.hpp"

#include <pgFoundation/MemStream.hpp>
#include <PhysFSPP/PhysFSPP_Stream.hpp>

pg::VFSDataProvider::~VFSDataProvider() {}

void pg::VFSDataProvider::open() {}

void pg::VFSDataProvider::close() {}

std::istream& pg::VFSDataProvider::asStream()
{
    auto file = _vfs->open(getUri());
    _stream = std::make_unique<physfspp::IFStream>(file);
    return *_stream;
}

std::vector<char> pg::VFSDataProvider::asBuffer()
{
    auto file = _vfs->open(getUri());
    auto buffer = std::vector<char>(file.length());
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    return buffer;
}

void pg::VFSDataProvider::toBuffer(std::vector<char>& buffer, size_t max_ch)
{
    auto file = _vfs->open(getUri());
    buffer.resize(std::min(max_ch, static_cast<size_t>(file.length())));
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
}

char pg::VFSDataProvider::readNext()
{
    throw std::runtime_error("Not implemented");
}

std::span<char> pg::VFSDataProvider::asSpan()
{
    //     thread_local auto buffer = std::vector<uint8_t>();
    //     buffer.resize(size());
    //
    //     _file->Read(buffer, buffer.size());
    //     return std::span<char>(reinterpret_cast<char*>(buffer.data()), buffer.size());
    throw std::runtime_error("Not implemented");
}

size_t pg::VFSDataProvider::size() const
{
    // return _file->Size();
    throw std::runtime_error("Not implemented");
}
