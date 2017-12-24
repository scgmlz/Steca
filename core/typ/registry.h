//**************************************************************************************************
//*  SCG generic C++ templates
//*  (C) Joachim Wuttke: Frida 2001-, Steca2 2017-
//*  License: this file is in the public domain
//**************************************************************************************************

//! \file  registry.h
//! \brief declares a generic registry mixin interface.

#ifndef IREGISTRY_H
#define IREGISTRY_H

#include "str.h" // Steca specific
#include "exception.h" // Steca specific
#include <QMap>
#include <QVector>

//! Mixin interface for registries holding objects of type T

template <typename T> class IRegistry {
protected:
    QMap<str, T> m_map; //!< unsorted hash, for quick lookup
    QVector<T> m_list; //!< sorted array, for listing all registered objects
public:
    void register_item(const str& key, T val) {
        if (m_map.find(key) != m_map.end())
            throw "Duplicate registry entry " + key;
        m_map.insert(key, val);
        m_list.push_back(val);
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
    T at(const unsigned int idx) const {
        return idx>=m_list.size() ? nullptr : m_list[idx];
    }
    T at_or_fail(const unsigned int idx) const {
        T ret = at(idx);
        if (!ret)
            throw Exception("Invalid index in registry lookup");
        return ret;
    }
    unsigned int size() const { return m_list.size(); }
    str name_at(const unsigned int idx) const { return at_or_fail(idx)->name(); }
};

#endif // IREGISTRY_H
