//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/load_mar.cpp
//! @brief     Implements function loadMar
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/misc/exception.h"
#include "core/raw/rawfile.h"
#include "3rdparty/Mar/MarReader.h"

namespace load {

// Code taken from the original STeCa, only slightly modified.

Rawfile loadMar(const QString& filePath) {
    typedef short WORD;

    Rawfile ret(filePath);

    FILE* fpIn = fopen(filePath.toLocal8Bit().data(), "rb");
    if(!fpIn)
        THROW("Cannot open data file " + filePath);

    struct CloseFile { // TODO remove, replace with QFile etc.
        CloseFile(FILE* fpIn) : fpIn_(fpIn) {}
        ~CloseFile() { fclose(fpIn_); }
    private:
        FILE* fpIn_;
    } _(fpIn);

    /* Check if byte_swapping is necessary */
    int mar345, byteswap, h1;
    size_t readElements = fread(&h1, sizeof(int), 1, fpIn);

    if (!(1 == readElements)) THROW("bad format");

    if (h1 == 1200 || h1 == 2000 || h1 == 1600 || h1 == 2300 || h1 == 3450 || h1 == 3000
        || h1 == 2400 || h1 == 1800 || h1 == 2560 || h1 == 3072) {
        mar345 = 0;
        byteswap = 0;
    } else if (h1 == 1234) {
        mar345 = 1;
        byteswap = 0;
    } else {
        byteswap = 1;
        swapint32((unsigned char*)&h1, 1 * sizeof(int));
    }

    if (byteswap) {
        if (h1 == 1234)
            mar345 = 1;
        else if (h1 > 10 && h1 < 5000)
            mar345 = 0;
        else
            THROW("bad format");
    }

    // Read Header
    unsigned int pixSizeX = 0, pixSizeY = 0, pixelSize = 0, numberOfHigh = 0;

    MAR300_HEADER h300;
    MAR345_HEADER h345;

    if (mar345) {
        h345 = Getmar345Header(fpIn);
        pixSizeX = h345.size;
        numberOfHigh = h345.high;
        pixelSize = h345.pixels;
        if ((h345.pixels != (int)(pixSizeX * pixSizeX)) && (h345.pixels > 0)) {
            pixSizeY = h345.pixels / pixSizeX;
        } else {
            pixSizeY = pixSizeX;
        }

    } else {
        h300 = Getmar300Header(fpIn);
        pixSizeX = h300.pixels_x;
        pixSizeY = h300.pixels_y;
        numberOfHigh = h300.high_pixels;
        pixelSize = pixSizeX * pixSizeY;
    }

    if ((pixSizeX < 10) || (pixSizeX > 4000) || (pixSizeY < 10) || (pixSizeY > 4000)
        || (numberOfHigh > 9999999)) {
        THROW("bad format");
    }

    WORD* i2_image = new WORD[pixelSize];
    int* i4_image = new int[pixelSize];

#define FPOS(a) ((int)(a / 8. + 0.875) * 64)
    /* Read core of i2_image */
    /* Go to first record */
    if (mar345) {
        fseek(fpIn, 4096 + FPOS(numberOfHigh), SEEK_SET);
        get_pck(fpIn, (short*)i2_image);
    } else {
        fseek(fpIn, pixSizeX + pixSizeY, SEEK_SET);
        int i = (int)fread((unsigned char*)i2_image, sizeof(short), pixelSize, fpIn);
        if (i != (int)pixelSize)
            THROW("did not read not all pixels"); // Does this happen? Would a warning suffice?
        if (byteswap)
            swapint16((unsigned char*)i2_image, pixelSize * sizeof(WORD));
    }

    // Copy to 32Bit-Array
    for (int i=0; i<pixelSize; ++i)
        i4_image[i] = (int)((unsigned short)i2_image[i]);

    //***********************************
    //*** Read and correct high bytes ***
    //***********************************
    /* There are some high intensity values stored in the image */
    if (numberOfHigh) {
        int pair[2];

        /* Go to start of overflow record */
        fseek(fpIn, 4096, SEEK_SET);

        /*
         * Read overflows. Read a pair of values ( strong ), where
         * first  value = address of pixel in i2_image array
         * second value = intensity of pixel (32-bit)
         * In order to save memory, we don't allocate a 32-bit array
         * for all data, but a 16-bit array and an 8-bit array.
         * The final intensity 24-bit intensity is stored as:
         * 24-bit-value = 16-bit-value + (8-bit-value)*65535;
         * Note: the maximum intensity the scanner may produce is 250.000
         *       Beyond the saturation of the ADC, all saturated pixels
         *       get an intensity of 999.999 !
         */
        for (int i=0; i<numberOfHigh; ++i) {
            readElements = fread(pair, sizeof(int), 2, fpIn);
            if (readElements < 2)
                break;

            if (byteswap)
                swapint32((unsigned char*)pair, sizeof(int) * 2);

            unsigned int address = pair[0];
            if (address >= pixelSize)
                continue;

            // Check for error pixel (overflow pixel will set to 250000 and in the
            // dataseq will set the overflow flag)
            if (pair[1] == 999999.0)
                pair[1] = 128000;

            // Correct high pixel
            i4_image[address] = pair[1];
        }
    }

    // Check Pixel Level um Daten mit defekten Pixeln lesen zu k�nnen
    // TODO REVIEW
    //  if (MeasurementData::isPixelLevelUsed()) {
    //    for (int i=0; i<pixelSize; ++i) {
    //      if (i4_image[i] > MeasurementData::pixelLevel)
    //        i4_image[i] = -1;
    //    }
    //  }

    // Create Dataset
    double twoTheta = 0, omega = 0, chi = 0, phi = 0;
    int exposureTime = 0, dosen = 0;
    unsigned int totalTime = 0;

    if (mar345) {
        // Objekt inizialisieren
        twoTheta = h345.theta;
        chi = h345.chi;
        omega = ((fabs(h345.omeend - h345.omebeg) / 2) + h345.omebeg);
        phi = ((fabs(h345.phiend - h345.phibeg) / 2) + h345.phibeg);

        dosen = h345.dosen;
        exposureTime = h345.time;
        totalTime += exposureTime;
    } else {
        // Objekt inizialisieren
        twoTheta = 0;
        chi = 0;
        omega = fabs(h300.omega);
        phi = ((fabs(h300.phi_end - h300.phi_start) / 2) + h300.phi_start);

        dosen = 0;
        exposureTime = h300.exptime_sec;
        totalTime += exposureTime;
    }

    size2d size(pixSizeX, pixSizeY);
    std::vector<float> convertedIntens(pixelSize);

    for (int i=0; i<pixelSize; ++i)
        convertedIntens[i] = float(i4_image[i]);

    Metadata md;

    md.motorOmg = omega;
    md.motorTth = twoTheta;
    md.motorPhi = phi;
    md.motorChi = chi;

    md.monitorCount = dosen; // ? REVIEW ? md.deltaMonitorCount
    md.deltaTime = exposureTime;
    md.time = totalTime;

    // REVIEW ?? pictureOverflow

    ret.addDataset(std::move(md), size2d(pixSizeX, pixSizeY), std::move(convertedIntens));

    delete[] i2_image;
    delete[] i4_image;

    return ret;
}

} // namespace load
