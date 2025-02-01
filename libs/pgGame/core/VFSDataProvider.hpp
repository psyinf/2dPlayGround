#pragma once
#include <vfspp/VFS.h>
#include <pgf/caching/DataProvider.hpp>

namespace pg {
class VFSDataProvider : public foundation::DataProvider
{
    std::shared_ptr<vfspp::VirtualFileSystem> _vfs;
    std::shared_ptr<std::istream>             _stream;
    vfspp::IFilePtr                           _file;
    std::string                               _localPrefix;

public:
    VFSDataProvider(const foundation::URI&                    uri,
                    std::shared_ptr<vfspp::VirtualFileSystem> vfs,
                    std::string                               local_prefix = "")
      : DataProvider(uri)
      , _vfs(std::move(vfs))
      , _file(_vfs->OpenFile({getUri()}, {vfspp::IFile::FileMode::Read}))
      , _localPrefix(local_prefix)
    {
        if (!_file && !_localPrefix.empty())
        {
            _file = _vfs->OpenFile({_localPrefix, getUri(), false}, {vfspp::IFile::FileMode::Read});
        }
        if (!_file) { throw std::runtime_error("Cannot open VFS file: " + std::string{getUri()}); }
    }

    ~VFSDataProvider() override;
    void              open() override;
    void              close() override;
    std::istream&     asStream() override;
    std::vector<char> asBuffer() override;
    void              toBuffer(std::vector<char>& buffer, size_t max_ch) override;
    char              readNext() override;
    std::span<char>   asSpan() override;
    size_t            size() override;
};
} // namespace pg