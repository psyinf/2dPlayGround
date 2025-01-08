
#include <vfspp/VFS.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
   auto rootFS = std::make_unique<vfspp::ZipFileSystem>("Resources.zip");
}

catch (const std::exception& e)
{
    std::cerr << "Terminated: " << e.what() << std::endl;
    return -1;
}