from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class UshiroConan(ConanFile):
    name = "ushiro"
    version = "0.3"
    license = "MIT"
    author = "Marius Elvert marius.elvert@googlemail.com"
    url = "https://github.com/ltjax/ushiro"
    description = "Experimental but battle-proven library for unidirectional UI in C++."
    topics = ("unidirectional-ui",)
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "CMakeDeps", "CMakeToolchain"
    exports_sources = "source/*", "tests/*", "CMakeLists.txt"
    test_requires = "catch2/2.13.10",

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        CMake(self).install()

    def package_info(self):
        self.cpp_info.libs = ["ushiro"]

    def layout(self):
        cmake_layout(self)
