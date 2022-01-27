from basico import *
import os
import glob
import time
import sbmltoodepy
import sys
from importlib import import_module
import multiprocessing
from roadrunner.roadrunner import RoadRunner
import get_biomodels


def worker(fname: str):
    try:
        r = RoadRunner(fname)
        return r.simulate(0, 100, 100)
    except Exception:
        print(f"failed: {fname}")
        pass


if __name__ == "__main__":
    biomodels_folder = get_biomodels.get_biomodels()
    biomodels_files = glob.glob(os.path.join(biomodels_folder, "*.xml"))

    for i in range(5):
        start = time.time()
        dct = dict()
        with multiprocessing.Pool(12) as p:
            p.map(worker, biomodels_files)
            end = time.time() - start
        print("Took: ", end, "seconds")

"""
All errors are ignored
MCJit:

LLJit:


"""