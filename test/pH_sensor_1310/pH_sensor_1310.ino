// DFRobot SEN0161 pH Sensor Test Code for Arduino MKR WAN 1310
// Connect pH sensor to analog pin A1
// Displays ADC value, voltage, and pH
// Calibrated with user-provided values: pH 4.0 (1.73V, ADC 536), pH 7.0 (1.98V, ADC 615), pH 10.01 (2.21V, ADC 687)

const int pH_Pin = A1; // Analog input pin for pH sensor
float pH_Voltage = 0.0;
float pH_Value = 0.0;

// Calibration values from user measurements
const float pH4_Voltage = 1.73;  // Voltage at pH 4.0
const float pH7_Voltage = 1.98;  // Voltage at pH 7.0
const float pH10_Voltage = 2.21; // Voltage at pH 10.01

// Linear interpolation parameters
const float m = (7.0 - 4.0) / (pH7_Voltage - pH4_Voltage); // Slope for pH 4 to 7
const float b = 7.0 - m * pH7_Voltage; // Intercept for pH 4 to 7

void setup() {
  Serial.begin(9600); // Start serial communication
  pinMode(pH_Pin, INPUT); // Set A1 as input
  delay(1000); // Wait for sensor stabilization
  Serial.println("pH Sensor Test - DFRobot SEN0161");
  Serial.println("ADC\tVoltage\tpH");
}

void loop() {
  // Read the analog value (0-1023 for MKR WAN 1310)
  int sensorValue = analogRead(pH_Pin);
  
  // Convert to voltage (MKR WAN 1310 uses 3.3V reference)
  pH_Voltage = sensorValue * (3.3 / 1024.0); // Convert to voltage
  
  // Convert voltage to pH using linear interpolation
  if (pH_Voltage >= pH7_Voltage) { // pH 7 to 10 range
    float m2 = (10.01 - 7.0) / (pH10_Voltage - pH7_Voltage); // Slope for pH 7 to 10
    float b2 = 7.0 - m2 * pH7_Voltage; // Intercept for pH 7 to 10
    pH_Value = m2 * pH_Voltage + b2;
  } else { // pH 4 to 7 range
    pH_Value = m * pH_Voltage + b;
  }
  
  // Ensure pH value is within 0-14 range
  if (pH_Value < 0) pH_Value = 0;
  if (pH_Value > 14) pH_Value = 14;

  // Output results to Serial Monitor
  Serial.print(sensorValue); // ADC value
  Serial.print("\t");
  Serial.print(pH_Voltage, 3); // Voltage with 3 decimal places
  Serial.print("\t");
  Serial.println(pH_Value, 2); // pH with 2 decimal places
  
  delay(1000); // Wait 1 second between readings
}