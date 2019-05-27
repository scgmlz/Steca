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

#include "core/loaders/load_yaml.h"
#include "core/loaders/fastyamlloader.h"
#include "core/raw/rawfile.h"
#include "core/base/exception.h"
#include "QCR/base/debug.h"
#include <functional>

namespace  {

using loadYAML::YamlNode;

void readSample(const YamlNode& node, Metadata& metadata)
{
    metadata.set("X", node["position"]["xt"]["value"].doubleValue(Q_QNAN));
    metadata.set("Y", node["position"]["yt"]["value"].doubleValue(Q_QNAN));
    metadata.set("Z", node["position"]["zt"]["value"].doubleValue(Q_QNAN));
    metadata.set("omega", deg{node["orientation"]["omgs"]["value"].doubleValue(Q_QNAN)});
    metadata.set("mid2theta", deg{node["orientation"]["tths"]["value"].doubleValue(Q_QNAN)});
    metadata.set("phi", deg{node["orientation"]["phis"]["value"].doubleValue(Q_QNAN)});
    metadata.set("chi", deg{node["orientation"]["chis"]["value"].doubleValue(Q_QNAN)});
}

void readSetup(const YamlNode& node, Metadata& metadata)
{
    metadata.set("PST", Q_QNAN); // node["orientation"]["pst"]["value"].doubleValue(Q_QNAN);
    metadata.set("SST", Q_QNAN); // node["orientation"]["sst"]["value"].doubleValue(Q_QNAN);
    metadata.set("OmegaM", deg{node["monochromator"]["omgm"]["value"].doubleValue(Q_QNAN)});
    metadata.set("T", Q_QNAN);
    metadata.set("teload", Q_QNAN);
    metadata.set("tepos", Q_QNAN);
    metadata.set("teext", Q_QNAN);
    metadata.set("xe", Q_QNAN);
    metadata.set("ye", Q_QNAN);
    metadata.set("ze", Q_QNAN);
}

void readSingleScan(const YamlNode& node, Metadata& metadata, Rawfile& rawfile)
{
    metadata.set("t", node["time"].doubleValue(Q_QNAN));
    metadata.set("mon", node["monitor"].doubleValue(Q_QNAN));
    const auto image      = node["image"].array2dValue();

    const size2d size(image->width, image->height);

    rawfile.addDataset(std::move(metadata), size, std::move(image->data));
}

void readScans(const YamlNode& node, Metadata& metadata, Rawfile& rawfile)
{
    if (!node.isSequence())
        THROW("invalid YAML format: 'measurement.scan' section should be a list, but isn't.")
    for (const YamlNode& innerNode: node) {
        Metadata metadataCopy(std::move(metadata));
        // Copy the QStrings back, because std::move removes them from metadata:
        if (metadataCopy.has("date"))
            metadata.set("date", metadataCopy.get<QString>("date"));
        if (metadataCopy.has("comment"))
            metadata.set("comment", metadataCopy.get<QString>("comment"));
        readSingleScan(innerNode, metadataCopy, rawfile);
    }
}

//! Processes the "measurement" section of the Yaml tree, and inserts contents into rawfile.

void readMeasurement(const YamlNode& node, Rawfile& rawfile)
{
    Metadata metadata;

    metadata.set("date", node["history"]["started"].value());
    metadata.set("comment", node["history"]["scan"].value());

    readSample(node["sample"], metadata);
    readSetup(node["setup"], metadata);
    readScans(node["scan"], metadata, rawfile); // adds the scans to the rawfile
}

} // namespace


namespace load {

//! Reads a YAML file, and returns contents as a Rawfile.
//!
//! Called from function load_low_level(..) in file loaders.cpp.

Rawfile loadYaml(const QString& filePath)
{
    try {
        qDebug() << "DEBUG[load_yaml] now load file";
        YamlNode yamlTree = loadYAML::loadYamlFast(filePath.toStdString()); // throws
        qDebug() << "DEBUG[load_yaml] file loaded, now processing tree";

        Rawfile rawfile(filePath); // sets file name, otherwise rawfile is tabula rasa.
        // for the time being, the "instrument", "format", and "experiment" scetions
        // of yamlTree are ignored for good; only the "measurement" section is read:
        readMeasurement(yamlTree["measurement"], rawfile);
        qDebug() << "DEBUG[load_yaml] tree processed, now returning rawfile";
        return rawfile;
    } catch (const Exception& ex) {
        THROW("Invalid data in file "+filePath+": " + ex.msg());
    }
    qFatal("unreachable"); // some statement needed here to avoid compiler warning
}

} // namespace load
