from conan import ConanFile
from conan.tools.cmake import CMake

class conansetup(ConanFile):
    name = "playground2d"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    
    def requirements(self):
        self.requires("catch2/3.5.0")
        self.requires("sdl/2.28.3")
        self.requires("sdl_image/2.6.3")
        self.requires("sdl_ttf/2.20.2")
        self.requires("fmt/10.1.1")
        self.requires("entt/3.12.2")
        self.requires("libxml2/2.11.4", override=True)
        self.requires("openssl/3.2.0", override=True)
        self.requires("zlib/1.2.13")
               
    generators = ["CMakeDeps", "CMakeToolchain"]
    
#    def configure(self):
#        self.options['vsg'].shared = False
#        #this is currently waiting for the PR to be accepted
#        #self.options['vsg'].shader_compiler = True
        
#    def imports(self):    
#        self.copy("*.dll", "bin", "bin") 