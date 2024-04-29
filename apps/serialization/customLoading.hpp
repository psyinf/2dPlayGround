#include <sstream>
#include <string>

#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <entt/entt.hpp>

struct IdMetaAny
{
    entt::id_type  id;
    entt::meta_any any;

    template <class Ar>
    void save(Ar& ar) const
    {
        ar(id);
        any.invoke(entt::hashed_string("save"), entt::forward_as_meta(ar));
    }

    template <class Ar>
    void load(Ar& ar)
    {
        ar(id);
        any = entt::resolve(id).construct();
        any.invoke(entt::hashed_string("load"), entt::forward_as_meta(ar));
    }
};

template <typename Component>
void move_emplace(Component& elem, entt::sparse_set& storage, entt::entity entity)
{
    static_cast<entt::storage_for_t<Component>&>(storage).emplace(entity, std::move(elem));
}

template <class Component>
void RegisterComponentForSerialize()
{
    using namespace entt::literals;
    auto&& f = entt::meta<Component>().template func<&move_emplace<Component>>("emplace"_hs);
    f = f.type(entt::type_id<Component>().hash());
    f = f.template func<&Component::template save<cereal::BinaryOutputArchive>>(entt::hashed_string("save"));
    f = f.template func<&Component::template load<cereal::BinaryInputArchive>>(entt::hashed_string("load"));
}

struct EntityWrapper
{
    entt::registry& registry;
    entt::entity    handle;
};

template <class Ar>
void save(Ar& ar, const EntityWrapper& wrapper)
{
    std::map<entt::id_type, IdMetaAny> components;

    for (auto&& [id, storage] : wrapper.registry.storage())
    {
        if (!storage.contains(wrapper.handle)) { continue; }

        if (auto type = entt::resolve(id); type)
        {
            components.emplace(id, IdMetaAny{id, type.from_void(storage.value(wrapper.handle))});
        }
    }
    ar(components);
}

template <class Ar>
void load(Ar& ar, EntityWrapper& wrapper)
{
    using namespace entt::literals;

    std::map<entt::id_type, IdMetaAny> components;
    ar(components);

    for (auto&& [id, storage] : wrapper.registry.storage())
    {
        if (auto itr = components.find(id); itr == components.end())
        {
            // undo for add component.
            storage.remove(wrapper.handle);
        }
        else if (storage.contains(wrapper.handle))
        {
            // undo for update component
            auto& any = itr->second.any;
            storage.remove(wrapper.handle);
            any.type().invoke("emplace"_hs, any, entt::forward_as_meta(storage), wrapper.handle);
        }
    }

    for (const auto& [id, id_any] : components)
    {
        if (auto storage = wrapper.registry.storage(id); !storage->contains(wrapper.handle))
        {
            // undo for remove component
            id_any.any.type().invoke("emplace"_hs, id_any.any, entt::forward_as_meta(storage), wrapper.handle);
        }
    }
}

struct Position
{
    double x, y, z;

    template <class Ar>
    void save(Ar& ar) const
    {
        ar(x, y, z);
    }

    template <class Ar>
    void load(Ar& ar)
    {
        ar(x, y, z);
    }
};

void no_entity_wrapper_case()
{
    entt::registry registry;
    auto           entity = registry.create();
    // initialize
    registry.emplace<Position>(entity, 1.0, 2.0, 3.0);

    // back up
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive ar(ss);
        ar(EntityWrapper{registry, entity});
    }
    // update
    {
        auto& comp = registry.get<Position>(entity);
        comp.x = 10;
        comp.y = 20;
        comp.z = 30;
    }
    // undo
    {
        cereal::BinaryInputArchive ar(ss);
        EntityWrapper              wrapper{registry, entity};
        ar(wrapper);
    }
    // check
    {
        auto& comp = registry.get<Position>(entity);
        assert((comp.x == 1.0) && (comp.y == 2.0) && (comp.z == 3.0));
    }
}

void testCustomLoading()
{
    RegisterComponentForSerialize<Position>();

    no_entity_wrapper_case();

    entt::registry registry;
    for (auto i = 0; i < 10; ++i)
    {
        auto entity = registry.create();
        registry.emplace<Position>(entity, i * 0.1f, i * 0.01f, i * 1.0f);
    }
    // save all
    // todo: for initial loading we need to store the list of entities as well
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive ar(ss);
        auto                        view = registry.view<entt::entity>();

        for (const auto entity : view)
        {
            ar(EntityWrapper{registry, entity});
        }
    }
}