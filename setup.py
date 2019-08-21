import os
import sys
import subprocess
try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup
from numpy.distutils.core import Extension
import argparse
import numpy as np

## Numpy header files
numpy_lib = os.path.split(np.__file__)[0]
numpy_include = os.path.join(numpy_lib, 'core/include')
#
# Make the git revision visible.  Most of this is copied from scipy
#
# Return the git revision as a string
def git_version():
    def _minimal_ext_cmd(cmd):
        # construct minimal environment
        env = {}
        for k in ['SYSTEMROOT', 'PATH']:
            v = os.environ.get(k)
            if v is not None:
                env[k] = v
        # LANGUAGE is used on win32
        env['LANGUAGE'] = 'C'
        env['LANG'] = 'C'
        env['LC_ALL'] = 'C'
        out = subprocess.Popen(cmd, stdout=subprocess.PIPE, env=env).communicate()[0]
        return out

    try:
        out = _minimal_ext_cmd(['git', 'rev-parse', 'HEAD'])
        GIT_REVISION = out.strip().decode('ascii')
    except OSError:
        GIT_REVISION = "Unknown"

    return GIT_REVISION


def write_version_py(filename='version.py'):
    cnt = """
# THIS FILE IS GENERATED FROM SCIPY SETUP.PY
git_revision = '%(git_revision)s'
"""
    GIT_REVISION = git_version()

    a = open(filename, 'w+')
    try:
        a.write(cnt % dict(git_revision=GIT_REVISION))
    finally:
        a.close()


write_version_py()

cython_flags = ["-I"] + ["-v"] + ["-X embedsignature=True"]


def generate_cython():
    cwd = os.path.abspath(os.path.dirname(__file__))
    print("Cythonizing sources")
    p = subprocess.call([sys.executable,
                         os.path.join(cwd, 'cythonize.py'),
                         'sweetsourcod'] + cython_flags,
                        cwd=cwd)
    if p != 0:
        raise RuntimeError("Running cythonize failed!")


if os.name == 'posix':
    os.environ["CC"] = "g++"
    os.environ["CXX"] = "g++"

generate_cython()


class ModuleList:
    def __init__(self, **kwargs):
        self.module_list = []
        self.kwargs = kwargs

    def add_module(self, filename):
        modname = filename.replace("/", ".")
        modname, ext = os.path.splitext(modname)
        self.module_list.append(Extension(modname, [filename], **self.kwargs))


setup(name='sweetsourcod',
      version='0.1',
      author='Stefano Martiniani',
      description="sweetsourcod: The `sweet source coding' library implements universal lossless coding (data compression)"
                  "algorithms, with a particular focus on estimating complexity measures and entropies of generic procsses",
      install_requires=["numpy", "cython", "numba", "zstd", "brotli", "zopfli", "backports.lzma"],
      packages=["sweetsourcod"
                ],
      )

#
# build the c++ files
#

include_sources_sweetsourcod = ["source/" + f
                                         for f in os.listdir("source")
                                         if f.endswith(".cpp") or f.endswith(".c")]

include_dirs = [numpy_include, "source"]

depends_sweetsourcod = [os.path.join("source/sweetsourcod", f) for f in os.listdir("source/sweetsourcod/")
                        if f.endswith(".cpp") or f.endswith(".c") or f.endswith(".h") or f.endswith(".hpp")]

# extract -c flag to set compiler
parser = argparse.ArgumentParser(add_help=False)
parser.add_argument("-j", type=int, default=1)
parser.add_argument("-c", "--compiler", type=str, default=None)
jargs, remaining_args = parser.parse_known_args(sys.argv)

# record c compiler choice. use unix (gcc) by default
# Add it back into remaining_args so distutils can see it also
idcompiler = None
if not jargs.compiler or jargs.compiler in ("unix", "gnu", "gcc"):
    idcompiler = "unix"
elif jargs.compiler in ("intel", "icc", "icpc"):
    idcompiler = "intel"

extra_compile_args = ["-std=c++11", "-Wall", "-Wextra", "-pedantic", "-O3", "-fPIC"]
if idcompiler.lower() == 'unix':
    extra_compile_args += ['-march=native', '-flto']  # , '-fopenmp'
else:
    extra_compile_args += ['-axCORE-AVX2', '-ipo', '-ip', '-unroll',
                           '-qopt-report-stdout', '-qopt-report-phase=openmp'] # '-qopenmp',

include_sources_all = include_sources_sweetsourcod

depends_all = depends_sweetsourcod

print depends_all

cxx_modules = [
    Extension("sweetsourcod.lempel_ziv",
              ["sweetsourcod/lempel_ziv.cxx"] + include_sources_all,
              include_dirs=include_dirs,
              extra_compile_args=extra_compile_args,
              libraries=['m'],
              extra_link_args=["-std=c++11"],
              language="c++", depends=depends_all,
              ),
    Extension("sweetsourcod.block_entropy",
                  ["sweetsourcod/block_entropy.cxx"] + include_sources_all,
                  include_dirs=include_dirs,
                  extra_compile_args=extra_compile_args,
                  libraries=['m'],
                  extra_link_args=["-std=c++11"],
                  language="c++", depends=depends_all,
                  ),
    Extension("sweetsourcod.hilbert",
                  ["sweetsourcod/hilbert.cxx"] + include_sources_all,
                  include_dirs=include_dirs,
                  extra_compile_args=extra_compile_args,
                  libraries=['m'],
                  extra_link_args=["-std=c++11"],
                  language="c++", depends=depends_all,
                  ),
]

setup(
    ext_modules=cxx_modules,
)
