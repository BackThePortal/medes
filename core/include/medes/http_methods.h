//
// Created by backo on 3/11/25.
//

#ifndef MEDES_HTTP_METHODS_H
#define MEDES_HTTP_METHODS_H

namespace medes {
    enum class http_method {
        get,
        post,
        put,
        patch,
        _delete // delete is a reserved keyword
    };

}

#endif //MEDES_HTTP_METHODS_H
