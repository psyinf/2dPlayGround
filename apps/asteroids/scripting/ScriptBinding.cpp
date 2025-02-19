#include <pybind11/embed.h>

namespace py = pybind11;

// embedded module: mymodule
PYBIND11_EMBEDDED_MODULE(mymodule, m)
{
    m.def("add", [](int a, int b) { return a + b; }, "A function that adds two numbers");
}