#pragma once
#include <PhysFSPP/PhysFSPP_File.hpp>
#include <PhysFSPP/PhysFSPP_StreamBuf.hpp>

#include <iostream>
#include <memory>

namespace physfspp {

namespace {
class StreamBase
{
    using Buffer = StreamBuf<2048>;

public:
    StreamBase(FileHandle file)
      : _file(file)
      , _rdBuf(std::make_unique<Buffer>(_file))
    {
    }

    virtual ~StreamBase() { _file.close(); };

    size_t length() { return _file.length(); }

    Buffer& getBuf() { return *_rdBuf; }

private:
    FileHandle              _file;
    std::unique_ptr<Buffer> _rdBuf;
};
} // namespace

class IFStream : public StreamBase, public std::istream
{
public:
    IFStream(FileHandle);
    ~IFStream() override;
};

class OFStream : public StreamBase, public std::ostream
{
public:
    OFStream(FileHandle);
    ~OFStream() override;
};

class FStream : public StreamBase, public std::iostream
{
public:
    FStream(FileHandle);
    ~FStream() override;
};
} // namespace physfspp