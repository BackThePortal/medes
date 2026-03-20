# Medes — The Modern C++ API Client Library

Medes is a **modern C++ library for building an HTTP API client**. Using metaprogramming and other advanced techniques,
Medes provides a simple yet powerful API for making **type-safe HTTP requests**. Make your code expressive while knowing
it'll work before even compiling.



Medes is used as a wrapper over your favourite HTTP requests library. Several adapters especially designed for some HTTP
libraries are available.

This library is a good alternative to OpenAPI for **small APIs** or if you don't want to use a code generator.

## Features

- **Compile-time routing.** 

## Roadmap

- [x] Recursive endpoint tree (groups of endpoints)
- [ ] File upload
- [ ] Multipart form data
- [ ] Nicer error messages
- [ ] JSON integration with Glaze and type validation

And in the very far future...

- [ ] CLion plugin for endpoint autocompletion (is it even possible?) and even better error messages

## Available adapters

- [curlpp](https://github.com/datacratic/curlpp)
- [dpp](https://github.com/brainboxdotcc/DPP) (dynamic linking recommended)