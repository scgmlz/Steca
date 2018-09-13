//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/load_caress.cpp
//! @brief     Implements function loadCaress
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/aux/exception.h"
#include "core/raw/rawfile.h"
#include <qmath.h>
#include <sstream>

; // preserve inclusion order
#include "3rdparty/Caress/raw.h" // inclusion order is critical !

namespace load {

Rawfile loadCaress(const QString& filePath) {
    Rawfile ret(filePath);

    if(open_data_file(filePath.toLocal8Bit().data(), nullptr))
        THROW("Cannot open data file " + filePath);

    struct CloseFile { // TODO remove, replace with QFile etc.
        ~CloseFile() { close_data_file(); }
    } _;

    bool newObject = false;
    bool workAfterStep = false;
    bool collectData = false;
    std::string s_masterCounter;
    bool isRobot = false, isTable = false;

    float tthAxis = 0, omgAxis = 0, phiAxis = 0, chiAxis = 0, tths = 0, omgs = 0, chis = 0,
          phis = 0, tthr = 0, omgr = 0, chir = 0, phir = 0, xAxis = 0, yAxis = 0, zAxis = 0,
          pstAxis = 0, sstAxis = 0, omgmAxis = 0, nmT = 0, nmTeload = 0, nmTepos = 0, nmTeext = 0,
          nmXe = 0, nmYe = 0, nmZe = 0;

    int* intens = NULL;
    int imageSize = 0;

    int mon = 0, tim1 = 0;

    std::string s_date, s_comment;
    char* c_comment;
    double deltaTime = 0;
    double prevTempTime = 0;
    int deltaMon = 0;
    int prevMon = 0;

    bool end = false;
    while (!end) {
        int e_number, e_type, d_type, d_number;
        char element[100];
        element[99] = '\0';
        char node[100];
        node[99] = '\0';
        // Return of next_data_unit 0=OK; 1=Error; 2=End of File
        int returnNextDataUnit =
            next_data_unit(&e_number, &e_type, element, node, &d_type, &d_number);
        element[8] = '\0';
        node[8] = '\0';

        if (returnNextDataUnit == 1) // Error
            continue;

        if (returnNextDataUnit == 2) { // EOF
            newObject = true;
            end = true; // terminate while-loop after treating the current data unit
        }

        if (!strncmp(element, "SETVALUE", 8)) {
            // SETVALUE STEP is mark for a new Dataset!
            if (!strncmp(node, "STEP  ", 6))
                workAfterStep = true;

            if (workAfterStep) {
                if (!strncmp(node, "STEP  ", 6)) {
                    if (!collectData) {
                        collectData = true;
                    } else {
                        newObject = true;
                    }
                }
            } else {
                if (collectData) {
                    collectData = false;
                    newObject = true;
                }
            }
        }

        if (!workAfterStep) {
            if (!strncmp(element, "MASTER1V", 8)) {
                if (!strncmp(node, s_masterCounter.c_str(), 6)) {
                    if (!collectData)
                        collectData = true;
                    else
                        newObject = true;
                }
            }
        }

        if (newObject) {
            newObject = false;

            if (isRobot && isTable) {
                if (tthr == 0 && tths != 0) // it's the robot
                    isRobot = false;
                else if (tths == 0 && tthr != 0) // it's the table
                    isTable = false;
                else
                    THROW("inconsistent data set: not clear whether it's robot or table");
            }

            if (isRobot) {
                // Mapping to euler angle and pos
                tthAxis = tthr;
                omgAxis = omgr;
                chiAxis = 180 - chir;
                phiAxis = phir;
            }
            if (isTable) {
                // Mapping to euler angle and pos
                tthAxis = tths;
                omgAxis = omgs;
                chiAxis = chis;
                phiAxis = phis;
            }

            // Objekt inizialisieren
            if (intens != NULL) {
                int d, y;
                char c_m[5];
                std::string s_m;
                std::istringstream istr(s_date);
                istr >> d;
                istr.ignore();
                istr.get(c_m, 4);
                istr.ignore();
                istr >> y;
                s_m = c_m;

                double tempTime = 0;
                if ((y < 2015)
                    || ((y == 2015) && ((s_m.compare("Jan") == 0) || (s_m.compare("Feb") == 0)))) {
                    tempTime = double(tim1) / 100; // TODO REVIEW this HACK how deltaTime is used!!
                } else {
                    tempTime = double(tim1);
                }
                deltaTime = tempTime - prevTempTime;
                prevTempTime = tempTime;
                deltaMon = mon - prevMon;
                prevMon = mon;

                int detRel = qRound(sqrt(imageSize));
                if (!(imageSize > 0 && imageSize == detRel * detRel)) THROW("bad image size");

                std::vector<float> convertedIntens(imageSize);
                for (int i=0; i<imageSize; ++i)
                    convertedIntens[i] = intens[i];

                size2d size(detRel, detRel);

                // this is only for testing of a non-square image
                // size.rheight() /= 2;

                // Objekt inizialisieren
                Metadata md;

                md.date = QString::fromStdString(s_date);
                md.comment = QString::fromStdString(s_comment);

                md.motorXT = xAxis;
                md.motorYT = yAxis;
                md.motorZT = zAxis;

                md.motorOmg = omgAxis;
                md.motorTth = tthAxis;
                md.motorPhi = phiAxis;
                md.motorChi = chiAxis;
                md.motorPST = pstAxis;
                md.motorSST = sstAxis;
                md.motorOMGM = omgmAxis;

                md.nmT = nmT;
                md.nmTeload = nmTeload;
                md.nmTepos = nmTepos;
                md.nmTeext = nmTeext;
                md.nmXe = nmXe;
                md.nmYe = nmYe;
                md.nmZe = nmZe;

                md.deltaMonitorCount = deltaMon;
                md.monitorCount = mon;
                md.deltaTime = deltaTime;
                md.time = tempTime;

                ret.addDataset(std::move(md), size, std::move(convertedIntens));

                delete[] intens;
                intens = NULL;
                imageSize = 0;
            }
        }

        // Read Master Counter
        if (!strncmp(element, "MM1 ", 4)) {
            s_masterCounter = node; // Master Counter steht in Node
        }
        if (!strncmp(element, "COM ", 4)) {
            c_comment = new char[d_number + 1];
            if (get_data_unit(c_comment) != 0)
                s_comment = "no comment";
            else {
                c_comment[d_number] = '\0'; // terminiere Char-Array
                s_comment = c_comment;
            }
            delete[] c_comment;
        }
        if (!strncmp(element, "DATE ", 5)) {
            char* c_date = NULL;
            c_date = new char[d_number + 1];
            if (get_data_unit(c_date) != 0)
                s_date = "unknown";
            else {
                c_date[d_number] = '\0'; // terminiere Char-Array
                s_date = c_date;
            }
            delete[] c_date;
        }
        if (!strncmp(element, "READ  ", 6)) {
            //      TR('R' << node)
            if (!strncmp(node, "TTHS  ", 6)) {
                if (get_data_unit(&tths) != 0)
                    tths = 0;
                isTable = true;
            }
            if (!strncmp(node, "TTHR  ", 6)) {
                if (get_data_unit(&tthr) != 0)
                    tthr = 0;
                isRobot = true;
            }
            if (!strncmp(node, "OMGS  ", 6)) {
                if (get_data_unit(&omgs) != 0)
                    omgs = 0;
                isTable = true;
            }
            if (!strncmp(node, "OMGR  ", 6)) {
                if (get_data_unit(&omgr) != 0)
                    omgr = 0;
                isRobot = true;
            }
            if (!strncmp(node, "CHIS  ", 6)) {
                if (get_data_unit(&chis) != 0)
                    chis = 0;
                isTable = true;
            }
            if (!strncmp(node, "CHIR  ", 6)) {
                if (get_data_unit(&chir) != 0)
                    chir = 0;
                isRobot = true;
            }
            if (!strncmp(node, "PHIS  ", 6)) {
                if (get_data_unit(&phis) != 0)
                    phis = 0;
                isTable = true;
            }
            if (!strncmp(node, "PHIR  ", 6)) {
                if (get_data_unit(&phir) != 0)
                    phir = 0;
                isRobot = true;
            }
            if (!strncmp(node, "XT    ", 6))
                if (get_data_unit(&xAxis) != 0)
                    xAxis = 0;
            if (!strncmp(node, "XR    ", 6))
                if (get_data_unit(&xAxis) != 0)
                    xAxis = 0;
            if (!strncmp(node, "YT    ", 6))
                if (get_data_unit(&yAxis) != 0)
                    yAxis = 0;
            if (!strncmp(node, "YR    ", 6))
                if (get_data_unit(&yAxis) != 0)
                    yAxis = 0;
            if (!strncmp(node, "ZT    ", 6))
                if (get_data_unit(&zAxis) != 0)
                    zAxis = 0;
            if (!strncmp(node, "ZR    ", 6))
                if (get_data_unit(&zAxis) != 0)
                    zAxis = 0;
            if (!strncmp(node, "PST   ", 6))
                if (get_data_unit(&pstAxis) != 0)
                    pstAxis = 0;
            if (!strncmp(node, "SST   ", 6))
                if (get_data_unit(&sstAxis) != 0)
                    sstAxis = 0;
            if (!strncmp(node, "OMGM  ", 6))
                if (get_data_unit(&omgmAxis) != 0)
                    omgmAxis = 0;
            if (!strncmp(node, "T     ", 6))
                if (get_data_unit(&nmT) != 0)
                    nmT = 0;
            if (!strncmp(node, "TELOAD", 6))
                if (get_data_unit(&nmTeload) != 0)
                    nmTeload = 0;
            if (!strncmp(node, "TEPOS ", 6))
                if (get_data_unit(&nmTepos) != 0)
                    nmTepos = 0;
            if (!strncmp(node, "TEEXT ", 6))
                if (get_data_unit(&nmTeext) != 0)
                    nmTeext = 0;
            if (!strncmp(node, "XE    ", 6))
                if (get_data_unit(&nmXe) != 0)
                    nmXe = 0;
            if (!strncmp(node, "YE    ", 6))
                if (get_data_unit(&nmYe) != 0)
                    nmYe = 0;
            if (!strncmp(node, "ZE    ", 6))
                if (get_data_unit(&nmZe) != 0)
                    nmZe = 0;
        }
        if (!strncmp(element, "SETVALUE", 8)) {
            if (!strncmp(node, "TTHS  ", 6)) {
                if (get_data_unit(&tths) != 0)
                    tths = 0;
                isTable = true;
            }
            if (!strncmp(node, "TTHR  ", 6)) {
                if (get_data_unit(&tthr) != 0)
                    tthr = 0;
                isRobot = true;
            }
            if (!strncmp(node, "OMGS  ", 6)) {
                if (get_data_unit(&omgs) != 0)
                    omgs = 0;
                isTable = true;
            }
            if (!strncmp(node, "OMGR  ", 6)) {
                if (get_data_unit(&omgr) != 0)
                    omgr = 0;
                isRobot = true;
            }
            if (!strncmp(node, "CHIS  ", 6)) {
                if (get_data_unit(&chis) != 0)
                    chis = 0;
                isTable = true;
            }
            if (!strncmp(node, "CHIR  ", 6)) {
                if (get_data_unit(&chir) != 0)
                    chir = 0;
                isRobot = true;
            }
            if (!strncmp(node, "PHIS  ", 6)) {
                if (get_data_unit(&phis) != 0)
                    phis = 0;
                isTable = true;
            }
            if (!strncmp(node, "PHIR  ", 6)) {
                if (get_data_unit(&phir) != 0)
                    phir = 0;
                isRobot = true;
            }
            if (!strncmp(node, "XT    ", 6))
                if (get_data_unit(&xAxis) != 0)
                    xAxis = 0;
            if (!strncmp(node, "XR    ", 6))
                if (get_data_unit(&xAxis) != 0)
                    xAxis = 0;
            if (!strncmp(node, "YT    ", 6))
                if (get_data_unit(&yAxis) != 0)
                    yAxis = 0;
            if (!strncmp(node, "YR    ", 6))
                if (get_data_unit(&yAxis) != 0)
                    yAxis = 0;
            if (!strncmp(node, "ZT    ", 6))
                if (get_data_unit(&zAxis) != 0)
                    zAxis = 0;
            if (!strncmp(node, "ZR    ", 6))
                if (get_data_unit(&zAxis) != 0)
                    zAxis = 0;
            if (!strncmp(node, "PST   ", 6))
                if (get_data_unit(&pstAxis) != 0)
                    pstAxis = 0;
            if (!strncmp(node, "SST   ", 6))
                if (get_data_unit(&sstAxis) != 0)
                    sstAxis = 0;
            if (!strncmp(node, "OMGM  ", 6))
                if (get_data_unit(&omgmAxis) != 0)
                    omgmAxis = 0;
            if (!strncmp(node, "T     ", 6))
                if (get_data_unit(&nmT) != 0)
                    nmT = 0;
            if (!strncmp(node, "TELOAD", 6))
                if (get_data_unit(&nmTeload) != 0)
                    nmTeload = 0;
            if (!strncmp(node, "TEPOS ", 6))
                if (get_data_unit(&nmTepos) != 0)
                    nmTepos = 0;
            if (!strncmp(node, "TEEXT ", 6))
                if (get_data_unit(&nmTeext) != 0)
                    nmTeext = 0;
            if (!strncmp(node, "XE    ", 6))
                if (get_data_unit(&nmXe) != 0)
                    nmXe = 0;
            if (!strncmp(node, "YE    ", 6))
                if (get_data_unit(&nmYe) != 0)
                    nmYe = 0;
            if (!strncmp(node, "ZE    ", 6))
                if (get_data_unit(&nmZe) != 0)
                    nmZe = 0;
        }
        if (!strncmp(element, "MASTER1V", 8)) {
            //      TR('M' << node)
            if (!strncmp(node, "TTHS  ", 6)) {
                if (get_data_unit(&tths) != 0)
                    tths = 0;
                isTable = true;
            }
            if (!strncmp(node, "TTHR  ", 6)) {
                if (get_data_unit(&tthr) != 0)
                    tthr = 0;
                isRobot = true;
            }
            if (!strncmp(node, "OMGS  ", 6)) {
                if (get_data_unit(&omgs) != 0)
                    omgs = 0;
                isTable = true;
            }
            if (!strncmp(node, "OMGR  ", 6)) {
                if (get_data_unit(&omgr) != 0)
                    omgr = 0;
                isRobot = true;
            }
            if (!strncmp(node, "CHIS  ", 6)) {
                if (get_data_unit(&chis) != 0)
                    chis = 0;
                isTable = true;
            }
            if (!strncmp(node, "CHIR  ", 6)) {
                if (get_data_unit(&chir) != 0)
                    chir = 0;
                isRobot = true;
            }
            if (!strncmp(node, "PHIS  ", 6)) {
                if (get_data_unit(&phis) != 0)
                    phis = 0;
                isTable = true;
            }
            if (!strncmp(node, "PHIR  ", 6)) {
                if (get_data_unit(&phir) != 0)
                    phir = 0;
                isRobot = true;
            }
            if (!strncmp(node, "XT    ", 6))
                if (get_data_unit(&xAxis) != 0)
                    xAxis = 0;
            if (!strncmp(node, "XR    ", 6))
                if (get_data_unit(&xAxis) != 0)
                    xAxis = 0;
            if (!strncmp(node, "YT    ", 6))
                if (get_data_unit(&yAxis) != 0)
                    yAxis = 0;
            if (!strncmp(node, "YR    ", 6))
                if (get_data_unit(&yAxis) != 0)
                    yAxis = 0;
            if (!strncmp(node, "ZT    ", 6))
                if (get_data_unit(&zAxis) != 0)
                    zAxis = 0;
            if (!strncmp(node, "ZR    ", 6))
                if (get_data_unit(&zAxis) != 0)
                    zAxis = 0;
            if (!strncmp(node, "PST   ", 6))
                if (get_data_unit(&pstAxis) != 0)
                    pstAxis = 0;
            if (!strncmp(node, "SST   ", 6))
                if (get_data_unit(&sstAxis) != 0)
                    sstAxis = 0;
            if (!strncmp(node, "OMGM  ", 6))
                if (get_data_unit(&omgmAxis) != 0)
                    omgmAxis = 0;
            if (!strncmp(node, "T     ", 6))
                if (get_data_unit(&nmT) != 0)
                    nmT = 0;
            if (!strncmp(node, "TELOAD", 6))
                if (get_data_unit(&nmTeload) != 0)
                    nmTeload = 0;
            if (!strncmp(node, "TEPOS ", 6))
                if (get_data_unit(&nmTepos) != 0)
                    nmTepos = 0;
            if (!strncmp(node, "TEEXT ", 6))
                if (get_data_unit(&nmTeext) != 0)
                    nmTeext = 0;
            if (!strncmp(node, "XE    ", 6))
                if (get_data_unit(&nmXe) != 0)
                    nmXe = 0;
            if (!strncmp(node, "YE    ", 6))
                if (get_data_unit(&nmYe) != 0)
                    nmYe = 0;
            if (!strncmp(node, "ZE    ", 6))
                if (get_data_unit(&nmZe) != 0)
                    nmZe = 0;
            if (!strncmp(node, "MON   ", 6))
                if (get_data_unit(&mon) != 0)
                    mon = 0;
            if (!strncmp(node, "TIM1  ", 6))
                if (get_data_unit(&tim1) != 0)
                    tim1 = 0;
            if (!strncmp(node, "ADET  ", 6)) {
                if (d_type == 2) {
                    union {
                        char ch[MAXNUMBEROFCHANNELS];
                        int16 i2[MAXNUMBEROFCHANNELS];
                        int32 i4[MAXNUMBEROFCHANNELS];
                        int64 i8[MAXNUMBEROFCHANNELS];
                        float r4[MAXNUMBEROFCHANNELS];
                        double r8[MAXNUMBEROFCHANNELS];
                    } buffer;

                    intens = new int[d_number];
                    imageSize = d_number;
                    int section = 1;
                    int start_item = 1;
                    int tempNumItems, ret_val;
                    int remaining_items = d_number;
                    do /* read area detector array in MAXNUMBEROFCHANNELS blocks */
                    {
                        if (remaining_items >= MAXNUMBEROFCHANNELS)
                            tempNumItems = MAXNUMBEROFCHANNELS;
                        else
                            tempNumItems = remaining_items;
                        ret_val = get_data_partition(
                            buffer.i4, &section, &start_item, &tempNumItems, &d_type);
                        memmove(
                            &intens[start_item - 1], buffer.i4, tempNumItems * sizeof(int));
                        remaining_items -= MAXNUMBEROFCHANNELS;
                        start_item += MAXNUMBEROFCHANNELS;
                    } while (!ret_val && (remaining_items > 0));
                }
            }
        }
    }

    return ret;
}

QString loadCaressComment(const QString& filePath) {
    QString s_comment;

    try {
        if(open_data_file(filePath.toLocal8Bit().data(), nullptr))
            THROW("Cannot open data file " + filePath);

        struct CloseFile { // TODO remove, replace with QFile etc.
            ~CloseFile() { close_data_file(); }
        } _;

        bool end = false;
        while (!end) {
            int e_number, e_type, d_type, d_number;
            char element[100];
            element[99] = '\0';
            char node[100];
            node[99] = '\0';

            int returnNextDataUnit =
                next_data_unit(&e_number, &e_type, element, node, &d_type, &d_number);
            element[8] = '\0';
            node[8] = '\0';

            if (returnNextDataUnit == 1) // Error
                continue;

            if (returnNextDataUnit == 2) // EOF
                end = true;

            if (!strncmp(element, "COM ", 4)) {
                char* c_comment = new char[d_number + 1];
                if (get_data_unit(c_comment) != 0)
                    s_comment = "no comment";
                else {
                    c_comment[d_number] = '\0'; // terminiere Char-Array
                    s_comment = c_comment;
                }
                delete[] c_comment;
            }
        }
    } catch (const Exception&) {}

    return s_comment;
}

} // namespace load
