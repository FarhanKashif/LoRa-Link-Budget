#ifndef SX127X_H
#define SX127X_H

#include <Arduino.h>
#include <SPI.h>

class Sx127x {
private:
    // SPI Pins
    uint8_t SS, MOSI, MISO, SCK;

    // LoRa Parameters
    uint8_t SF;              // Spreading Factor

    float freq;              // MHz
    float SNR;               // dB
    float coding_rate;       // e.g. 4/5, 4/6
    float bandwidth;         // kHz
    float noise_floor;       // dBm
    float snr_margin;        // dB

    /* Change this value if you have different antenna with different gain */
    int antenna_gain = 3;    // dBi (omnidirectional antenna - 433MHz)

    bool freq_flag = false;

    // SX127x Registers (LoRa mode)
    enum Register {
        REG_MSBFRQ_ADDR   = 0x06,
        REG_MIDFRQ_ADDR   = 0x07,
        REG_LSBFRQ_ADDR   = 0x08,
        REG_PA_ADDR       = 0x09,
        REG_PACKET_SNR    = 0x19,
        REG_PACKET_RSSI   = 0x1A,
        REG_MODEM_CONFIG  = 0x1D,
        REG_MODEM_CONFIG2 = 0x1E,
        REG_RSSI          = 0x1B
    };

    // Internal helpers
    void select();
    void deselect();
    void update();

public:
    // Initialization
    void init(uint8_t ss, uint8_t mosi, uint8_t miso, uint8_t sck);

    // SPI Read
    uint8_t read(uint8_t addr);

    // RF Parameter Readers
    float Read_Output_Power();   // dBm
    float Read_Frequency();      // Hz
    float Packet_SNR();          // dB
    float Packet_RSSI();         // dBm
    float Sig_BW();              // kHz
    void  Spreading_Factor();    // sets SF

    // RF Metrics
    float Noise_Floor();             // dBm
    float Receiver_Sensitivity();    // dBm
    float Fade_Margin();             // dB
    float Path_Loss();               // dB
    float EIRP();                   // dBm
};

#endif