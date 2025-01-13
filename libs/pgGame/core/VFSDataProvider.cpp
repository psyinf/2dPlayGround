#include "VFSDataProvider.hpp"

class MemBuf : public std::basic_streambuf<char>
{
public:
    MemBuf(uint8_t* p, size_t l)
      : begin(reinterpret_cast<char*>(p))
      , end(reinterpret_cast<char*>(p + l))

    {
        setg(begin, begin, end);
    }

    virtual pos_type seekoff(off_type                                 off,
                             std::ios_base::seekdir                   dir,
                             [[maybe_unused]] std::ios_base::openmode which = std::ios_base::in) override
    {
        if (dir == std::ios_base::cur)
            gbump(static_cast<int>(off));
        else if (dir == std::ios_base::end)
            setg(begin, end + off, end);
        else if (dir == std::ios_base::beg)
            setg(begin, begin + off, end);

        return gptr() - eback();
    }

    virtual pos_type seekpos(std::streampos pos, std::ios_base::openmode mode) override
    {
        return seekoff(pos - pos_type(off_type(0)), std::ios_base::beg, mode);
    }

    char *begin, *end;
};

class OwningMemBuf : public MemBuf
{
public:
    OwningMemBuf(std::vector<uint8_t>&& buffer)
      : MemBuf(buffer.data(), buffer.size())
      , _buffer(std::move(buffer)) {};

private:
    std::vector<uint8_t> _buffer;
};

class MemStream : public std::istream
{
public:
    MemStream(uint8_t* p, size_t l)
      : std::istream(&_buffer)
      , _buffer(p, l)
    {
        rdbuf(&_buffer);
    }

private:
    MemBuf _buffer;
};

class OwningMemStream : public std::istream
{
public:
    OwningMemStream(std::vector<uint8_t>&& buffer)
      : std::istream(&_buffer)
      , _buffer(std::move(buffer))
    {
        rdbuf(&_buffer);
    }

private:
    OwningMemBuf _buffer;
};

pg::VFSDataProvider::~VFSDataProvider() {}

void pg::VFSDataProvider::open() {}

void pg::VFSDataProvider::close() {}

std::istream& pg::VFSDataProvider::asStream()
{
    auto file = _vfs->OpenFile({getUri()}, {vfspp::IFile::FileMode::Read});
    if (!file) { throw std::runtime_error("File not found: " + getUri().uri); }
    auto buffer = std::vector<uint8_t>(file->Size());
    file->Read(buffer, buffer.size());

    _stream = std::make_shared<OwningMemStream>(std::move(buffer));
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
