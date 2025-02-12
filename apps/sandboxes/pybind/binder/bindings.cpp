#include <pybind11/embed.h>

namespace py = pybind11;

int add_two_integers(int a, int b)
{
    return a + b;
}

PYBIND11_EMBEDDED_MODULE(libmath, m)
{
    m.def("add_two_integers", &add_two_integers, "Add two integers");
}