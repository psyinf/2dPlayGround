#pragma once

namespace pg::foundation {

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
      , _buffer(std::move(buffer)){};

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

} // namespace pg::foundation