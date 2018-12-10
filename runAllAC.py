from subprocess import call
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument('-e', '--exe', action='store', dest='exe', default='finalFF', help='name of executable')
parser.add_argument('-y', '--year', action='store', dest='year', default='2016', help='year to process')
parser.add_argument('-t', '--tree', action='store', dest='tree', default='etau_tree', help='name of input tree')
parser.add_argument('-d', '--input-dir', action='store', dest='dir', default='Output/templates/', help='path to input files')
parser.add_argument('--suffix', action='store', dest='suffix', default='final', help='suffix for output file')
args = parser.parse_args()

acWeights = [
    "wt_ggH_a1", "wt_ggH_a3", "wt_ggH_a3int", "wt_wh_a1", "wt_wh_a2", "wt_wh_a2int", "wt_wh_a3", "wt_wh_a3int",
    "wt_wh_L1", "wt_wh_L1int", "wt_wh_L1Zg", "wt_wh_L1Zgint", "wt_zh_a1", "wt_zh_a2", "wt_zh_a2int", "wt_zh_a3",
    "wt_zh_a3int", "wt_zh_L1", "wt_zh_L1int", "wt_zh_L1Zg", "wt_zh_L1Zgint", "wt_a1", "wt_a2", "wt_a2int", "wt_a3",
    "wt_a3int", "wt_L1", "wt_L1int", "wt_L1Zg", "wt_L1Zgint",
]

for weight in acWeights:
    callstring = './{} -y {} -t {} -d {} -w {} --suf {}_{}'.format(args.exe, args.year, args.tree, args.dir, weight, weight, args.suffix)
    call(callstring, shell=True)
