#include <spdlog/spdlog.h>

#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Formats.hpp>

#include "soapy_rfnm.h"
#include <librfnm/librfnm.h>


SoapyRFNM::SoapyRFNM(const SoapySDR::Kwargs& args) {

    spdlog::info("RFNMDevice::RFNMDevice()");

    if (args.count("serial") != 0) {
        lrfnm = new librfnm(LIBRFNM_TRANSPORT_USB, (std::string)args.at("serial"));
    }
    else {
        lrfnm = new librfnm(LIBRFNM_TRANSPORT_USB);
    }

    if (!lrfnm->librfnm_s->transport_status.theoretical_mbps) {
        throw std::runtime_error("Couldn't open the RFNM USB device handle");
    }
}

SoapyRFNM::~SoapyRFNM() {
    spdlog::info("RFNMDevice::~RFNMDevice()");
}


std::string SoapyRFNM::getDriverKey() const {
    spdlog::info("RFNMDevice::getDriverKey()");
    return { "RFNM" };
}

std::string SoapyRFNM::getHardwareKey() const {
    spdlog::info("RFNMDevice::getHardwareKey()");
    return { "RFNM" };
}

SoapySDR::Kwargs SoapyRFNM::getHardwareInfo() const {
    spdlog::info("RFNMDevice::getHardwareInfo()");
    return {};
}

size_t SoapyRFNM::getStreamMTU(SoapySDR::Stream* stream) const {
    return 101001;
}

size_t SoapyRFNM::getNumChannels(const int direction) const {
    return direction;
}

std::vector<double> SoapyRFNM::listSampleRates(const int direction, const size_t channel) const {
    std::vector<double> rates;
    rates.push_back(122880000);
    return rates;
}

std::string SoapyRFNM::getNativeStreamFormat(const int direction, const size_t /*channel*/, double& fullScale) const {

    fullScale = 32768;
    return SOAPY_SDR_CS16;
}

std::vector<std::string> SoapyRFNM::getStreamFormats(const int direction, const size_t channel) const {
    std::vector<std::string> formats;
    formats.push_back(SOAPY_SDR_CS16);
    formats.push_back(SOAPY_SDR_CF32);
    formats.push_back(SOAPY_SDR_CS8);
    return formats;
}







int SoapyRFNM::activateStream(SoapySDR::Stream* stream, const int flags, const long long timeNs,
    const size_t numElems) {
    spdlog::info("RFNMDevice::activateStream()");


    return 0;
}

int SoapyRFNM::deactivateStream(SoapySDR::Stream* stream, const int flags0, const long long int timeNs) {
    spdlog::info("RFNMDevice::deactivateStream()");


    return 0;
}


void SoapyRFNM::setFrequency(int direction, size_t channel, double frequency, const SoapySDR::Kwargs& args)
{
    volatile rfnm_api_failcode fail;

    lrfnm->librfnm_s->rx.ch[0].freq = frequency;
    fail = lrfnm->set(LIBRFNM_APPLY_CH0_RX /*| LIBRFNM_APPLY_CH0_TX  | LIBRFNM_APPLY_CH1_RX*/);


}

void SoapyRFNM::setGain(const int direction, const size_t channel, const double value)
{
    volatile rfnm_api_failcode fail;

    lrfnm->librfnm_s->rx.ch[0].gain = value;
    fail = lrfnm->set(LIBRFNM_APPLY_CH0_RX /*| LIBRFNM_APPLY_CH0_TX  | LIBRFNM_APPLY_CH1_RX*/);
}

SoapySDR::Range SoapyRFNM::getGainRange(const int direction, const size_t channel) const
{
    return SoapySDR::Range(-100, 100);
}

void SoapyRFNM::setBandwidth(const int direction, const size_t channel, const double bw)
{
    if (bw == 0.0) return; //special ignore value

    volatile rfnm_api_failcode fail;

    lrfnm->librfnm_s->rx.ch[0].iq_lpf_bw = bw / 1e6;
    fail = lrfnm->set(LIBRFNM_APPLY_CH0_RX /*| LIBRFNM_APPLY_CH0_TX  | LIBRFNM_APPLY_CH1_RX*/);

}

SoapySDR::RangeList SoapyRFNM::getBandwidthRange(const int direction, const size_t channel) const
{
    SoapySDR::RangeList bws;
    bws.push_back(SoapySDR::Range(1e6, 100e6));
    return bws;
}

std::vector<std::string> SoapyRFNM::listAntennas(const int direction, const size_t channel) const
{
    std::vector<std::string> ants;
    if (direction == SOAPY_SDR_RX) {
        for (int a = 0; a < 10; a++) {
            if (lrfnm->librfnm_s->rx.ch[0].path_possible[a] == RFNM_PATH_NULL) {
                break;
            }
            ants.push_back(librfnm::rf_path_to_string(lrfnm->librfnm_s->rx.ch[0].path_possible[a]));
        }
    }
    else if (direction == SOAPY_SDR_TX) {
     //   ants.push_back("TXH");
    //    ants.push_back("TXW");
    }
    return ants;
}

void SoapyRFNM::setAntenna(const int direction, const size_t channel, const std::string& name)
{
    name.c_str();

    volatile rfnm_api_failcode fail;

    lrfnm->librfnm_s->rx.ch[0].path = librfnm::string_to_rf_path(name);
    fail = lrfnm->set(LIBRFNM_APPLY_CH0_RX /*| LIBRFNM_APPLY_CH0_TX  | LIBRFNM_APPLY_CH1_RX*/);


}






SoapySDR::Stream* SoapyRFNM::setupStream(const int direction, const std::string& format, const std::vector<size_t>& channels, const SoapySDR::Kwargs& args) {


    

    lrfnm->librfnm_s->rx.ch[0].enable = RFNM_CH_ON;
    lrfnm->librfnm_s->rx.ch[0].freq = RFNM_MHZ_TO_HZ(2450);
    lrfnm->librfnm_s->rx.ch[0].path = lrfnm->librfnm_s->rx.ch[0].path_preferred;
    lrfnm->librfnm_s->rx.ch[0].samp_freq_div_n = 1;

    //librfnm_s->rx.ch[1].enable = RFNM_CH_ON;
    //librfnm_s->rx.ch[1].freq = RFNM_MHZ_TO_HZ(2450);
    //librfnm_s->rx.ch[1].path = librfnm_s->rx.ch[1].path_preferred;
    //librfnm_s->tx.ch[1].samp_freq_div_n = 2;

    //librfnm_s->tx.ch[0].enable = RFNM_CH_ON;
    //librfnm_s->tx.ch[0].freq = RFNM_MHZ_TO_HZ(2450);
    //librfnm_s->tx.ch[0].path = librfnm_s->tx.ch[0].path_preferred;
    //librfnm_s->tx.ch[0].samp_freq_div_n = 2;

    volatile rfnm_api_failcode fail;
    fail = lrfnm->set(LIBRFNM_APPLY_CH0_RX /*| LIBRFNM_APPLY_CH0_TX  | LIBRFNM_APPLY_CH1_RX*/);
    

    if (!format.compare(SOAPY_SDR_CF32)) {
        //m_outbuf.format = format;
        //m_outbuf.bytes_per_sample = SoapySDR_formatToSize(SOAPY_SDR_CF32);
        lrfnm->rx_stream(LIBRFNM_STREAM_FORMAT_CF32, &outbufsize);
    }
    else if (!format.compare(SOAPY_SDR_CS16)) {
        //m_outbuf.format = format;
        //m_outbuf.bytes_per_sample = SoapySDR_formatToSize(SOAPY_SDR_CS16);
        lrfnm->rx_stream(LIBRFNM_STREAM_FORMAT_CS16, &outbufsize);
    }
    else if (!format.compare(SOAPY_SDR_CS8)) {
        //m_outbuf.format = format;
        //m_outbuf.bytes_per_sample = SoapySDR_formatToSize(SOAPY_SDR_CS8);
        lrfnm->rx_stream(LIBRFNM_STREAM_FORMAT_CS8, &outbufsize);
    }
    else {
        throw std::runtime_error("setupStream invalid format " + format);
    }

    
    //lrfnm->tx_stream(LIBRFNM_STREAM_FORMAT_CS16, &inbufsize);


    std::queue<struct librfnm_tx_buf*> ltxqueue;
    //std::queue<struct librfnm_rx_buf*> lrxqueue;

    for (int i = 0; i < SOAPY_RFNM_BUFCNF; i++) {
        rxbuf[i].buf = (uint8_t*)malloc(outbufsize);
        lrfnm->rx_qbuf(&rxbuf[i]);
        //txbuf[i].buf = rxbuf[i].buf;
        //txbuf[i].buf = (uint8_t*)malloc(inbufsize);

        //ltxqueue.push(&txbuf[i]);
    }

    spdlog::info("outbufsize {} inbufsize {} ", outbufsize, inbufsize);



    return (SoapySDR::Stream*)this;
}

void SoapyRFNM::closeStream(SoapySDR::Stream* stream) {
    spdlog::info("RFNMDevice::closeStream() -> Closing stream");
}




int SoapyRFNM::readStream(SoapySDR::Stream* stream, void* const* buffs, const size_t numElems, int& flags,
    long long int& timeNs, const long timeoutUs) {

    long total_data, data_diff;
    double time_diff, m_readstream_time_diff;

    int bytes_per_ele = lrfnm->librfnm_s->transport_status.rx_stream_format;

    std::chrono::time_point<std::chrono::system_clock> m_readstream_start_time = std::chrono::system_clock::now();

    struct librfnm_rx_buf* lrxbuf;
    rfnm_api_failcode err;
    int read_elems = 0;

keep_waiting:

    

    

    while (!lrfnm->rx_dqbuf(&lrxbuf, LIBRFNM_CH0 /* | LIBRFNM_CH1*/, 0)) {

        std::memcpy(((uint8_t*)buffs[0]) + (outbufsize * read_elems), lrxbuf->buf, outbufsize);

        lrfnm->rx_qbuf(lrxbuf);

        read_elems += (outbufsize / bytes_per_ele);

        if (read_elems >= numElems) {
            break;
        }
    }


    if (read_elems < numElems) {

        m_readstream_time_diff = std::chrono::duration<double>(std::chrono::system_clock::now() - m_readstream_start_time).count();
        if (m_readstream_time_diff < ((double)timeoutUs) / 1000000.0) {
            goto keep_waiting;
        }
    }

    //spdlog::info("read {} elems", numElems);
    return read_elems;
}




SoapySDR::Device* rfnm_device_create(const SoapySDR::Kwargs& args)
{
    spdlog::info("rfnm_device_create()");



    return new SoapyRFNM(args);
}




SoapySDR::KwargsList rfnm_device_find(const SoapySDR::Kwargs &args) {
    
    std::vector<struct rfnm_dev_hwinfo> hwlist = librfnm::find(LIBRFNM_TRANSPORT_USB);
    std::vector< SoapySDR::Kwargs> ret;

    for (auto& hw: hwlist)
    {
        SoapySDR::Kwargs deviceInfo;

        deviceInfo["device_id"] = "RFNM";
        deviceInfo["label"] = "RFNM";
        if (hw.daughterboard[0].board_id) {
            std::string dgbn = reinterpret_cast<char*>(hw.daughterboard[0].user_readable_name);
            deviceInfo["label"] += " with " + dgbn;
        }

        if (hw.daughterboard[1].board_id) {
            std::string dgbn = reinterpret_cast<char*>(hw.daughterboard[1].user_readable_name);
            deviceInfo["label"] += " and " + dgbn + " daughterboards";
        }
        else {
            deviceInfo["label"] += " daughterboard";
        }

        std::string serial = reinterpret_cast<char*>(hw.motherboard.serial_number);
        deviceInfo["serial"] = serial;

        ret.push_back(deviceInfo);
    }

    return ret;
}

[[maybe_unused]] static SoapySDR::Registry rfnm_module_registration("RFNM", &rfnm_device_find, &rfnm_device_create, SOAPY_SDR_ABI_VERSION);