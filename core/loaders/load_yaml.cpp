//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/load_yaml.cpp
//! @brief     Implements function loadYaml
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/loaders/fastyamlloader.h"
#include "core/aux/exception.h"
#include "qcr/base/debug.h"
#include <functional>

namespace  {

using loadYAML::YamlNode;

///  Applies the given function to a range and stores the result in a new std::vector.
///
/// @param  begin First element in the range to transform.
/// @param  end The end itereator.
/// @param  func The transform function of following signature: `Ret fun(const Type &a);`.
/// @return A new std::vector<Ret> with the stored results
template <class InputIt, class Function>
auto transformToVector(InputIt begin, InputIt end , Function func)
{
    std::vector<typename std::result_of<Function(const typename InputIt::value_type&)>::type> ret;
    std::transform(begin, end, std::back_inserter(ret), func);
    return std::move(ret);
}

/// Applies the given function to the elements of a container and stores the result in a new std::vector.
///
/// @param  cont Container whos elements are to be transformed
/// @param  func The transform function of following signature: `Ret fun(const Type &a);`.
/// @return A new std::vector<Ret> with the stored results
template <class Container, class Function>
auto transformToVector(Container cont , Function func)
{
    return transformToVector(cont.begin(), cont.end(), func);
}


void readSample(const YamlNode& node, Metadata& metadata)
{
    if (!node.IsDefined())
        return;
    metadata.motorXT = node["position"]["xt"]["value"].doubleValue(Q_QNAN);
    metadata.motorYT = node["position"]["yt"]["value"].doubleValue(Q_QNAN);
    metadata.motorZT = node["position"]["zt"]["value"].doubleValue(Q_QNAN);
    metadata.motorOmg = node["orientation"]["omgs"]["value"].doubleValue(Q_QNAN);
    metadata.motorTth = node["orientation"]["tths"]["value"].doubleValue(Q_QNAN);
    metadata.motorPhi = node["orientation"]["phis"]["value"].doubleValue(Q_QNAN);
    metadata.motorChi = node["orientation"]["chis"]["value"].doubleValue(Q_QNAN);
}

void readSetup(const YamlNode& node, Metadata& metadata)
{
    if (!node.IsDefined())
        return;
    metadata.motorPST = Q_QNAN; // node["orientation"]["pst"]["value"].doubleValue(Q_QNAN);
    metadata.motorSST = Q_QNAN; // node["orientation"]["sst"]["value"].doubleValue(Q_QNAN);
    metadata.motorOMGM = node["monochromator"]["omgm"]["value"].doubleValue(Q_QNAN);
    metadata.nmT = Q_QNAN;
    metadata.nmTeload = Q_QNAN;
    metadata.nmTepos = Q_QNAN;
    metadata.nmTeext = Q_QNAN;
    metadata.nmXe = Q_QNAN;
    metadata.nmYe = Q_QNAN;
    metadata.nmZe = Q_QNAN;
}

void readSingleScan(const YamlNode& node, Metadata& metadata, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    metadata.time         = node["time"].doubleValue(Q_QNAN);
    metadata.monitorCount = node["monitor"].doubleValue(Q_QNAN);
    const auto image      = node["image"].array2dValue();

    const size2d size(image->width, image->height);

    rawfile.addDataset(std::move(metadata), size, std::move(image->data));
}

void readScans(const YamlNode& node, Metadata& metadata, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;
    if (!node.IsSequence()) {
        THROW("invalid YAML format: 'scans' should be a list, but isn't.")
    }
    for (const YamlNode& innerNode: node) {
        Metadata metadataCopy(std::move(metadata));
        // Copy the QStrings back, because std::move removes them from metadata:
        metadata.date    = metadataCopy.date;
        metadata.comment = metadataCopy.comment;
        readSingleScan(innerNode, metadataCopy, rawfile);
    }
}

void readMeasurement(const YamlNode& node, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    // const auto unique_identifier = node["unique_identifier"].value();
    // const auto number = node["number"].value(); // Integer maybe?

    auto metadata = Metadata();

    metadata.date    = node["history"]["started"].value();
    metadata.comment = node["history"]["scan"].value();

    readSample(node["sample"], metadata);
    readSetup(node["setup"], metadata);
    readScans(node["scan"], metadata, rawfile); // adds the scans to the rawfile
}

} // namespace


namespace load {

Rawfile loadYaml(const QString& filePath)
{
    try {
        qDebug() << "DEBUG[load_yaml] before load file";
        YamlNode yamlFile = loadYAML::loadYamlFast(filePath.toStdString()); // throws
        qDebug() << "DEBUG[load_yaml] after load file";

        Rawfile rawfile(filePath);
        // readInstrument (yamlFile["instrument"] , rawfile);
        // readFormat     (yamlFile["format"]     , rawfile);
        // readExperiment (yamlFile["experiment"] , rawfile);
        readMeasurement(yamlFile["measurement"], rawfile);
        return rawfile;
    qDebug() << "DEBUG[load_yaml] done";
    } catch (Exception ex) {
        THROW("Invalid data in file "+filePath+":\n" + ex.msg());
    }
    // just to avoid compiler warnings:
    return Rawfile("");
}

} // namespace load
