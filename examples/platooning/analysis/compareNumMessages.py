import re
import pandas as pd
from glob import glob
import matplotlib.pyplot as plt
import seaborn as sns


regex = re.compile("SimpleScenario.node\[([\d+])\].appl")
def renameModule(s):
    nc = regex.search(s).groups()[0]
    return int(nc)


csvs = glob("../results/*NoGui_*.csv")

dfs = []
for csv in csvs:
    fname = csv.lstrip("../results/")
    df = pd.read_csv(csv, sep='\t')
    ttl = int(re.search("NoGui_([\d+])", fname).groups()[0])
    avoidDup = True if fname.startswith("Default") else False
    df['node'] = df['node'].apply(lambda x: renameModule(x))
    df['ttl'] = ttl
    df['dup'] = not avoidDup
    dfs.append(df)

df = pd.concat(dfs)

sns.set_style("whitegrid")
sns.catplot(x='node', hue='ttl', y='generatedWSMs',
            data=df, kind='bar', col='dup')

plt.savefig("comparisonNumMSGS.pdf", format='pdf')