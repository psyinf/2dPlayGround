#pragma once

#include <sndX/BufferCache.hpp>
#include <sndX/Loader.hpp>
#include <sndX/Buffer.hpp>
#include <memory>

namespace pg::foundation {
template <>
inline std::shared_ptr<soundEngineX::Buffer> loadResource<std::shared_ptr<soundEngineX::Buffer>>(
    DataProvider& dataProvider,
    float&        percentResourcesLoaded)
{
    auto file_type = soundEngineX::loader::getType(dataProvider.getUri().type.empty() ? dataProvider.getUri().uri
                                                                                      : dataProvider.getUri().type);

    return soundEngineX::BufferCache::get(
        dataProvider.getUri(),
        dataProvider.asBuffer(),
        file_type,
        {.resource = dataProvider.getUri().uri,
         .cb = [&](soundEngineX::loader::LoadProgressInfo progress) { percentResourcesLoaded = progress.percent(); }});
}
} // namespace pg::foundation