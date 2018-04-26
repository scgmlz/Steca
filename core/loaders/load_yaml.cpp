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

#include "core/raw/rawfile.h"
#include "qcr/engine/debug.h"
#include "yaml-cpp/include/yaml-cpp/yaml.h"

namespace  {

void readInstrument(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    // const auto name       = node["name"].as<std::string>("");
    // const auto operators  = node["operators"].as<std::vector<std::string>>();
    // const auto facility   = node["facility"].as<std::string>("");
    // const auto website    = node["website"].as<std::string>("");
    // const auto references = node["references"].as<std::vector<std::string>>();
}

void readFormat(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    // const auto identifier = node["identifier"].as<std::string>("");
      // units = name: unit. eg: time: second, or clearance: millimeter:
    // const auto units  = node["units"].as<std::map<std::string, std::string>>();
}

void readExperiment(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    // const auto number = node["number"].as<std::string>("");
    // const auto proposal = node["proposal"].as<std::string>("");
    // const auto title = node["title"].as<std::string>("");
    // const auto remark = node["remark"].as<std::string>("");
}

void readSample(const YAML::Node& node, Metadata& metadata)
{
    metadata.motorXT = node["position"]["xt"]["value"].as<double>(Q_QNAN);
    metadata.motorYT = node["position"]["yt"]["value"].as<double>(Q_QNAN);
    metadata.motorZT = node["position"]["zt"]["value"].as<double>(Q_QNAN);
    metadata.motorOmg = node["orientation"]["omgs"]["value"].as<double>(Q_QNAN);
    metadata.motorTth = node["orientation"]["tths"]["value"].as<double>(Q_QNAN);
    metadata.motorPhi = node["orientation"]["phis"]["value"].as<double>(Q_QNAN);
    metadata.motorChi = node["orientation"]["chis"]["value"].as<double>(Q_QNAN);
}

void readSetup(const YAML::Node& node, Metadata& metadata)
{
    metadata.motorPST = Q_QNAN; // node["orientation"]["xt"]["value"].as<double>(Q_QNAN);
    metadata.motorSST = Q_QNAN; // node["orientation"]["xt"]["value"].as<double>(Q_QNAN);
    metadata.motorOMGM = node["monochromator"]["omgm"]["value"].as<double>(Q_QNAN);
    metadata.nmT = Q_QNAN;
    metadata.nmTeload = Q_QNAN;
    metadata.nmTepos = Q_QNAN;
    metadata.nmTeext = Q_QNAN;
    metadata.nmXe = Q_QNAN;
    metadata.nmYe = Q_QNAN;
    metadata.nmZe = Q_QNAN;
}

void readSingleScan(const YAML::Node& node, Metadata& metadata, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    metadata.time = node["time"].as<double>(Q_QNAN);
    metadata.monitorCount = node["monitor"].as<double>(Q_QNAN);
    const auto sum = node["sum"].as<double>(Q_QNAN);
    const auto imageNode = node["image"];
    const auto dimensionsNode = node["dimensions"];

    const size2d size(dimensionsNode["width"].as<int>(), dimensionsNode["height"].as<int>());

    std::vector<float> image;
    // fill image row after row...:
    qDebug() << "DEBUG[load_yaml] before read scan";
    std::stringstream imageStr(imageNode.as<std::string>());
    while (!imageStr.eof()) {
        float v;
        imageStr >> v;
        image.push_back(v);
    }
    qDebug() << "DEBUG[load_yaml] after read scan";

    rawfile.addDataset(std::move(metadata), size, std::move(image));
}

void readScans(const YAML::Node& node, Metadata& metadata, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;
    if (!node.IsSequence()) {
        THROW("invalid YAML format: 'scans' should be a list, but isn't.")
    }
    for (const YAML::Node& innerNode: node) {
        Metadata metadataCopy(std::move(metadata));
        // Copy the QStrings back, because std::move removes them from metadata:
        metadata.date    = metadataCopy.date;
        metadata.comment = metadataCopy.comment;
        readSingleScan(innerNode, metadataCopy, rawfile);
    }
}

void readMeasurement(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    // const auto unique_identifier = node["unique_identifier"].as<std::string>("");
    // const auto number = node["number"].as<std::string>(""); // Integer maybe?

    auto metadata = Metadata();

    metadata.date    = QString::fromStdString(node["history"]["started"].as<std::string>(""));
    metadata.comment = QString::fromStdString(node["history"]["scan"].as<std::string>(""));

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
        YAML::Node yamlFile = YAML::LoadFile(filePath.toStdString()); // throws: ParserException, BadFile;
        qDebug() << "DEBUG[load_yaml] after load file";

        Rawfile rawfile(filePath);
        // readInstrument (yamlFile["instrument"] , rawfile);
        // readFormat     (yamlFile["format"]     , rawfile);
        // readExperiment (yamlFile["experiment"] , rawfile);
        readMeasurement(yamlFile["measurement"], rawfile);
        return rawfile;
    qDebug() << "DEBUG[load_yaml] done";
    } catch (YAML::Exception e) {
        THROW("Invalid data in file "+filePath+":\n" + e.what());
    }
    // just to avoid compiler warnings:
    return Rawfile("");
}

} // namespace load
