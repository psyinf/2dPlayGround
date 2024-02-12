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

struct Entity
{
    Entity(entt::registry* in_registry)
      : registry{in_registry}
    {
        if (this->registry) { this->handle = this->registry->create(); }
    }

    template <typename Comp, typename... Args>
    Comp& add(Args&&... args)
    {
        return this->registry->emplace<Comp>(this->handle, args...);
    }

    template <typename Comp>
    Comp* get()
    {
        return this->registry->try_get<Comp>(this->handle);
    }

    template <typename Comp>
    void remove()
    {
        this->registry->remove<Comp>(this->handle);
    }

    template <class Ar>
    void save(Ar& ar) const
    {
        std::map<entt::id_type, IdMetaAny> components;

        for (auto&& [id, storage] : this->registry->storage())
        {
            if (!storage.contains(this->handle)) { continue; }

            if (auto type = entt::resolve(id); type)
            {
                components.emplace(id, IdMetaAny{id, type.from_void(storage.value(this->handle))});
            }
        }
        ar(components);
    }

    template <class Ar>
    void load(Ar& ar)
    {
        using namespace entt::literals;

        std::map<entt::id_type, IdMetaAny> components;
        ar(components);

        for (auto&& [id, storage] : this->registry->storage())
        {
            if (auto itr = components.find(id); itr == components.end())
            {
                // undo for add component.
                storage.remove(this->handle);
            }
            else if (storage.contains(this->handle))
            {
                // undo for update component
                auto& any = itr->second.any;
                storage.remove(this->handle);
                any.type().invoke("emplace"_hs, any, entt::forward_as_meta(storage), this->handle);
            }
        }

        for (const auto& [id, id_any] : components)
        {
            if (auto storage = this->registry->storage(id); !storage->contains(this->handle))
            {
                // undo for remove component
                id_any.any.type().invoke("emplace"_hs, id_any.any, entt::forward_as_meta(storage), this->handle);
            }
        }
    }

private:
    entt::registry* registry = nullptr;
    entt::entity    handle{entt::null};
};

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

bool trivially_copyable_case()
{
    entt::registry registry;
    Entity         e{&registry};

    // initialize
    e.add<Position>(1.0, 2.0, 3.0);

    // back up
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive ar(ss);
        ar(e);
    }
    // update
    {
        auto comp = e.get<Position>();
        comp->x = 10;
        comp->y = 20;
        comp->z = 30;
    }
    // undo
    {
        cereal::BinaryInputArchive ar(ss);
        ar(e);
    }
    // check
    {
        auto comp = e.get<Position>();
        return (comp != nullptr) && (comp->x == 1.0) && (comp->y == 2.0) && (comp->z == 3.0);
    }
}

struct Data
{
    std::unique_ptr<double> value;

    template <class Ar>
    void save(Ar& ar) const
    {
        ar(value);
    }

    template <class Ar>
    void load(Ar& ar)
    {
        ar(value);
    }
};

bool unique_ptr_case()
{
    entt::registry registry;
    Entity         e{&registry};

    // initialize
    auto& comp = e.add<Data>();
    comp.value = std::make_unique<double>(10.0);

    // back up
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive ar(ss);
        ar(e);
    }
    // update
    {
        auto comp = e.get<Data>();
        comp->value = nullptr;
    }
    // undo
    {
        cereal::BinaryInputArchive ar(ss);
        ar(e);
    }
    // check
    {
        auto comp = e.get<Data>();
        return (comp != nullptr) && (comp->value != nullptr) && (*(comp->value) == 10.0);
    }
}

int main()
{
    RegisterComponentForSerialize<Position>();
    RegisterComponentForSerialize<Data>();

    assert(trivially_copyable_case() && L"fail to trivially_copyable_case");
    assert(unique_ptr_case() && L"fail to unique_ptr_case");
}