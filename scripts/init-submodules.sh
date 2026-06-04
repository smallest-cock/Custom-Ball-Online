#!/bin/bash
set -e

git submodule init

# Shallow clone for json
git submodule update --depth=1 external/json

# Configure branch-tracked submodules
setup_branch_submodule() {
    local path=$1
    local branch=$2
    git submodule set-branch --branch "$branch" "$path"
    git submodule update --remote "$path"
    cd "$path"
    git checkout "$branch"
    git pull
    cd ../..
}

setup_branch_submodule external/RLSDK main
setup_branch_submodule external/ModUtils main
setup_branch_submodule external/BMSDK master
setup_branch_submodule external/BakkesmodPluginTemplate master

git submodule status
