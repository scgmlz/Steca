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
#include "3rdparty/yaml-cpp/include/yaml-cpp/yaml.h"

namespace  {

void readInstrument(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    const auto name       = node["name"].as<std::string>("");
    const auto operators  = node["operators"].as<std::vector<std::string>>();
    const auto facility   = node["facility"].as<std::string>("");
    const auto website    = node["website"].as<std::string>("");
    const auto references = node["references"].as<std::vector<std::string>>();
}

void readFormat(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    const auto identifier = node["identifier"].as<std::string>("");
    // units = name: unit. eg: time: second, or clearance: millimeter:
    const auto units  = node["units"].as<std::map<std::string, std::string>>();
}

void readExperiment(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    const auto number = node["number"].as<std::string>("");
    const auto proposal = node["proposal"].as<std::string>("");
    const auto title = node["title"].as<std::string>("");
    const auto remark = node["remark"].as<std::string>("");
}

void readSample(const YAML::Node& node, Metadata& metadata)
{
    metadata.comment = QString::fromStdString(node["description"]["name"].as<std::string>());
    metadata.motorXT = node["position"]["xt"]["value"].as<double>(NAN);
    metadata.motorYT = node["position"]["yt"]["value"].as<double>(NAN);
    metadata.motorZT = node["position"]["zt"]["value"].as<double>(NAN);
    metadata.motorOmg = node["orientation"]["omgs"]["value"].as<double>(NAN);
    metadata.motorTth = node["orientation"]["tths"]["value"].as<double>(NAN);
    metadata.motorPhi = node["orientation"]["phis"]["value"].as<double>(NAN);
    metadata.motorChi = node["orientation"]["chis"]["value"].as<double>(NAN);
}

void readSetup(const YAML::Node& node, Metadata& metadata)
{
    metadata.motorPST = NAN; // node["orientation"]["xt"]["value"].as<double>(NAN);
    metadata.motorSST = NAN; // node["orientation"]["xt"]["value"].as<double>(NAN);
    metadata.motorOMGM = node["monochromator"]["omgm"]["value"].as<double>(NAN);
    metadata.nmT;
    metadata.nmTeload;
    metadata.nmTepos;
    metadata.nmTeext;
    metadata.nmXe;
    metadata.nmYe;
    metadata.nmZe; // nm = new metadata
    // TODO: readSetup(const YAML::Node& node, const Metadata& metadata)
}

void readSingleScan(const YAML::Node& node, Metadata& metadata, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    metadata.time = node["time"].as<double>(NAN);
    metadata.monitorCount = node["monitor"].as<double>(NAN);
    const auto sum = node["sum"].as<double>(NAN);
    const auto imageNode = node["image"];

    const size2d size(imageNode[0].size(), imageNode.size());

    std::vector<float> image;
    // fill image row after row...:
    for (auto rowNode: imageNode) {
        auto row = rowNode.as<std::vector<float>>();
        image.insert(image.end(), row.begin(), row.end());
    }

    rawfile.addDataset(std::move(metadata), size, std::move(image));
}

void readScans(const YAML::Node& node, Metadata& metadata, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;
    if (!node.IsSequence())
        THROW("invalid YAML format: 'scans' should be a list, but isn't.")

    for (const YAML::Node& innerNode: node) {
        Metadata metadataCopy(std::move(metadata));
        { // Copy the metadata in a dirty way:
            metadataCopy.date              =  metadata.date             ;
            metadataCopy.comment           =  metadata.comment          ;
            metadataCopy.motorXT           =  metadata.motorXT          ;
            metadataCopy.motorYT           =  metadata.motorYT          ;
            metadataCopy.motorZT           =  metadata.motorZT          ;
            metadataCopy.motorOmg          =  metadata.motorOmg         ;
            metadataCopy.motorTth          =  metadata.motorTth         ;
            metadataCopy.motorPhi          =  metadata.motorPhi         ;
            metadataCopy.motorChi          =  metadata.motorChi         ;
            metadataCopy.motorPST          =  metadata.motorPST         ;
            metadataCopy.motorSST          =  metadata.motorSST         ;
            metadataCopy.motorOMGM         =  metadata.motorOMGM        ;
            metadataCopy.nmT               =  metadata.nmT              ;
            metadataCopy.nmTeload          =  metadata.nmTeload         ;
            metadataCopy.nmTepos           =  metadata.nmTepos          ;
            metadataCopy.nmTeext           =  metadata.nmTeext          ;
            metadataCopy.nmXe              =  metadata.nmXe             ;
            metadataCopy.nmYe              =  metadata.nmYe             ;
            metadataCopy.nmZe              =  metadata.nmZe             ;

            metadataCopy.monitorCount      =  metadata.monitorCount     ;
            metadataCopy.deltaMonitorCount =  metadata.deltaMonitorCount;

            metadataCopy.time              =  metadata.time             ;
            metadataCopy.deltaTime         =  metadata.deltaTime        ;
        }

        readSingleScan(innerNode, metadataCopy, rawfile);
    }
    // TODO: readScan(const YAML::Node& node, const Metadata& metadata)
}

void readMeasurement(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined())
        return;

    const auto unique_identifier = node["unique_identifier"].as<std::string>("");
    const auto number = node["number"].as<std::string>(""); // Integer maybe?

    auto metadata = Metadata();

    metadata.date = QString::fromStdString(node["history"]["started"].as<std::string>());

    readSample(node["sample"], metadata);
    readSetup(node["setup"], metadata);
    readScans(node["scan"], metadata, rawfile); // adds the scanns to the rawfile

    // TODO: readMeasurement(const YAML::Node& node, const Rawfile& rawfile)
}

} // namespace


namespace load {

Rawfile loadYaml(const QString& filePath)
{
    YAML::Node yamlFile = YAML::LoadFile(filePath.toStdString()); // throws: ParserException, BadFile;

    Rawfile rawfile(filePath);
    readInstrument (yamlFile["instrument"] , rawfile);
    readFormat     (yamlFile["format"]     , rawfile);
    readExperiment (yamlFile["experiment"] , rawfile);
    readMeasurement(yamlFile["measurement"], rawfile);

    return rawfile;
}

} // namespace load
