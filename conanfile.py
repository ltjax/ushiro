from conans import ConanFile, CMake, tools


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
    default_options = "shared=False"
    generators = "cmake"
    exports_sources = "source/*", "tests/*", "CMakeLists.txt"
    requires = "Catch2/2.7.2@catchorg/stable",

    def _configured_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        return cmake

    def build(self):
        self._configured_cmake().build()

    def package(self):
        self._configured_cmake().install()

    def package_info(self):
        self.cpp_info.libs = ["ushiro"]

