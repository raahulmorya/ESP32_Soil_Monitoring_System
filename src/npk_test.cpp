  // #include <Arduino.h>
  // #include <HardwareSerial.h>

  // // MAX485 control pins
  // #define RE 4
  // #define DE 5

  // // Sensor configuration from your datasheet
  // #define NPK_ADDRESS 0x01
  // #define READ_HOLDING_REGISTERS 0x03

  // // Register addresses from your datasheet
  // #define NITROGEN_REGISTER 0x001E   // 0x001E (30 in decimal)
  // #define PHOSPHORUS_REGISTER 0x001F // 0x001F (31 in decimal)
  // #define POTASSIUM_REGISTER 0x0020  // 0x0020 (32 in decimal)

  // // Function prototypes
  // float readNPKParameter(uint16_t registerAddress);
  // uint16_t calculateCRC(uint8_t *bytes, uint16_t length);
  // void printHex(uint8_t *data, uint8_t length);

  // void setup()
  // {
  //   Serial.begin(115200);
  //   Serial1.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17

  //   pinMode(RE, OUTPUT);
  //   pinMode(DE, OUTPUT);

  //   // Set to receive mode initially
  //   digitalWrite(DE, LOW);
  //   digitalWrite(RE, LOW);

  //   Serial.println("NPK Sensor Reader - Configured for Your Sensor");
  //   delay(1000); // Give sensor time to initialize
  // }

  // void loop()
  // {
  //   // Read and display all parameters
  //   Serial.println("\nReading NPK values...");

  //   float nitrogen = readNPKParameter(NITROGEN_REGISTER);
  //   Serial.print("Nitrogen: ");
  //   if (nitrogen >= 0)
  //     Serial.print(nitrogen);
  //   else
  //     Serial.print("Read failed");
  //   Serial.println(" mg/kg");

  //   float phosphorus = readNPKParameter(PHOSPHORUS_REGISTER);
  //   Serial.print("Phosphorus: ");
  //   if (phosphorus >= 0)
  //     Serial.print(phosphorus);
  //   else
  //     Serial.print("Read failed");
  //   Serial.println(" mg/kg");

  //   float potassium = readNPKParameter(POTASSIUM_REGISTER);
  //   Serial.print("Potassium: ");
  //   if (potassium >= 0)
  //     Serial.print(potassium);
  //   else
  //     Serial.print("Read failed");
  //   Serial.println(" mg/kg");

  //   delay(5000); // Wait 5 seconds between readings
  // }

  // float readNPKParameter(uint16_t registerAddress)
  // {
  //   // Create Modbus RTU request frame (8 bytes)
  //   uint8_t request[8] = {
  //       NPK_ADDRESS,               // Slave address
  //       READ_HOLDING_REGISTERS,    // Function code
  //       highByte(registerAddress), // Register address high byte
  //       lowByte(registerAddress),  // Register address low byte
  //       0x00,                      // Number of registers high byte
  //       0x01,                      // Number of registers low byte
  //       0x00,                      // CRC placeholder (low byte)
  //       0x00                       // CRC placeholder (high byte)
  //   };

  //   // Calculate CRC
  //   uint16_t crc = calculateCRC(request, 6);
  //   request[6] = lowByte(crc);
  //   request[7] = highByte(crc);

  //   // Debug print
  //   Serial.print("Sending request for register 0x");
  //   Serial.print(registerAddress, HEX);
  //   Serial.print(": ");
  //   printHex(request, sizeof(request));

  //   // Set to transmit mode
  //   digitalWrite(DE, HIGH);
  //   digitalWrite(RE, HIGH);
  //   delay(10);

  //   // Send request
  //   Serial1.write(request, sizeof(request));
  //   Serial1.flush();

  //   // Set back to receive mode
  //   digitalWrite(DE, LOW);
  //   digitalWrite(RE, LOW);
  //   delay(10);

  //   // Wait for response
  //   uint8_t response[7];
  //   uint8_t idx = 0;
  //   unsigned long startTime = millis();

  //   while (millis() - startTime < 1000 && idx < sizeof(response))
  //   {
  //     if (Serial1.available())
  //     {
  //       response[idx++] = Serial1.read();
  //     }
  //   }

  //   // Debug print
  //   if (idx > 0)
  //   {
  //     Serial.print("Received response: ");
  //     printHex(response, idx);
  //   }
  //   else
  //   {
  //     Serial.println("No response from sensor");
  //     return -1.0;
  //   }

  //   // Verify response
  //   if (idx >= 5 &&
  //       response[0] == NPK_ADDRESS &&
  //       response[1] == READ_HOLDING_REGISTERS &&
  //       response[2] == 0x02)
  //   {
  //     // Combine the two data bytes
  //     uint16_t rawValue = (response[3] << 8) | response[4];
  //     return rawValue; // Your datasheet shows values are already in mg/kg
  //   }

  //   return -1.0; // Return error value
  // }

  // // Modbus CRC calculation
  // uint16_t calculateCRC(uint8_t *bytes, uint16_t length)
  // {
  //   uint16_t crc = 0xFFFF;
  //   for (uint16_t pos = 0; pos < length; pos++)
  //   {
  //     crc ^= (uint16_t)bytes[pos];
  //     for (int i = 8; i != 0; i--)
  //     {
  //       if ((crc & 0x0001) != 0)
  //       {
  //         crc >>= 1;
  //         crc ^= 0xA001;
  //       }
  //       else
  //       {
  //         crc >>= 1;
  //       }
  //     }
  //   }
  //   return crc;
  // }

  // // Helper function to print hex values
  // void printHex(uint8_t *data, uint8_t length)
  // {
  //   for (int i = 0; i < length; i++)
  //   {
  //     if (data[i] < 0x10)
  //       Serial.print("0");
  //     Serial.print(data[i], HEX);
  //     Serial.print(" ");
  //   }
  //   Serial.println();
  // }