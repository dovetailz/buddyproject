#!/bin/sh
find ../ -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\)' -not -path "../build/*" -exec clang-format --verbose -style=google -i {} \;