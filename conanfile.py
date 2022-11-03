from conans import ConanFile, CMake

class chceckcert(ConanFile):
    settings = "os", "compiler", "build_type", "arch", "cppstd"
    license = "MIT"
    author = "Michal Widera"
    description = "CheckCert openssl conan example"
    homepage = "https://github.com/michalwidera/Antlr4ConanExample"
    generators = "cmake" , "cmake_find_package"
    testing = []

    options = {
        "openssl" : ["3.0.7","1.1.1s"]
    }

    default_options = {
        "openssl": "1.1.1s"
    }

    def configure(self):
        self.settings.compiler.cppstd = 20
        self.settings.compiler.libcxx = "libstdc++11"

    def package_info(self):
        self.cpp_info.system_libs = ["dl", "rt", "pthread"]

    def requirements(self):
        self.requires("openssl/"+str(self.options.openssl))

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.install()
