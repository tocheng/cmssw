#ifndef SIPIXELMODULESTATUS_h
#define SIPIXELMODULESTATUS_h

#include "CalibTracker/SiPixelQuality/interface/SiPixelRocStatus.h"

#include <vector>


// ----------------------------------------------------------------------
class SiPixelModuleStatus {

public:

  SiPixelModuleStatus(int det = 0, int nrocs = 16);
  ~SiPixelModuleStatus();

  /// fill with online coordinates
  void fill(int iroc, int idc);

  /// fill with online coordinates (nhit > 1)
  void fill(int iroc, int idc, int nhit);

  /// return DC status of a ROC (=hits on DC idc on ROC iroc)
  int status(int iroc, int idc);

  /// return ROC status (= hits on ROC iroc)
  int status(int iroc);

  /// return module status (= hits on module)
  int status();

  /// do dc counts analysis
  void analysis(double thr0 = 10, double rms = 1);

  /// get a ROC
  SiPixelRocStatus* getRoc(int i);

  /// accessors and setters
  int    detid();
  int    nrocs();
  void   setNrocs(int iroc);
  double average();
  double sigma();

  /// calculate (averaged over this module's ROCs) mean hit number and its sigma
  void occupancy();

private:

  int fDetid, fNrocs;
  double fModAverage, fModSigma;
  std::vector<SiPixelRocStatus> fRocs;

};

#endif
