// ESP32S with CQRobot TDS Sensor test code
// Connect the TDS Sensor to pin D12

// Define the TDS sensor pin
#define TDS_SENSOR_PIN 12
 
// Define calibration values
#define VREF 3.3      // analog reference voltage(Volt) of the ADC
#define TEMPERATURE 25 // current water temperature (℃)

// Global variables
int analogValue = 0;   // Raw analog reading
float tdsValue = 0;    // Calculated TDS value
float voltage = 0;     // Calculated voltage

void setup() {
  Serial.begin(115200);
  pinMode(TDS_SENSOR_PIN, INPUT);
  
  delay(2000);  // Give time for serial connection
  Serial.println("ESP32S TDS Sensor Test Code");
  Serial.println("Reading from sensor connected to D12");
  Serial.println("----------------------------------");
}

void loop() {
  // Read analog value
  analogValue = analogRead(TDS_SENSOR_PIN);
  
  // Convert to voltage (ESP32 has 12-bit ADC, so max value is 4095)
  voltage = analogValue * (VREF / 4095.0);
  
  // Temperature compensation formula: fFactor = 1.0 + 0.02 * (temperature - 25.0);
  float compensationCoefficient = 1.0 + 0.02 * (TEMPERATURE - 25.0);
  
  // Temperature compensation
  float compensatedVoltage = voltage / compensationCoefficient;
  
  // Convert voltage value to TDS value
  // TDS = (133.42 * compensatedVoltage * compensatedVoltage * compensatedVoltage 
  //        - 255.86 * compensatedVoltage * compensatedVoltage 
  //        + 857.39 * compensatedVoltage) * 0.5;
  tdsValue = (133.42 * compensatedVoltage * compensatedVoltage * compensatedVoltage 
             - 255.86 * compensatedVoltage * compensatedVoltage 
             + 857.39 * compensatedVoltage) * 0.5;
  
  Serial.print("TDS Value: ");
  Serial.print(tdsValue, 0);
  Serial.println(" ppm");
  
  delay(2000); // Read every 2 seconds
}