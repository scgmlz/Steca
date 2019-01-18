//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/dialog_save.cpp
//! @brief     Implements classes DialogSave, DialogMultisave
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

//  ***********************************************************************************************

DialogSave::DialogSave(
    QWidget* _parent, const QString& _name, const QString& _title, const QStringList& _extensions)
    : QcrDialog(_parent, _name)
{
    // Dialog widget settings

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(_title);
    progressBar.hide();

    // Subwidgets

    pathField = new DialogfieldPath{this};
    auto* fileExtensionGroup = new QButtonGroup;
    auto* ftypeGrid = new QVBoxLayout;
    for (const QString fmt: _extensions) {
        auto* rb = new QcrRadioButton{"fmt."+fmt, "."+fmt};
        rb->programaticallySetValue(saveFmt == fmt);
        connect(rb, &QRadioButton::clicked, [this,fmt]() { saveFmt = fmt; });
        fileExtensionGroup->addButton(rb);
        ftypeGrid->addWidget(rb);
    }

    auto* actCancel_ = new QcrTrigger{"cancel", "Cancel"};
    auto* actSave_   = new QcrTrigger{"save", "Save"};

    connect(actCancel_, &QAction::triggered, [this]() { close(); });
    connect(actSave_, &QAction::triggered,
            [this]()->void{
                progressBar.show();
                save();
                close(); });

    auto updateSaveable = [this,actSave_](const QString) {
                              actSave_->setEnabled(!pathField->stem().isEmpty()); };
    updateSaveable("");
    pathField->dirEdit ->setHook(updateSaveable);
    pathField->fileEdit->setHook(updateSaveable);

    // Layout

    auto* ftype = new QGroupBox("File type");
    ftype->setVisible(_extensions.size()>1);
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

    layout = new QVBoxLayout;
    layout->addLayout(setup);
    layout->addLayout(bottom);

    setLayout(layout);
}

void DialogSave::saveCurrent()
{
    const QString name = pathField->stem();
    ASSERT(!name.isEmpty()); // "save" button should be disabled if name is empty
    QFile* file = file_dialog::openFileConfirmOverwrite("file", parentWidget(), name);
    QTextStream stream{file};
    writeCurrent(stream);
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
    QWidget* _parent, const QString& _name, const QString& _title,
    const QStringList& _extensions, const QString& _content)
    : DialogSave(_parent, _name, _title, _extensions)
{
    ASSERT(multiplicity()>0);
    if (multiplicity()==1)
        return; // no multiFileMode menu
    const QStringList saveModes { {"Current "+_content+" only",
                                   "All "+_content+"s in one file",
                                   "All "+_content+"s to numbered files"} };
    auto* saveWhat = new QcrRadioBox{
        "saveMode", "Save What", &currentSaveModeIdx, saveModes, new QVBoxLayout};
    layout->insertWidget(0, saveWhat);
}

void DialogMultisave::save()
{
    switch(currentSaveModeIdx.val()) {
    case 0:
        saveCurrent();
        break;
    case 1:
        saveJointfile();
        break;
    case 2:
        saveMultifile();
        break;
    default:
        qFatal("impossible case");
    }
}

void DialogMultisave::saveJointfile()
{
    const QString name = pathField->stem();
    ASSERT(!name.isEmpty()); // "save" button should be disabled if name is empty
    QFile* file = file_dialog::openFileConfirmOverwrite("file", parentWidget(), name);
    QTextStream stream{file};
    writeJointfile(stream);
}

void DialogMultisave::saveMultifile()
{
    // check whether any of the numbered files already exists
    QStringList paths;
    for (int i=0; i<nClusters; ++i) {
        QString currPath = data_export::numberedFileName(path, i, nClusters+1);
            if (QFile(currPath).exists())
                paths << QFileInfo(currPath).fileName();
        }
        if (paths.size() &&
            !file_dialog::confirmOverwrite(paths.size()>1 ?
                                           "Files exist" : "File exists",
                                           parent, paths.join(", ")))
            return;
    }
}

/*
void ExportDfgram::saveAll(QFile* file, const QString& format, ExportDfgram* parent)
{
    // In one-file mode, start output stream; in multi-file mode, only do prepations.
    const QString path = parent->fileField_->path(true, !file);
    if (path.isEmpty())
        return;
    QTextStream* stream = nullptr;
    int nClusters = gSession->activeClusters.size();
    ASSERT(nClusters>0);
    if (file) {
        stream = new QTextStream(file);
    } else {
    TakesLongTime progress("save diffractograms", nClusters, &parent->fileField_->progressBar);
    int picNum = 0;
    int fileNum = 0;
    int nSlices = gSession->gammaSelection.numSlices.val();
    const QString separator = data_export::separator(format);
    for (const Cluster* cluster : gSession->activeClusters.clusters.yield()) {
        ++picNum;
        progress.step();
        for (int i=0; i<qMax(1,nSlices); ++i) {
            if (!file) {
                QFile* file = new QFile(data_export::numberedFileName(
                                            path, ++fileNum, nClusters+1));
                if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
                    THROW("Cannot open file for writing: " + path);
                delete stream;
                stream = new QTextStream(file);
            }
            ASSERT(stream);
            const Range gmaStripe = gSession->gammaSelection.slice2range(cluster->rgeGma(), i);
            const Curve& curve = cluster->dfgrams.yield_at(i,cluster).curve;
            *stream << "Picture Nr: " << picNum << '\n';
            if (nSlices > 1)
                *stream << "Gamma slice Nr: " << i+1 << '\n';
            data_export::writeCurve(*stream, curve, cluster, gmaStripe, separator);
        }
    }
    delete stream;
}
*/
