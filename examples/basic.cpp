//
// Created by backo on 4/12/25.
//
#include <medes.h>


using service = medes::service<medes::dummy_request_protocol, "my_service", "https://dummyjson.com/",
    medes::get<"fortune", "/fortune", medes::optional<"words", int>>,
    medes::group<"posts", "/posts",
        medes::get<"get", "/get/{id}">
    >
>;

int main() {

}