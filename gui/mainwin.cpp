// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/mainwin.cpp
//! @brief     Implements class MainWin
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "mainwin.h"
#include "../manifest.h"
#include "core/session.h"
#include "gui/actions/menus.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/capture_and_replay/console.h"
#include "gui/panels/subframe_diffractogram.h"
#include "gui/panels/subframe_files.h"
#include "gui/panels/subframe_image.h"
#include "gui/panels/subframe_clusters.h"
#include "gui/panels/subframe_metadata.h"
#include "gui/panels/subframe_setup.h"
#include "gui/base/filedialog.h"
#include <QApplication>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSplitter>
#include <QStatusBar>
#include <QStringBuilder> // for ".." % ..

MainWin* gGui; //!< global pointer to _the_ main window

// ************************************************************************** //
//  class MainWin
// ************************************************************************** //

MainWin::MainWin()
    : isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
    , settings_("main_settings")
{
    gSession = Session::instance();
    gConsole = Console::instance();
    gGui = this;

    triggers = new Triggers();
    toggles = new Toggles();
    menus = new Menus(menuBar());

    setWindowIcon(QIcon(":/icon/retroStier"));
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    // connect signals
    QObject::connect(gSession, &Session::sigFiles, [this]() {
            updateActionEnabling();
        });
    QObject::connect(gSession, &Session::sigCorr, [this]() {
            bool hasCorr = gSession->hasCorrFile();
            triggers->corrFile.setIcon(QIcon(hasCorr ? ":/icon/rem" : ":/icon/add"));
            QString text = QString(hasCorr ? "Remove" : "Add") + " correction file";
            triggers->corrFile.setText(text);
            triggers->corrFile.setToolTip(text.toLower());
            toggles->enableCorr.setChecked(gSession->corrset().isEnabled());
            updateActionEnabling();
            emit gSession->sigDiffractogram();
            emit gSession->sigImage();
        });
    QObject::connect(gSession, &Session::sigPeaks, [this]() {
            updateActionEnabling();
            emit gSession->sigDiffractogram();
        });
    QObject::connect(gSession, &Session::sigBaseline, [this]() {
            updateActionEnabling();
            emit gSession->sigDiffractogram();
        });

    initLayout();
    readSettings();
    updateActionEnabling();

    saveDir = settings_.readStr("export_directory");
    saveFmt = settings_.readStr("export_format");
}

MainWin::~MainWin()
{
    settings_.saveStr("export_directory", saveDir);
    settings_.saveStr("export_format", saveFmt);
}


void MainWin::initLayout()
{
    addDockWidget(Qt::LeftDockWidgetArea, (dockFiles_ = new SubframeFiles()));
    addDockWidget(Qt::LeftDockWidgetArea, (dockClusters_ = new SubframeClusters()));
    addDockWidget(Qt::LeftDockWidgetArea, (dockMetadata_ = new SubframeMetadata()));

    auto splMain = new QSplitter(Qt::Vertical);
    splMain->setChildrenCollapsible(false);

    auto splTop = new QSplitter(Qt::Horizontal);
    splTop->setChildrenCollapsible(false);

    setCentralWidget(splMain);

    splMain->addWidget(splTop);
    splMain->addWidget(new SubframeDiffractogram());
    splMain->setStretchFactor(1, 1);

    splTop->addWidget(new SubframeSetup());
    splTop->addWidget(new SubframeImage());
    splTop->setStretchFactor(1, 1);

    statusBar();
}

void MainWin::checkUpdate()
{
    QNetworkRequest req;
    QString ver = qApp->applicationVersion();
    QString qry = ver % "\t| " % QSysInfo::prettyProductName();
    req.setUrl(QUrl(QString(STECA2_VERSION_URL) % "?" % qry));
    QNetworkReply* reply = QNetworkAccessManager().get(req);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (QNetworkReply::NoError != reply->error()) {
            qWarning() << "Network Error: " << reply->errorString();
            return;
        }
        QString ver = qApp->applicationVersion();
        QString lastVer = reply->readAll().trimmed();
        QString name = qApp->applicationName();
        QString result;
        if (ver != lastVer)
            result = QString(
                "<p>The latest released %1 version is %2. You have "
                "version %3.</p>"
                "<p><a href='%4'>Open download location in external browser</a></p>")
                .arg(name, lastVer, ver, STECA2_DOWNLOAD_URL);
        else
            result = QString(
                "<p>You have the latest released %1 version (%2).</p>").arg(name).arg(ver);
        QMessageBox::information(this, QString("%1 update").arg(name), result);
        });
}

void MainWin::addFiles()
{
    QStringList fileNames = file_dialog::openFileNames(this, "Add files", dataDir_, dataFormats_);
    repaint();
    if (fileNames.isEmpty())
        return;
    TakesLongTime __;
    gSession->dataset().addGivenFiles(fileNames);
}

void MainWin::loadSession()
{
    QString fileName = file_dialog::openFileName(
        this, "Load session", sessionDir_, "Session files (*.ste)");
    if (fileName.isEmpty())
        return;
    try {
        TR("going to load session from file '"+fileName+"'");
        sessionFromFile(fileName);
    } catch(Exception& ex) {
        qWarning() << "Could not load session from file " << fileName << ":\n"
                   << ex.msg() << "\n"
                   << "The application may now be in an inconsistent state.\n"
                   << "Please consider to quit the application, and start afresh.\n";
        gSession->clear();
    }
}

void MainWin::saveSession()
{
    QString fileName = file_dialog::saveFileName(
        this, "Save session", sessionDir_, "Session files (*.ste)");
    if (!fileName.endsWith(".ste"))
        fileName += ".ste";
    saveSessionTo(QFileInfo(fileName));
}

void MainWin::closeEvent(QCloseEvent* event)
{
    saveSettings();
    event->accept();
}

void MainWin::readSettings()
{
    if (initialState_.isEmpty())
        initialState_ = saveState();
    Settings s("MainWin");
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("state").toByteArray());
}

void MainWin::saveSettings()
{
    Settings s("MainWin");
    s.setValue("geometry", saveGeometry());
    s.setValue("state", saveState());
}

void MainWin::viewReset()
{
    restoreState(initialState_);
#ifndef Q_OS_OSX
    toggles->fullScreen.setChecked(false);
#endif
    toggles->viewStatusbar.setChecked(true);
    toggles->viewClusters.setChecked(true);
    toggles->viewFiles.setChecked(true);
    toggles->viewMetadata.setChecked(true);
}

void MainWin::saveSessionTo(const QFileInfo& fileInfo)
{
    QFile* file = file_dialog::OutputFile("file", this, fileInfo.filePath());
    if (!file)
        return;
    const int result = file->write(serializeSession());
    if (!(result >= 0)) THROW("Could not write session");
}

QByteArray MainWin::serializeSession() const
{
    QJsonObject top;

    const Geometry& geo = gSession->geometry();
    QJsonObject sub {
        { "distance", QJsonValue(geo.detectorDistance()) },
        { "pixel size", QJsonValue(geo.pixSize()) },
        { "beam offset", geo.midPixOffset().to_json() }
    };
    top.insert("detector", sub);

    const ImageCut& cut = gSession->imageCut();
    sub = {
        { "left", cut.left() },
        { "top", cut.top() },
        { "right", cut.right() },
        { "bottom", cut.bottom() } };
    top.insert("cut", sub);

    const ImageTransform& trn = gSession->imageTransform();
    top.insert("image transform", trn.val);

    top.insert("files", gSession->dataset().to_json());
    top.insert("combine", gSession->dataset().binning());

    if (gSession->hasCorrFile()) {
        QString absPath = gSession->corrset().raw().fileInfo().absoluteFilePath();
        QString relPath = QDir::current().relativeFilePath(absPath);
        top.insert("correction file", relPath);
    }

    // TODO save cluster selection

    top.insert("baseline", gSession->baseline().toJson());
    top.insert("peaks", gSession->peaks().toJson());

    top.insert("averaged intensity ", gSession->intenScaledAvg());
    top.insert("intensity scale", qreal_to_json((qreal)gSession->intenScale()));

    return QJsonDocument(top).toJson();
}

void MainWin::sessionFromFile(const QString& filePath) THROWS {
    QFile file(filePath);
    if (!(file.open(QIODevice::ReadOnly | QIODevice::Text)))
        THROW("Cannot open file for reading: " % filePath);
    QDir::setCurrent(QFileInfo(filePath).absolutePath());
    sessionFromJson(file.readAll());
}

void MainWin::sessionFromJson(const QByteArray& json) THROWS
{
    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(json, &parseError));
    if (!(QJsonParseError::NoError == parseError.error))
        THROW("Error parsing session file");

    TakesLongTime __;

    gSession->clear();
    TR("sessionFromJson: cleared old session");

    JsonObj top(doc.object());

    const QJsonArray& files = top.loadArr("files");
    QStringList paths;
    for (const QJsonValue& file : files) {
        QString filePath = file.toString();
        QDir dir(filePath);
        if(!dir.makeAbsolute())
            THROW("Invalid file path: " + filePath);
        paths.append(dir.absolutePath());
    }
    gSession->dataset().addGivenFiles(paths);

    const QJsonArray& sels = top.loadArr("selected files", true);
    vec<int> selIndexes;
    for (const QJsonValue& sel : sels) {
        int i = sel.toInt();
        int index = qBound(0, i, files.count());
        if(i != index)
            THROW(QString("Invalid selection index: %1").arg(i));
        selIndexes.append(index);
    }

    std::sort(selIndexes.begin(), selIndexes.end());
    int lastIndex = -1;
    for (int index : selIndexes) {
        if (index >= lastIndex)
            THROW("Duplicate selection index");
        lastIndex = index;
    }

    TR("sessionFromJson: going to collect cluster");
    gSession->dataset().setBinning(top.loadPint("combine", 1));

    TR("sessionFromJson: going to set correction file");
    gSession->corrset().loadFile(top.loadString("correction file", ""));

    TR("sessionFromJson: going to load detector geometry");
    const JsonObj& det = top.loadObj("detector");
    gSession->geometry().setDetectorDistance(det.loadPreal("distance"));
    gSession->geometry().setPixSize(det.loadPreal("pixel size"));
    gSession->geometry().setOffset(det.loadIJ("beam offset"));

    TR("sessionFromJson: going to load image cut");
    const JsonObj& cut = top.loadObj("cut");
    int x1 = cut.loadUint("left"), y1 = cut.loadUint("top"),
         x2 = cut.loadUint("right"), y2 = cut.loadUint("bottom");
    gSession->imageCut().setLinked(false);
    gSession->imageCut().setLeft(x1);
    gSession->imageCut().setRight(x2);
    gSession->imageCut().setTop(y1);
    gSession->imageCut().setBottom(y2);
    setImageRotate(ImageTransform(top.loadUint("image transform")));

    TR("sessionFromJson: going to load fit setup");
    gSession->baseline().fromJson(top.loadObj("baseline"));

    bool arg1 = top.loadBool("averaged intensity ", true);
    qreal arg2 = top.loadPreal("intensity scale", 1);
    gSession->setIntenScaleAvg(arg1, arg2);

    TR("sessionFromJson: going to load peaks info");
    const QJsonArray& peaksInfo = top.loadArr("peaks");
    for_i (peaksInfo.count())
        gSession->peaks().add(peaksInfo.at(i).toObject());

    TR("installed session from file");
}

void MainWin::loadCorrFile()
{
    if (gSession->corrset().hasFile()) {
        gSession->corrset().removeFile();
    } else {
        QString fileName = file_dialog::openFileName(
            this, "Set correction file", dataDir_, dataFormats_);
        if (fileName.isEmpty())
            return;
        gSession->corrset().loadFile(fileName);
    }
}

void MainWin::setImageRotate(ImageTransform rot)
{
    const char* rotateIconFile;
    const char* mirrorIconFile;

    switch (rot.val & 3) {
    default /* case */:
        rotateIconFile = ":/icon/rotate0";
        mirrorIconFile = ":/icon/mirrorHorz";
        break;
    case 1:
        rotateIconFile = ":/icon/rotate1";
        mirrorIconFile = ":/icon/mirrorVert";
        break;
    case 2:
        rotateIconFile = ":/icon/rotate2";
        mirrorIconFile = ":/icon/mirrorHorz";
        break;
    case 3:
        rotateIconFile = ":/icon/rotate3";
        mirrorIconFile = ":/icon/mirrorVert";
        break;
    }

    triggers->rotateImage.setIcon(QIcon(rotateIconFile));
    toggles->mirrorImage.setIcon(QIcon(mirrorIconFile));
    gSession->setImageTransformRotate(rot);
    // TODO gSession->imageCut().prevent_invalid_cuts()
    emit gSession->sigDetector();
}

void MainWin::setImageMirror(bool on)
{
    toggles->mirrorImage.setChecked(on);
    gSession->setImageTransformMirror(on);
    emit gSession->sigDetector();
}

void MainWin::updateActionEnabling()
{
    bool hasFile = gSession->dataset().countFiles();
    bool hasCorr = gSession->hasCorrFile();
    bool hasPeak = gSession->peaks().count();
    bool hasBase = gSession->baseline().ranges().count();
    toggles->enableCorr.setEnabled(hasCorr);
    triggers->removeFile.setEnabled(hasFile);
    triggers->removePeak.setEnabled(hasPeak);
    triggers->clearBackground.setEnabled(hasBase);
    triggers->outputDiagrams.setEnabled(hasFile && hasPeak);
    triggers->outputDiffractograms.setEnabled(hasFile);
    triggers->outputPolefigures.setEnabled(hasFile && hasPeak);
    menus->dgram_->setEnabled(hasFile);
    menus->image_->setEnabled(hasFile);
    menus->output_->setEnabled(hasFile);
}
