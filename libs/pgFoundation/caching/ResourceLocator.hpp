#pragma once

namespace pg::foundation {
class ResourceLocatorBase
{
public:
    std::filesystem::path locate(this auto&& self, const std::string& uri) { return self.loc_impl(uri); }

    void addLocation(this auto&& self, const std::string& uri, const std::filesystem::path& path)
    {
        self.add_impl(uri, path);
    }

    void hasLocation(this auto&& self, const std::string& uri) { return self.has_impl(uri); }

private:
    std::filesystem::path loc_impl(const std::string& uri) { return _locations.at(uri); }

    void add_impl(const std::string& uri, const std::filesystem::path& path) { _locations[uri] = path; }

    void has_impl(const std::string& uri) { return _locations.find(uri) != _locations.end(); }

    std::unordered_map<std::string, std::filesystem::path> _locations;
};

} // namespace pg::foundation
