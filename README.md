# Overview

This is a pedagogical library to reimagine and expand on some features of the C++ standard library. The intention is to write as minimal code with minimal dependency on the STL.

# Current Features

- Defer macro to create an RAII guard.
- Implementation of `relocate` to wrap move-and-destroy semantics.
- Implementation of `option` and `result` to achieve best-effort Rust-like semantics.

# TODO

- More rigorous unit testing and benchmarking to measure abstraction overhead.
- Update CMakeLists.txt for this to be includable in other projects.
- Checked arithmetic using option types.
- Allocator-aware pointers types.
- An alternative API for associative containers (`std::map`, `std::set`, etc.)
