
#include <PhysFSPP/PhysFSPP.hpp>
#include <PhysFSPP/PhysFSPP_Stream.hpp>

#include <iostream>
#include <vector>
#include <fstream>
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    physfspp::VirtualFileSystem vfs("sandbox");

    vfs.mount(R"(..\data\music\cylinders\Chris Zabriskie - Cylinders.zip)",
              "/music",
              physfspp::VirtualFileSystem::AppendMode::Append);
    vfs.mount(R"(..\data\music\cylinders)", "/test", physfspp::VirtualFileSystem::AppendMode::Append);
    vfs.setWriteDir("../data/");

    auto f = vfs.open("/music/Chris Zabriskie - Cylinders - 08 Cylinder Eight.mp3",
                      physfspp::VirtualFileSystem::FileOpenMode::Read);

    auto ifs = physfspp::IFStream(f);
    auto out = vfs.open("/test.txt", physfspp::VirtualFileSystem::FileOpenMode::Write);

    std::vector<uint8_t> buffer(f.length());
    ifs.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    // std::ofstream of("../data/sandbox.mp3", std::ios::binary);
    // of.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

    auto ofs = physfspp::OFStream(out);
    ofs.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    //      auto rootFS = std::make_unique<pg::::ZipFileSystem>(R"(..\data\music\cylinders\Chris Zabriskie -
    //      Cylinders.zip)"); rootFS->Initialize();
    //
    //      auto vfs = std::make_unique<vfspp::VirtualFileSystem>();
    //      vfs->AddFileSystem("/music", std::move(rootFS));
    //
    //      auto f =
    //          vfs->OpenFile({"/music/Chris Zabriskie - Cylinders - 01 Cylinder One.mp3"},
    //          {vfspp::IFile::FileMode::Read});
    //      // soundEngineX::loader::load()
    //      auto buffer = std::vector<uint8_t>(f->Size());
    //      f->Read(buffer, buffer.size());
}

catch (const std::exception& e)
{
    std::cerr << "Terminated: " << e.what() << std::endl;
    return -1;
}
