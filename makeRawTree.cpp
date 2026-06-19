#include <iostream>
#include <vector>
#include <string>

#include "lecroyparser.h"
#include "AndCommon.h"

#include "TFile.h"
#include "TTree.h"

int main(int argc, char* argv[]){
    if(argc != 3){
        std::cout << "USAGE: ./makeRawTree folder file.trc" << std::endl;
        return 1;
    }

    std::string folder(argv[1]);
    std::string filename(argv[2]);

    lecroyparser::lecroy_wavedesc_2_3 header;
    int16_t* waveform = nullptr;

    std::cout << "Reading " << filename << std::endl;

    bool ok = lecroyparser::read(filename, header, waveform);

    if(!ok || waveform == nullptr){
        std::cerr << "Error reading file." << std::endl;
        return 1;}

    uint32_t nWaveforms = (header.subarray_count > 0) ? header.subarray_count : 1;

    uint32_t pointsPerWave = header.wave_array_count / nWaveforms;

    std::cout << "Template          : " << header.template_name << std::endl;
    std::cout << "Waveforms found   : " << nWaveforms << std::endl;
    std::cout << "Points/waveform   : " << pointsPerWave << std::endl;
    std::cout << "Sampling interval : " << header.horiz_interval << " s" << std::endl;

    std::string outname = filename;

    size_t pos = outname.rfind(".trc");
    if(pos != std::string::npos)
        outname.replace(pos,4,"_treeraw.root");
    else
        outname += ".root";

    outname.replace(outname.find("/trc/"), 5, "/root/");

    system(Form("mkdir -p data/root/%s", folder.c_str()));
    TFile* outfile = TFile::Open(Form("%s", outname.c_str()),"RECREATE");
    TTree* tree = new TTree("treeraw","treeraw");

    int event = 0;
    int lumi;
    float sampling_time = header.horiz_interval;
    double trigtime_d   = header.trigger_time.days;
    double trigtime_h   = header.trigger_time.hours;
    double trigtime_m   = header.trigger_time.minutes;
    double trigtime_s   = header.trigger_time.seconds;

    std::vector<float> pshape;

    //calcolo lumi
    std::vector<std::string> parts = AndCommon::splitString(filename, "/");
    std::string file_only = parts.back();
    file_only = AndCommon::splitString(file_only, ".")[0];
    std::string number_part = file_only.substr(file_only.size() - 5);
    lumi = std::stoi(number_part);
    
    tree->Branch("event"         , &event        ,         "event/I");
    tree->Branch("lumi"          , &lumi         ,          "lumi/I");
    //points per waveform
    tree->Branch("npoints"       , &pointsPerWave,       "npoints/I");
    tree->Branch("sampling_time" , &sampling_time, "sampling_time/F");
    tree->Branch("trigtime_d"    , &trigtime_d   ,    "trigtime_d/D");
    tree->Branch("trigtime_h"    , &trigtime_h   ,    "trigtime_h/D");
    tree->Branch("trigtime_m"    , &trigtime_m   ,    "trigtime_m/D");
    tree->Branch("trigtime_s"    , &trigtime_s   ,    "trigtime_s/D");
    tree->Branch("waveform",&pshape);

    for(uint32_t iWave = 0; iWave < nWaveforms; ++iWave){
        pshape.clear();
        pshape.reserve(pointsPerWave);
        uint32_t offset = iWave * pointsPerWave;

        for(uint32_t i = 0; i < pointsPerWave; ++i){
            float value = waveform[offset+i] * header.vertical_gain - header.vertical_offset;
            pshape.push_back(value);
        }

        event = iWave;
        tree->Fill();
    }

    outfile->cd();
    tree->Write();
    outfile->Close();

    delete[] waveform;

    std::cout << "Saved " << outname << std::endl;

    return 0;
}
