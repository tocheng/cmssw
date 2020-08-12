
import ROOT

def setPlotStyle():

    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetPadLeftMargin(0.08)
    ROOT.gStyle.SetPadRightMargin(0.1)
    #ROOT.gPad.SetTickx()
    #ROOT.gPad.SetTicky()
    ROOT.gStyle.SetLegendTextSize(0.025)

    ROOT.gStyle.SetLineStyleString(11,"4 4")
