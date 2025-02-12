#include <pybind11/pybind11.h>
#include <pybind11/embed.h> // for embedding Python interpreter

namespace py = pybind11;

// Simple function in the module
int add(int a, int b)
{
    return a + b;
}

// Create the module
PYBIND11_EMBEDDED_MODULE(mymodule, m)
{
    m.def("add", &add, "A function that adds two numbers");
}

int main()
{
    // Initialize Python interpreter
    py::scoped_interpreter guard{};

    // Create the module dynamically (this is an embedded module)
    py::module mymodule = py::module::import("mymodule");

    // Define some Python code to execute
    const char* code = R"(
        import mymodule
        result = mymodule.add(3, 4)
        print("Result from embedded module:", result)
        )";

    // Execute the Python code using py::exec
    py::exec(code);

    return 0;
}