double GetSF(int WP, float x, int flavour, int syst){

   if (WP==1){
      if (fabs(flavour)==4 or fabs(flavour)==5){
        if (syst==0){
           return 0.498094*((1.+(0.422991*x))/(1.+(0.210944*x)));
        }
        if (syst==-1){
               if (x < 30) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.078885495662689209;
               else if (x<50) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.025339335203170776;
               else if (x < 70) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.022487226873636246;
               else if (x < 100) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.021372253075242043;
               else if (x < 140) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.021989239379763603;
               else if (x < 200) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.023777997121214867;
               else if (x < 300) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.030794138088822365;
               else if (x < 600) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.041836585849523544;
               else return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))-0.063810773193836212;
        }
        if (syst==1){
               if (x < 30) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.078885495662689209;
               else if (x<50) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.025339335203170776;
               else if (x < 70) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.022487226873636246;
               else if (x < 100) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.021372253075242043;
               else if (x < 140) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.021989239379763603;
               else if (x < 200) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.023777997121214867;
               else if (x < 300) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.030794138088822365;
               else if (x < 600) return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.041836585849523544;
               else return (0.498094*((1.+(0.422991*x))/(1.+(0.210944*x))))+0.063810773193836212;
        }
      }
      else {
        if (syst==0) return 1.0589+0.000382569*x+-2.4252e-07*x*x+2.20966e-10*x*x*x;
        if (syst==1) return (1.0589+0.000382569*x+-2.4252e-07*x*x+2.20966e-10*x*x*x)*(1+(0.100485+3.95509e-05*x+-4.90326e-08*x*x));
        if (syst==-1) return (1.0589+0.000382569*x+-2.4252e-07*x*x+2.20966e-10*x*x*x)*(1-(0.100485+3.95509e-05*x+-4.90326e-08*x*x));
     }
   }
   else return 0;
}


double bTagEventWeight(int nBtaggedJets, float bjetpt_1, int bjetflavour_1, float bjetpt_2, int bjetflavour_2, int WP, int syst, int nBTags)
{
  if (nBtaggedJets > 2) return -10000;
  if( nBTags > 2 ) return -10000;

  /*
    ##################################################################
    Event weight matrix:
    ------------------------------------------------------------------
    nBTags\b-tagged jets  |    0        1             2
    ------------------------------------------------------------------
      0                   |    1      1-SF      (1-SF1)(1-SF2)
                          |
      1                   |    0       SF    SF1(1-SF2)+(1-SF1)SF2
                          |
      2                   |    0        0           SF1SF2
    ##################################################################
  */
  
  if( nBTags > nBtaggedJets) return 0;
  if( nBTags==0 && nBtaggedJets==0) return 1;

  double weight = 0;
  if(nBtaggedJets==1){
    double SF = GetSF(WP,bjetpt_1,bjetflavour_1,syst);
    for( unsigned int i=0; i<=1; ++i ){
      if( i != nBTags ) continue;
      weight += pow(SF,i)*pow(1-SF,1-i);
    }
  }
  else if(nBtaggedJets==2 ){
    double SF1 = GetSF(WP,bjetpt_1,bjetflavour_1,syst);
    double SF2 = GetSF(WP,bjetpt_2,bjetflavour_2,syst);
    
    for( unsigned int i=0; i<=1; ++i ){
      for( unsigned int j=0; j<=1; ++j ){
        if( (i+j) != nBTags ) continue;
        weight += pow(SF1,i)*pow(1-SF1,1-i)*pow(SF2,j)*pow(1-SF2,1-j);
      }
    }
  }
  return weight;
}

