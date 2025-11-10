#!/usr/bin/env python3
"""
plot_comparison.py - Generate comparison plots between algorithms

Visualizes the comparison between Breakthrough algorithm and Dijkstra variants.

Usage:
    python plot_comparison.py comparison_results.csv

Requirements:
    pip install matplotlib numpy pandas scipy
"""

import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy import stats
from pathlib import Path

def plot_performance_comparison(df, output_dir):
    """Plot all three algorithms on same axes with error bars"""
    plt.figure(figsize=(12, 7))

    # Plot with confidence intervals
    plt.errorbar(df['n'], df['breakthrough_mean'],
                yerr=[df['breakthrough_mean'] - df['breakthrough_ci_low'],
                      df['breakthrough_ci_high'] - df['breakthrough_mean']],
                label='Breakthrough Algorithm', marker='o', capsize=5, linewidth=2)

    plt.errorbar(df['n'], df['binary_mean'],
                yerr=[df['binary_mean'] - df['binary_ci_low'],
                      df['binary_ci_high'] - df['binary_mean']],
                label='Dijkstra (Binary Heap)', marker='s', capsize=5, linewidth=2)

    plt.errorbar(df['n'], df['fibonacci_mean'],
                yerr=[df['fibonacci_mean'] - df['fibonacci_ci_low'],
                      df['fibonacci_ci_high'] - df['fibonacci_mean']],
                label='Dijkstra (Fibonacci Heap)', marker='^', capsize=5, linewidth=2)

    plt.xlabel('Number of Vertices (n)', fontsize=13)
    plt.ylabel('Time (seconds)', fontsize=13)
    plt.title('Algorithm Performance Comparison\n(with 95% confidence intervals)',
              fontsize=14, fontweight='bold')
    plt.legend(fontsize=12, loc='upper left')
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output_path = Path(output_dir) / 'comparison_performance.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_speedup_comparison(df, output_dir):
    """Plot speedup vs both Dijkstra variants"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    # Speedup vs Binary Heap
    ax1.plot(df['n'], df['speedup_vs_binary'], 'o-', linewidth=2, markersize=8)
    ax1.axhline(y=1.0, color='r', linestyle='--', linewidth=2, label='Break-even')
    ax1.fill_between(df['n'], 0, df['speedup_vs_binary'],
                     where=(df['speedup_vs_binary'] > 1),
                     alpha=0.3, color='green', label='Breakthrough faster')
    ax1.fill_between(df['n'], 0, df['speedup_vs_binary'],
                     where=(df['speedup_vs_binary'] <= 1),
                     alpha=0.3, color='red', label='Dijkstra faster')
    ax1.set_xlabel('Number of Vertices (n)', fontsize=12)
    ax1.set_ylabel('Speedup', fontsize=12)
    ax1.set_title('Speedup vs Dijkstra (Binary Heap)', fontsize=13, fontweight='bold')
    ax1.legend(fontsize=10)
    ax1.grid(True, alpha=0.3)

    # Speedup vs Fibonacci Heap
    ax2.plot(df['n'], df['speedup_vs_fibonacci'], 's-', linewidth=2, markersize=8,
            color='orange')
    ax2.axhline(y=1.0, color='r', linestyle='--', linewidth=2, label='Break-even')
    ax2.fill_between(df['n'], 0, df['speedup_vs_fibonacci'],
                     where=(df['speedup_vs_fibonacci'] > 1),
                     alpha=0.3, color='green', label='Breakthrough faster')
    ax2.fill_between(df['n'], 0, df['speedup_vs_fibonacci'],
                     where=(df['speedup_vs_fibonacci'] <= 1),
                     alpha=0.3, color='red', label='Dijkstra faster')
    ax2.set_xlabel('Number of Vertices (n)', fontsize=12)
    ax2.set_ylabel('Speedup', fontsize=12)
    ax2.set_title('Speedup vs Dijkstra (Fibonacci Heap)', fontsize=13, fontweight='bold')
    ax2.legend(fontsize=10)
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    output_path = Path(output_dir) / 'comparison_speedup.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_log_scale_comparison(df, output_dir):
    """Log-log plot to visualize complexity differences"""
    plt.figure(figsize=(10, 7))

    plt.loglog(df['n'], df['breakthrough_mean'], 'o-',
              label='Breakthrough', linewidth=2, markersize=8)
    plt.loglog(df['n'], df['binary_mean'], 's-',
              label='Binary Heap', linewidth=2, markersize=8)
    plt.loglog(df['n'], df['fibonacci_mean'], '^-',
              label='Fibonacci Heap', linewidth=2, markersize=8)

    plt.xlabel('Number of Vertices (n)', fontsize=13)
    plt.ylabel('Time (seconds)', fontsize=13)
    plt.title('Algorithm Comparison (Log-Log Scale)', fontsize=14, fontweight='bold')
    plt.legend(fontsize=12)
    plt.grid(True, alpha=0.3, which='both')
    plt.tight_layout()

    output_path = Path(output_dir) / 'comparison_loglog.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_theoretical_vs_actual(df, output_dir):
    """Plot theoretical complexity vs actual performance"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    # Breakthrough: Time vs m * log^(2/3) n
    ax1.scatter(df['complexity_breakthrough'], df['breakthrough_mean'],
               s=100, alpha=0.7, label='Actual data')

    # Fit line
    slope, intercept, r_value, _, _ = stats.linregress(
        df['complexity_breakthrough'], df['breakthrough_mean'])
    fit_line = slope * df['complexity_breakthrough'] + intercept
    ax1.plot(df['complexity_breakthrough'], fit_line, 'r-',
            linewidth=2, label=f'Linear fit (R² = {r_value**2:.4f})')

    ax1.set_xlabel('Theoretical Complexity: m · log^(2/3) n', fontsize=12)
    ax1.set_ylabel('Actual Time (seconds)', fontsize=12)
    ax1.set_title('Breakthrough: Theoretical vs Actual', fontsize=13, fontweight='bold')
    ax1.legend(fontsize=11)
    ax1.grid(True, alpha=0.3)

    # Binary Heap: Time vs (m+n) * log n
    ax2.scatter(df['complexity_binary'], df['binary_mean'],
               s=100, alpha=0.7, label='Actual data', color='green')

    slope2, intercept2, r_value2, _, _ = stats.linregress(
        df['complexity_binary'], df['binary_mean'])
    fit_line2 = slope2 * df['complexity_binary'] + intercept2
    ax2.plot(df['complexity_binary'], fit_line2, 'r-',
            linewidth=2, label=f'Linear fit (R² = {r_value2**2:.4f})')

    ax2.set_xlabel('Theoretical Complexity: (m+n) · log n', fontsize=12)
    ax2.set_ylabel('Actual Time (seconds)', fontsize=12)
    ax2.set_title('Dijkstra (Binary): Theoretical vs Actual', fontsize=13, fontweight='bold')
    ax2.legend(fontsize=11)
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    output_path = Path(output_dir) / 'comparison_theoretical.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    print(f"Breakthrough R² = {r_value**2:.6f}")
    print(f"Binary Heap R² = {r_value2**2:.6f}")
    plt.close()

def plot_relative_performance(df, output_dir):
    """Plot relative performance normalized to breakthrough"""
    plt.figure(figsize=(10, 7))

    # Normalize to breakthrough (breakthrough = 1.0)
    breakthrough_norm = df['breakthrough_mean'] / df['breakthrough_mean']
    binary_norm = df['binary_mean'] / df['breakthrough_mean']
    fibonacci_norm = df['fibonacci_mean'] / df['breakthrough_mean']

    plt.plot(df['n'], breakthrough_norm, 'o-',
            label='Breakthrough (baseline)', linewidth=2, markersize=8)
    plt.plot(df['n'], binary_norm, 's-',
            label='Binary Heap', linewidth=2, markersize=8)
    plt.plot(df['n'], fibonacci_norm, '^-',
            label='Fibonacci Heap', linewidth=2, markersize=8)

    plt.axhline(y=1.0, color='gray', linestyle='--', linewidth=1)

    plt.xlabel('Number of Vertices (n)', fontsize=13)
    plt.ylabel('Relative Time (normalized to Breakthrough)', fontsize=13)
    plt.title('Relative Performance Comparison', fontsize=14, fontweight='bold')
    plt.legend(fontsize=12)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output_path = Path(output_dir) / 'comparison_relative.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_standard_deviations(df, output_dir):
    """Plot standard deviations to show variance"""
    plt.figure(figsize=(10, 7))

    plt.plot(df['n'], df['breakthrough_stddev'], 'o-',
            label='Breakthrough', linewidth=2, markersize=8)
    plt.plot(df['n'], df['binary_stddev'], 's-',
            label='Binary Heap', linewidth=2, markersize=8)
    plt.plot(df['n'], df['fibonacci_stddev'], '^-',
            label='Fibonacci Heap', linewidth=2, markersize=8)

    plt.xlabel('Number of Vertices (n)', fontsize=13)
    plt.ylabel('Standard Deviation (seconds)', fontsize=13)
    plt.title('Timing Variance Comparison', fontsize=14, fontweight='bold')
    plt.legend(fontsize=12)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output_path = Path(output_dir) / 'comparison_variance.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def plot_all_in_one_comparison(df, output_dir):
    """Create comprehensive comparison figure"""
    fig = plt.figure(figsize=(16, 10))
    gs = fig.add_gridspec(3, 2, hspace=0.3, wspace=0.3)

    # 1. Main performance comparison
    ax1 = fig.add_subplot(gs[0, :])
    ax1.plot(df['n'], df['breakthrough_mean'], 'o-',
            label='Breakthrough', linewidth=2, markersize=6)
    ax1.plot(df['n'], df['binary_mean'], 's-',
            label='Binary Heap', linewidth=2, markersize=6)
    ax1.plot(df['n'], df['fibonacci_mean'], '^-',
            label='Fibonacci Heap', linewidth=2, markersize=6)
    ax1.set_xlabel('Vertices (n)')
    ax1.set_ylabel('Time (s)')
    ax1.set_title('Performance Comparison', fontweight='bold')
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    # 2. Speedup vs Binary
    ax2 = fig.add_subplot(gs[1, 0])
    ax2.plot(df['n'], df['speedup_vs_binary'], 'o-', linewidth=2)
    ax2.axhline(y=1.0, color='r', linestyle='--')
    ax2.set_xlabel('Vertices (n)')
    ax2.set_ylabel('Speedup')
    ax2.set_title('Speedup vs Binary Heap', fontweight='bold')
    ax2.grid(True, alpha=0.3)

    # 3. Speedup vs Fibonacci
    ax3 = fig.add_subplot(gs[1, 1])
    ax3.plot(df['n'], df['speedup_vs_fibonacci'], 's-',
            linewidth=2, color='orange')
    ax3.axhline(y=1.0, color='r', linestyle='--')
    ax3.set_xlabel('Vertices (n)')
    ax3.set_ylabel('Speedup')
    ax3.set_title('Speedup vs Fibonacci Heap', fontweight='bold')
    ax3.grid(True, alpha=0.3)

    # 4. Breakthrough theoretical fit
    ax4 = fig.add_subplot(gs[2, 0])
    ax4.scatter(df['complexity_breakthrough'], df['breakthrough_mean'], alpha=0.7)
    slope, intercept, r_value, _, _ = stats.linregress(
        df['complexity_breakthrough'], df['breakthrough_mean'])
    fit_line = slope * df['complexity_breakthrough'] + intercept
    ax4.plot(df['complexity_breakthrough'], fit_line, 'r-',
            label=f'R² = {r_value**2:.4f}')
    ax4.set_xlabel('m · log^(2/3) n')
    ax4.set_ylabel('Time (s)')
    ax4.set_title('Breakthrough: Theoretical Fit', fontweight='bold')
    ax4.legend()
    ax4.grid(True, alpha=0.3)

    # 5. Log-log comparison
    ax5 = fig.add_subplot(gs[2, 1])
    ax5.loglog(df['n'], df['breakthrough_mean'], 'o-', label='Breakthrough')
    ax5.loglog(df['n'], df['binary_mean'], 's-', label='Binary Heap')
    ax5.set_xlabel('Vertices (n)')
    ax5.set_ylabel('Time (s)')
    ax5.set_title('Log-Log Comparison', fontweight='bold')
    ax5.legend()
    ax5.grid(True, alpha=0.3, which='both')

    fig.suptitle('SSSP Algorithm Comparison Summary', fontsize=16, fontweight='bold', y=0.995)

    output_path = Path(output_dir) / 'comparison_summary.png'
    plt.savefig(output_path, dpi=300)
    print(f"Saved: {output_path}")
    plt.close()

def generate_comparison_report(df, output_dir):
    """Generate text report with statistical analysis"""
    report_path = Path(output_dir) / 'comparison_report.txt'

    with open(report_path, 'w') as f:
        f.write("=" * 70 + "\n")
        f.write("Algorithm Comparison Statistical Report\n")
        f.write("=" * 70 + "\n\n")

        f.write(f"Total data points: {len(df)}\n")
        f.write(f"Graph size range: {df['n'].min()} to {df['n'].max()} vertices\n\n")

        # Overall statistics
        f.write("Overall Performance:\n")
        f.write("-" * 70 + "\n")
        f.write(f"  Breakthrough:\n")
        f.write(f"    Mean: {df['breakthrough_mean'].mean():.6f} s\n")
        f.write(f"    Median: {df['breakthrough_mean'].median():.6f} s\n")
        f.write(f"  Binary Heap:\n")
        f.write(f"    Mean: {df['binary_mean'].mean():.6f} s\n")
        f.write(f"    Median: {df['binary_mean'].median():.6f} s\n")
        f.write(f"  Fibonacci Heap:\n")
        f.write(f"    Mean: {df['fibonacci_mean'].mean():.6f} s\n")
        f.write(f"    Median: {df['fibonacci_mean'].median():.6f} s\n\n")

        # Speedup statistics
        f.write("Speedup Statistics:\n")
        f.write("-" * 70 + "\n")
        f.write(f"  vs Binary Heap:\n")
        f.write(f"    Average: {df['speedup_vs_binary'].mean():.3f}x\n")
        f.write(f"    Median: {df['speedup_vs_binary'].median():.3f}x\n")
        f.write(f"    Best: {df['speedup_vs_binary'].max():.3f}x at n={df.loc[df['speedup_vs_binary'].idxmax(), 'n']}\n")
        f.write(f"  vs Fibonacci Heap:\n")
        f.write(f"    Average: {df['speedup_vs_fibonacci'].mean():.3f}x\n")
        f.write(f"    Median: {df['speedup_vs_fibonacci'].median():.3f}x\n")
        f.write(f"    Best: {df['speedup_vs_fibonacci'].max():.3f}x at n={df.loc[df['speedup_vs_fibonacci'].idxmax(), 'n']}\n\n")

        # Theoretical fit
        slope_b, intercept_b, r_b, _, _ = stats.linregress(
            df['complexity_breakthrough'], df['breakthrough_mean'])
        slope_bin, intercept_bin, r_bin, _, _ = stats.linregress(
            df['complexity_binary'], df['binary_mean'])

        f.write("Theoretical Complexity Fit:\n")
        f.write("-" * 70 + "\n")
        f.write(f"  Breakthrough (O(m · log^(2/3) n)):\n")
        f.write(f"    R² = {r_b**2:.6f}\n")
        f.write(f"    Slope = {slope_b:.6e}\n")
        if r_b**2 > 0.95:
            f.write("    Assessment: Excellent fit ✓\n")
        elif r_b**2 > 0.85:
            f.write("    Assessment: Good fit ✓\n")
        else:
            f.write("    Assessment: Moderate fit\n")

        f.write(f"  Binary Heap (O((m+n) · log n)):\n")
        f.write(f"    R² = {r_bin**2:.6f}\n")
        f.write(f"    Slope = {slope_bin:.6e}\n")
        if r_bin**2 > 0.95:
            f.write("    Assessment: Excellent fit ✓\n")
        elif r_bin**2 > 0.85:
            f.write("    Assessment: Good fit ✓\n")
        else:
            f.write("    Assessment: Moderate fit\n")

        f.write("\n" + "=" * 70 + "\n")

    print(f"Report saved: {report_path}")

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_comparison.py <comparison_results.csv>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_dir = Path(input_file).parent

    # Read data
    print(f"Reading data from {input_file}...")
    df = pd.read_csv(input_file)
    print(f"Loaded {len(df)} data points\n")

    # Generate plots
    print("Generating comparison plots...")
    plot_performance_comparison(df, output_dir)
    plot_speedup_comparison(df, output_dir)
    plot_log_scale_comparison(df, output_dir)
    plot_theoretical_vs_actual(df, output_dir)
    plot_relative_performance(df, output_dir)
    plot_standard_deviations(df, output_dir)
    plot_all_in_one_comparison(df, output_dir)

    # Generate report
    print("\nGenerating statistical report...")
    generate_comparison_report(df, output_dir)

    print("\n✓ All comparison plots and reports generated successfully")

if __name__ == '__main__':
    main()
