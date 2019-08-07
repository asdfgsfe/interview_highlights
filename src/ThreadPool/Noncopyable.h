#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace zen {

class noncopyable {
protected:
    noncopyable() = default;
    ~noncopyable() = default;
    noncopyable( const noncopyable& ) = delete;
    noncopyable& operator=( const noncopyable& ) = delete;
};

}

#endif
