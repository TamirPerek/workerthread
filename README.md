# workerthread

[![CMake](https://github.com/TamirPerek/workerthread/actions/workflows/cmake.yml/badge.svg?branch=master)](https://github.com/TamirPerek/workerthread/actions/workflows/cmake.yml) [![CodeQL](https://github.com/TamirPerek/workerthread/actions/workflows/codeql.yml/badge.svg?branch=master)](https://github.com/TamirPerek/workerthread/actions/workflows/codeql.yml)

A small library to manage small helper threads.

## Usage

```cpp
if(!ThreadPool::isCreated());
   ThreadPool::createInstance();
    
auto res = ThreadPool::getInstance().add([]()
            {
                return 40 + 2;
            });
std::println("Result is {}", res.get());
```

You can change the amount of threads

```cpp
ThreadPool::createInstance(64);
```
