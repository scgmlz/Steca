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
    void invalidate() { valid_ = false; }
    const T& get(const K key) {
        if (!valid_ || key!=key_) {
            cached_.recompute(key);
            valid_ = true;
            key_ = key;
        }
        return cached_;
    }
private:
    T cached_;
    bool valid_ {false};
    K key_;
};

//! Caches a value until the key changes.
template<class Key, class Value>
class Keyed {
public:
    //! Returns updated value. Takes ownership of argument key.
    const Value& update(Key* key, std::function<Value*()>recompute) {
        if (!value_ || *key!=*key_) {
            value_.reset(recompute());
            key_.reset(key);
        } else
            delete key;
        return *value_;
    }
private:
    std::unique_ptr<Value> value_;
    std::unique_ptr<Key> key_;
};

#endif // CACHED_H
