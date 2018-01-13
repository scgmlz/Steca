// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/widgets4output.cpp
//! @brief     Implements classes Params, TabSave
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/output/widgets4output.h"
#include "gui/output/dialog_panels.h"
#include "gui/popup/filedialog.h"
#include "gui/thehub.h"
#include <QDir>


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

    setLayout((box_ = newQ::BoxLayout(Qt::Horizontal)));

    if (REFLECTION & panels)
        box_->addWidget((panelReflection = new PanelReflection()));

    debug::ensure(panels & GAMMA);
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

    if (panelGammaSlices)
        panelGammaSlices->updateValues();

    if (panelGammaRange)
        panelGammaRange->updateValues();
}


// ************************************************************************** //
//  class TabSave
// ************************************************************************** //

static str const DAT_SFX(".dat"), DAT_SEP(" "), // suffix, separator
    CSV_SFX(".csv"), CSV_SEP(", ");

TabSave::TabSave(bool withTypes) {

    setLayout((grid_ = newQ::GridLayout()));
    actBrowse = newQ::Trigger("Browse...");
    actSave = newQ::Trigger("Save");

    str dir = gHub->saveDir;
    if (!QDir(dir).exists())
        dir = QDir::current().absolutePath();

    auto* gp = new GridPanel("Destination");
    grid_->addWidget(gp, 0, 0);
    QGridLayout* g = gp->grid();

    dir_ = new QLineEdit(dir);
    dir_->setReadOnly(true);

    file_ = new QLineEdit();

    g->addWidget(newQ::Label("Save to folder:"), 0, 0, Qt::AlignRight);
    g->addWidget(dir_, 0, 1);
    g->addWidget(newQ::TextButton(actBrowse), 0, 2);

    g->addWidget(newQ::Label("File name:"), 1, 0, Qt::AlignRight);
    g->addWidget(file_, 1, 1);

    connect(actBrowse, &QAction::triggered, [this]() {
        str dir = file_dialog::saveDirName(this, "Select folder", dir_->text());
        if (!dir.isEmpty())
            dir_->setText((gHub->saveDir = dir));
    });

    gp = new GridPanel("File type");
    grid_->addWidget(gp, 0, 1);
    g = gp->grid();

    g->addWidget((rbDat_ = newQ::RadioButton(DAT_SFX)), 0, 0);
    g->addWidget((rbCsv_ = newQ::RadioButton(CSV_SFX)), 1, 0);

    connect(rbDat_, &QRadioButton::clicked, [this]() { gHub->saveFmt = DAT_SFX; });

    connect(rbCsv_, &QRadioButton::clicked, [this]() { gHub->saveFmt = CSV_SFX; });

    (CSV_SFX == gHub->saveFmt ? rbCsv_ : rbDat_)->setChecked(true);

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
