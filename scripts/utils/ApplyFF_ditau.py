import ROOT
import math


class FFApplicationTool():
    def __init__(self, theFFDirectory, channel):
        """open ROOT files and grab all the fits"""
        self.theFFDirectory = theFFDirectory

        # raw fake factors
        self.raw_file = ROOT.TFile.Open(theFFDirectory+"uncorrected_fakefactors_tt.root")
        if self.raw_file.IsZombie():
            raise RuntimeError("Problem loading the files!")

        self.ff_raw_0jet = self.raw_file.Get("rawFF_tt_qcd_0jet")
        self.ff_raw_1jet = self.raw_file.Get("rawFF_tt_qcd_1jet")
        self.ff_raw_2jet = self.raw_file.Get("rawFF_tt_qcd_2jet")

        self.ff_raw_0jet_shape = self.raw_file.Get("rawFF_tt_qcd_0jet_linear")
        self.ff_raw_1jet_shape = self.raw_file.Get("rawFF_tt_qcd_1jet_linear")
        self.ff_raw_2jet_shape = self.raw_file.Get("rawFF_tt_qcd_2jet_linear")

        # visible mass corrections
        self.vis_mass_closure_file = ROOT.TFile.Open(theFFDirectory+"FF_corrections_1.root")
        if self.vis_mass_closure_file.IsZombie():
            raise RuntimeError("Problem loading the files!")

        self.vis_mass_closure = self.vis_mass_closure_file.Get("closure_OSSS_mvis_tt_qcd")
        self.vis_mass_closure_shape = self.vis_mass_closure_file.Get("closure_OSSS_mvis_tt_qcd_linear")

        # closure correction
        self.tau_pt_closure_file = ROOT.TFile.Open(theFFDirectory+"FF_QCDcorrectionOSSS.root")
        if self.tau_pt_closure_file.IsZombie():
            raise RuntimeError("Problem loading the files!")

        self.tau_pt_closure_0jet = self.tau_pt_closure_file.Get("closure_tau2pt_tt_qcd_0jet")
        self.tau_pt_closure_1jet = self.tau_pt_closure_file.Get("closure_tau2pt_tt_qcd_1jet")
        self.tau_pt_closure_2jet = self.tau_pt_closure_file.Get("closure_tau2pt_tt_qcd_2jet")

    def get_raw_FF(self, pt, fct, cutoff=False):
        """evaluate raw FF. Cutoff is used for systematics"""
        if cutoff:
            return fct.Eval(pt) if pt < 80 else fct.Eval(80)
        return fct.Eval(pt)

    def get_mvis_closure(self, mvis, cutoff=False):
        """evaluate vis_mass closure correction. Cutoff used in nominal case."""
        if cutoff:
            return self.vis_mass_closure.Eval(mvis) if mvsis < 90 else self.vis_mass_closure_shape.Eval(mvis)
        return self.vis_mass_closure_shape.Eval(mvis)

    def get_tau2_pt_closure(self, pt2, fct):
        """evaluate tau2 pT closure correction"""
        return fct.Eval(pt2)

    def get_functions(self, njets):
        """pick correct TF1 based on njets"""
        if njets == 0:
            return self.ff_raw_0jet, self.ff_raw_0jet_shape, self.tau_pt_closure_0jet
        elif njets == 1:
            return self.ff_raw_1jet, self.ff_raw_1jet_shape, self.tau_pt_closure_1jet
        else:
            return self.ff_raw_2jet, self.ff_raw_2jet_shape, self.tau_pt_closure_2jet

    def get_ff(self, pt1, pt2, mvis, njets, unc='', upOrDown=''):
        raw_func, raw_linear_func, tau2_func = get_functions(njets)

        raw_factor = self.get_raw_FF(pt1, raw_func, cutoff=False)
        tau2_factor = self.get_tau2_pt_closure(pt2, tau2_func)
        mvis_factor = self.get_mvis_closure(mvis, cutoff=True)
        nominal = max(raw_factor * tau2_factor * mvis_factor, 0.)

        if unc == '':
            return nominal
        elif 'raw' in unc:
            raw_linear_factor = self.get_raw_FF(pt1, raw_linear_func, cutoff=True)
            shifted = max(raw_linear_factor * tau2_factor * mvis_factor, 0.)
            syst_factor = nominal

            # handle up or down
            if upOrDown == 'up':
                syst_factor = shifted # shift up uses linear fit with cutoff
            elif upOrDown == 'down':
                syst_factor = 2 * nominal - shifted # shift down is nominal - (shifted - nominal)

            # njets splitting
            if unc == 'raw_0jet' and njets == 0:
                return syst_factor
            elif unc == 'raw_1jet' and njets == 1:
                return syst_factor
            elif unc == 'raw_2jet' and njets > 1:
                return syst_factor

            # return nominal
            return nominal
        elif 'tau2' in unc:
            # handle up or down
            if upOrDown == 'up':
                syst_factor = nominal * tau2_factor  # apply tau2 correction twice for up
            elif upOrDown == 'down':
                syst_factor = nominal / max(tau2_factor, 0.)  # don't apply tau2 correction for down

            # njets splitting
            if unc == 'tau2_0jet' and njets == 0:
                return syst_factor
            elif unc == 'tau2_1jet' and njets == 1:
                return syst_factor
            elif unc == 'tau2_2jet' and njets > 1:
                return syst_factor

            # return nominal
            return nominal
        elif unc == 'vis_mass':
            mvis_factor_shifted = self.get_mvis_closure(mvis, cutoff=False)
            syst_factor = max(raw_factor * tau2_factor * mvis_factor_shifted, 0.)

            # handle up or down
            if upOrDown == 'up':
                return syst_factor # shift up uses linear fit in entire mvis range
            elif upOrDown == 'down':
                return 2 * nominal - syst_factor # shift down is nominal - (shifted - nominal)
            else:
                return nominal

        return nominal
