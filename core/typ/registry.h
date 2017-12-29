//**************************************************************************************************
//*  SCG generic C++ templates
//*  (C) Joachim Wuttke: Frida 2001-, Steca2 2017-
//*  License: this file is in the public domain
//**************************************************************************************************

//! \file  registry.h
//! \brief declares a generic registry mixin interface.

#ifndef REGISTRY_H
#define REGISTRY_H

#include "str.h" // Steca specific
#include "exception.h" // Steca specific
#include <QVector>

//! Mixin interface for registries holding objects of type T

template <typename T> class IRegistry {
protected:
    QMap<str, T> m_map; //!< unsorted hash, for quick lookup
    QStringList m_keys; //!< sorted array, for listing all registered objects
public:
    void register_item(const str& key, T val) {
        if (m_map.find(key) != m_map.end())
            throw "Duplicate registry entry " + key;
        m_map.insert(key, val);
        m_keys.push_back(key);
    }
    T find(const str& key) const {
        auto pos = m_map.find(key);
        return pos == m_map.end() ? nullptr : pos.value(); // here std::map and QMap differ
    }
    T find_or_fail(const str& key) const {
        T ret = find(key);
        if (!ret)
            throw Exception("Cannot find '" + key + "' in function registry");
        return ret;
    }
    str key_at(const unsigned int idx) const {
        return idx>=m_keys.size() ? "" : m_keys[idx];
    }
    str key_at_or_fail(const unsigned int idx) const {
        str ret = key_at(idx);
        if (ret=="")
            throw Exception("Invalid index in registry lookup");
        return ret;
    }
    unsigned int size() const { return m_keys.size(); }
    QStringList keys() const { return m_keys; }
};

#endif // REGISTRY_H
