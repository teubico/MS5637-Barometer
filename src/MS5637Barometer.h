enum OSR {
  OSR_256 = 0,
  OSR_512 = 1,
  OSR_1024 = 2,
  OSR_2048 = 3,
  OSR_4096 = 4,
  OSR_8192 = 5
};

enum CONV_TYP {
  PRESSURE = 0,
  TEMPERATURE = 1
};

class  {
private:
  uint16_t PSENS;
  uint16_t POFFSET;
  uint16_t TCS;
  uint16_t TCO;
  uint16_t TREF;
  uint16_t TEMPSENS;
  uint32_t D1;
  uint32_t D2;
  int32_t dT;
  int32_t TEMP;
  int64_t AOFF;
  int64_t SENS;
  int32_t TCP;
  enum OSR oversampling;
  boolean device_available;
  uint16_t readEEPROM(uint8_t addr);
public:
  MS5637Barometer();
  boolean init();
  void setOSR(enum OSR sampling);
  uint32_t readADC(enum CONV_TYP type,enum OSR sampling);
  int32_t readTCPressure();
};
