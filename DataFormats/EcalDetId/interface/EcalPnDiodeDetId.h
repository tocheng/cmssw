#ifndef ECALDETID_ECALPNDIODEDETID_H
#define ECALDETID_ECALPNDIODEDETID_H

#include <ostream>
#include <boost/cstdint.hpp>
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"


/** \class EcalPnDiodeDetId
    
   DetId for an Calo Trigger tower
   Packing:

   [31:28] Global det == ECAL
   [27:25] ECAL det == EcalLaserPnDiode
   [24:12] Not Used
   [10]    SubDetectorId: EB (1) ,EE (2)
   [9:4]   DCCId (relative to SubDetector. In barrel it is the SupermoduleId from 1-36)
   [3:0]   PnId (In barrel from 1-10 according CMS IN-2005/021)

   $Id: EcalPnDiodeDetId.h,v 1.3 2005/10/06 11:02:55 meridian Exp $
*/


class EcalPnDiodeDetId : public DetId {
 public:
  /** Constructor of a null id */
  EcalPnDiodeDetId();
  /** Constructor from a raw value */
  EcalPnDiodeDetId(uint32_t rawid);  
  /** \brief Constructor from signed EcalSubDetectorId, DCCId, PnId
   */
  EcalPnDiodeDetId(int EcalSubDetectorId, int DCCId, int PnId);
  /** Constructor from a generic cell id */
  EcalPnDiodeDetId(const DetId& id);
  /** Assignment from a generic cell id */
  EcalPnDiodeDetId& operator=(const DetId& id);

  /// get EcalSubDetectorId
  int iEcalSubDetectorId() const { return (id_ & 0x400 ) ? (EcalEndcap):(EcalBarrel); }
  /// get the DCCId
  int iDCCId() const { return (id_>>4) & 0x3F; }
  /// get the PnId
  int iPnId() const { return id_&0xF; }
  /// get a compact index for arrays [TODO: NEEDS WORK]
  int hashedIndex() const;

};

std::ostream& operator<<(std::ostream&,const EcalPnDiodeDetId& id);

#endif
