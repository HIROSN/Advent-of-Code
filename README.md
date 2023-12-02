# Advent of Code
https://adventofcode.com/

[C/C++ for Visual Studio Code](https://code.visualstudio.com/docs/languages/cpp)

MSVC (cl) tasks.json
```
    "tasks": [
        {
            "args": [
                "/I..\\include",
                "/std:c++17",
```
MinGW (g++) tasks.json
```
    "tasks": [
        {
            "args": [
                "-I..\\include",
```
Mac (clang++) tasks.json
```
    "tasks": [
        {
            "args": [
                "-I${fileDirname}/../include",
                "-std=c++17",
                "-stdlib=libc++",
```
c_cpp_properties.json
```
    "configurations": [
        {
            "includePath": [
                "${workspaceFolder}/../include/**"
            ],
            "cStandard": "c11",
            "cppStandard": "c++17",
```
