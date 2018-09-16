import FWCore.ParameterSet.Config as cms
import pickle
handle = open('RunExpressProcessingCfg.pkl')
process = pickle.load(handle)
handle.close()
