#!/usr/bin/env bash
# Copyright 2016 Google Inc. All Rights Reserved.
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
# ==============================================================================
#
# Test user-defined ops against installation of TensorFlow.
#
# Usage: test_user_ops.sh [--virtualenv] [--gpu]
#
# If the flag --virtualenv is set, the script will use "python" as the Python
# binary path. Otherwise, it will use tools/python_bin_path.sh to determine
# the Python binary path.
#
# The --gpu flag informs the script that this is a GPU build, so that the
# appropriate test blacklists can be applied accordingly.
#

# Helper functions
# Get the absolute path from a path
abs_path() {
  [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo ""
echo "=== Testing user ops ==="

# Process input arguments
IS_VIRTUALENV=0
IS_GPU=0
while true; do
  if [[ "$1" == "--virtualenv" ]]; then
    IS_VIRTUALENV=1
  elif [[ "$1" == "--gpu" ]]; then
    IS_GPU=1
  fi
  shift

  if [[ -z "$1" ]]; then
    break
  fi
done

TMP_DIR=$(mktemp -d)
mkdir -p "${TMP_DIR}"

cleanup() {
  rm -rf "${TMP_DIR}"
}

die() {
  echo $@
  cleanup
  exit 1
}


# Obtain the path to Python binary
if [[ ${IS_VIRTUALENV} == "1" ]]; then
  PYTHON_BIN_PATH="$(which python)"
else
  source tools/python_bin_path.sh
  # Assume: PYTHON_BIN_PATH is exported by the script above
fi
echo "PYTHON_BIN_PATH: ${PYTHON_BIN_PATH}"


pushd "${TMP_DIR}"

# Obtain paths include and lib paths to the TensorFlow installation
TF_INC=$("${PYTHON_BIN_PATH}" \
         -c 'import tensorflow as tf; print(tf.sysconfig.get_include())')

if [[ -z "${TF_INC}" ]]; then
  die "FAILED to determine TensorFlow include path"
else
  echo "TensorFlow include path: ${TF_INC}"
fi

# Check g++ availability
GPP_BIN="g++"
if [[ -z $(which "${GPP_BIN}") ]]; then
  die "ERROR: ${GPP_BIN} not on path"
fi

echo ""
echo "g++ version:"
"${GPP_BIN}" -v
echo ""

if [[ ${IS_GPU} == "0" ]]; then
  echo "Testing user ops in CPU environment"

  # Locate the op kernel C++ file
  OP_KERNEL_CC="${SCRIPT_DIR}/../../../g3doc/how_tos/adding_an_op/zero_out_op_kernel_1.cc"
  OP_KERNEL_CC=$(abs_path "${OP_KERNEL_CC}")

  if [[ ! -f "${OP_KERNEL_CC}" ]]; then
    die "ERROR: Unable to find user-op kernel C++ file at: ${OP_KERNEL_CC}"
  fi

  # Copy the file to a non-TensorFlow source directory
  cp "${OP_KERNEL_CC}" ./

  # Compile the op kernel into an .so file
  SRC_FILE=$(basename "${OP_KERNEL_CC}")

  echo "Compiling user op C++ source file ${SRC_FILE}"

  USER_OP_SO="zero_out.so"
  "${GPP_BIN}" -std=c++11 -shared "${SRC_FILE}" -o "${USER_OP_SO}" \
    -fPIC -I "${TF_INC}" || \
    die "g++ compilation of ${SRC_FILE} FAILED"
else
  echo "Testing user ops in GPU environment"

  # Check nvcc availability
  NVCC_BIN="/usr/local/cuda/bin/nvcc"
  if [[ -z $(which "${NVCC_BIN}") ]]; then
    die "ERROR: ${NVCC_BIN} not on path"
  fi

  echo ""
  echo "nvcc version:"
  "${NVCC_BIN}" --version
  echo ""

  OP_KERNEL_CU="${SCRIPT_DIR}/../../../g3doc/how_tos/adding_an_op/cuda_op_kernel.cu.cc"
  OP_KERNEL_CU=$(abs_path "${OP_KERNEL_CU}")
  if [[ ! -f "${OP_KERNEL_CU}" ]]; then
    die "ERROR: Unable to find user-op kernel CUDA file at: ${OP_KERNEL_CU}"
  fi

  OP_KERNEL_CC="${SCRIPT_DIR}/../../../g3doc/how_tos/adding_an_op/cuda_op_kernel.cc"
  OP_KERNEL_CC=$(abs_path "${OP_KERNEL_CC}")
  if [[ ! -f "${OP_KERNEL_CC}" ]]; then
    die "ERROR: Unable to find user-op kernel C++ file at: ${OP_KERNEL_CC}"
  fi

  # Copy the file to a non-TensorFlow source directory
  cp "${OP_KERNEL_CU}" ./
  cp "${OP_KERNEL_CC}" ./

  OP_KERNEL_O=$(echo "${OP_KERNEL_CC}" | sed -e 's/\.cc/\.o/')
  "${NVCC_BIN}" -std=c++11 -c -o "${OP_KERNEL_O}" "${OP_KERNEL_CC}" \
      -I "${TF_INC}" -D GOOGLE_CUDA=1 -x cu -Xcompiler -fPIC || \
      die "nvcc compilation of ${OP_KERNEL_CC} FAILED"

  # USER_OP_SO=$(basename $(echo "${OP_KERNEL_CC}" | sed -e 's/\.cc/\.so/'))
  USER_OP_SO="add_one.so"
  "${GPP_BIN}" -std=c++11 -shared -o "${USER_OP_SO}" "${OP_KERNEL_CC}" \
      "${OP_KERNEL_O}" -I "${TF_INC}" -L "/usr/local/cuda/lib64" \
      -fPIC -lcudart || \
      die "g++ compilation of ${OP_KERNEL_CC}" FAILED
fi


# Try running the op
USER_OP=$(echo "${USER_OP_SO}" | sed -e 's/\.so//')
echo "Invoking user op ${USER_OP} defined in file ${USER_OP_SO} "\
"via pip installation"

ORIG_OUTPUT=$("${PYTHON_BIN_PATH}" -c "import tensorflow as tf; print(tf.Session('').run(tf.load_op_library('./${USER_OP_SO}').${USER_OP}([42, 43, 44])))")

# Format OUTPUT for analysis
OUTPUT=$(echo "${ORIG_OUTPUT}" | sed -e 's/\[//g' | sed -e 's/\]//g')
if [[ $(echo "${OUTPUT}" | wc -w) != "3" ]]; then
  die "ERROR: Unexpected number of elements in user op output: \""\
"${ORIG_OUTPUT}\""
fi

N0=$(echo "${OUTPUT}" | awk '{print $1}')
N1=$(echo "${OUTPUT}" | awk '{print $2}')
N2=$(echo "${OUTPUT}" | awk '{print $3}')

if [[ $(echo "${N0}==42" | bc -l) != "1" ]] || \
   [[ $(echo "${N1}==0" | bc -l) != "1" ]] || \
   [[ $(echo "${N2}==0" | bc -l) != "1" ]]; then
  die "FAILED: Incorrect output from user op: ${OUTPUT} "
fi

popd

cleanup

echo ""
echo "SUCCESS: Testing of user ops PASSED"
