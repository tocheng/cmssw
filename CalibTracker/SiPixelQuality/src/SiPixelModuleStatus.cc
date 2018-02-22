#include <iostream>
#include <map>
#include <cmath>
#include <vector>

#include <TMath.h>
#include <TH1.h>

#include "CalibTracker/SiPixelQuality/interface/SiPixelModuleStatus.h"

using namespace std;

// ----------------------------------------------------------------------
SiPixelModuleStatus::SiPixelModuleStatus(int detId, int nrocs): fDetid(detId), fNrocs(nrocs) {

  for (int i = 0; i < fNrocs; ++i) {
    SiPixelRocStatus a;
    fRocs.push_back(a);
  }

};


// ----------------------------------------------------------------------
SiPixelModuleStatus::~SiPixelModuleStatus() {};


// ----------------------------------------------------------------------
void SiPixelModuleStatus::fill(int iroc, int idc) {

  if (iroc < fNrocs) 
     fRocs[iroc].fill(idc);

}

// ----------------------------------------------------------------------
void SiPixelModuleStatus::fill(int iroc, int idc, int nhit) {

  if (iroc < fNrocs) 
     fRocs[iroc].fill(idc, nhit);

}


// ----------------------------------------------------------------------
int SiPixelModuleStatus::statusDC(int iroc, int idc) {

  return (iroc < fNrocs ? fRocs[iroc].statusDC(idc) : -1);

}


// ----------------------------------------------------------------------
int SiPixelModuleStatus::statusROC(int iroc) {

  return (iroc < fNrocs ? fRocs[iroc].statusROC() : -1);

}


// ----------------------------------------------------------------------
int SiPixelModuleStatus::statusMOD() {

  int count(0);
  for (int iroc = 0; iroc < fNrocs; ++iroc) {
    count += statusROC(iroc);
  }
  return count;

}


// ----------------------------------------------------------------------
int SiPixelModuleStatus::detid() {

  return fDetid;

}


// ----------------------------------------------------------------------
int SiPixelModuleStatus::nrocs() {
  return fNrocs;
}

// ----------------------------------------------------------------------
void SiPixelModuleStatus::setNrocs(int iroc) {
  fNrocs = iroc;
}


// ----------------------------------------------------------------------
double SiPixelModuleStatus::average() {
  return fModAverage;
}


// ----------------------------------------------------------------------
double SiPixelModuleStatus::sigma() {
  return fModSigma;
}

// ----------------------------------------------------------------------
void SiPixelModuleStatus::occupancy() {

  fModAverage = fModSigma = 0.;
  double ave(0.), sig(0.);
  for (int iroc = 0; iroc < fNrocs; ++iroc) {
    int inc = statusROC(iroc);
    ave += inc;
  }
  fModAverage = ave/fNrocs;

  for (int iroc = 0; iroc < fNrocs; ++iroc) {
    int inc = statusROC(iroc);
    sig += (fModAverage-inc)*(fModAverage-inc);
  }

  fModSigma   = sig/(fNrocs-1);
  fModSigma   = TMath::Sqrt(fModSigma);

}


// ----------------------------------------------------------------------
// Be careful : return the address not the value of ROC status
SiPixelRocStatus* SiPixelModuleStatus::getRoc(int i) {
  return &fRocs[i];
}

// ----------------------------------------------------------------------
void SiPixelModuleStatus::updateModuleStatus(SiPixelModuleStatus newData) {

     bool isSameModule = true;
     if( fDetid!=newData.detid() || fNrocs!=newData.nrocs()) {
         isSameModule = false;
     }

     if(isSameModule){

        for (int iroc = 0; iroc < fNrocs; ++iroc) {
             int nDC = fRocs[iroc].nDC();
             for(int idc = 0; idc < nDC; ++idc) {
                 fRocs[iroc].fill(idc,newData.statusDC(iroc,idc));
             }
        }
        std::vector<PixelFEDChannel> newPixelFEDChannels = newData.getStuckTBMs();
        fPixelFEDChannels.reserve( fPixelFEDChannels.size() + newPixelFEDChannels.size() );
        fPixelFEDChannels.insert( fPixelFEDChannels.end(), newPixelFEDChannels.begin(), newPixelFEDChannels.end() );
     }

}
