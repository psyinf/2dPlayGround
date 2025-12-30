#pragma once

#include <array>
#include <streambuf>

namespace physfspp {
template <size_t BufferSize = 2048>
class StreamBuf : public std::streambuf
{
public:
    StreamBuf(FileHandle file)
      : _file(file)
    {
        auto end = buffer.data() + BufferSize;
        setg(end, end, end);
        setp(buffer.data(), end);
    }

    ~StreamBuf() override { sync(); }

    StreamBuf(const StreamBuf& other) = delete;
    StreamBuf& operator=(const StreamBuf& other) = delete;

    StreamBuf(StreamBuf&&) = delete;
    StreamBuf& operator=(StreamBuf&&) = delete;

private:
    int_type underflow() override
    {
        if (_file.eof()) { return traits_type::eof(); }

        const auto bytesRead = _file.read(buffer.data(), BufferSize);

        if (bytesRead < 1) { return traits_type::eof(); }
        setg(buffer.data(), buffer.data(), buffer.data() + bytesRead);
        return (unsigned char)*gptr();
    }

    pos_type seekoff(off_type pos, std::ios_base::seekdir dir, std::ios_base::openmode mode) override
    {
        switch (dir)
        {
        case std::ios_base::beg:
            _file.seek(pos);
            break;
        case std::ios_base::cur:
            // subtract characters currently in buffer from seek position
            _file.seek((_file.tell() + pos) - (egptr() - gptr()));
            break;
        case std::ios_base::end:
            _file.seek(_file.length() + pos);
            break;
        default:
            break;
        }

        if (mode & std::ios_base::in) { setg(egptr(), egptr(), egptr()); }

        if (mode & std::ios_base::out) { setp(buffer.data(), buffer.data()); }
        return _file.tell();
    }

    pos_type seekpos(pos_type pos, std::ios_base::openmode mode) override
    {
        _file.seek(pos);
        if (mode & std::ios_base::in) { setg(egptr(), egptr(), egptr()); }

        if (mode & std::ios_base::out) { setp(buffer.data(), buffer.data()); }

        return _file.tell();
    }

    int_type overflow(int_type c = traits_type::eof()) override
    {
        // Write buffer
        if (flush() < 0) { return traits_type::eof(); }

        setp(buffer.data(), buffer.data() + BufferSize); // reset the put pointers

        if (c != traits_type::eof())
        {
            *pptr() = (char)c; // Add the overflow character to the put buffer
            pbump(1);          // increment the put pointer by one
        }

        return c;
    }

    int sync() override
    {
        const auto result = flush();
        setp(buffer.data(), buffer.data() + BufferSize); // reset the put pointers

        return result;
    }

    int flush() const
    {
        const auto len = pptr() - pbase();
        if (len == 0)
        {
            return 0; // nothing to do
        }

        if (_file.write(pbase(), len) < len) { return -1; }

        return 0;
    }

    std::array<char, BufferSize> buffer;
    FileHandle                   _file;
};
} // namespace physfspp