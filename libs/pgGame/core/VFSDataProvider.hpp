#pragma once
#include <pgf/caching/DataProvider.hpp>
#include <PhysFSPP/PhysFSPP.hpp>
#include <PhysFSPP/PhysFSPP_Stream.hpp>

namespace pg {
class VFSDataProvider : public foundation::DataProvider
{
    std::shared_ptr<physfspp::VirtualFileSystem> _vfs;
    std::shared_ptr<physfspp::IFStream>          _stream;

public:
    VFSDataProvider(const foundation::URI& uri, std::shared_ptr<physfspp::VirtualFileSystem> vfs)
      : DataProvider(uri)
      , _vfs(std::move(vfs))
    {
    }

    ~VFSDataProvider() override;
    void              open() override;
    void              close() override;
    std::istream&     asStream() override;
    std::vector<char> asBuffer() override;
    void              toBuffer(std::vector<char>& buffer, size_t max_ch) override;
    char              readNext() override;
    std::span<char>   asSpan() override;
    size_t            size() const override;
};
} // namespace pg
