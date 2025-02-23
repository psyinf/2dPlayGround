#pragma once

namespace pg {

/**
 * Lifetime scope wrapper for objects with explicit make and destroy functions
 * /param Construct
 * /param Destroy
 */
template <auto Construct, auto Destroy>
class Lifetime
{
public:
    Lifetime() { Construct(); }

    ~Lifetime() { Destroy(); }

    Lifetime(const Lifetime&) = delete;
    Lifetime(Lifetime&&) = delete;
    Lifetime& operator=(const Lifetime&) = delete;
    Lifetime  operator=(Lifetime&&) = delete;
};
} // namespace pg
