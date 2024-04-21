import os
import pathlib

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as build_ext_orig
from wheel.bdist_wheel import bdist_wheel


class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])


class bdist_wheel_abi3(bdist_wheel):
    def get_tag(self):
        python, abi, plat = super().get_tag()

        if python.startswith("cp"):
            # on CPython, our wheels are abi3 and compatible back to 3.6
            return python, "abi3", plat

        return python, abi, plat


class build_ext(build_ext_orig):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)

    def build_cmake(self, ext):
        cwd = pathlib.Path().absolute()

        # these dirs will be created in build_py, so if you don't have
        # any python sources to bundle, the dirs will be missing
        build_temp = pathlib.Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        extdir = (
            pathlib.Path(self.get_ext_fullpath(ext.name)).parent
            / self.distribution.get_name()
        )
        extdir.mkdir(parents=True, exist_ok=True)

        # example of cmake args
        config = "Debug" if self.debug else "Release"
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$<1:{extdir.absolute()}>",
            "-DCMAKE_BUILD_TYPE=" + config,
        ]

        build_args = ["--target", "_C", "--config", config]

        os.chdir(str(build_temp))
        self.spawn(["cmake", str(cwd)] + cmake_args)
        if not self.dry_run:
            self.spawn(["cmake", "--build", "."] + build_args)
        os.chdir(str(cwd))


setup(
    name="mesh2nvdb",
    version="0.1",
    packages=["mesh2nvdb"],
    package_dir={"mesh2nvdb": "mesh2nvdb"},
    ext_modules=[CMakeExtension("mesh2nvdb")],
    cmdclass={"build_ext": build_ext, "bdist_wheel": bdist_wheel_abi3},
)
