//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/dataset.cpp
//! @brief     Implements class Dataset
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/data/cluster.h"
#include "core/loaders/loaders.h"
#include "core/session.h"
#include "qcr/engine/mixin.h" // remakeAll
#include "qcr/base/debug.h" // ASSERT

//  ***********************************************************************************************
//! @class Datafile

Qt::CheckState Datafile::activated() const
{
    bool allActivated = true;
    bool noneActivated = true;
    for (const Cluster* cluster : clusters_) {
        if (cluster->isActivated())
            noneActivated = false;
        else
            allActivated = false;
    }
    if (allActivated)
        return Qt::Checked;
    else if (noneActivated)
        return Qt::Unchecked;
    return Qt::PartiallyChecked;
}

//  ***********************************************************************************************
//! @class HighlightedData

void HighlightedData::clear()
{
    current_ = nullptr;
}

void HighlightedData::setFile(int i)
{
    if (i<0)
        return clear();
    ASSERT(i<gSession->dataset.countFiles());
    setCluster(gSession->dataset.fileAt(i).clusters_[0]->index());
    ASSERT(i==current_->file().index_);
}

void HighlightedData::setCluster(int i)
{
    if (i<0)
        return clear();
    ASSERT(i<gSession->dataset.allClusters.size());
    current_ = gSession->dataset.allClusters.at(i).get();
}

void HighlightedData::reset()
{
    if (!gSession->dataset.allClusters.size())
        return clear();
    setCluster(0);
}


//  ***********************************************************************************************
//! @class Dataset

Dataset::Dataset() {
    binning.       setHook( [this](int)  { onClusteringChanged(); } );
    dropIncomplete.setHook( [this](bool) { onClusteringChanged(); } );
}

void Dataset::clear()
{
    //qDebug() << "Dataset::clear";
    highlight_.clear();
    files_.clear();
    onFileChanged();
    gSession->updateImageSize();
    gSession->params.imageCut.clear();
    gRoot->remakeAll();
    //qDebug() << "Dataset::clear/";
}

void Dataset::removeFile()
{
    const Cluster* cluster = highlight_.cluster();
    if (!cluster)
        return;
    int i = cluster->file().index();
    files_.erase(files_.begin()+i);
    if (files_.empty())
        return clear();
    onFileChanged();
    // reset highlight, which was temporarily unset at the beginning of this function
    if (i<countFiles())
        highlight_.setFile(i);
    else if (i>0)
        highlight_.setFile(i-1);
    else
        qFatal("impossible case in Dataset::removeFile");
    gRoot->remakeAll();
}

void Dataset::addGivenFiles(const QStringList& filePaths)
{
    int i = 0;
    if (const Cluster* cluster = highlight_.cluster())
        i = cluster->file().index();
    highlight_.clear();
    for (const QString& path: filePaths) {
        if (path.isEmpty() || hasFile(path))
            continue;
        Rawfile rawfile { load::loadRawfile(path) };
        gSession->setImageSize(rawfile.imageSize());
        files_.push_back(Datafile {std::move(rawfile)});
        onFileChanged();
    }
    if (countFiles())
        highlight_.setFile( i<0 ? 0 : i );
    gRoot->remakeAll();
}

void Dataset::setClusterActivation(int index, bool on)
{
    allClusters.at(index)->setActivated(on);
    gSession->activeClusters.invalidate();
}

void Dataset::setFileActivation(int index, bool on)
{
    const Datafile& fil = fileAt(index);
    for (Cluster* cluster : fil.clusters_)
        cluster->setActivated(on);
    gSession->activeClusters.invalidate();
}

void Dataset::onFileChanged()
{
    int idx = 0;
    int cnt = 0;
    for (Datafile& file: files_) {
        file.index_ = idx++;
        file.offset_ = cnt;
        cnt += file.numMeasurements();
    }
    updateClusters();
}

void Dataset::onClusteringChanged()
{
    updateClusters();
    highlight_.reset();
}

void Dataset::updateClusters()
{
    allClusters.clear();
    hasIncomplete_ = false;
    for (Datafile& file : files_) {
        file.clusters_.clear();
        for (int i=0; i<file.numMeasurements(); i+=binning.val()) {
            if (i+binning.val()>file.numMeasurements()) {
                hasIncomplete_ = true;
                if (dropIncomplete.val())
                    break;
            }
            std::vector<const Measurement*> group;
            for (int ii=i; ii<file.numMeasurements() && ii<i+binning.val(); ii++)
                group.push_back(file.raw_.measurements().at(ii));
            std::unique_ptr<Cluster> cluster(new Cluster(group, file, allClusters.size(), i));
            file.clusters_.push_back(cluster.get());
            allClusters.push_back(std::move(cluster));
        }
    }
    gSession->activeClusters.invalidate();
}

//! Returns list of activated clusters.
std::vector<const Cluster*> Dataset::activeClustersList() const
{
    std::vector<const Cluster*> ret;
    for (const auto& pCluster : allClusters)
        if (pCluster->isActivated())
            ret.push_back(pCluster.get());
    return ret;
}

QJsonObject Dataset::toJson() const
{
    QJsonObject ret;
    QJsonArray arr;
    for (const Datafile& file : files_)
        arr.append(file.raw_.fileInfo().absoluteFilePath());
    ret.insert("files", arr);
    ret.insert("binning", binning.val());
    return ret;
}

void Dataset::fromJson(const JsonObj& obj)
{
    const QJsonArray& files = obj.loadArr("files");
    QStringList paths;
    for (const QJsonValue& file : files)
        paths.append(file.toString());
    addGivenFiles(paths);
    binning.setVal(obj.loadPint("binning", 1));
}

bool Dataset::hasFile(const QString& fileName) const
{
    QFileInfo fileInfo(fileName);
    for (const Datafile& file : files_)
        if (fileInfo == file.raw_.fileInfo())
            return true;
    return false;
}
