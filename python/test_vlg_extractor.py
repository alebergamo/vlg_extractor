import glob
import numpy as np
import os
import os.path
import tempfile

if __name__ == '__main__':
    TMPDIR = os.path.join(tempfile.gettempdir(), 'test_vlg_extractor')
    for fname in glob.glob(os.path.join(TMPDIR, '*')):
        os.remove(fname)

    val = os.system('./vlg_extractor --extract_concatenationLowLevelFeatures=ASCII --extract_classemes=ASCII --extract_classemes_bin=ASCII --extract_picodes128=ASCII --extract_picodes1024=ASCII --extract_picodes2048=ASCII --extract_mc=ASCII --extract_mc_bit=ASCII test/list_images.txt test ' + TMPDIR)

    for fname in open('test/list_images.txt', 'r'):
        fname = fname.strip()
        feats = ['low_level_features_v2', 'classemes', 'classemesBIN', 'picodes128', 'picodes1024', 'picodes2048', 'mc', 'mc_bit']
        for feat in feats:
            gt = np.loadtxt(os.path.join('test', '{}_{}.ascii'.format(fname[:-4], feat)))
            pred = np.loadtxt(os.path.join(TMPDIR, '{}_{}.ascii'.format(fname[:-4], feat)))
            np.testing.assert_almost_equal(gt, pred, decimal=3)
