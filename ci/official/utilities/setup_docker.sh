#!/bin/bash
# Copyright 2023 The TensorFlow Authors. All Rights Reserved.
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
if [[ "$TFCI_DOCKER_PULL_ENABLE" == 1 ]]; then
  # Simple retry logic for docker-pull errors. Sleeps if a pull fails.
  # Pulling an already-pulled container image will finish instantly, so
  # repeating the command costs nothing.
  docker pull "$TFCI_DOCKER_IMAGE" || sleep 15
  docker pull "$TFCI_DOCKER_IMAGE" || sleep 30
  docker pull "$TFCI_DOCKER_IMAGE" || sleep 60
  docker pull "$TFCI_DOCKER_IMAGE"
fi 

if [[ "$TFCI_DOCKER_REBUILD_ENABLE" == 1 ]]; then
  DOCKER_BUILDKIT=1 docker build --cache-from "$TFCI_DOCKER_IMAGE" -t "$TFCI_DOCKER_IMAGE" $TFCI_DOCKER_REBUILD_ARGS
  if [[ "$TFCI_DOCKER_REBUILD_UPLOAD_ENABLE" == 1 ]]; then
    docker push "$TFCI_DOCKER_IMAGE"
  fi
fi

# Keep the existing "tf" container if it's already present.
# The container is not cleaned up automatically! Remove it with:
# docker rm tf
if ! docker container inspect tf >/dev/null 2>&1 ; then
  # Pass all existing TFCI_ variables into the Docker container
  env_file=$(mktemp)
  env | grep ^TFCI_ > "$env_file"
  if [[ `uname -s | grep -P '^MSYS_NT'` ]]; then
    # Docker on Windows doesn't understand Linux-like paths, so these are
    # converted back to the actual Windows paths.
    # Host dirs can also only be mapped to an existing drive inside the
    # container, so T:\ is replaced with C:\.
    TFCI_GIT_DIR_WIN=$(cygpath -m $TFCI_GIT_DIR)
    TFCI_OUTPUT_DIR_WIN=`cygpath -m $TFCI_OUTPUT_DIR | sed -E 's|^[a-zA-Z]:|C:|g'`
    sed -iE 's|^TFCI_OUTPUT_DIR=.*|TFCI_OUTPUT_DIR='"$TFCI_OUTPUT_DIR_WIN"'|g' $env_file
    grep TFCI_OUTPUT_DIR $env_file
    WORKING_DIR=`echo "$TFCI_GIT_DIR_WIN" | sed -E 's|^[a-zA-Z]:|C:|g'`
    APPLICATION_CREDS=`echo "$GOOGLE_APPLICATION_CREDENTIALS" | sed -E 's|^[a-zA-Z]:|C:|g'`
    echo "GOOGLE_APPLICATION_CREDENTIALS=$APPLICATION_CREDS" > "$env_file"
    env_file=$(cygpath -w $env_file)
  fi

  docker run $TFCI_DOCKER_ARGS --name tf -w "$WORKING_DIR" -itd --rm \
      -v "$TFCI_GIT_DIR_WIN:$WORKING_DIR" \
      --env-file "$env_file" \
      "$TFCI_DOCKER_IMAGE" \
    bash
fi
tfrun() { docker exec tf "$@"; }
