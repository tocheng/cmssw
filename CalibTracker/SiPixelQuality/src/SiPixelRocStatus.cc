#include <iostream>
#include <map>
#include <cmath>
#include <vector>

#include "CalibTracker/SiPixelQuality/interface/SiPixelRocStatus.h"

using namespace std;

// ----------------------------------------------------------------------
SiPixelRocStatus::SiPixelRocStatus() {

  for (int i = 0; i < nDC_; ++i) {
       fDC[i] = 0;
  }

}


// ----------------------------------------------------------------------
SiPixelRocStatus::~SiPixelRocStatus() {

}

// ----------------------------------------------------------------------
void SiPixelRocStatus::fill(int idc) {

  if (idc<nDC_) fDC[idc]++;

}

// ----------------------------------------------------------------------
void SiPixelRocStatus::fill(int idc, int hits) {

  if (idc<nDC_) fDC[idc] += hits;

}


// ----------------------------------------------------------------------
int SiPixelRocStatus::statusDC(int idc) {

  return (idc<nDC_?fDC[idc]:-1);

}

// ----------------------------------------------------------------------
int SiPixelRocStatus::statusROC() {

  int count(0) ;
  for (int idc = 0; idc < nDC_; ++idc) {
    count += fDC[idc];
  }
  return count;
}
