// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/models.h
//! @brief     Defines classes TableModel, FilesModel, DatasetsModel, MetadataModel, ReflectionsModel
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MODELS_H
#define MODELS_H

#include "core/data/suite.h"
#include "core/typ/str.h"
#include "core/typ/vec.h"
#include <QAbstractTableModel>
#include <QSharedPointer> // no auto rm

extern QVariant const EMPTY_VAR;
extern QModelIndex const ANY_INDEX;

//! The base class of all table-like models

class TableModel : public QAbstractTableModel {
public:
    using Index = QModelIndex;
    using rcIndex = Index const&;

    TableModel() {}

    void signalReset(); //!< force-emits reset() signal

protected:
    static int const DCOL = 1; //!< the left-most column is hidden
};

class Experiment;
class Metadata;

class FilesModel : public TableModel {
public:
    int columnCount(rcIndex = ANY_INDEX) const;
    int rowCount(rcIndex = ANY_INDEX) const;

    QVariant data(rcIndex, int) const;

public:
    enum { GetFileRole = Qt::UserRole };

    void removeFile(uint i);
};


class DatasetsModel : public TableModel {
public:
    DatasetsModel();

    int columnCount(rcIndex = ANY_INDEX) const;
    int rowCount(rcIndex = ANY_INDEX) const;

    QVariant data(rcIndex, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_NUMBER = DCOL, COL_ATTRS };

public:
    enum { GetDatasetRole = Qt::UserRole };

    void showMetaInfo(vec<bool> const&);

private:
    Experiment const& experiment_;
    uint_vec metaInfoNums_; // selected metadata items to show
};


class MetadataModel : public TableModel {
public:
    MetadataModel();

    int columnCount(rcIndex = ANY_INDEX) const;
    int rowCount(rcIndex = ANY_INDEX) const;

    QVariant data(rcIndex, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_CHECK = DCOL, COL_TAG, COL_VALUE, NUM_COLUMNS };

    vec<bool> const& rowsChecked() const { return rowsChecked_; }

    void reset(shp_Suite dataseq);
    void flipCheck(uint row);

private:
    QSharedPointer<Metadata const> metadata_;
    vec<bool> rowsChecked_;
};


class ReflectionsModel : public TableModel {
public:
    ReflectionsModel();

    int columnCount(rcIndex = ANY_INDEX) const;
    int rowCount(rcIndex = ANY_INDEX) const;

    str displayData(uint row, uint col) const;
    str displayData(uint row) const;

    QVariant data(rcIndex, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_ID = DCOL, COL_TYPE, NUM_COLUMNS };

public:
    enum { GetDatasetRole = Qt::UserRole };

    void addReflection(QString const&);
    void remReflection(uint);

    QStringList names() const;
};

#endif // MODELS_H
