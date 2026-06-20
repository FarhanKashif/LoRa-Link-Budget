/**** FOR LORA MODULATION TECHNIQUE ONLY ******/

#include <cmath>
#include "Sx127x.h"

// ================= LoRa SELECTION =================
void Sx127x::select() {
    digitalWrite(SS, LOW);
}

void Sx127x::deselect() {
    digitalWrite(SS, HIGH);
}

// ================= INIT =================
void Sx127x::init(uint8_t ss, uint8_t mosi, uint8_t miso, uint8_t sck) {
    SS = ss;
    MOSI = mosi;
    MISO = miso;
    SCK = sck;

    pinMode(SS, OUTPUT);
    deselect();

    SPI.begin(SCK, MISO, MOSI, SS);
}

// ================= SPI READ =================
uint8_t Sx127x::read(uint8_t addr) {
    select();
    SPI.transfer(addr & 0x7F);  // Read Mode
    uint8_t val = SPI.transfer(0x00);
    deselect();
    return val;
}

// ================= UPDATE ALL =================
void Sx127x::update() {
    Read_Frequency();
    Spreading_Factor();
    Sig_BW();
    Packet_SNR();
}

// ================= TX POWER =================
float Sx127x::Read_Output_Power() {
    uint8_t value = read(REG_PA_ADDR);

    bool paBoost = (value >> 7) & 0x01;
    uint8_t maxPower = (value >> 4) & 0x07;
    uint8_t outputPower = value & 0x0F;

    if (!paBoost) {
        float P_max = 10.8 + 0.6 * maxPower;
        return P_max - (15 - outputPower);  // dBm
    } else {
        return 17 - (15 - outputPower);     // dBm
    }
}

// ================= FREQUENCY =================
float Sx127x::Read_Frequency() {
    uint8_t msb = read(REG_MSBFRQ_ADDR);
    uint8_t mid = read(REG_MIDFRQ_ADDR);
    uint8_t lsb = read(REG_LSBFRQ_ADDR);

    uint32_t frf = ((uint32_t)msb << 16) |
                   ((uint32_t)mid << 8)  |
                   lsb;

    float F_rf = (32000000.0f * frf) / 524288.0f;

    freq = F_rf / 1e6; // freq = MHz

    return F_rf;    // return Hz
}

// ================= SNR =================
float Sx127x::Packet_SNR() {
    int8_t raw_snr = (int8_t)read(REG_PACKET_SNR);

    SNR = raw_snr / 4.0f;
    return SNR; // dB
}

// ================= RSSI =================
float Sx127x::Packet_RSSI() {

    uint8_t raw = read(REG_PACKET_RSSI);
    Packet_SNR();
    float rssi;

    rssi = -164 + raw;

    if (SNR < 0) {
        rssi += SNR;
    }

    return rssi;    // dBm
}

// ================= SPREADING FACTOR =================
void Sx127x::Spreading_Factor() {
    uint8_t raw = read(REG_MODEM_CONFIG2);
    SF = (raw >> 4) & 0x0F;
}

// ================= BANDWIDTH =================
float Sx127x::Sig_BW() {
    uint8_t reg = read(REG_MODEM_CONFIG);

    uint8_t cr_raw = (reg >> 1) & 0x07; // coding rate
    uint8_t bw_raw = (reg >> 4) & 0x0F; // bandwidth

    switch (cr_raw) {
        case 1: coding_rate = 4.0f/5.0f; break;
        case 2: coding_rate = 4.0f/6.0f; break;
        case 3: coding_rate = 4.0f/7.0f; break;
        case 4: coding_rate = 4.0f/8.0f; break;
        default: coding_rate = 0; break;
    }

    switch (bw_raw) {   // KHz
        case 0x00: bandwidth = 7.8; break;
        case 0x01: bandwidth = 10.4; break;
        case 0x02: bandwidth = 15.6; break;
        case 0x03: bandwidth = 20.8; break;
        case 0x04: bandwidth = 31.25; break;
        case 0x05: bandwidth = 41.7; break;
        case 0x06: bandwidth = 62.5; break;
        case 0x07: bandwidth = 125; break;
        case 0x08: bandwidth = 250; break;
        case 0x09: bandwidth = 500; break;
        default: bandwidth = 0; break;
    }

    return bandwidth;   // KHz
}

// ================= NOISE FLOOR =================
float Sx127x::Noise_Floor() {
    Sig_BW();
    float bw_hz = bandwidth * 1000.0f;  // Hz
    // Noise Floor = -174dBm(thermal noise of room) + 10*log10(BW) + Noise Figure
    noise_floor = -174 + 10 * log10(bw_hz) + 6; 
    return noise_floor; // dBm
}

// ================= SENSITIVITY =================
float Sx127x::Receiver_Sensitivity() {
    float snr_limit;

    Spreading_Factor();
    Noise_Floor();

    switch (SF) {
        case 7: snr_limit = -7.5; break;
        case 8: snr_limit = -10; break;
        case 9: snr_limit = -12.5; break;
        case 10: snr_limit = -15; break;
        case 11: snr_limit = -17.5; break;
        case 12: snr_limit = -20; break;
        default: snr_limit = 0; break;
    }

    snr_margin = SNR - snr_limit;   // dB
    //Serial.print("SNR Margin: "); Serial.println(snr_margin);
    //Serial.print("SF: "); Serial.println(SF);
    //Serial.print("SNR Limit: "); Serial.print(snr_limit); Serial.println(" dB");
    // Sensitivity = -174 + 10*log10(BW) + Noise_Figure + SNR_limit 
    return (noise_floor + snr_limit);   // dBm
}

// ================= FADE MARGIN =================
float Sx127x::Fade_Margin() {
    /* A good Fade margin is > 10dB */
    update();
    // fade margin = RSSI - Sensitivity
    return Packet_RSSI() - Receiver_Sensitivity();  // dB
}

// ================= PATH LOSS =================
float Sx127x::Path_Loss() {
    // Path Loss = transmission power + tx_ant gain + rx_ant gain - packet RSSI - (FSPL & other losses not included yet)
    return Read_Output_Power() + antenna_gain + antenna_gain - Packet_RSSI();
}

// ================= EIRP =================
float Sx127x::EIRP() {
    // EIRP = transmission power - cable losses + antenna gain
    return Read_Output_Power() - 0.5 + antenna_gain;
}

