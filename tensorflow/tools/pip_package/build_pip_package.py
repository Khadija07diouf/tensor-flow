# Copyright 2023 The Tensorflow Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Tool to rearrange files and build the wheel.

In a nutshell this script does:
1) Takes lists of paths to .h/.py/.so/etc files.
2) Creates a temporary directory.
3) Copies files from #1 to #2 with some exceptions and corrections.
4) A wheel is created from the files in the temp directory.

Most of the corrections are related to tsl/xla vendoring:
These files used to be a part of source code but were moved to an external repo.
To not break the TF API, we pretend that it's still part of the it.
"""

import argparse
import glob
import os
import shutil
import subprocess
import sys
import tempfile

from utils import utils as utils_module


# The "default" argument must be added because the "required" is set to "True".
# If no values for output and project names are mentioned after the build process
# then the default values will be considered.

# If the devault value is not specified, the ".whl" packaging might fail.
def parse_args() -> argparse.Namespace:
  """Arguments parser."""
  parser = argparse.ArgumentParser(
      description="Helper for building pip package", fromfile_prefix_chars="@")
  parser.add_argument(
        "--output-name", 
        required=True,
        help="Output file for the wheel, mandatory",
        default='default_output_name'
      )
  parser.add_argument(
    "--project-name", 
    required=True,
    help="Project name to be passed to setup.py",
    default='default_project_name'
  )
  parser.add_argument(
      "--headers", help="header files for the wheel", action="append")
  parser.add_argument("--srcs", help="source files for the wheel",
                      action="append", default=[])
  parser.add_argument("--xla_aot", help="xla aot compiled sources",
                      action="append", default=[])
  parser.add_argument("--version", help="TF version")
  parser.add_argument("--collab", help="True if collaborator build")
  return parser.parse_args()


def prepare_headers(headers: list[str], srcs_dir: str) -> None:
  """Copy and rearrange header files in the target directory.

  Filter out headers by their path and replace paths for some of them.

  Args:
    headers: a list of paths to header files.
    srcs_dir: target directory where headers are copied to.
  """
  path_to_exclude = [
      "external/pypi",
      "external/jsoncpp_git/src",
      "local_config_cuda/cuda/_virtual_includes",
      "local_config_tensorrt",
      "python_x86_64",
      "python_aarch64",
      "llvm-project/llvm/",
  ]

  path_to_replace = {
      "external/com_google_absl/": "",
      "external/eigen_archive/": "",
      "external/jsoncpp_git/": "",
      "external/com_google_protobuf/src/": "",
      "external/local_xla/": "tensorflow/compiler",
      "external/local_tsl/": "tensorflow",
  }

  for file in headers:
    if file.endswith("cc.inc"):
      continue

    if any(i in file for i in path_to_exclude):
      continue

    for path, val in path_to_replace.items():
      if path in file:
        utils_module.copy_file(file, os.path.join(srcs_dir, val), path)
        break
    else:
      utils_module.copy_file(file, srcs_dir)

  create_local_config_python(os.path.join(srcs_dir,
                                          "external/local_config_python"))

  shutil.copytree(os.path.join(srcs_dir, "external/local_config_cuda/cuda"),
                  os.path.join(srcs_dir, "third_party/gpus"))
  shutil.copytree(os.path.join(srcs_dir, "tensorflow/compiler/xla"),
                  os.path.join(srcs_dir, "xla"))
  shutil.copytree(os.path.join(srcs_dir, "tensorflow/tsl"),
                  os.path.join(srcs_dir, "tsl"))


def prepare_srcs(deps: list[str], srcs_dir: str) -> None:
  """Rearrange source files in target the target directory.

  Exclude `external` files and move vendored xla/tsl files accordingly.

  Args:
    deps: a list of paths to files.
    srcs_dir: target directory where files are copied to.
  """
  path_to_replace = {
      "external/local_xla/": "tensorflow/compiler",
      "external/local_tsl/": "tensorflow",
  }

  for file in deps:
    for path, val in path_to_replace.items():
      if path in file:
        utils_module.copy_file(file, os.path.join(srcs_dir, val), path)
        break
    else:
      # exclude external py files
      if "external" not in file:
        utils_module.copy_file(file, srcs_dir)


def prepare_aot(aot: list[str], srcs_dir: str) -> None:
  """Rearrange xla_aot files in target the target directory.
  
  Args:
    aot: a list of paths to files that should be in xla_aot directory.
    srcs_dir: target directory where files are copied to.
  """
  for file in aot:
    if "external/local_tsl/" in file:
      utils_module.copy_file(file, srcs_dir, "external/local_tsl/")
    elif "external/local_xla/" in file:
      utils_module.copy_file(file, srcs_dir, "external/local_xla/")
    else:
      utils_module.copy_file(file, srcs_dir)

  shutil.move(
      os.path.join(
          srcs_dir, "tensorflow/tools/pip_package/xla_build/CMakeLists.txt"
      ),
      os.path.join(srcs_dir, "CMakeLists.txt"),
  )


def prepare_wheel_srcs(
    headers: list[str], srcs: list[str], aot: list[str], srcs_dir: str,
    version: str) -> None:
  """Rearrange source and header files.
  
  Args: 
    headers: a list of paths to header files.
    srcs: a list of paths to the rest of files.
    aot: a list of paths to files that should be in xla_aot directory.
    srcs_dir: directory to copy files to.
    version: tensorflow version.
  """
  prepare_headers(headers, os.path.join(srcs_dir, "tensorflow/include"))
  prepare_srcs(srcs, srcs_dir)
  prepare_aot(aot, os.path.join(srcs_dir, "tensorflow/xla_aot_runtime_src"))

  # Every directory that contains a .py file gets an empty __init__.py file.
  utils_module.create_init_files(os.path.join(srcs_dir, "tensorflow"))

  # move MANIFEST and THIRD_PARTY_NOTICES to the root
  shutil.move(
      os.path.join(srcs_dir, "tensorflow/tools/pip_package/MANIFEST.in"),
      os.path.join(srcs_dir, "MANIFEST.in"),
  )
  shutil.move(
      os.path.join(srcs_dir,
                   "tensorflow/tools/pip_package/THIRD_PARTY_NOTICES.txt"),
      os.path.join(srcs_dir, "tensorflow/THIRD_PARTY_NOTICES.txt"),
  )

  update_xla_tsl_imports(os.path.join(srcs_dir, "tensorflow"))
  if not utils_module.is_windows():
    rename_libtensorflow(os.path.join(srcs_dir, "tensorflow"), version)
  if not utils_module.is_macos() and not utils_module.is_windows():
    patch_so(srcs_dir)


def update_xla_tsl_imports(srcs_dir: str) -> None:
  """Workaround for TSL and XLA vendoring."""
  utils_module.replace_inplace(srcs_dir, "from tsl", "from tensorflow.tsl")
  utils_module.replace_inplace(
      srcs_dir,
      "from local_xla.xla",
      "from tensorflow.compiler.xla",
      )
  utils_module.replace_inplace(
      srcs_dir, "from xla", "from tensorflow.compiler.xla"
  )


def patch_so(srcs_dir: str) -> None:
  """Patch .so files.
  
  We must patch some of .so files otherwise auditwheel will fail.
  
  Args:
    srcs_dir: target directory with .so files to patch.
  """
  to_patch = {
      "tensorflow/python/_pywrap_tensorflow_internal.so": (
          "$ORIGIN/../../tensorflow/compiler/xla/tsl/python/lib/core"
      ),
      (
          "tensorflow/compiler/mlir/quantization/tensorflow/python/"
          "pywrap_function_lib.so"
      ): "$ORIGIN/../../../../../python",
      (
          "tensorflow/compiler/mlir/quantization/tensorflow/python/"
          "pywrap_quantize_model.so"
      ): "$ORIGIN/../../../../../python",
      (
          "tensorflow/compiler/mlir/tensorflow_to_stablehlo/python/"
          "pywrap_tensorflow_to_stablehlo.so"
      ): "$ORIGIN/../../../../python",
      (
          "tensorflow/compiler/mlir/lite/python/_pywrap_converter_api.so"
      ): "$ORIGIN/../../../../python",
  }
  for file, path in to_patch.items():
    rpath = subprocess.check_output(
        ["patchelf", "--print-rpath",
         "{}/{}".format(srcs_dir, file)]).decode().strip()
    new_rpath = rpath + ":" + path
    subprocess.run(["patchelf", "--set-rpath", new_rpath,
                    "{}/{}".format(srcs_dir, file)], check=True)
    subprocess.run(["patchelf", "--shrink-rpath",
                    "{}/{}".format(srcs_dir, file)], check=True)


def rename_libtensorflow(srcs_dir: str, version: str):
  """Update libtensorflow_cc file name.
  
  Bazel sets full TF version in name but libtensorflow_cc must contain only 
  major. Update accordingly to the platform:
  e.g. libtensorflow_cc.so.2.15.0 -> libtensorflow_cc.2
  
  Args:
    srcs_dir: target directory with files.
    version: Major version to be set.
  """
  major_version = version.split(".")[0]
  if utils_module.is_macos():
    shutil.move(
        os.path.join(srcs_dir, "libtensorflow_cc.{}.dylib".format(version)),
        os.path.join(
            srcs_dir, "libtensorflow_cc.{}.dylib".format(major_version)
        ),
    )
    shutil.move(
        os.path.join(
            srcs_dir, "libtensorflow_framework.{}.dylib".format(version)
        ),
        os.path.join(
            srcs_dir, "libtensorflow_framework.{}.dylib".format(major_version)
        ),
    )
  else:
    shutil.move(
        os.path.join(srcs_dir, "libtensorflow_cc.so.{}".format(version)),
        os.path.join(srcs_dir, "libtensorflow_cc.so.{}".format(major_version)),
    )
    shutil.move(
        os.path.join(srcs_dir, "libtensorflow_framework.so.{}".format(version)),
        os.path.join(
            srcs_dir, "libtensorflow_framework.so.{}".format(major_version)
        ),
    )


def create_local_config_python(dst_dir: str) -> None:
  """Copy python and numpy header files to the destination directory."""
  shutil.copytree(
      "external/pypi_numpy/site-packages/numpy/core/include",
      os.path.join(dst_dir, "numpy_include"),
  )
  if utils_module.is_windows():
    path = "external/python_*/include"
  else:
    path = "external/python_*/include/python*"
  shutil.copytree(glob.glob(path)[0], os.path.join(dst_dir, "python_include"))


def build_wheel(dir_path, project_location: str, project_name: str,
                collab: str = False) -> None:
  """Build the wheel in the target directory.
  
  Args:
    dir_path: directory where the wheel will be stored
    cwd: path to directory with wheel source files
    project_name: name to pass to setup.py.
    collab: defines if this is a collab build
  """
  env = os.environ.copy()
  if utils_module.is_windows():
    # HOMEPATH is not set by bazel but it's required by setuptools.
    env["HOMEPATH"] = env.get("HOMEPATH", "C:")
  # project_name is needed by setup.py.
  env["project_name"] = project_name

  if collab == "True":
    env["collaborator_build"] = "1"

  # constructing a full path to setup.py within the project.
  setup_py_path = os.path.join(project_location, 'tensorflow', 'tools', 'pip_package', 'setup.py')

  subprocess.run(
      [
          sys.executable,
          setup_py_path,
          "bdist_wheel",
          f"--dist-dir={dir_path}",
      ],
      check=True,
      cwd=project_location,
      env=env,
  )

  # listing the files in the directory where the wheel file should be stored.
  wheel_files = os.listdir(dir_path)
  if wheel_files:
      # fetch the name of the first wheel file created.
      output_name = wheel_files[0]
      # determining the final output path, defaulting to dir_path if WHEEL_OUTPUT_DIR is not set.
      final_output_path = os.environ.get('WHEEL_OUTPUT_DIR', dir_path)
      print('---------------------', final_output_path)
      # Now, we move the wheel file to the final output directory.
      os.rename(os.path.join(dir_path, output_name),
                os.path.join(final_output_path, output_name))


if __name__ == "__main__":
  args = parse_args()
  project_location = os.getcwd()  
  # creating a temporary directory for the wheel building process.
  with tempfile.TemporaryDirectory(prefix="tensorflow_wheel") as temp_dir_path:
      if args.headers or args.srcs or args.xla_aot:
          prepare_wheel_srcs(args.headers, args.srcs, args.xla_aot, temp_dir_path, args.version)
      build_wheel(temp_dir_path, project_location, args.project_name, args.collab)