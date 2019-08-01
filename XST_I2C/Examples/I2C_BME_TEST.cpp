// Variable Declarations
volatile uint32_t i2c_irq_triggered = 0;
uint8_t dataToWrite = 0;
uint8_t i, tStandby, filter, humidOverSample, tempOverSample, pressOverSample, runMode;
//test procedures
void interrupt_test(void);
void read_sensor_test(void);
int32_t t1,t2,t3,adct;
float hasil, T;
int var1,var2,t_fine;

#define BME280 I2C0
#define addr 0x77

XST_UART u(UART2);
XST_I2C i(BME280);

int main(void)
{
	// UART init
	u.begin(9600);
	u.print("I2C Test\n");
	
    //I2C init
	u.println("Initiliaze I2C");
	if (i.init (25000000, 1) == I2C_ERROR) 
        return I2C_ERROR;
	
    //test procedures
	read_sensor_test();
	u.println("Test Passed");
	
    // End simulation
	u.EndSimulation();
	return 0;
}

float jadiTempC(int32_t adc_T, int32_t T1, int32_t T2, int32_t T3 ){
    
    adc_T >>= 4;

    var1 = ((((adc_T>>3) - (T1<<1))) * (T2)) >> 11;
             
    var2 = (((((adc_T>>4) - (T1)) * ((adc_T>>4) - (T1))) >> 12) * (T3)) >> 14;
    t_fine = var1 + var2;

    T = (t_fine * 5 + 128) >> 8;
    return T/100;
}

void read_sensor_test(void){
	u.println("BME280 Sensor Test");
	u.println("Sensor Configuration Begin");
	i.write(addr, BME280_CTRL_MEAS_REG, 0x00);
	runMode = 3;
	tStandby = 0;
	filter = 0;
	tempOverSample = 1;
	pressOverSample = 1;
	humidOverSample = 1;
	//set the config
	
    dataToWrite = (tStandby << 0x05) & 0xE0;
	dataToWrite |= (filter << 0x02) & 0x1C;
	i.write(addr, BME280_CONFIG_REG, dataToWrite);
	
    //set ctrl_hum first, then ctrl_meas to activate ctrl_hum
	dataToWrite = humidOverSample & 0x07;
	i.write(addr, BME280_CTRL_HUMIDITY_REG, dataToWrite);
	
    //set ctrl_meas
	//First, set temp oversampling
	dataToWrite = (tempOverSample << 0x05) & 0xE0;
	//Next, pressure oversampling
	dataToWrite |= (pressOverSample << 0x02) & 0x1C;
	//Last, set mode
	dataToWrite |= (runMode) & 0x03;
	//Load the byte
	i.write(addr, BME280_CTRL_MEAS_REG, dataToWrite);
	u.println("Sensor Configuration Done");
	u.println("Begin Reading Registers");
	
    u.print("ID (0xD0): 0x"); u.println(i.read(0x77, BME280_CHIP_ID_REG),HEX);
	u.print("Reset register (0xE0): 0x"); u.println(i.read(0x77, BME280_RST_REG),HEX);
	u.print("ctrl_meas (0xF4): 0x"); u.println(i.read(0x77, BME280_CTRL_MEAS_REG),HEX);
	u.print("ctrl_hum (0xF2): 0x"); u.println(i.read(0x77, BME280_CTRL_HUMIDITY_REG),HEX);
	u.print("Temp MSB (0xFA): 0x"); u.println(i.read(0x77, 0xFA),HEX);
	u.print("Temp LSB (0xFB): 0x"); u.println(i.read(0x77, 0xFB),HEX);
	u.print("Temp XLSB (0xFC): 0x"); u.println(i.read(0x77, 0xFC),HEX);

	u.print("DIG T1 MSB-LSB: 0x"); u.print(i.read(0x77, 0x89),HEX); u.println(i.read(0x77, 0x88),HEX);
	u.print("DIG T2 MSB-LSB: 0x"); u.print(i.read(0x77, 0x8B),HEX); u.println(i.read(0x77, 0x8A),HEX);
	u.print("DIG T3 MSB-LSB: 0x"); u.print(i.read(0x77, 0x8D),HEX); u.println(i.read(0x77, 0x8C),HEX);
	
	adct = (int32_t)(i.read(0x77, 0xFA)<<16)+ (int32_t)(i.read(0x77, 0xFB)<<8) + (int32_t)(i.read(0x77, 0xFC));
	t1 = (int32_t)(i.read(0x77, 0x89)<<8)+ (int32_t)(i.read(0x77, 0x88));
	t2 = (int32_t)(i.read(0x77, 0x8B)<<8)+ (int32_t)(i.read(0x77, 0x8A));
	t3 = (int32_t)(i.read(0x77, 0x8D)<<8)+ (int32_t)(i.read(0x77, 0x8C));
	u.print("adc : ");u.println(adct);
	u.print("t1 : ");u.println(t1);
	u.print("t2 : ");u.println(t2);
	u.print("t3 : ");u.println(t3);
	
	hasil = jadiTempC(adct, t1, t2, t3);
	u.print("temperaturnya : ");u.println(hasil);
	/*
	printf("Press MSB (0xF7): 0x%x\n", i.read(0x77, 0xF7));
	printf("Press LSB (0xF8): 0x%x\n", i.read(0x77, 0xF8));
	printf("Press XLSB (0xF9): 0x%x\n", i.read(0x77, 0xF9));
	printf("Hum MSB (0xFD): 0x%x\n", i.read(0x77, 0xFD));
	printf("Hum LSB (0xFE): 0x%x\n", i.read(0x77, 0xFE));
	puts("Read BME280 Sensor Test Done");  */
}