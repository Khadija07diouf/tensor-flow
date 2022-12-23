# Copyright 2019 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Lit runner configuration."""

import os
import platform
import sys
import lit.formats
from lit.llvm import llvm_config
from lit.llvm.subst import ToolSubst

# Lint for undefined variables is disabled as config is not defined inside this
# file, instead config is injected by way of evaluating runlit.cfg.py from
# runlit.site.cfg.py which in turn is evaluated by lit.py. The structure is
# common for lit tests and intended to only persist temporarily (b/136126535).
# pylint: disable=undefined-variable
# Configuration file for the 'lit' test runner.

# name: The name of this test suite.
config.name = 'MLIR ' + os.path.basename(config.mlir_test_dir)

config.test_format = lit.formats.ShTest(not llvm_config.use_lit_shell)

# suffixes: A list of file extensions to treat as test files.
config.suffixes = ['.cc', '.hlo', '.hlotxt', '.json', '.mlir', '.pbtxt', '.py']

# test_source_root: The root path where tests are located.
config.test_source_root = config.mlir_test_dir

# test_exec_root: The root path where tests should be run.
config.test_exec_root = os.environ['RUNFILES_DIR']

if platform.system() == 'Windows':
  tool_patterns = [
      ToolSubst('FileCheck.exe', unresolved='fatal'),
      #  Handle these specially as they are strings searched for during testing.
      ToolSubst('count.exe', unresolved='fatal'),
      ToolSubst('not.exe', unresolved='fatal')
  ]

  llvm_config.config.substitutions.append(
      ('%python', '"%s"' % (sys.executable)))

  llvm_config.add_tool_substitutions(tool_patterns,
                                     [llvm_config.config.llvm_tools_dir])
else:
  llvm_config.use_default_substitutions()

llvm_config.config.substitutions.append(
    ('%tfrt_bindir', 'tensorflow/compiler/aot'))

# Tweak the PATH to include the tools dir.
llvm_config.with_environment('PATH', config.llvm_tools_dir, append_path=True)

for key in ['HIP_VISIBLE_DEVICES', 'CUDA_VISIBLE_DEVICES',
            'TF_PER_DEVICE_MEMORY_LIMIT_MB']:
  value = os.environ.get(key, None)
  if value != None:
    llvm_config.with_environment(key, value)

tool_dirs = config.mlir_tf_tools_dirs + [
    config.mlir_tools_dir, config.llvm_tools_dir
]
tool_names = [
    'mlir-opt',
    'mlir-hlo-opt',
    'mlir-translate',
    'tf-opt',
    'tf-reduce',
    'tf_tfl_translate',
    'tf_tfjs_translate',
    'flatbuffer_to_string',
    'flatbuffer_translate',
    'tf-mlir-translate',
    'mlir-tflite-runner',
    'tfcompile',
    'json_to_flatbuffer',
    'xla-cpu-opt',
    'xla-gpu-opt',
    'xla-mlir-gpu-opt',
    'xla-opt',
    'hlo_to_llvm_ir',
    'kernel-gen-opt',
    'tf_to_kernel',
    'tf_to_gpu_binary',
    'tfjs-opt',
    'tac-opt-all-backends',
    'tac-translate',
    'tfg-opt-no-passes',
    'tfg-transforms-opt',
    'tfg-translate',
    'tf-tfrt-opt',
    'lhlo-tfrt-opt',
    'tf-quant-opt',
    'mhlo-tosa-opt',
    'xla-runtime-opt',
    'odml-to-stablehlo-opt',
    'odml_to_stablehlo',
    'xla-translate',
    'xla-translate-opt',
    'xla-translate-gpu-opt',
]
tools = [ToolSubst(s, unresolved='ignore') for s in tool_names]
llvm_config.add_tool_substitutions(tools, tool_dirs)
# pylint: enable=undefined-variable
