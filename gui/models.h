// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/models.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MODELS_H
#define MODELS_H

#include "fit/fit_fun.h"
#include "typ/typ_strlst.h"
#include "types/type_models.h"

class TreeView;

namespace models {

class FilesModel : public TableModel {
    CLASS(FilesModel) SUPER(TableModel) public : FilesModel(gui::TheHub&);

    int columnCount(rcIndex = ANY_INDEX) const;
    int rowCount(rcIndex = ANY_INDEX) const;

    QVariant data(rcIndex, int) const;

public:
    enum { GetFileRole = Qt::UserRole };

    void remFile(uint i);
};


class DatasetsModel : public TableModel {
    CLASS(DatasetsModel) SUPER(TableModel) public : DatasetsModel(gui::TheHub&);

    int columnCount(rcIndex = ANY_INDEX) const;
    int rowCount(rcIndex = ANY_INDEX) const;

    QVariant data(rcIndex, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_NUMBER = DCOL, COL_ATTRS };

public:
    enum { GetDatasetRole = Qt::UserRole };

    void showMetaInfo(typ::vec<bool> const&);

private:
    data::Datasets::rc datasets_; // the selected datasets
    uint_vec metaInfoNums_; // selected metadata items to show
};


class MetadataModel : public TableModel {
    CLASS(MetadataModel) SUPER(TableModel) public : MetadataModel(gui::TheHub&);

    int columnCount(rcIndex = ANY_INDEX) const;
    int rowCount(rcIndex = ANY_INDEX) const;

    QVariant data(rcIndex, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_CHECK = DCOL, COL_TAG, COL_VALUE, NUM_COLUMNS };

    typ::vec<bool> const& rowsChecked() const { return rowsChecked_; }

    void flipCheck(uint row);

private:
    data::shp_Metadata metadata_;
    typ::vec<bool> rowsChecked_;
};


class ReflectionsModel : public TableModel {
    CLASS(ReflectionsModel)
    SUPER(TableModel) public : ReflectionsModel(gui::TheHub&);

    int columnCount(rcIndex = ANY_INDEX) const;
    int rowCount(rcIndex = ANY_INDEX) const;

    str displayData(uint row, uint col) const;
    str displayData(uint row) const;

    QVariant data(rcIndex, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_ID = DCOL, COL_TYPE, NUM_COLUMNS };

public:
    enum { GetDatasetRole = Qt::UserRole };

    void addReflection(fit::ePeakType);
    void remReflection(uint);

    str_lst names() const;
};


}
#endif // MODELS_H
