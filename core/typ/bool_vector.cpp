//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/bool_vector.cpp
//! @brief     Implements class BoolVector
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/typ/bool_vector.h"
#include "qcr/base/debug.h"

namespace {

std::vector<int> computeList(const BoolVector* ms)
{
    std::vector<int> ret;
    for (int i=0; i<ms->vec.size(); ++i)
        if (ms->vec[i].val())
            ret.push_back(i);
    return ret;
}

} // namespace

BoolVector::BoolVector()
    : list{&computeList}
{}

void BoolVector::set(int idx, bool on)
{
    vec[idx].setVal(on);
    list.invalidate();
}


void BoolMap::replaceKeys(const QStringList& keys, bool on)
{
    availableKeys_ = keys;
    for (const QString& key: availableKeys_) {
        auto d = data.find(key);
        if (d==data.end())
            data.emplace(key, on);
    }
}

void BoolMap::set(const QString& key, bool on)
{
    ASSERT(availableKeys_.contains(key));
    auto d = data.find(key);
    if (d!=data.end())
        d->second.pureSetVal(on);
    else
        data.emplace(key, on);
}

void BoolMap::setAll(bool on)
{
    for(auto& d: data)
        d.second.pureSetVal(on);
}

QcrCell<bool>* BoolMap::cellAt(const QString& key)
{
    return &data.at(key);
}

QStringList BoolMap::selectedKeys() const
{
    QStringList ret;
    for (const QString& key: availableKeys_)
        if (data.at(key).val())
            ret.append(key);
    return ret;
}
