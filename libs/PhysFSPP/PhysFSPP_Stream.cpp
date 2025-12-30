#include "PhysFSPP_Stream.hpp"
#include <streambuf>

physfspp::IFStream::IFStream(physfspp::FileHandle file)
  : StreamBase(file)
  , std::istream(&getBuf())
{
}

physfspp::IFStream::~IFStream()
{
    // flush();
}

physfspp::OFStream::OFStream(physfspp::FileHandle file)

  : StreamBase(file)
  , std::ostream(&getBuf())

{
}

physfspp::OFStream::~OFStream()
{
    flush();
}

physfspp::FStream::FStream(physfspp::FileHandle file)
  : StreamBase(file)
  , std::iostream(&getBuf())
{
}

physfspp::FStream::~FStream()
{
    flush();
}
