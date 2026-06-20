/*
  ESP32 LoRa Receiver
  Using SX127XLT Library to send/receive packets. You can change the library according to your needs
  LoRa Modulation
*/

#include <SPI.h>
#include <SX127XLT.h>
#include "Sx127x.h"

// LoRa module pin connections for ESP32
#define NSS     5
#define NRESET  4
#define DIO0    2
#define LORA_DEVICE DEVICE_SX1278  // Change to your module (SX1276/SX1277/SX1278)

#define FREQUENCY                 433000000
#define OFFSET                    0
#define BANDWIDTH                 LORA_BW_125 // 125kHz
#define SPREADING_FACTOR          LORA_SF7
#define CODING_RATE               LORA_CR_4_5  // 4/5
#define HEADERMODE                0x00 // Explicit Header Modes
#define PA_OUTPUT_PA_BOOST_PIN    5
#define RAMP_TIME                 0x02
#define TX_POWER                  14
// Define Device
#define LORA_DEVICE DEVICE_SX1278
SX127XLT LT;
Sx127x lora;
uint8_t rxBuffer[255];

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(1000);
  
  Serial.println(F("\n=== ESP32 LoRa RX ==="));

  lora.init(5, 23, 19, 18);

  SPI.begin(); 
  if (LT.begin(NSS, NRESET, DIO0, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found!"));
  }
  else
  {
    Serial.println(F("No device!"));
    while (1);
  }
  
    // Frequency: 433MHz,  Bandwidth: 250kHz, Spreading Factor: 7 
  LT.setMode(MODE_STDBY_RC);
  LT.setPacketType(PACKET_TYPE_LORA); // Set LoRa packet type
  LT.setRfFrequency(FREQUENCY, OFFSET); // Set frequency and offset
  LT.calibrateImage(0); // Calibrate image for frequency
  LT.setModulationParams(SPREADING_FACTOR, BANDWIDTH, CODING_RATE, LDRO_AUTO); // Set modulation parameters
  LT.setBufferBaseAddress(0x00, 0x00); // Set buffer base address for TX and RX
  LT.setPacketParams(8, LORA_PACKET_VARIABLE_LENGTH, 255, LORA_CRC_ON, LORA_IQ_NORMAL);
  //LT.setSyncWord(LORA_SYNCWORD); // Set sync word for private LoRa network
  LT.setHighSensitivity(); // Enable high sensitivity mode
  LT.setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0);
  
  Serial.println(F("\nListening...\n"));
}

void loop()
{
  uint8_t RXPacketL = LT.receive(rxBuffer, sizeof(rxBuffer), 5000, WAIT_RX);

  if (RXPacketL > 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    
    int16_t RSSI = LT.readPacketRSSI();
    //int8_t snr = LT.readPacketSNR();

    Serial.println("PACKET RECEIVED\n\n\n");
    
    Serial.print(F("Data: \""));
    for (uint8_t i = 0; i < RXPacketL; i++)
    {
      Serial.write(rxBuffer[i]);
    }
    Serial.println(F("\""));
        
    Serial.print(F("RSSI: ")); float rssi = lora.Packet_RSSI(); Serial.print(rssi); Serial.println(F(" dBm"));
    //Serial.print(F("SNR: ")); Serial.print(snr); Serial.println(F(" dB"));
    Serial.print("MY SNR: "); Serial.print(lora.Packet_SNR()); Serial.println(" dB");
    Serial.print("Bandwidth: "); float bw; bw = lora.Sig_BW(); Serial.print(bw); Serial.println(" KHz");
    Serial.print("Noise Floor: "); float noise_floor = lora.Noise_Floor(); Serial.print(noise_floor); Serial.println(" dBm");
    float sens = lora.Receiver_Sensitivity(); Serial.print("My Receiver Sens "); Serial.print(sens); Serial.println(" dBm");
    float fade_margin = lora.Fade_Margin(); Serial.print("Fade Margin: "); Serial.print(fade_margin); Serial.println(" dB");
    float path_loss = lora.Path_Loss(); Serial.print("Path Loss: "); Serial.print(path_loss); Serial.println(" dB");
    float freq = lora.Read_Frequency(); Serial.print("Freq: "); Serial.print(freq); Serial.println(" Hz");
    float eirp = lora.EIRP(); Serial.print("EIRP: "); Serial.print(eirp); Serial.println(" dBm");
    Serial.println();
    
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (RXPacketL == 0)
  {
    Serial.print(F("."));  // Timeout - still listening
  }
  else
  {
    Serial.println(F("\n  Error!"));
    uint16_t IRQStatus = LT.readIrqStatus();
    Serial.print(F("IRQ: 0x"));
    Serial.println(IRQStatus, HEX);
    
    if (IRQStatus & IRQ_CRC_ERROR)
      Serial.println(F("CRC Error - Settings mismatch?"));
  }
}
