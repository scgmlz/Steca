// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/thehub.cpp
//! @brief     Implements class TheHub
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/thehub.h"
#include "core/session.h"
#include "gui/base/new_q.h"
#include "gui/mainwin.h" // TODO after merge with mainwin: replace gMainWin by this
#include "gui/popup/filedialog.h"
#include <QApplication>
#include <QDir>
#include <QJsonDocument>
#include <QStringBuilder> // for ".." % ..

TheHub* gHub; //!< global, for signalling and command flow

TheHub::TheHub()
    : isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
    , settings_("main_settings")
{
    qDebug() << "TheHub/";

    // create actions

    trigger_about = newQ::Trigger("About " + qApp->applicationName());

    trigger_online = newQ::Trigger("Open docs in external browser");

    trigger_checkUpdate = newQ::Trigger("Check for update");

    trigger_quit = newQ::Trigger("Quit");
    trigger_quit->setShortcut(QKeySequence::Quit);

    toggle_viewStatusbar = newQ::Toggle("Statusbar", true);
    toggle_viewStatusbar->setShortcut(Qt::Key_F12);

    toggle_viewFiles = newQ::Toggle("Files", true);
    toggle_viewFiles->setShortcut(Qt::Key_F8);

    toggle_viewDatasets = newQ::Toggle("Datasets", true);
    toggle_viewDatasets->setShortcut(Qt::Key_F9);

    toggle_viewMetadata = newQ::Toggle("Metadata", true);
    toggle_viewMetadata->setShortcut(Qt::Key_F10);

    trigger_viewReset = newQ::Trigger("Reset");

#ifndef Q_OS_OSX
    toggle_fullScreen = newQ::Toggle("FullScreen", false);
    toggle_fullScreen->setShortcut(Qt::Key_F11);
#endif

    trigger_loadSession = newQ::Trigger("Load session...");

    trigger_saveSession = newQ::Trigger("Save session...");

    trigger_clearSession = newQ::Trigger("Clear session");

    trigger_addFiles = newQ::Trigger("Add files...", ":/icon/add");
    trigger_addFiles->setShortcut(Qt::CTRL | Qt::Key_O);

    trigger_removeFile = newQ::Trigger("Remove highlighted file", ":/icon/rem");
    trigger_removeFile->setShortcut(QKeySequence::Delete);
    trigger_removeFile->setEnabled(false);
    QObject::connect(trigger_removeFile, &QAction::triggered, []() {
            gSession->dataset().removeFile(); });
    QObject::connect(gSession, &Session::sigFiles, [this]() {
            trigger_removeFile->setEnabled(gSession->dataset().countFiles()); });

    trigger_corrFile = newQ::Trigger("Add correction file", ":/icon/add");
    trigger_corrFile->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_O);
    connect(trigger_corrFile, &QAction::triggered, this, &TheHub::loadCorrFile);
    QObject::connect(gSession, &Session::sigCorr, [this]() {
            bool hasFile = gSession->hasCorrFile();
            qDebug() << "on sigCorr " << hasFile;
            trigger_corrFile->setIcon(QIcon(hasFile ? ":/icon/rem" : ":/icon/add"));
            QString text = QString(hasFile ? "Remove" : "Add") + " correction file";
            trigger_corrFile->setText(text);
            trigger_corrFile->setToolTip(text.toLower());
        });

    toggle_enableCorr = newQ::Toggle("Enable correction file", false, ":/icon/useCorrection");
    connect(toggle_enableCorr, &QAction::toggled, [this](bool on) {
            gSession->corrset().tryEnable(on); });
    QObject::connect(gSession, &Session::sigCorr, [this]() {
            toggle_enableCorr->setEnabled(gSession->hasCorrFile());
            toggle_enableCorr->setChecked(gSession->corrset().isEnabled());
        });

    trigger_rotateImage = newQ::Trigger("Rotate", ":/icon/rotate0");
    trigger_rotateImage->setShortcut(Qt::CTRL | Qt::Key_R);
    connect(trigger_rotateImage, &QAction::triggered, [this]() {
        setImageRotate(gSession->imageTransform().nextRotate()); });

    toggle_mirrorImage = newQ::Toggle("Mirror", false, ":/icon/mirrorHorz");
    connect(toggle_mirrorImage, &QAction::toggled, [this](bool on) { setImageMirror(on); });

    toggle_linkCuts = newQ::Toggle("Link cuts", false, ":/icon/link");

    toggle_showOverlay = newQ::Toggle("Show overlay", false, ":/icon/crop");

    toggle_stepScale = newQ::Toggle("Scale in steps", false, ":/icon/steps");

    toggle_showBins = newQ::Toggle("Show bins", false, ":/icon/angle");

    toggle_fixedIntenImage = newQ::Toggle("Global intensity scale", false, ":/icon/scale");
    connect(toggle_fixedIntenImage, &QAction::toggled, [this](bool on) {
        isFixedIntenImageScale_ = on;
        emit sigDisplayChanged(); });

    toggle_fixedIntenDgram = newQ::Toggle("Fixed intensity scale", false);
    connect(toggle_fixedIntenDgram, &QAction::toggled, [this](bool on) {
        isFixedIntenDgramScale_ = on;
        emit sigDisplayChanged(); });

    toggle_combinedDgram = newQ::Toggle("All measurements", true);
    toggle_combinedDgram->setChecked(false);
    connect(toggle_combinedDgram, &QAction::toggled, [this](bool on) {
        isCombinedDgram_ = on;
        emit sigDisplayChanged(); });

    toggle_selRegions = newQ::Toggle("Select regions", false, ":/icon/selRegion");

    toggle_showBackground = newQ::Toggle("Show fitted background", false, ":/icon/showBackground");

    trigger_clearBackground = newQ::Trigger("Clear background regions", ":/icon/clear");
    connect(trigger_clearBackground, &QAction::triggered, [this]() {
            gSession->baseline().setRanges({}); });

    trigger_clearPeaks = newQ::Trigger("Clear peaks", ":/icon/clear");

    trigger_addPeak = newQ::Trigger("Add peak", ":/icon/add");

    trigger_removePeak = newQ::Trigger("Remove peak", ":/icon/rem");
    trigger_removePeak->setEnabled(false);
    QObject::connect(gSession, &Session::sigPeaks, [this]() {
            trigger_removePeak->setEnabled(gSession->peaks().count()); });

    trigger_outputPolefigures = newQ::Trigger("Pole figures...");

    trigger_outputDiagrams = newQ::Trigger("Diagrams...");

    trigger_outputDiffractograms = newQ::Trigger("Diffractograms...");

    saveDir = settings_.readStr("export_directory");
    saveFmt = settings_.readStr("export_format");

    qDebug() << "/TheHub";
}

TheHub::~TheHub() {
    settings_.saveStr("export_directory", saveDir);
    settings_.saveStr("export_format", saveFmt);
}

void TheHub::saveSession(QFileInfo const& fileInfo) const {
    QFile* file = newQ::OutputFile(fileInfo.filePath());
    if (!file)
        return;
    QDir::setCurrent(fileInfo.absolutePath());
    const int result = file->write(saveSession());
    if (!(result >= 0)) THROW("Could not write session");
}

QByteArray TheHub::saveSession() const {

    QJsonObject top;

    const Geometry& geo = gSession->geometry();
    QJsonObject sub {
        { "distance", QJsonValue(geo.detectorDistance) },
        { "pixel size", QJsonValue(geo.pixSize) },
        { "beam offset", geo.midPixOffset.to_json() }
    };
    top.insert("detector", sub);

    const ImageCut& cut = gSession->imageCut();
    sub = {
        { "left", cut.left },
        { "top", cut.top },
        { "right", cut.right },
        { "bottom", cut.bottom } };
    top.insert("cut", sub);

    const ImageTransform& trn = gSession->imageTransform();
    top.insert("image transform", trn.val);

    top.insert("files", gSession->dataset().to_json());
    top.insert("combine", gSession->dataset().binning());

    if (gSession->hasCorrFile()) {
        str absPath = gSession->corrset().raw().fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        top.insert("correction file", relPath);
    }

    // TODO save cluster selection

    top.insert("baseline", gSession->baseline().toJson());
    top.insert("peaks", gSession->peaks().toJson());

    top.insert("averaged intensity ", gSession->intenScaledAvg());
    top.insert("intensity scale", qreal_to_json((qreal)gSession->intenScale()));

    return QJsonDocument(top).toJson();
}

void TheHub::sessionFromFile(rcstr filePath) THROWS {
    QFile file(filePath);
    if (!(file.open(QIODevice::ReadOnly | QIODevice::Text)))
        THROW("Cannot open file for reading: " % filePath);
    QDir::setCurrent(QFileInfo(filePath).absolutePath());
    sessionFromJson(file.readAll());
}

void TheHub::sessionFromJson(QByteArray const& json) THROWS {
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
        str filePath = file.toString();
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
            THROW(str("Invalid selection index: %1").arg(i));
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
    gSession->setGeometry(
        det.loadPreal("distance"), det.loadPreal("pixel size"), det.loadIJ("beam offset"));

    TR("sessionFromJson: going to load image cut");
    const JsonObj& cut = top.loadObj("cut");
    int x1 = cut.loadUint("left"), y1 = cut.loadUint("top"),
         x2 = cut.loadUint("right"), y2 = cut.loadUint("bottom");
    gSession->setImageCut(true, false, ImageCut(x1, y1, x2, y2));
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

void TheHub::loadCorrFile() {
    if (gSession->corrset().hasFile()) {
        gSession->corrset().removeFile();
    } else {
        QString fileName = file_dialog::openFileName(
            gMainWin, "Set correction file", QDir::current().absolutePath(),
            "Data files (*.dat *.mar*);;All files (*.*)");
        if (fileName.isEmpty())
            return;
        QDir::setCurrent(QFileInfo(fileName).absolutePath());
        gSession->corrset().loadFile(fileName);
    }
}

void TheHub::setFittingTab(eFittingTab tab) { // TODO rm
    emit sigFittingTab((fittingTab_ = tab));
}

void TheHub::setImageRotate(ImageTransform rot) {
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

    trigger_rotateImage->setIcon(QIcon(rotateIconFile));
    toggle_mirrorImage->setIcon(QIcon(mirrorIconFile));
    gSession->setImageTransformRotate(rot);
    gSession->setImageCut(true, false, gSession->imageCut());
}

void TheHub::setImageMirror(bool on) {
    toggle_mirrorImage->setChecked(on);
    gSession->setImageTransformMirror(on);
    emit gSession->sigDetector();
}
