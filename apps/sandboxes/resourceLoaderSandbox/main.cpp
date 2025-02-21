
#include <vfspp/VFS.h>

// for testing

class membuf : public std::basic_streambuf<char>
{
public:
    membuf(const uint8_t* p, size_t l) { setg((char*)p, (char*)p, (char*)p + l); }
};

class memstream : public std::istream
{
public:
    memstream(const uint8_t* p, size_t l)
      : std::istream(&_buffer)
      , _buffer(p, l)
    {
        rdbuf(&_buffer);
    }

private:
    membuf _buffer;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    //     auto rootFS = std::make_unique<pg::::ZipFileSystem>(R"(..\data\music\cylinders\Chris Zabriskie -
    //     Cylinders.zip)"); rootFS->Initialize();
    //
    //     auto vfs = std::make_unique<vfspp::VirtualFileSystem>();
    //     vfs->AddFileSystem("/music", std::move(rootFS));
    //
    //     auto f =
    //         vfs->OpenFile({"/music/Chris Zabriskie - Cylinders - 01 Cylinder One.mp3"},
    //         {vfspp::IFile::FileMode::Read});
    //     // soundEngineX::loader::load()
    //     auto buffer = std::vector<uint8_t>(f->Size());
    //     f->Read(buffer, buffer.size());
}

catch (const std::exception& e)
{
    std::cerr << "Terminated: " << e.what() << std::endl;
    return -1;
}