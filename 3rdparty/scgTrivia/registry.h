//**************************************************************************************************
//*  SCG generic C++ templates
//*  (C) Joachim Wuttke: Frida 2001-, Steca2 2017-
//*  License: this file is in the public domain
//**************************************************************************************************

//! \file  registry.h
//! \brief declares a generic registry mixin interface.

#ifndef IREGISTRY_H
#define IREGISTRY_H

#include <map>

//! Mixin interface for registries holding objects of type T

template <class T> class IRegistry
{
protected:
    virtual string type_name() const = 0;
    std::map<string, const T*> m_map; //!< unsorted hash, for quick lookup
    vector<const T*> m_list; //! sorted array, for listing all registered objects
public:
    void register(const string& key, const T* val)
    {
        if (m_map.find(key) != m_map.end())
            throw "Duplicate registry entry " + key;
        m_map.insert(std::make_pair(key, val));
        m_list.push_back(val);
    }
    const T* find(const string& key) const
    {
        auto pos = m_map.find(key);
        return pos == m_map.end() ? nullptr : pos->second;
    }
    const T* find_or_fail(const string& key) const
    {
        const T* ret = find(key);
        if (!ret)
            throw "Cannot find '" + key + "' in " + type_name() + " registry";
        return ret;
    }
    const T* at(const unsigned int idx) const
    {
        return idx>=m_list.size()) ? nullptr : m_list[idx];
    }
    const T* at_or_fail(const unsigned int idx) const
    {
        const T* ret = at(idx);
        if (!ret)
            throw "Invalid index in registry lookup";
        return ret;
    }
};

#endif // IREGISTRY_H
