//:Description: 
//:Synonyms::::
//:Source:  B. Lasiuk (trs)
//:Update:
//:Update frequncy:
//:Reminder:
//:Recall frequency:
//:Size of Data:
//:Pointer to data:  TPCElectronics.time:
struct tpcElectronics {

  //  type varnam;    //Units : Comments

  int    numberOfTimeBins;    // 
  double nominalGain;         // mV/fC
  double samplingFrequency;   // MHz
  double tZero;               // us (microseconds)
  double adcCharge;           // fC/adc count
  double adcConversion;       // mV/adc count
  double averagePedestal;     // adc counts
  double shapingTime;         // ns
  double tau;                 // ns
   
};

