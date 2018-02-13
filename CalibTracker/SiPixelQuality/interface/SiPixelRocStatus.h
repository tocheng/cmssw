#ifndef SIPIXELROCSTATUS_h
#define SIPIXELROCSTATUS_h


// ----------------------------------------------------------------------
class SiPixelRocStatus {
public:
  SiPixelRocStatus();
  ~SiPixelRocStatus();
  void fill(int idc);
  void fill(int idc, int hits);

  int status(int idc);
  int status();

private:
  const int nDC = 26;
  int fDC[26];

};

#endif
