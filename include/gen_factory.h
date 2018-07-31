#include "TTree.h"

class gen_factory {
private:
  Float_t genpX, genpY, genM, genpT, numGenJets, genweight, gen_Higgs_pt, gen_Higgs_mass;
  Float_t npv, npu, rho, m_sv, pt_sv, m_vis, pt_tt, l2_decayMode;
  Int_t run, lumi;
  ULong64_t evt;

public:
  gen_factory (TTree*);
  virtual ~gen_factory () {};

  // getters
  float getGenPx()        { return genpX;           };
  float getGenPy()        { return genpY;           };
  float getGenM()         { return genM;            };
  float getGenPt()        { return genpT;           };
  float getNumGenJets()   { return numGenJets;      };
  float getGenWeight()    { return genweight;       };
  float getGenHiggsPt()   { return gen_Higgs_pt;    };
  float getGenHiggsMass() { return gen_Higgs_mass;  };

  // setters
  void setGenPx(float GenpX)                  { genpX = GenpX;                    };
  void setGenPy(float GenpY)                  { genpY = GenpY;                    };
  void setGenM(float GenM)                    { genM = GenM;                      };
  void setGenPt(float GenpT)                  { genpT = GenpT;                    };
  void setNumGenJets(float NumGenJets)        { numGenJets = NumGenJets;          };
  void setGenWeight(float Genweight)          { genweight = Genweight;            };
  void setGenHiggsPt(float Gen_Higgs_pt)      { gen_Higgs_pt = Gen_Higgs_pt;      };
  void setGenHiggsMass(float Gen_Higgs_mass)  { gen_Higgs_mass = Gen_Higgs_mass;  };

};

// read data from tree into member variables
gen_factory::gen_factory(TTree* input) {
  input -> SetBranchAddress( "genpX",           &genpX          );
  input -> SetBranchAddress( "genpY",           &genpY          );
  input -> SetBranchAddress( "genM",            &genM           );
  input -> SetBranchAddress( "genpT",           &genpT          );
  input -> SetBranchAddress( "numGenJets",      &numGenJets     );
  input -> SetBranchAddress( "genweight",       &genweight      );
  input -> SetBranchAddress( "gen_Higgs_pt",    &gen_Higgs_pt   );
  input -> SetBranchAddress( "gen_Higgs_mass",  &gen_Higgs_mass );
}
