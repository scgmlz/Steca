#include "loadcaress.h"
#include "dataset.h"
#include "loaders/Caress/raw.h"

#include <sstream>

// Taken from the original STeCa, modified.

void loadCaress(core::rcstr filePath, core::Datasets& vectorDataset) throw (core::Exception) {
  bool const calcDiff = false, rebindTo256 = false; // TODO args

  if (0 != open_data_file(filePath.toLocal8Bit().data(),nullptr))
    core::raiseError("Cannot open data file " + filePath);

  try {
    bool newObject = false;
    bool workAfterStep = false;
    bool collectData = false;
    std::string s_masterCounter;
    bool isRobot = false, isTable = false;

    float tthAxis = 0, omgAxis = 0, phiAxis = 0, chiAxis = 0, tths = 0, omgs = 0, chis = 0,
        phis = 0, tthr = 0, omgr = 0, chir = 0, phir = 0, xAxis = 0, yAxis = 0, zAxis = 0,
        pstAxis = 0, sstAxis = 0, omgmAxis = 0;

    int *intens = NULL;
    int *lastIntens = NULL;
    int imageSize = 0;

    int mon = 0, tim1 = 0;
    int lastMon = 0, lastTim1 = 0;

    std::string s_date, s_comment;
    char* c_comment;

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
            throw "inconsistent data set: not clear whether it's robot or table";
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
          if (calcDiff) {
            // Calculate difference of data
            // ****************************
            // Set last to zero if, it is the first dataset
            if (lastIntens == NULL) {
              lastMon = 0;
              lastTim1 = 0;
              lastIntens = new int[imageSize];
              memset(lastIntens, 0, sizeof(int) * imageSize);
            }
            // Create tmp values for the corrected information
            int tmpMon = mon - lastMon;
            int tmpTim1 = tim1 - lastTim1;
            int* tmpIntens = new int[imageSize];
            for (int k = 0; k < imageSize; k++)
              tmpIntens[k] = intens[k] - lastIntens[k];

            // Copy current to last
            memcpy(lastIntens, intens, sizeof(int) * imageSize);
            lastMon = mon;
            lastTim1 = tim1;

            // Copy corrected to current
            memcpy(intens, tmpIntens, sizeof(int) * imageSize);
            mon = tmpMon;
            tim1 = tmpTim1;

            // Delete tmp
            delete tmpIntens;
          }

          core::Dataset* dataset;
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
          if ((y < 2015) ||
              ((y == 2015) && ((s_m.compare("Jan") == 0) || (s_m.compare("Feb") == 0)))) {
            tempTime = (double)tim1 / 100;
          } else {
            tempTime = (double)tim1;
          }

          QSize detRel;

          detRel.setHeight((int)sqrt((double)imageSize));
          detRel.setWidth((int)sqrt((double)imageSize));

          int* intensRebind = NULL;
          if (rebindTo256 && ((detRel.height() > 256) || (detRel.width() > 256))) {
            // Rebind data to 256 x 256
            int factor = (int)(detRel.height() / 256.0);
//            if ((factor * 256) != detRel.height)
//              std::cout << "Attention: The Images can't be rebind to 256x256."
//                        << std::endl;

            intensRebind = new int[256 * 256];
            std::fill_n(intensRebind, 256 * 256, 0);

            int x = 0;
            int y = 0;
            int oldY = 0;
            int subCounterY = 0;
            while (y < 256) {
              int oldX = 0;
              int subCounterX = 0;
              while (x < 256) {
                intensRebind[y * 256 + x] += intens[oldY * detRel.width() + oldX];
                oldX++;
                subCounterX++;
                if (subCounterX == factor) {
                  subCounterX = 0;
                  x++;
                }
              }
              oldY++;
              if (subCounterY == factor) {
                subCounterY = 0;
                y++;
              }
            }

            detRel.setHeight(256);
            detRel.setWidth(256);

            // Delete intens
            delete[] intens;
            intens = NULL;
          } else {
            intensRebind = intens;
            intens = NULL;
          }
          // Objekt inizialisieren
          float* tmpImage = new float[detRel.height() * detRel.width()];
          for (int i = 0; i < (detRel.height() * detRel.width()); i++)
            tmpImage[i] = (float)intensRebind[i];
          constexpr double deg2rad = 3.1415926535897932384626433832795 / 180;
          dataset = new core::Dataset(detRel, tmpImage, core::str::fromStdString(s_comment), xAxis, yAxis, zAxis, omgAxis * deg2rad,
                                tthAxis * deg2rad, phiAxis * deg2rad, chiAxis * deg2rad,
                                (double)mon, tempTime, /*tmpImage, detRel.height() * detRel.width(),*/
                                pstAxis, sstAxis, omgmAxis * deg2rad);
          vectorDataset.append(dataset);

          delete[] tmpImage;
          delete[] intensRebind;
          intensRebind = NULL;
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
      }
      if (!strncmp(element, "MASTER1V", 8)) {
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
              ret_val = get_data_partition(buffer.i4, &section, &start_item,
                                           &tempNumItems, &d_type);
              memmove(&intens[start_item - 1], buffer.i4, tempNumItems * sizeof(int));
              remaining_items -= MAXNUMBEROFCHANNELS;
              start_item += MAXNUMBEROFCHANNELS;
            } while (!ret_val && (remaining_items > 0));

            int integral = 0;
            for (int i = 0; i < imageSize; i++) {
              integral += intens[i];
            }
          }
        }
      }
    }

  } catch (...) {
    close_data_file();
    throw;
  }

//      if (lastIntens != NULL)
//          delete lastIntens;

//      if (vectorDataset.size() < 1)
//          throw "no data read";
//  }

}

//
