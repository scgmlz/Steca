//**************************************************************************************************
//*  SCG generic C++ templates
//*  (C) Joachim Wuttke: Frida 2001-, Steca 2017-
//*  License: this file is in the public domain
//**************************************************************************************************

//! \file  registry.h
//! \brief declares a generic registry mixin interface.

#ifndef REGISTRY_H
#define REGISTRY_H

#include "exception.h"
#include <QMap>        // no auto rm
#include <QStringList> // no auto rm

//! Mixin interface for registries holding objects of type T

template <typename T> class IRegistry {
protected:
    QMap<QString, T> m_map; //!< unsorted hash, for quick lookup
    QStringList m_keys; //!< sorted array, for listing all registered objects
public:
    void register_item(const QString& key, T val) {
        if (m_map.find(key) != m_map.end())
            THROW("Duplicate registry entry " + key);
        m_map.insert(key, val);
        m_keys.push_back(key);
    }
    T find(const QString& key) const {
        auto pos = m_map.find(key);
        return pos == m_map.end() ? nullptr : pos.value(); // here std::map and QMap differ
    }
    T find_or_fail(const QString& key) const {
        T ret = find(key);
        if (!ret)
            THROW("Cannot find '" + key + "' in function registry");
        return ret;
    }
    QString key_at(const unsigned int idx) const {
        return idx>=m_keys.size() ? "" : m_keys[idx];
    }
    QString key_at_or_fail(const unsigned int idx) const {
        QString ret = key_at(idx);
        if (ret=="")
            THROW("Invalid index in registry lookup");
        return ret;
    }
    unsigned int size() const { return m_keys.size(); }
    QStringList keys() const { return m_keys; }
};

#endif // REGISTRY_H
