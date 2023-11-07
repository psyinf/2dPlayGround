#pragma once
#include "sdlpp.hpp"
#include "Transform.h"

namespace pg {
struct PrimitiveInterface
{
    void draw(sdl::Renderer& r, const Transform& t) = 0;
};

/**
 * Wrap any type that has a draw method that takes a renderer and a transform into a PrimitiveInterface
**/

template <typename T>
class PrimitiveWrapper : public PrimitiveInterface
{
    T primitive;

public:
    template <typename... Arguments>
    PrimitiveWrapper(Arguments&&... args)
      : primitive(std::forward<Arguments>(args)...)
    {
    }

    void draw(sdl::Renderer& r, const Transform& t) { primitive.draw(r,t); }
};
}


/*
// Type your code here, or load an example.
#include <cstdio>
#include <string>
#include <utility>
struct Dog{
    void says() const {std::puts("Wuff");}
};
struct Cow{
    std::string t;
    std::string u;
    Cow(std::string t, std::string u)
        :t(t)
        ,u(u)
    {

    }
    void says() const {std::puts(t.c_str()); std::puts(u.c_str());}
};

struct SayInterface {
    virtual void says () const = 0;
};


template <typename T>
class AnimalWrapper : public SayInterface
{
    T m_animal;

public:
    template <typename ... Arguments >
    AnimalWrapper(Arguments&&... args)
     :m_animal(std::forward<Arguments>(args)...)
    {}

     void says() const { m_animal.says(); }
};
int main(){
    Dog d;
    AnimalWrapper<Dog> a;
    AnimalWrapper<Cow> c(std::string("ddd"), "dd");

    a.says();
    c.says();
    return 0;
}
*/