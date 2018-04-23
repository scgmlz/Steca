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

#include "core/def/idiomatic_for.h"
#include "core/raw/rawfile.h"
#include <qmath.h>
#include <sstream>

; // preserve inclusion order
#include "3rdparty/yaml-cpp/include/yaml-cpp/yaml.h" // inclusion order is critical ?

namespace  {
void readInstrument(const YAML::Node& node, const Rawfile& rawfile);
void readFormat(const YAML::Node& node, const Rawfile& rawfile);
void readExperiment(const YAML::Node& node, const Rawfile& rawfile);
void readMeasurement(const YAML::Node& node, const Rawfile& rawfile);

typedef std::function<void (const YAML::Node& node, const Rawfile& rawfile)> ParserFunction;

const std::map<std::string, ParserFunction> parserFunctions = {
    std::pair<std::string, ParserFunction>("instrument",  &readInstrument),
    std::pair<std::string, ParserFunction>("format",      &readFormat),
    std::pair<std::string, ParserFunction>("experiment",  &readExperiment),
    std::pair<std::string, ParserFunction>("measurement", &readMeasurement),
};

}


namespace load {



Rawfile loadYaml(const QString& filePath)
{
    YAML::Node yamlFile = YAML::LoadFile(filePath.toStdString()); // throws: ParserException, BadFile;

    Rawfile rawfile(filePath);
    for (auto keywordHandler : parserFunctions ) {
        const auto keyword = keywordHandler.first;
        const auto func = keywordHandler.second;
        func(yamlFile[keyword], rawfile);
    }

    return rawfile;
}


void readInstrument(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined()) {
        return;
    };

    const auto name       = node["name"].as<std::string>("");
    const auto operators  = node["operators"].as<std::vector<std::string>>();
    const auto facility   = node["facility"].as<std::string>("");
    const auto website    = node["website"].as<std::string>("");
    const auto references = node["references"].as<std::vector<std::string>>();
    // TODO: readInstrument(const YAML::Node& node, const Rawfile& rawfile)
}

void readFormat(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined()) {
        return;
    };

    const auto identifier = node["identifier"].as<std::string>("");
    // units = name: unit. eg: time: second, or clearance: millimeter:
    const auto units  = node["units"].as<std::map<std::string, std::string>>();
    // TODO: readFormat(const YAML::Node& node, const Rawfile& rawfile)
}



void readExperiment(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined()) {
        return;
    };

    const auto number = node["number"].as<std::string>(""); // Integer maybe?
    const auto proposal = node["proposal"].as<std::string>(""); // not an Integer!!
    const auto title = node["title"].as<std::string>("");
    // const auto authors = node["authors"].as<std::vector<Author>>(""); TODO: uncomment.
    const auto remark = node["remark"].as<std::string>("");

    /*
    author:

    name: "Dr. John Doe"
    email: John.Doe@frm2.tum.de
    roles:
    -   local_contact
    -   principal_investigator
    */
    // TODO: readExperiment(const YAML::Node& node, const Rawfile& rawfile)
}

void readSample(const YAML::Node& node, Metadata& metadata)
{
    // TODO: readSample(const YAML::Node& node, const Metadata& metadata)
}

void readSetup(const YAML::Node& node, Metadata& metadata)
{
    // TODO: readSetup(const YAML::Node& node, const Metadata& metadata)
}

void readSingleScan(const YAML::Node& node, Metadata& metadata, Rawfile& rawfile)
{
    if (!node.IsDefined()) {
        return;
    };

    // const auto scan_parameters = node["scan_parameters"].as<std::tuple>();
    metadata.time = node["time"].as<double>(NAN);
    metadata.monitorCount = node["monitor"].as<double>(NAN);
    const auto sum = node["sum"].as<double>(NAN);
    const auto imageNode = node["image"];

    const size2d size(node[0].size(), node.size());
    std::vector<float> image;

    // fill image row after row...:
    for (auto rowNode: imageNode) {
        auto row = rowNode.as<std::vector<float>>();
        image.insert(image.end(), row.begin(), row.end());
    }

    rawfile.addDataset(std::move(metadata), size, std::move(image));


    /*
    -   scan_parameters: {}
        time: 300.0
        monitor: 1 555 189
        sum: 670025
        image:
            -   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            -   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      */
    // TODO: readSingleScan(const YAML::Node& node, const Metadata& metadata)
}

void readScans(const YAML::Node& node, Metadata& metadata, Rawfile& rawfile)
{
    if (!node.IsDefined()) {
        return;
    };
    if (!node.IsSequence()) {
        THROW("invalid YAML format: 'scans' should be a list, but isn't.")
    };

    for (const YAML::Node& innerNode: node) {
        Metadata metadataCopy;
        { // Copy the metadata in a dirty way:
            // type = string:
            metadataCopy.date              =  metadata.date             ;
            metadataCopy.comment           =  metadata.comment          ;
            // type = deg:
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
            // type = double:
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

        readSingleScan(node, metadataCopy, rawfile);
    }
    // TODO: readScan(const YAML::Node& node, const Metadata& metadata)
}

void readMeasurement(const YAML::Node& node, Rawfile& rawfile)
{
    if (!node.IsDefined()) {
        return;
    };

    const auto unique_identifier = node["unique_identifier"].as<std::string>("");
    const auto number = node["number"].as<std::string>(""); // Integer maybe?

    auto metadata = Metadata();

    // type = string:
    metadata.date;
    metadata.comment;
    // type = deg:
    metadata.motorXT;
    metadata.motorYT;
    metadata.motorZT;
    metadata.motorOmg;
    metadata.motorTth;
    metadata.motorPhi;
    metadata.motorChi;
    metadata.motorPST;
    metadata.motorSST;
    metadata.motorOMGM;
    // type = double:
    metadata.nmT;
    metadata.nmTeload;
    metadata.nmTepos;
    metadata.nmTeext;
    metadata.nmXe;
    metadata.nmYe;
    metadata.nmZe; // nm = new metadata

    metadata.monitorCount;
    metadata.deltaMonitorCount;

    metadata.time;
    metadata.deltaTime;

    const auto proposal = node["proposal"].as<std::string>(""); // not an Integer!!
    const auto title = node["title"].as<std::string>("");
    // const auto authors = node["authors"].as<std::vector<Author>>(""); TODO: uncomment.
    const auto remark = node["remark"].as<std::string>("");

    /*
measurement:
    unique_identifier: "urn:uuid:dfc44406-a700-4c9b-bd2e-11b8ef383b1e"
    number: 116953
    history:
        started: "2017-09-29 05:33:22"
        stopped: "2017-09-29 05:38:23"
        scan: "110ss_Reflection_chis=,90,time=,300,highestForce=,0.0,Force=,15599.75,highestEps=,0.0,Eps=,7.24507045746, - timescan(1, t=300)"
    sample:
        description:
            name: Nickelbasislegierung / Ni-20Cr-10Co-8Mo
        temperature: {}
        orientation:
            tths:
                value: "45.00"
                offset_coder: "3319.25"
                precision: "0.03"
            omgs:
                value: "22.50"
                offset_coder: "0.00"
                precision: "0.01"
            chis:
                value: "90.07"
                offset_coder: "0.00"
                precision: "0.10"
            phis:
                value: 0
                offset_coder: 0
                precision: 0
        position:
            xt:
                value: "-0.18"
                offset_coder: "0.00"
                precision: "0.01"
            zt:
                offset_coder: "0.00"
                precision: "0.05"
                value: "8.93"
            yt:
                offset_coder: "-10242.9"
                precision: "0.01"
                value: "-0.10"
    setup:
        collimator_1: "15'"
        slit_m:
            horizontal_clearance:
                value: 0
                offset_coder: 0
                precision: 0
            vertical_clearance:
                value: 0
                offset_coder: 0
                precision: 0
        monochromator:
            crystal: Ge
            omgm:
                value: "41.21"
                offset_coder: "0.00"
                precision: "0.01"
            tthm:
                value: "70.50"
                offset_coder: 0
                precision: 0
            angle: 0
            incident_wavelength: "1.75"
        slit_e:
            clearance:
                value: 0
                offset_coder: 0
                precision: 0
        slit_p:
            horizontal_clearance:
                value: 0
                offset_coder: 0
                precision: 0
            horizontal_translation:
                value: 0
                offset_coder: 0
                precision: 0
            vertical_clearance:
                value: 0
                offset_coder: 0
                precision: 0
            vertical_translation:
                value: 0
                offset_coder: 0
                precision: 0
        collimator_2:
            fhwm: 5
    scan:
    -   scan_parameters: {}
        time: 300.0
        monitor: 1555189
        sum: 670025
        image:
            -   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            -   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

    */
    // TODO: readMeasurement(const YAML::Node& node, const Rawfile& rawfile)
}





} // namespace load
