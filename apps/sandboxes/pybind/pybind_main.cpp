
#include <pgEngine/scripting/PythonScripter.hpp>
#include <iostream>

/*

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

std::unique_ptr<py::scoped_interpreter> interpreter;

int __main()
{
    // Initialize Python interpreter

    // Create the module dynamically (this is an embedded module)

    // Define some Python code to execute
    const char* code = R"(import mymodule
for i in range(1000):
    result = mymodule.add(3, 4)
#print("Result from embedded module:", result)
)";
    {
        // accumulate time
        auto                    now = std::chrono::high_resolution_clock::now();
        auto                    interpreter = std::make_unique<py::scoped_interpreter>();
        std::vector<py::module> modules;
        modules.emplace_back(py::module::import("mymodule"));
        int i = 0;
        try
        {
            while (++i < 10000)
            {
                // py::scoped_interpreter guard{};
                // benchmarks

                //

                // Execute the Python code using py::exec
                py::exec(code);
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Elapsed time: "
                      << std::chrono::duration_cast<std::chrono::microseconds>(end - now).count() / 10000
                      << "us per run \n";
        }
        catch (const py::error_already_set& e)
        {
            // py::module::import("traceback").attr("print_exception")(e.type(), e.value(), e.trace());
            std::cout << e.what() << "\n";
        }
    }

    // Execute the Python code using py::exec

    return 0;
}
*/

int main()
{
    auto script = pg::scripting::PythonScripter::scriptFromString(R"(print("Hello, World!"))");

    pg::scripting::PythonScripter scripter(std::move(script));
    while (true)
    {
        try
        {
            scripter.run();
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}