#ifndef SIPIXELROCSTATUS_h
#define SIPIXELROCSTATUS_h


// ----------------------------------------------------------------------
class SiPixelRocStatus {
public:
  SiPixelRocStatus();
  ~SiPixelRocStatus();
  void fill(int idc);
  void fill(int idc, int hits);

  int statusDC(int idc);
  int statusROC();

  int nDC(){ return nDC_;}

private:
  const int nDC_ = 26;
  int fDC[26];

};

#endif
