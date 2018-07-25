//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/cached.h
//! @brief     Defines class CACHED
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef CACHED_H
#define CACHED_H

#include <memory>

//! Cached object.
template<typename T>
class Cached {
public:
    void invalidate() { valid_ = false; }
    const T& get() {
        if (!valid_) {
            cached_.recompute();
            valid_ = true;
        }
        return cached_;
    }
private:
    T cached_;
    bool valid_ {false};
};

//! Cached object with key.
template<typename T, typename K>
class KeyedCache {
public:
    void invalidate() { cached_.release(); }
    const T& get(const K key) {
        if (!cached_ || key!=key_) {
            cached_ = std::make_unique<T>(T(key));
            key_ = key;
        }
        return *cached_;
    }
private:
    std::unique_ptr<T> cached_;
    K key_;
};

#endif // CACHED_H
