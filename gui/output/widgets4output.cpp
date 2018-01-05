// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/widgets4output.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "widgets4output.h"
#include "actions.h"
#include "def/idiomatic_for.h"
#include "dialog_panels.h"
#include "popup/filedialog.h"
#include "models.h"
#include "cfg/settings.h"
#include "thehub.h"
#include "widgets/widget_makers.h"
#include <QAction>
#include <QDir>
#include <QHeaderView>


// ************************************************************************** //
//  class Params
// ************************************************************************** //

Params::Params(ePanels panels)
    : panelReflection(nullptr)
    , panelGammaSlices(nullptr)
    , panelGammaRange(nullptr)
    , panelPoints(nullptr)
    , panelInterpolation(nullptr)
    , panelDiagram(nullptr) {
    debug::ensure(panels & GAMMA);

    setLayout((box_ = wmaker::newBoxLayout(Qt::Horizontal)));

    if (REFLECTION & panels)
        box_->addWidget((panelReflection = new PanelReflection()));

    if (GAMMA & panels) {
        box_->addWidget((panelGammaSlices = new PanelGammaSlices()));
        box_->addWidget((panelGammaRange = new PanelGammaRange()));
    }

    if (POINTS & panels)
        box_->addWidget((panelPoints = new PanelPoints()));

    if (INTERPOLATION & panels)
        box_->addWidget((panelInterpolation = new PanelInterpolation()));

    if (DIAGRAM & panels)
        box_->addWidget((panelDiagram = new PanelDiagram()));

    box_->addStretch();

    readSettings();
}

Params::~Params() {
    saveSettings();
}

void Params::readSettings() {
    Settings s("output");

    if (panelGammaSlices)
        panelGammaSlices->numSlices->setValue(s.readInt("num slices", 0));

    if (panelGammaRange)
        panelGammaRange->cbLimitGamma->setChecked(s.readBool("limit gamma", false));

    if (panelPoints)
        (s.readBool("interpolated", false) ? panelPoints->rbInterp : panelPoints->rbCalc)
            ->setChecked(true);

    if (panelInterpolation) {
        panelInterpolation->stepAlpha->setValue(s.readReal("step alpha", 5));
        panelInterpolation->stepBeta->setValue(s.readReal("step beta", 5));
        panelInterpolation->idwRadius->setValue(s.readReal("idw radius", 10));

        panelInterpolation->avgAlphaMax->setValue(s.readReal("avg alpha max", 15));
        panelInterpolation->avgRadius->setValue(s.readReal("avg radius", 5));
        panelInterpolation->avgThreshold->setValue(s.readInt("avg threshold", 100));
    }

    saveDir = s.readStr("save dir");
    saveFmt = s.readStr("save format");

    if (panelGammaSlices)
        panelGammaSlices->updateValues();

    if (panelGammaRange)
        panelGammaRange->updateValues();
}

void Params::saveSettings() const {
    Settings s("output");

    if (panelGammaSlices)
        s.saveInt("num slices", panelGammaSlices->numSlices->value());

    if (panelGammaRange)
        s.saveBool("limit gamma", panelGammaRange->cbLimitGamma->isChecked());

    if (panelPoints)
        s.saveBool("interpolated", panelPoints->rbInterp->isChecked());

    if (panelInterpolation) {
        s.saveReal("step alpha", panelInterpolation->stepAlpha->value());
        s.saveReal("step beta", panelInterpolation->stepBeta->value());
        s.saveReal("idw radius", panelInterpolation->idwRadius->value());

        s.saveReal("avg alpha max", panelInterpolation->avgAlphaMax->value());
        s.saveReal("avg radius", panelInterpolation->avgRadius->value());
        s.saveInt("avg threshold", panelInterpolation->avgThreshold->value());
    }

    s.saveStr("save dir", saveDir);
    s.saveStr("save format", saveFmt);
}

// ************************************************************************** //
//  class TabularModel
// ************************************************************************** //

class TabularModel : public TableModel {
private:
public:
    TabularModel(uint numCols_);

    int columnCount(rcIndex = ANY_INDEX) const;
    int rowCount(rcIndex = ANY_INDEX) const;

    QVariant data(rcIndex, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    void moveColumn(uint from, uint to);

    void setColumns(QStringList const& headers, cmp_vec const&);

    void setSortColumn(int);

    void clear();
    void addRow(row_t const&, bool sort = true);

    row_t const& row(uint);

    void sortData();

private:
    uint numCols_;
    int sortColumn_;

    QStringList headers_;
    uint_vec colIndexMap_;
    cmp_vec cmpFunctions_;

    struct numRow {
        typedef numRow const& rc;
        numRow() : n(0), row() {}
        numRow(int n_, row_t const& row_) : n(n_), row(row_) {}
        int n;
        row_t row;
    };

    vec<numRow> rows_;
};

TabularModel::TabularModel(uint numColumns_)
    : TableModel(), numCols_(numColumns_), sortColumn_(-1) {
    colIndexMap_.resize(numCols_);
    for_i (numCols_)
        colIndexMap_[i] = i;
}

int TabularModel::columnCount(rcIndex) const {
    return to_i(numCols_) + 1;
}

int TabularModel::rowCount(rcIndex) const {
    return to_i(rows_.count());
}

// The first column contains row numbers. The rest numCols columns contain data.

QVariant TabularModel::data(rcIndex index, int role) const {
    int indexRow = index.row(), indexCol = index.column();
    int numRows = rowCount(), numCols = columnCount();

    if (indexCol < 0 || indexRow < 0)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if (0 == indexCol)
            return rows_.at(to_u(indexRow)).n;

        if (--indexCol < numCols && indexRow < numRows) {
            QVariant var = rows_.at(to_u(indexRow)).row.at(to_u(indexCol));
            if (isNumeric(var) && qIsNaN(var.toDouble()))
                var = QVariant(); // hide nans
            return var;
        }

        break;

    case Qt::TextAlignmentRole:
        if (0 == indexCol)
            return Qt::AlignRight;

        if (--indexCol < numCols && indexRow < numRows) {
            QVariant const& var = rows_.at(to_u(indexRow)).row.at(to_u(indexCol));
            if (isNumeric(var))
                return Qt::AlignRight;
        }

        return Qt::AlignLeft;

    default: break;
    }

    return QVariant();
}

QVariant TabularModel::headerData(int section, Qt::Orientation, int role) const {
    if (section < 0 || headers_.count() < section)
        return QVariant();

    if (Qt::DisplayRole == role)
        return 0 == section ? "#" : headers_.at(to_u(section) - 1);

    return QVariant();
}

void TabularModel::moveColumn(uint from, uint to) {
    debug::ensure(from < colIndexMap_.count() && to < colIndexMap_.count());
    qSwap(colIndexMap_[from], colIndexMap_[to]);
}

void TabularModel::setColumns(QStringList const& headers, cmp_vec const& cmps) {
    debug::ensure(to_u(headers.count()) == numCols_ && cmps.count() == numCols_);
    headers_ = headers;
    cmpFunctions_ = cmps;
}

void TabularModel::setSortColumn(int col) {
    sortColumn_ = col < 0 ? col : colIndexMap_.at(col);
}

void TabularModel::clear() {
    beginResetModel();
    rows_.clear();
    endResetModel();
}

void TabularModel::addRow(row_t const& row, bool sort) {
    rows_.append(numRow(rows_.count() + 1, row));
    if (sort)
        sortData();
}

row_t const& TabularModel::row(uint index) {
    return rows_.at(index).row;
}

void TabularModel::sortData() {
    auto cmpRows = [this](uint col, row_t const& r1, row_t const& r2) {
        col = colIndexMap_.at(col);
        return cmpFunctions_.at(col)(r1.at(col), r2.at(col));
    };

    // sort by sortColumn first, then left-to-right
    auto cmp = [this, cmpRows](numRow const& r1, numRow const& r2) {
        if (0 <= sortColumn_) {
            int c = cmpRows(to_u(sortColumn_), r1.row, r2.row);
            if (c < 0)
                return true;
            if (c > 0)
                return false;
        } else if (-1 == sortColumn_) {
            if (r1.n < r2.n)
                return true;
            if (r1.n > r2.n)
                return false;
        }

        for_int (col, numCols_) {
            if (to_i(col) != sortColumn_) {
                int c = cmpRows(col, r1.row, r2.row);
                if (c < 0)
                    return true;
                if (c > 0)
                    return false;
            }
        }

        return false;
    };

    beginResetModel();
    std::sort(rows_.begin(), rows_.end(), cmp);
    endResetModel();
}

// ************************************************************************** //
//  class Tab
// ************************************************************************** //

OutputTab::OutputTab(Params& params) : params_(params) {
    setLayout((grid_ = wmaker::newGridLayout()));
}

// ************************************************************************** //
//  class Table
// ************************************************************************** //

Table::Table(uint numDataColumns) : model_(nullptr) {
    model_.reset(new TabularModel(numDataColumns));
    setModel(model_.ptr());
    setHeader(new QHeaderView(Qt::Horizontal));

    auto& h = *header();

    h.setSectionResizeMode(0, QHeaderView::Fixed);
    h.setSectionsMovable(true);
    h.setSectionsClickable(true);

    int w = QFontMetrics(h.font()).width("000000000");
    setColumnWidth(0, w);
}

void Table::setColumns(
    QStringList const& headers, QStringList const& outHeaders, cmp_vec const& cmps) {
    model_->setColumns(headers, cmps);
    debug::ensure(headers.count() == outHeaders.count());
    outHeaders_ = outHeaders;

    connect(
        header(), &QHeaderView::sectionMoved,
        [this](int /*logicalIndex*/, int oldVisualIndex, int newVisualIndex) {
            debug::ensure(oldVisualIndex > 0 && newVisualIndex > 0);
            auto& h = *header();
            h.setSortIndicatorShown(false);
            //            model_->setSortColumn(-2);
            model_->moveColumn(to_u(oldVisualIndex - 1), to_u(newVisualIndex - 1));
            model_->sortData();
        });

    connect(header(), &QHeaderView::sectionClicked, [this](int logicalIndex) {
        auto& h = *header();
        h.setSortIndicatorShown(true);
        h.setSortIndicator(logicalIndex, Qt::AscendingOrder);
        model_->setSortColumn(logicalIndex - 1);
        model_->sortData();
    });
}

void Table::clear() {
    model_->clear();
}

void Table::addRow(row_t const& row, bool sort) {
    model_->addRow(row, sort);
}

void Table::sortData() {
    model_->sortData();
}

uint Table::rowCount() const {
    return to_u(model_->rowCount());
}

const row_t& Table::row(uint i) const {
    return model_->row(i);
}

static str const DAT_SFX(".dat"), DAT_SEP(" "), // suffix, separator
    CSV_SFX(".csv"), CSV_SEP(", ");

TabSave::TabSave(Params& params, bool withTypes) : OutputTab(params) {
    actBrowse = newTrigger("Browse...");
    actSave = newTrigger("Save");

    str dir = params_.saveDir;
    if (!QDir(dir).exists())
        dir = QDir::current().absolutePath();

    auto gp = new GridPanel("Destination");
    grid_->addWidget(gp, 0, 0);
    auto g = gp->grid();

    dir_ = new QLineEdit(dir);
    dir_->setReadOnly(true);

    file_ = new QLineEdit();

    g->addWidget(wmaker::newLabel("Save to folder:"), 0, 0, Qt::AlignRight);
    g->addWidget(dir_, 0, 1);
    g->addWidget(wmaker::newTextButton(actBrowse), 0, 2);

    g->addWidget(wmaker::newLabel("File name:"), 1, 0, Qt::AlignRight);
    g->addWidget(file_, 1, 1);

    connect(actBrowse, &QAction::triggered, [this]() {
        str dir = file_dialog::saveDirName(this, "Select folder", dir_->text());
        if (!dir.isEmpty())
            dir_->setText((params_.saveDir = dir));
    });

    gp = new GridPanel("File type");
    grid_->addWidget(gp, 0, 1);
    g = gp->grid();

    g->addWidget((rbDat_ = wmaker::newRadioButton(DAT_SFX)), 0, 0);
    g->addWidget((rbCsv_ = wmaker::newRadioButton(CSV_SFX)), 1, 0);

    connect(rbDat_, &QRadioButton::clicked, [this]() { params_.saveFmt = DAT_SFX; });

    connect(rbCsv_, &QRadioButton::clicked, [this]() { params_.saveFmt = CSV_SFX; });

    (CSV_SFX == params_.saveFmt ? rbCsv_ : rbDat_)->setChecked(true);

    gp->setVisible(withTypes);
}

str TabSave::filePath(bool withSuffix) {
    str dir = dir_->text().trimmed();
    str file = file_->text().trimmed();

    if (dir.isEmpty() || file.isEmpty())
        return "";

    str suffix;
    if (withSuffix)
        suffix = rbDat_->isChecked() ? DAT_SFX : CSV_SFX;

    return QFileInfo(dir + '/' + fileSetSuffix(suffix)).absoluteFilePath();
}

str TabSave::separator() const {
    return rbDat_->isChecked() ? DAT_SEP : CSV_SEP;
}

str TabSave::fileSetSuffix(rcstr suffix) {
    str file = file_->text().trimmed();
    if (!suffix.isEmpty()) {
        file = QFileInfo(file).completeBaseName();
        if (!file.isEmpty())
            file += suffix;
    }

    file_->setText(file);
    return file;
}
