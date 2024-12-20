#pragma once

#include <sndX/BufferCache.hpp>
#include <sndX/Loader.hpp>
#include <sndX/Buffer.hpp>
#include <memory>

namespace pg::foundation {
template <>
inline std::shared_ptr<soundEngineX::Buffer> loadResource<std::shared_ptr<soundEngineX::Buffer>>(
    const std::string& file,
    float&             percentResourcesLoaded)
{
    return soundEngineX::BufferCache::get(file, {file, [&](soundEngineX::loader::LoadProgressInfo progress) {
                                                     percentResourcesLoaded = progress.percent();
                                                 }});
}
} // namespace pg::foundation