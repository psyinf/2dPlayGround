#include <fstream>
#include <string>

#include <cereal/archives/json.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/core/type_traits.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>

struct Position
{
    int x;
    int y;
};

template <typename Archive>
void serialize(Archive& archive, Position& position)
{
    archive(position.x, position.y);
}

void saveRegistry(entt::registry& reg, std::string_view file_name)
{
    std::ofstream storage("../data/test.out");

    cereal::JSONOutputArchive output{storage};

    entt::snapshot{reg}.get<entt::entity>(output).get<Position>(output);
}

void loadRegistry(entt::registry& reg, std::string_view file_name)
{
    std::ifstream storage("../data/test.out");

    cereal::JSONInputArchive output{storage};

    entt::basic_snapshot_loader{reg}.get<entt::entity>(output).get<Position>(output);
}

int main()
{
    entt::registry registry;
    auto           entity = registry.create();
    registry.emplace<Position>(entity, Position{42, 42});
    // registry.add<Position>(registry.create(), {3, 14});
    saveRegistry(registry, "data/test.out");
    {
        auto& pos = registry.get<Position>(entity);
        pos.x = 0;
    }
    registry.clear();
    loadRegistry(registry, "data/test.out");
    auto& pos2 = registry.get<Position>(entity);
}