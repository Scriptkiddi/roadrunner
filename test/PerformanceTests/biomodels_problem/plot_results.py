import seaborn as sns
import matplotlib.pyplot as plt
import os, numpy, pandas

df = pandas.read_csv("results.csv")

fig = plt.figure()
sns.barplot(
    data=df, x="Tool", y='Time (sec)', units="Repeat",
    linewidth=1.0, edgecolor='black', color="white"
)
plt.xticks(rotation=90)
sns.despine(fig=fig)
fname = os.path.join(os.path.dirname(__file__), "biomodels_problem_results.png")
plt.savefig(fname, dpi=250, bbox_inches='tight')
