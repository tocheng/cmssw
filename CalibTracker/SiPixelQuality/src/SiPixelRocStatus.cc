#include <iostream>
#include <map>
#include <cmath>
#include <vector>

#include "CalibTracker/SiPixelQuality/interface/SiPixelRocStatus.h"

using namespace std;

// ----------------------------------------------------------------------
SiPixelRocStatus::SiPixelRocStatus() {

  for (int i = 0; i < nDC; ++i) {
       fDC[i] = 0;
  }

}


// ----------------------------------------------------------------------
SiPixelRocStatus::~SiPixelRocStatus() {

}

// ----------------------------------------------------------------------
void SiPixelRocStatus::fill(int idc) {

  if (idc<nDC) fDC[idc]++;

}

// ----------------------------------------------------------------------
void SiPixelRocStatus::fill(int idc, int hits) {

  if (idc<nDC) fDC[idc] += hits;

}


// ----------------------------------------------------------------------
int SiPixelRocStatus::status(int idc) {

  return (idc<nDC?fDC[idc]:-1);

}

// ----------------------------------------------------------------------
int SiPixelRocStatus::status() {

  int count(0) ;
  for (int idc = 0; idc < nDC; ++idc) {
    count += fDC[idc];
  }
  return count;
}
