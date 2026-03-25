# Medes — The Modern C++ API Client Library

Medes is a **modern C++ library for building an HTTP API client**. Using metaprogramming and other advanced techniques,
Medes provides a simple yet powerful API for making **type-safe HTTP requests**. Make your code expressive while knowing
it'll work before even compiling.



Medes is used as a wrapper over your favourite HTTP requests library. Several adapters especially designed for some HTTP
libraries are available.

This library is a good alternative to OpenAPI for **small APIs** or if you don't want to use a code generator.

## Features

- **Endpoint tree.** Support for nested endpoints and path parameters in both groups and individual endpoints.
- **Compile-time routing.** Specify the endpoint you want to use in a template parameter. The request code is 
  generated at runtime. No more building URLs manually; Medes handles it for you.
- **Path and query parameters validation.** The number of path parameters and type and key name of query parameters 
  are all checked at compile-time, with support for required and optional query parameters.

In a nutshell: **Medes is as fast as your HTTP library but a hundred times safer.**

Note that some IDEs may not be able to highlight all the errors before compiling, so make sure to compile frequently.


## Available adapters

- [curlpp](https://github.com/datacratic/curlpp)
- [dpp](https://github.com/brainboxdotcc/DPP) - Support for both callback and coroutine style

## Example

```cpp
#include <medes/medes.h>
#include <medes/adapters/curlpp_adapter.h>

using medes::curlpp_adapter::curlpp_request_protocol;

using my_service = medes::service<curlpp_request_protocol, "basic_service", "https://dummyjson.com/",
    medes::get<"test", "/test">,
    medes::group<"products", "/products",
        medes::get<"all", "">,
        medes::get<"get", "/{id}">,
        medes::get<"search", "search", medes::required<"q">, medes::optional<"limit", int>>
    >
>;

int main() {
    // Create a service instance for the endpoint tree
    my_service service_instance;
    
    // The request function returns the raw response object from the HTTP library.
    // Since curlpp returns a std::stringstream, we can convert it to a string.
    std::string response_body = service_instance.request<"test">().body.str();
    
    std::cout << response_body << std::endl;
    
    // All of the following lines cause a **compile-time** error
    // No endpoint with the name "invalid_endpoint" exists in the endpoint tree
    service_instance.request<"invalid_endpoint">();      
    // Missing path parameter "id"
    service_instance.request<"products.get">({});
    // Missing required query parameter "q"
    service_instance.request<"products.search">({});

    // In the future, Glaze will be used to parse the response body and
    // return a typed object.
}

```



## Roadmap

- [x] Recursive endpoint tree (groups of endpoints)
- [x] Concept-based adapter system
- [x] Typed query parameters
- [ ] Typed path parameters
- [ ] File upload
- [ ] Multipart form data
- [ ] Nicer error messages
- [ ] Authentication
- [ ] Middleware in endpoint tree
- [ ] JSON integration with Glaze and type validation

And in the very far future...

- [ ] CLion plugin for endpoint autocompletion (is it even possible?) and even better error messages
