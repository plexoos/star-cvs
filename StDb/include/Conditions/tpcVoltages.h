#ifndef TPC_VOLTAGES_H
#define TPC_VOLTAGES_H
//:Description: tpc voltage values
//:Synonyms::::
//:Source:
//:Update:
//:Update frequency:
//:Reminder:
//:Recall frequency:
//:Size of Data:
//:Pointer to data: Conditions_tpc/tpcVoltages
struct tpcVoltages {

  //  type varnam;    //Units : Comments
  double membraneVoltage;                // volts
  double stripe181EastIFCVoltage;        // volts
  double stripe182EastIFCVoltage;        // volts
  double stripe181WestIFCVoltage;        // volts
  double stripe182WestIFCVoltage;        // volts
  double stripe181EastOFCVoltage;        // volts
  double stripe182EastOFCVoltage;        // volts
  double stripe181WestOFCVoltage;        // volts
  double stripe182WestOFCVoltage;        // volts

};
#endif
