# per4m

`per4m` is a C++20 library that provides probabilistic stopping utilities based on kernel density estimation (KDE).

## Public API

The supported public API currently consists of:

- `per4m::ProbabilisticStop`
- `per4m::KDE`
- `per4m::Bandwidth`

## Requirements

- CMake 3.26+
- A C++20 compiler
- `pkg-config`
- FFTW3 development files (`fftw3`)

On Debian/Ubuntu-like systems, the required packages are typically:

```bash
sudo apt install cmake pkg-config libfftw3-dev doxygen
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

To install the library:

```bash
cmake --install build --prefix /tmp/per4m-install
```

## Using the library in another CMake project

After installation:

```cmake
find_package(per4m REQUIRED)
target_link_libraries(my_target PRIVATE per4m::per4m)
```

The package configuration resolves the exported FFTW3 dependency through `pkg-config`, so the consumer project must also have `pkg-config` and FFTW3 available.

## Using the library with `FetchContent`

If you prefer to vendor `per4m` directly from CMake instead of installing it first, you can use `FetchContent`:

```cmake
include(FetchContent)

set(PER4M_THREAD_SAFE ON CACHE BOOL "Enable thread-safe mode in per4m")

FetchContent_Declare(per4m
    URL https://github.com/fdaniele85/per4m/archive/refs/tags/v1.0.0.zip
)

FetchContent_MakeAvailable(per4m)

target_link_libraries(my_target PRIVATE per4m::per4m)
```

Notes:

- with `FetchContent`, you do **not** call `find_package(per4m)`; the target is created directly by the fetched project
- `PER4M_THREAD_SAFE` must be set **before** `FetchContent_MakeAvailable(per4m)` so that `per4m` is configured with the desired option
- FFTW3 is still required at configure time, so the host system must provide both `pkg-config` and the `fftw3` development package
- if FFTW3 is not found, CMake configuration still fails because the dependency is `REQUIRED`

## `PER4M_THREAD_SAFE`

The project exposes the CMake option:

```bash
-DPER4M_THREAD_SAFE=ON
```

When enabled, the library adds the public compile definition `PER4M_THREAD_SAFE` and the internal synchronization helpers use `std::mutex`/`std::lock_guard`.

When disabled (default), the same locking helpers become no-op lightweight placeholders, so the code builds without mutex-based synchronization overhead.

Example configure command:

```bash
cmake -S . -B build -DPER4M_THREAD_SAFE=ON
```

## FFTW3 dependency

`per4m` currently looks for FFTW3 through `pkg-config`:

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW3 REQUIRED IMPORTED_TARGET fftw3)
```

If FFTW3 is found:

- the library links against `PkgConfig::FFTW3`
- the public compile definition `PER4M_USE_FFTW` is enabled
- the FFTW-backed bandwidth code is compiled in

If FFTW3 is **not** found:

- CMake configuration **fails immediately** because the dependency is marked as `REQUIRED`
- no build files are generated for `per4m`

At runtime, when FFTW support is available and the ISJ bandwidth computation throws an exception, `per4m::KDE` falls back to Silverman's rule.

## Generating documentation with Doxygen

A `Doxyfile` is provided in the project root.

Generate the documentation with:

```bash
doxygen Doxyfile
```

The HTML output is generated under:

```text
docs/doxygen/html/
```

The Doxygen configuration excludes:

- `include/per4m/detail/`
- `include/per4m/ProbabilisticFilter.h`
- `include/per4m/Ribeiro.h`

