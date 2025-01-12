#pragma once
#include <vfspp/VFS.h>
#include <pgf/caching/DataProvider.hpp>

namespace pg {
class VFSDataProvider : public foundation::DataProvider
{
    std::shared_ptr<vfspp::VirtualFileSystem> _vfs;
    std::shared_ptr<std::istream>             _stream;

public:
    VFSDataProvider(const foundation::URI& uri, std::shared_ptr<vfspp::VirtualFileSystem> vfs)
      : DataProvider(uri)
      , _vfs(std::move(vfs))
    {
    }

    ~VFSDataProvider() override;
    void          open() override;
    void          close() override;
    std::istream& asStream() override;
    void          asBuffer(std::vector<char>& buffer, size_t max_ch) override;
    char          readNext() override;
};
} // namespace pg