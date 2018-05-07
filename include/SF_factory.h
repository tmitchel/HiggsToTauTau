

class SF_factory {
private:
  TH1D* etaBinsH;
  double nEtaBins;
  std::map<std::string, TGraphAsymmErrors*> eff_data, eff_mc;

public:
  SF_factory (std::string);
  virtual ~SF_factory () {};

  void SetAxisBins(TGraphAsymmErrors*);
  bool checkBinning(TGraphAsymmErrors*, TGraphAsymmErrors*);
  double getDataEfficiency(double pt, double eta);
  double getMCEfficiency(double pt, double eta);
  double getSF(double pt, double eta);
  int getPtBin(TGraphAsymmErrors*, double);
  std::string getEtaLabel(std::string, double);
};


SF_factory::SF_factory(std::string fname) {

  TFile* fin = new TFile(fname.c_str(), "read");
  std::string prefix = "ZMass";
  std::string data_name, mc_name, eta_label;
  etaBinsH = (TH1D*)fin->Get("etaBinsH");
  nEtaBins = etaBinsH->GetNbinsX();

  for (int ibin = 0; ibin < nEtaBins; ibin++) {
    eta_label = etaBinsH->GetXaxis()->GetBinLabel(ibin+1);
    data_name = prefix+eta_label+"_Data";
    mc_name = prefix+eta_label+"_MC";

    if (fin->GetListOfKeys()->Contains(data_name.c_str())) {
      eff_data[eta_label] = (TGraphAsymmErrors*)fin->Get(data_name.c_str());
      SetAxisBins(eff_data[eta_label]);
    }
    else
      eff_data[eta_label] = 0;

    if (fin->GetListOfKeys()->Contains(mc_name.c_str())) {
      eff_mc[eta_label] = (TGraphAsymmErrors*)fin->Get(mc_name.c_str());
      SetAxisBins(eff_mc[eta_label]);
    }
    else
      eff_mc[eta_label] = 0;

    if (eff_data[eta_label] != 0 && eff_mc[eta_label] != 0) {
      if (!checkBinning(eff_data[eta_label], eff_mc[eta_label]))
        std::cout << "ERROR in ScaleFactor::ScaleFactor(std::string)" << std::endl;
    }

  }
}

void SF_factory::SetAxisBins(TGraphAsymmErrors* graph){
   int NPOINTS = graph->GetN();
   double AXISBINS [NPOINTS+1] = {};
   for (int i=0; i<NPOINTS; i++) { AXISBINS[i] = (graph->GetX()[i] - graph->GetErrorXlow(i)); }
   AXISBINS[NPOINTS] = (graph->GetX()[NPOINTS-1] + graph->GetErrorXhigh(NPOINTS-1));
   graph->GetXaxis()->Set(NPOINTS, AXISBINS);
}

bool SF_factory::checkBinning(TGraphAsymmErrors* g1, TGraphAsymmErrors* g2) {
  if (g1->GetXaxis()->GetNbins() != g2->GetXaxis()->GetNbins())
    return false;
  else {
    for (int ibin = 0; ibin < g1->GetXaxis()->GetNbins(); ibin++) {
      if ( (g1->GetXaxis()->GetXbins())->GetArray()[ibin] != (g2->GetXaxis()->GetXbins())->GetArray()[ibin] )
        return false;
    }
  }
  return true;
}

int SF_factory::getPtBin(TGraphAsymmErrors* eff, double pt) {
  int npoint = eff->GetN();
  double pt_highest = eff->GetX()[npoint-1]+eff->GetErrorXhigh(npoint-1);
  double pt_lowest = eff->GetX()[0]-eff->GetErrorXlow(0);
  if (pt >= pt_highest)
    return npoint;
  else if (pt < pt_lowest)
    return -9999;
  else
    return eff->GetXaxis()->FindFixBin(pt);
}

std::string SF_factory::getEtaLabel(std::string name, double eta) {
  eta = fabs(eta);
  int bin = etaBinsH->GetXaxis()->FindFixBin(eta);
  std::string etaLabel = etaBinsH->GetXaxis()->GetBinLabel(bin);
  std::map<std::string, TGraphAsymmErrors*>::iterator it;
  if (name == "data") {
    it = eff_data.find(etaLabel);
  }
  else if (name == "mc") {
    it = eff_mc.find(etaLabel);
  }
  return etaLabel;
}

double SF_factory::getDataEfficiency(double pt, double eta) {
  double eff;
  auto label = getEtaLabel("data", eta);
  int ptbin = getPtBin(eff_data[label], pt);
  if (ptbin == -9999)
    eff = 1;
  else
    eff = eff_data[label]->GetY()[ptbin-1];

  return eff;
}

double SF_factory::getMCEfficiency(double pt, double eta) {
  double eff;
  auto label = getEtaLabel("mc", eta);
  int ptbin = getPtBin(eff_mc[label], pt);
  if (ptbin == -9999)
    eff = 1;
  else
    eff = eff_mc[label]->GetY()[ptbin-1];

  return eff;
}

double SF_factory::getSF(double pt, double eta) {
  double SF;
  auto data_eff = getDataEfficiency(pt, eta);
  auto mc_eff = getMCEfficiency(pt, eta);

  if (mc_eff != 0)
    SF = data_eff/mc_eff;
  else
    SF = 0;

  return SF;
}
