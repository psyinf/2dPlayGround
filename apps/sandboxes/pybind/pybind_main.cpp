#include <pybind11/pybind11.h>
#include <pybind11/embed.h> // for embedding Python interpreter
#include <iostream>

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

    // Define some Python code to execute
    const char* code = R"(import mymodule
result = mymodule.add(3, 4,7)
print("Result from embedded module:", result)
)";
    try
    {
        py::module mymodule = py::module::import("mymodule");
        // Execute the Python code using py::exec
        // py::eval_file("script.py");
        py::exec(code);
    }
    catch (const py::error_already_set& e)
    {
        // py::module::import("traceback").attr("print_exception")(e.type(), e.value(), e.trace());
        std::cout << e.what() << "\n";
    }
    // Execute the Python code using py::exec

    return 0;
}