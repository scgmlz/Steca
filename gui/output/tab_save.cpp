// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/tab_save.cpp
//! @brief     Implements classes Params, TabSave
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/output/tab_save.h"
#include "gui/output/dialog_panels.h"
#include "gui/base/filedialog.h"
#include "gui/mainwin.h"
#include <QDir>

static QString const DAT_SFX(".dat"), DAT_SEP(" "), // suffix, separator
    CSV_SFX(".csv"), CSV_SEP(", ");

TabSave::TabSave(bool withTypes)
    : rbDat_("rbDat", DAT_SFX)
    , rbCsv_("rbCsv", CSV_SFX)
{

    setLayout((grid_ = newQ::GridLayout()));
    actBrowse = newQ::Trigger("actBrowse", "Browse...");
    actSave = newQ::Trigger("actSave", "Save");

    QString dir = gGui->saveDir;
    if (!QDir(dir).exists())
        dir = QDir::current().absolutePath();

    auto* gp = new GridPanel("Destination");
    grid_->addWidget(gp, 0, 0);
    QGridLayout* g = gp->grid();

    dir_ = new QLineEdit(dir);
    dir_->setReadOnly(true);

    file_ = new QLineEdit();

    g->addWidget(new QLabel("Save to folder:"), 0, 0, Qt::AlignRight);
    g->addWidget(dir_, 0, 1);
    g->addWidget(new XTextButton(actBrowse), 0, 2);

    g->addWidget(new QLabel("File name:"), 1, 0, Qt::AlignRight);
    g->addWidget(file_, 1, 1);

    connect(actBrowse, &QAction::triggered, [this]() {
        QString dir = file_dialog::saveDirName(this, "Select folder", dir_->text());
        if (!dir.isEmpty())
            dir_->setText((gGui->saveDir = dir));
    });

    gp = new GridPanel("File type");
    grid_->addWidget(gp, 0, 1);
    g = gp->grid();

    g->addWidget(&rbDat_, 0, 0);
    g->addWidget(&rbCsv_, 1, 0);

    connect(&rbDat_, &QRadioButton::clicked, [this]() { gGui->saveFmt = DAT_SFX; });
    connect(&rbCsv_, &QRadioButton::clicked, [this]() { gGui->saveFmt = CSV_SFX; });

    (CSV_SFX == gGui->saveFmt ? &rbCsv_ : &rbDat_)->setChecked(true);

    gp->setVisible(withTypes);
}

QString TabSave::filePath(bool withSuffix, bool withNumber) {
    QString dir = dir_->text().trimmed();
    QString fileName = file_->text().trimmed();
    if (dir.isEmpty() || fileName.isEmpty())
        return "";
    if (withNumber && !fileName.contains("%d"))
        fileName += ".%d";
    if (withSuffix) {
        QString suffix = rbDat_.isChecked() ? DAT_SFX : CSV_SFX;
        if ("."+QFileInfo(fileName).suffix()!=suffix)
            fileName += suffix;
    }
    file_->setText(fileName);

    return QFileInfo(dir + '/' + fileName).absoluteFilePath();
}

QString TabSave::separator() const {
    return rbDat_.isChecked() ? DAT_SEP : CSV_SEP;
}
