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
    virtual const T& get() const { if (!valid_) recompute(); return cached_; }
protected:
    void recompute() { cached_.recompute(); valid_ = true; }
    T cached_;
    bool valid_ {false};
};

//! Cached object with key.
template<typename T, typename K>
class KeyedCache : public Cached<T> {
public:
    const T& get() const override = delete;
    const T& get(const K& key) const {
        if (!Cached<T>::valid_ || key!=key_) {
            Cached<T>::recompute(); key = key_; }
        return Cached<T>::cached_; }
private:
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
