#!/usr/bin/env python3
"""
plot_scalability.py - Generate plots from scalability test results

This script reads the CSV output from scalability_test.c and generates
publication-quality plots for analysis.

Usage:
    python plot_scalability.py scalability_results.csv

Requirements:
    pip install matplotlib numpy pandas scipy
"""

import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy import stats
from pathlib import Path

def plot_time_vs_size(df, output_dir):
    """Plot execution time vs graph size"""
    plt.figure(figsize=(10, 6))

    # Breakthrough algorithm
    plt.errorbar(df['n'], df['breakthrough_mean'],
                yerr=df['breakthrough_stddev'],
                label='Breakthrough Algorithm', marker='o',
                capsize=5, capthick=2)

    # Dijkstra algorithm
    if 'dijkstra_mean' in df.columns and df['dijkstra_mean'].notna().any():
        plt.errorbar(df['n'], df['dijkstra_mean'],
                    yerr=df['dijkstra_stddev'],
                    label="Dijkstra's Algorithm", marker='s',
                    capsize=5, capthick=2)

    plt.xlabel('Number of Vertices (n)', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.title('Execution Time vs Graph Size', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output_path = Path(output_dir) / 'time_vs_size.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_time_vs_size_log(df, output_dir):
    """Plot execution time vs graph size (log-log scale)"""
    plt.figure(figsize=(10, 6))

    plt.loglog(df['n'], df['breakthrough_mean'],
              label='Breakthrough Algorithm', marker='o', basex=10, basey=10)

    if 'dijkstra_mean' in df.columns and df['dijkstra_mean'].notna().any():
        plt.loglog(df['n'], df['dijkstra_mean'],
                  label="Dijkstra's Algorithm", marker='s', basex=10, basey=10)

    plt.xlabel('Number of Vertices (n)', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.title('Execution Time vs Graph Size (Log-Log)', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3, which='both')
    plt.tight_layout()

    output_path = Path(output_dir) / 'time_vs_size_loglog.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_speedup(df, output_dir):
    """Plot speedup vs graph size"""
    if 'speedup' not in df.columns or df['speedup'].isna().all():
        print("Skipping speedup plot (no Dijkstra comparison data)")
        return

    plt.figure(figsize=(10, 6))

    plt.plot(df['n'], df['speedup'], marker='o', linewidth=2, markersize=8)
    plt.axhline(y=1.0, color='r', linestyle='--', label='Break-even point')

    plt.xlabel('Number of Vertices (n)', fontsize=12)
    plt.ylabel('Speedup (Dijkstra time / Breakthrough time)', fontsize=12)
    plt.title('Speedup vs Graph Size', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output_path = Path(output_dir) / 'speedup.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_complexity_validation(df, output_dir):
    """Plot time / complexity measure to validate O(m log^(2/3) n)"""
    plt.figure(figsize=(10, 6))

    plt.plot(df['n'], df['time_per_complexity'],
            marker='o', linewidth=2, markersize=8)

    # Add trend line
    z = np.polyfit(df['n'], df['time_per_complexity'], 1)
    p = np.poly1d(z)
    plt.plot(df['n'], p(df['n']), "r--", alpha=0.8, label='Linear fit')

    plt.xlabel('Number of Vertices (n)', fontsize=12)
    plt.ylabel('Time / (m · log^(2/3) n)', fontsize=12)
    plt.title('Complexity Validation: Time per Complexity Unit', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output_path = Path(output_dir) / 'complexity_validation.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_time_vs_edges(df, output_dir):
    """Plot execution time vs number of edges"""
    plt.figure(figsize=(10, 6))

    plt.scatter(df['m'], df['breakthrough_mean'],
               label='Breakthrough Algorithm', marker='o', s=100, alpha=0.7)

    if 'dijkstra_mean' in df.columns and df['dijkstra_mean'].notna().any():
        plt.scatter(df['m'], df['dijkstra_mean'],
                   label="Dijkstra's Algorithm", marker='s', s=100, alpha=0.7)

    plt.xlabel('Number of Edges (m)', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.title('Execution Time vs Number of Edges', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output_path = Path(output_dir) / 'time_vs_edges.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_memory_usage(df, output_dir):
    """Plot memory usage vs graph size"""
    if 'memory_peak_mb' not in df.columns or df['memory_peak_mb'].isna().all():
        print("Skipping memory plot (no memory data)")
        return

    plt.figure(figsize=(10, 6))

    plt.plot(df['n'], df['memory_peak_mb'],
            marker='o', label='Peak Memory', linewidth=2, markersize=8)
    plt.plot(df['n'], df['memory_avg_mb'],
            marker='s', label='Average Memory', linewidth=2, markersize=8)

    plt.xlabel('Number of Vertices (n)', fontsize=12)
    plt.ylabel('Memory Usage (MB)', fontsize=12)
    plt.title('Memory Usage vs Graph Size', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output_path = Path(output_dir) / 'memory_usage.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_complexity_fit(df, output_dir):
    """Compare fit to different complexity models"""
    plt.figure(figsize=(12, 6))

    # Actual data
    plt.scatter(df['complexity_measure'], df['breakthrough_mean'],
               label='Actual Data', s=100, alpha=0.7)

    # Fit to O(m log^(2/3) n)
    X = df['complexity_measure'].values.reshape(-1, 1)
    y = df['breakthrough_mean'].values

    # Linear fit (assumes time = C * complexity_measure)
    slope, intercept, r_value, p_value, std_err = stats.linregress(
        df['complexity_measure'], df['breakthrough_mean'])

    fit_line = slope * df['complexity_measure'] + intercept
    plt.plot(df['complexity_measure'], fit_line, 'r-',
            label=f'O(m·log^(2/3) n) fit (R² = {r_value**2:.4f})', linewidth=2)

    plt.xlabel('m · log^(2/3) n', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.title('Fit to O(m · log^(2/3) n) Complexity Model', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output_path = Path(output_dir) / 'complexity_fit.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    print(f"R² = {r_value**2:.6f}, Slope = {slope:.6e}")
    plt.close()

def plot_all_in_one(df, output_dir):
    """Create a comprehensive figure with multiple subplots"""
    fig, axes = plt.subplots(2, 3, figsize=(18, 10))

    # 1. Time vs size
    axes[0, 0].plot(df['n'], df['breakthrough_mean'], 'o-', label='Breakthrough')
    if 'dijkstra_mean' in df.columns and df['dijkstra_mean'].notna().any():
        axes[0, 0].plot(df['n'], df['dijkstra_mean'], 's-', label='Dijkstra')
    axes[0, 0].set_xlabel('Vertices (n)')
    axes[0, 0].set_ylabel('Time (s)')
    axes[0, 0].set_title('Time vs Size')
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)

    # 2. Log-log plot
    axes[0, 1].loglog(df['n'], df['breakthrough_mean'], 'o-', label='Breakthrough')
    if 'dijkstra_mean' in df.columns and df['dijkstra_mean'].notna().any():
        axes[0, 1].loglog(df['n'], df['dijkstra_mean'], 's-', label='Dijkstra')
    axes[0, 1].set_xlabel('Vertices (n)')
    axes[0, 1].set_ylabel('Time (s)')
    axes[0, 1].set_title('Time vs Size (Log-Log)')
    axes[0, 1].legend()
    axes[0, 1].grid(True, alpha=0.3, which='both')

    # 3. Speedup
    if 'speedup' in df.columns and df['speedup'].notna().any():
        axes[0, 2].plot(df['n'], df['speedup'], 'o-')
        axes[0, 2].axhline(y=1.0, color='r', linestyle='--')
        axes[0, 2].set_xlabel('Vertices (n)')
        axes[0, 2].set_ylabel('Speedup')
        axes[0, 2].set_title('Speedup vs Size')
    axes[0, 2].grid(True, alpha=0.3)

    # 4. Time per complexity unit
    axes[1, 0].plot(df['n'], df['time_per_complexity'], 'o-')
    axes[1, 0].set_xlabel('Vertices (n)')
    axes[1, 0].set_ylabel('Time / (m·log^(2/3) n)')
    axes[1, 0].set_title('Complexity Validation')
    axes[1, 0].grid(True, alpha=0.3)

    # 5. Time vs edges
    axes[1, 1].scatter(df['m'], df['breakthrough_mean'], alpha=0.7)
    axes[1, 1].set_xlabel('Edges (m)')
    axes[1, 1].set_ylabel('Time (s)')
    axes[1, 1].set_title('Time vs Edges')
    axes[1, 1].grid(True, alpha=0.3)

    # 6. Complexity fit
    axes[1, 2].scatter(df['complexity_measure'], df['breakthrough_mean'], alpha=0.7)
    slope, intercept, r_value, _, _ = stats.linregress(
        df['complexity_measure'], df['breakthrough_mean'])
    fit_line = slope * df['complexity_measure'] + intercept
    axes[1, 2].plot(df['complexity_measure'], fit_line, 'r-',
                   label=f'R² = {r_value**2:.4f}')
    axes[1, 2].set_xlabel('m · log^(2/3) n')
    axes[1, 2].set_ylabel('Time (s)')
    axes[1, 2].set_title('Fit to O(m·log^(2/3) n)')
    axes[1, 2].legend()
    axes[1, 2].grid(True, alpha=0.3)

    plt.tight_layout()
    output_path = Path(output_dir) / 'scalability_summary.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def generate_report(df, output_dir):
    """Generate text report with statistics"""
    report_path = Path(output_dir) / 'plot_report.txt'

    with open(report_path, 'w') as f:
        f.write("=" * 60 + "\n")
        f.write("Scalability Analysis Report\n")
        f.write("=" * 60 + "\n\n")

        f.write(f"Total data points: {len(df)}\n")
        f.write(f"Graph size range: {df['n'].min()} to {df['n'].max()} vertices\n")
        f.write(f"Edge count range: {df['m'].min()} to {df['m'].max()} edges\n\n")

        # Complexity validation
        slope, intercept, r_value, p_value, std_err = stats.linregress(
            df['complexity_measure'], df['breakthrough_mean'])

        f.write("Complexity Model Fit:\n")
        f.write(f"  Model: Time = C * (m * log^(2/3) n)\n")
        f.write(f"  R² = {r_value**2:.6f}\n")
        f.write(f"  Slope (C) = {slope:.6e}\n")
        f.write(f"  p-value = {p_value:.6e}\n")
        f.write(f"  Standard error = {std_err:.6e}\n\n")

        if r_value**2 > 0.95:
            f.write("  ✓ Excellent fit to O(m * log^(2/3) n)\n")
        elif r_value**2 > 0.85:
            f.write("  ✓ Good fit to O(m * log^(2/3) n)\n")
        elif r_value**2 > 0.70:
            f.write("  ~ Moderate fit to O(m * log^(2/3) n)\n")
        else:
            f.write("  ✗ Poor fit to O(m * log^(2/3) n)\n")
        f.write("\n")

        # Speedup analysis
        if 'speedup' in df.columns and df['speedup'].notna().any():
            f.write("Speedup Analysis:\n")
            f.write(f"  Average speedup: {df['speedup'].mean():.2f}x\n")
            f.write(f"  Maximum speedup: {df['speedup'].max():.2f}x at n={df.loc[df['speedup'].idxmax(), 'n']}\n")
            f.write(f"  Minimum speedup: {df['speedup'].min():.2f}x at n={df.loc[df['speedup'].idxmin(), 'n']}\n")

            crossover = df[df['speedup'] > 1.0]
            if len(crossover) > 0:
                f.write(f"  Crossover point: n={crossover.iloc[0]['n']} (Breakthrough becomes faster)\n")
            else:
                f.write("  No crossover point found in tested range\n")
            f.write("\n")

        # Memory analysis
        if 'memory_peak_mb' in df.columns and df['memory_peak_mb'].notna().any():
            f.write("Memory Usage:\n")
            f.write(f"  Average peak memory: {df['memory_peak_mb'].mean():.2f} MB\n")
            f.write(f"  Maximum peak memory: {df['memory_peak_mb'].max():.2f} MB at n={df.loc[df['memory_peak_mb'].idxmax(), 'n']}\n")
            f.write(f"  Memory per vertex: {(df['memory_peak_mb'] / df['n']).mean():.4f} MB/vertex\n")
            f.write("\n")

        f.write("=" * 60 + "\n")

    print(f"Report saved: {report_path}")

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_scalability.py <scalability_results.csv>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_dir = Path(input_file).parent

    # Read data
    print(f"Reading data from {input_file}...")
    df = pd.read_csv(input_file)
    print(f"Loaded {len(df)} data points")

    # Generate plots
    print("\nGenerating plots...")
    plot_time_vs_size(df, output_dir)
    plot_time_vs_size_log(df, output_dir)
    plot_speedup(df, output_dir)
    plot_complexity_validation(df, output_dir)
    plot_time_vs_edges(df, output_dir)
    plot_memory_usage(df, output_dir)
    plot_complexity_fit(df, output_dir)
    plot_all_in_one(df, output_dir)

    # Generate report
    print("\nGenerating report...")
    generate_report(df, output_dir)

    print("\n✓ All plots and reports generated successfully")

if __name__ == '__main__':
    main()
