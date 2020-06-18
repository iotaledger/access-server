#!/usr/bin/env bash

# Install code-style checker into pre-commit hook
repo_root=$(git rev-parse --show-toplevel)
git_hooks="${repo_root}/.git/hooks"
hook_pre_commit="${repo_root}/.git/hooks/pre-commit"
script_file="${repo_root}/git_hooks/format_check"

if [ -f ${hook_pre_commit} ]; then
    echo "pre-commit hook exists."
    exit 1
else
    ln -s "${script_file}"  "${hook_pre_commit}" 
    echo "done"
fi

exit 0
