#ifndef SIPIXELDETECTORSTATUS_h
#define SIPIXELDETECTORSTATUS_h

#include <map>
#include <string>

#include "CalibTracker/SiPixelQuality/interface/SiPixelModuleStatus.h"

// ----------------------------------------------------------------------
class SiPixelDetectorStatus {

public:

  SiPixelDetectorStatus();
  ~SiPixelDetectorStatus();

  /// file I/O
  void readFromFile(std::string filename);
  void dumpToFile(std::string filename);

  /// add SiPixelModuleStatus for detID
  void addModule(int detid);

  /// add SiPixelModuleStatus for detID, specifying nrocs
  void addModule(int detid, int nrocs);

  /// fill hit in double idc in ROC roc into module detid
  void fill(int detid, int roc, int idc);

  ///// fill hit in OFFLINE col/row into module detid
  //void fillOffline(int detid, int col, int row);

  /// determine detector average nhits and RMS
  void occupancy();

  /// analysis of detector performance
  void analysis();

  /// number of modules in detector
  int nmodules();

  /// get a Module
  SiPixelModuleStatus* getModule(int detid);

  /// provide for iterating over the entire detector
  std::map<int, SiPixelModuleStatus>::iterator begin();
  std::map<int, SiPixelModuleStatus>::iterator next();
  std::map<int, SiPixelModuleStatus>::iterator end();

  void setBeginLS(int ls) {fLS0 = ls;}
  void setEndLS(int ls) {fLS1 = ls;}

  void setBeginRun(int run) {fRun0 = run;}
  void setEndRun(int run) {fRun1 = run;}

  std::pair<int,int> getLSRange()  { return std::make_pair(fLS0,fLS1); }
  std::pair<int,int> getRunRange() { return std::make_pair(fRun0,fRun1); }

  void resetDetectorStatus() { fModules.clear(); }

 private:

  std::map<int, SiPixelModuleStatus> fModules;
  //std::map<int, SiPixelModuleStatus>::iterator fNext;

  /// first and last lumisection seen in this instance
  int fLS0, fLS1;

  /// first and last run seen in this instance (likely to be the same number!)
  int fRun0, fRun1;

  /// average (per module) number of hits over entire detector
  int fDetAverage, fDetSigma;

  /// total hits in detector
  unsigned long int fDetHits;

};

#endif
