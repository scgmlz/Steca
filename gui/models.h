// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/models.h
//! @brief     Defines classes FilesModel, DatasetsModel, MetadataModel, ReflectionsModel
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MODELS_H
#define MODELS_H

#include "table_model.h"
#include "data/dataset.h"

namespace models {

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

    void showMetaInfo(typ::vec<bool> const&);

private:
    Datasets const& datasets_; // the selected datasets
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

    typ::vec<bool> const& rowsChecked() const { return rowsChecked_; }

    void reset(shp_Dataset dataset);
    void flipCheck(uint row);

private:
    QSharedPointer<class Metadata const> metadata_;
    typ::vec<bool> rowsChecked_;
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

} // namespace models

#endif // MODELS_H
