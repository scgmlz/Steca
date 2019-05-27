//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/bool_vector.h
//! @brief     Defines class BoolVector
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef BOOL_VECTOR_H
#define BOOL_VECTOR_H

#include "QCR/engine/cell.h"
#include "core/typ/lazy_data.h"
#include <vector>

//! Holds a vector of bool-valued QcrCells. Used for metadata selection.

class BoolVector {
public:
    BoolVector();
    BoolVector(const BoolVector&) = delete;
    BoolVector(BoolVector&&) = default;

    void set(int, bool);

    bool isSelected(int i) const { return vec.at(i).val(); }
    int numSelected() const { return list.yield(this).size(); }
    int selectedOf(int i) const { return list.yield(this).at(i); }

    std::vector<QcrCell<bool>> vec; //!< true if to be displayed
private:
    //! indices of metadata items selected for display
    // TODO: mv somewhere else, bc it is not BoolVector specific and only used by subframe_clusters:
    lazy_data::Cached<std::vector<int>,const BoolVector*> list;
};

//! Holds a map of bool-valued QcrCells. Used for metadata selection.

class BoolMap {
public:
    BoolMap() {}
    BoolMap(const BoolMap&) = delete;
    BoolMap(BoolMap&&) = default;

    void replaceKeys(const QStringList&, bool on);
    void set(const QString&, bool);
    void set(int i, bool on) { set(availableKeys_.at(i), on); }
    void setAll(bool);
    QcrCell<bool>* cellAt(const QString&);
//    QcrCell<bool>* cellAt(int i) { return cellAt(availableKeys_[i]); }

    bool isSelected(const QString& name) const { return data.at(name).val(); }
    bool isSelected(int i) const { return data.at(availableKeys_.at(i)).val(); }
    int size() const { return availableKeys_.size(); }
    const QString& keyAt(int i) const  { return availableKeys_.at(i); }
    const QStringList& availableKeys() const { return availableKeys_; }
    QStringList selectedKeys() const;
//    int numSelected() const { return list.yield(this).size(); }
//    int selectedOf(int i) const { return list.yield(this).at(i); }

private:
    QStringList availableKeys_;
    std::map<QString,QcrCell<bool>> data; //!< true if to be displayed
};

#endif // BOOL_VECTOR_H
