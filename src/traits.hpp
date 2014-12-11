#pragma once

namespace otus {

    struct NonCopyable {
        NonCopyable& operator=(const NonCopyable&)& = delete;
        virtual ~NonCopyable() {}
    };

    struct NonMovable {
        NonMovable& operator=(NonMovable&&)& = delete;
        virtual ~NonMovable() {}
    };

} // namespace otus
