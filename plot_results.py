import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def main():
    try:
        df = pd.read_csv("benchmark_results.csv")
    except FileNotFoundError:
        print("Error: benchmark_results.csv not found.")
        return

    # Set style
    sns.set(style="whitegrid")

    # Create a plot for each distribution
    distributions = df["Distribution"].unique()
    
    plt.figure(figsize=(12, 6))
    
    # Plotting all in one if possible, or subplots?
    # Let's do a grouped bar chart for the largest N, or lines for N growth.
    
    # Line plot: Time vs N, hue=Algorithm, style=Distribution
    g = sns.relplot(
        data=df, 
        x="N", 
        y="Time_ms", 
        hue="Algorithm", 
        col="Distribution",
        kind="line", 
        marker="o",
        height=4, 
        aspect=1.2
    )
    
    g.set_axis_labels("N (Number of Operations)", "Time (ms)")
    g.fig.suptitle("Performance Comparison: Li-Chao Tree vs Dynamic CHT", y=1.05)
    
    plt.savefig("benchmark_plot.png", bbox_inches='tight')
    print("Plot saved to benchmark_plot.png")

if __name__ == "__main__":
    main()
