//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/dialog_save.cpp
//! @brief     Implements classes DialogSave
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/dialog_save.h"
#include "gui/dialogs/file_dialog.h"
//#include "qcr/base/debug.h"
#include <QGroupBox>

//  ***********************************************************************************************

//! A box with widgets to set the export directory and file name.

class DialogfieldPath : public QGroupBox {
public:
    DialogfieldPath() = delete;
    DialogfieldPath(QcrDialog* _parent);
    DialogfieldPath(const DialogfieldPath&) = delete;
    QString stem();
    QFile* file();
    QcrLineEdit* dirEdit;
    QcrLineEdit* fileEdit;
private:
    QcrDialog* parent;
};

DialogfieldPath::DialogfieldPath(QcrDialog* _parent)
    : QGroupBox{"ExportPath"}
    , parent{_parent}
{
    static QDir defaultDir = QDir::homePath();
    dirEdit = new QcrLineEdit("dir", defaultDir.absolutePath());
    dirEdit->setReadOnly(true);

    fileEdit = new QcrLineEdit("file");

    auto* actBrowse_ = new QcrTrigger{"selectDir", "Browse..."};
    connect(actBrowse_, &QAction::triggered, [this]() {
            dirEdit->programaticallySetValue(
                file_dialog::queryDirectory(
                    parent, "Select folder", dirEdit->text())); });

    auto* grid = new QGridLayout;
    grid->addWidget(new QLabel("Save to folder:"),       0, 0, Qt::AlignRight);
    grid->addWidget(dirEdit,                             0, 1);
    grid->addWidget(new QcrTextTriggerButton(actBrowse_),0, 2);
    grid->addWidget(new QLabel("File name:"),            1, 0, Qt::AlignRight);
    grid->addWidget(fileEdit,                            1, 1);

    setLayout(grid);
}

QString DialogfieldPath::stem()
{
    const QString dir = dirEdit->text().trimmed();
    const QString ret = fileEdit->text().trimmed();
    if (dir.isEmpty() || ret.isEmpty())
        return "";
    return ret;
}

QFile* DialogfieldPath::file()
{
    const QString tmp = stem();
    if (tmp.isEmpty())
        return nullptr;
    return file_dialog::openFileConfirmOverwrite("file", parentWidget(), tmp);
}

//  ***********************************************************************************************

DialogSave::DialogSave(
    QcrDialog* _parent, QStringList extensions,
    std::function<void(QFile* file, const QString& format, QcrDialog* parent)> _onSave)
    : parent{_parent}
    , onSave{_onSave}
{
    progressBar.hide();

    // Widgets

    pathField = new DialogfieldPath{parent};
    auto* fileExtensionGroup = new QButtonGroup;
    auto* ftypeGrid = new QVBoxLayout;
    for (const QString fmt: extensions) {
        auto* rb = new QcrRadioButton{"fmt."+fmt, "."+fmt};
        rb->programaticallySetValue(saveFmt == fmt);
        connect(rb, &QRadioButton::clicked, [this,fmt]() { saveFmt = fmt; });
        fileExtensionGroup->addButton(rb);
        ftypeGrid->addWidget(rb);
    }

    auto* actCancel_ = new QcrTrigger{"cancel", "Cancel"};
    auto* actSave_   = new QcrTrigger{"save", "Save"};

    connect(actCancel_, &QAction::triggered, [this]() { parent->close(); });
    connect(actSave_, &QAction::triggered,
            [this]()->void{
                progressBar.show();
                onSave(pathField->file(), format(), parent);
                parent->close(); });

    auto updateSaveable = [this,actSave_](const QString) {
                              actSave_->setEnabled(!pathField->stem().isEmpty()); };
    updateSaveable("");
    pathField->dirEdit ->setHook(updateSaveable);
    pathField->fileEdit->setHook(updateSaveable);

    // Layout

    auto* ftype = new QGroupBox("File type");
    ftype->setVisible(extensions.size()>1);
    ftype->setLayout(ftypeGrid);

    auto* setup = new QHBoxLayout;
    setup->addWidget(pathField);
    setup->addWidget(ftype);

    auto* bottom = new QHBoxLayout();
    bottom->addWidget(&progressBar);
    bottom->setStretchFactor(&progressBar, 333);
    bottom->addStretch(1);
    bottom->addWidget(new QcrTextTriggerButton(actCancel_));
    bottom->addWidget(new QcrTextTriggerButton(actSave_));

    addLayout(setup);
    addLayout(bottom);
}

QString DialogSave::path(bool withSuffix, bool withNumber)
{
    QString fileName = pathField->stem();
    if (withNumber && !fileName.contains("%d"))
        fileName += ".%d";
    if (withSuffix) {
        QString suffix = saveFmt;
        if (QFileInfo(fileName).suffix().toLower()!=saveFmt.toLower())
            fileName += "."+saveFmt;
    }
    qDebug() << "return file name " << fileName;

    return QFileInfo(pathField->dirEdit->text() + '/' + fileName).absoluteFilePath();
}

//  ***********************************************************************************************

DialogMultisave::DialogMultisave(
    QcrDialog* _parent, QStringList _extensions,
    std::function<void(QFile* file, const QString& format, QcrDialog* parent)> _onSave,
    const QString& content, const bool _haveMulti)
    : DialogSave(_parent, _extensions, _onSave)
{
    if (!_haveMulti) // only one file available => no multiFileMode menu
        return;
    const QStringList saveModes { {"Current "+content+" only",
                                   "All "+content+"s in one file",
                                   "All "+content+"s to numbered files"} };
    auto* saveWhat = new QcrRadioBox{
        "saveMode", "Save What", &currentSaveModeIdx, saveModes, new QVBoxLayout};
    insertWidget(0, saveWhat);
}
