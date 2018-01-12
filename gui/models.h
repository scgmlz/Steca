// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/models.h
//! @brief     Defines classes TableModel, FilesModel, MeasurementsModel, MetadataModel, ReflectionsModel
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MODELS_H
#define MODELS_H

#include "core/data/experiment.h"
#include "core/typ/str.h"
#include "core/typ/vec.h"
#include <QAbstractTableModel>

extern QVariant const EMPTY_VAR;
extern QModelIndex const ANY_INDEX;

//! The base class of all table-like models

class TableModel : public QAbstractTableModel {
public:
    void signalReset(); //!< force-emits reset() signal
};

class FilesModel : public TableModel {
public:
    int columnCount(const QModelIndex& = ANY_INDEX) const;
    int rowCount(const QModelIndex& = ANY_INDEX) const;

    QVariant data(const QModelIndex&, int) const;

public:
    enum { GetFileRole = Qt::UserRole };

    void removeFile(uint i);
};


class MeasurementsModel : public TableModel {
public:
    MeasurementsModel();

    int columnCount(const QModelIndex& = ANY_INDEX) const;
    int rowCount(const QModelIndex& = ANY_INDEX) const;

    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_NUMBER = 1, COL_ATTRS };

public:
    enum { GetMeasurementRole = Qt::UserRole };

    void showMetaInfo(vec<bool> const&);

private:
    Experiment const& experiment_;
    uint_vec metaInfoNums_; // selected metadata items to show
};


class MetadataModel : public TableModel {
public:
    MetadataModel();

    int columnCount(const QModelIndex& = ANY_INDEX) const;
    int rowCount(const QModelIndex& = ANY_INDEX) const;

    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_CHECK = 1, COL_TAG, COL_VALUE, NUM_COLUMNS };

    vec<bool> const& rowsChecked() const { return rowsChecked_; }

    void reset(shp_Suite dataseq);
    void flipCheck(uint row);

private:
    shp_Metadata metadata_;
    vec<bool> rowsChecked_;
};


class ReflectionsModel : public TableModel {
public:
    ReflectionsModel();

    int columnCount(const QModelIndex& = ANY_INDEX) const;
    int rowCount(const QModelIndex& = ANY_INDEX) const;

    str displayData(uint row, uint col) const;
    str displayData(uint row) const;

    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_ID = 1, COL_TYPE, NUM_COLUMNS };

public:
    enum { GetMeasurementRole = Qt::UserRole };

    void addReflection(const QString&);
    void remReflection(uint);
};

#endif // MODELS_H
