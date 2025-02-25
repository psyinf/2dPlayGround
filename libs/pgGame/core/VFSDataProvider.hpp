#pragma once
#include <pgf/caching/DataProvider.hpp>
#include <PhysFSPP/PhysFSPP.hpp>
#include <PhysFSPP/PhysFSPP_Stream.hpp>

namespace pg {
class VFSDataProvider : public foundation::DataProvider
{
    std::shared_ptr<physfspp::VirtualFileSystem> _vfs;
    std::shared_ptr<physfspp::IFStream>          _stream;
    std::string                                  _localPrefix;

public:
    VFSDataProvider(const foundation::URI&                       uri,
                    std::shared_ptr<physfspp::VirtualFileSystem> vfs,
                    std::string                                  local_prefix = "")
      : DataProvider(uri)
      , _vfs(std::move(vfs))
      , _localPrefix(local_prefix)
    {
        //         if (!_file && !_localPrefix.empty())
        //         {
        //             _file = _vfs->OpenFile({_localPrefix, getUri(), false}, {vfspp::IFile::FileMode::Read});
        //         }
        //         if (!_file) { throw std::runtime_error("Cannot open VFS file: " + std::string{getUri()}); }
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
