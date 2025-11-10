#!/usr/bin/env python3
"""
Plot performance test results for SSSP Breakthrough Algorithm

Usage:
    python plot_performance.py [performance_results.csv]

Generates multiple plots:
    - performance_speedup.png - Speedup vs graph size
    - performance_time.png - Absolute time comparison
    - performance_memory.png - Memory usage
"""

import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def load_results(filename='performance_results.csv'):
    """Load performance results from CSV file"""
    try:
        df = pd.read_csv(filename)
        print(f"Loaded {len(df)} test results from {filename}")
        return df
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found")
        print("Please run the performance test first:")
        print("  ./bin/examples/performance_test")
        sys.exit(1)
    except Exception as e:
        print(f"Error loading results: {e}")
        sys.exit(1)

def plot_speedup(df, output='performance_speedup.png'):
    """Plot speedup vs graph size"""
    plt.figure(figsize=(12, 7))

    # Get unique graph types
    graph_types = df['graph_type'].unique()

    # Color scheme
    colors = plt.cm.Set2(np.linspace(0, 1, len(graph_types)))

    for i, graph_type in enumerate(graph_types):
        data = df[df['graph_type'] == graph_type].sort_values('n')
        plt.plot(data['n'], data['speedup'],
                marker='o',
                label=graph_type.capitalize(),
                color=colors[i],
                linewidth=2,
                markersize=8)

    # Add horizontal line at y=1 (equal performance)
    plt.axhline(y=1.0, color='red', linestyle='--',
                linewidth=2, alpha=0.7, label='Equal Performance')

    plt.xlabel('Graph Size (vertices)', fontsize=12)
    plt.ylabel('Speedup (Dijkstra / Breakthrough)', fontsize=12)
    plt.title('SSSP Algorithm Performance: Speedup Comparison', fontsize=14, fontweight='bold')
    plt.legend(fontsize=10, loc='best')
    plt.grid(True, alpha=0.3)
    plt.xscale('log')

    # Add annotations
    plt.text(0.02, 0.98, 'Breakthrough faster →',
             transform=plt.gca().transAxes,
             fontsize=10, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.3))
    plt.text(0.02, 0.45, '← Dijkstra faster',
             transform=plt.gca().transAxes,
             fontsize=10, verticalalignment='top',
             bbox=dict(boxstyle='round', facecolor='lightcoral', alpha=0.3))

    plt.tight_layout()
    plt.savefig(output, dpi=300, bbox_inches='tight')
    print(f"Saved speedup plot to {output}")

def plot_absolute_times(df, output='performance_time.png'):
    """Plot absolute execution times"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

    graph_types = df['graph_type'].unique()
    colors = plt.cm.Set2(np.linspace(0, 1, len(graph_types)))

    # Plot 1: Breakthrough time
    for i, graph_type in enumerate(graph_types):
        data = df[df['graph_type'] == graph_type].sort_values('n')
        ax1.plot(data['n'], data['time_breakthrough'] * 1000,
                marker='o',
                label=graph_type.capitalize(),
                color=colors[i],
                linewidth=2,
                markersize=8)

    ax1.set_xlabel('Graph Size (vertices)', fontsize=12)
    ax1.set_ylabel('Time (milliseconds)', fontsize=12)
    ax1.set_title('SSSP Breakthrough Algorithm', fontsize=13, fontweight='bold')
    ax1.legend(fontsize=10)
    ax1.grid(True, alpha=0.3)
    ax1.set_xscale('log')
    ax1.set_yscale('log')

    # Plot 2: Dijkstra time
    for i, graph_type in enumerate(graph_types):
        data = df[df['graph_type'] == graph_type].sort_values('n')
        ax2.plot(data['n'], data['time_dijkstra'] * 1000,
                marker='s',
                label=graph_type.capitalize(),
                color=colors[i],
                linewidth=2,
                markersize=8)

    ax2.set_xlabel('Graph Size (vertices)', fontsize=12)
    ax2.set_ylabel('Time (milliseconds)', fontsize=12)
    ax2.set_title('Dijkstra Algorithm', fontsize=13, fontweight='bold')
    ax2.legend(fontsize=10)
    ax2.grid(True, alpha=0.3)
    ax2.set_xscale('log')
    ax2.set_yscale('log')

    plt.tight_layout()
    plt.savefig(output, dpi=300, bbox_inches='tight')
    print(f"Saved time comparison plot to {output}")

def plot_memory_usage(df, output='performance_memory.png'):
    """Plot memory usage"""
    plt.figure(figsize=(12, 7))

    graph_types = df['graph_type'].unique()
    colors = plt.cm.Set2(np.linspace(0, 1, len(graph_types)))

    for i, graph_type in enumerate(graph_types):
        data = df[df['graph_type'] == graph_type].sort_values('n')
        plt.plot(data['n'], data['memory_mb'],
                marker='o',
                label=graph_type.capitalize(),
                color=colors[i],
                linewidth=2,
                markersize=8)

    plt.xlabel('Graph Size (vertices)', fontsize=12)
    plt.ylabel('Peak Memory Usage (MB)', fontsize=12)
    plt.title('Memory Usage Analysis', fontsize=14, fontweight='bold')
    plt.legend(fontsize=10)
    plt.grid(True, alpha=0.3)
    plt.xscale('log')
    plt.yscale('log')

    plt.tight_layout()
    plt.savefig(output, dpi=300, bbox_inches='tight')
    print(f"Saved memory usage plot to {output}")

def plot_combined_comparison(df, output='performance_combined.png'):
    """Create a combined 2x2 plot"""
    fig = plt.figure(figsize=(16, 12))

    graph_types = df['graph_type'].unique()
    colors = plt.cm.Set2(np.linspace(0, 1, len(graph_types)))

    # Plot 1: Speedup
    ax1 = plt.subplot(2, 2, 1)
    for i, graph_type in enumerate(graph_types):
        data = df[df['graph_type'] == graph_type].sort_values('n')
        ax1.plot(data['n'], data['speedup'],
                marker='o', label=graph_type.capitalize(),
                color=colors[i], linewidth=2, markersize=6)
    ax1.axhline(y=1.0, color='red', linestyle='--', linewidth=1.5, alpha=0.5)
    ax1.set_xlabel('Graph Size (vertices)')
    ax1.set_ylabel('Speedup (Dijkstra / Breakthrough)')
    ax1.set_title('Speedup Comparison', fontweight='bold')
    ax1.legend(fontsize=9)
    ax1.grid(True, alpha=0.3)
    ax1.set_xscale('log')

    # Plot 2: Time comparison
    ax2 = plt.subplot(2, 2, 2)
    for i, graph_type in enumerate(graph_types):
        data = df[df['graph_type'] == graph_type].sort_values('n')
        ax2.plot(data['n'], data['time_breakthrough'] * 1000,
                marker='o', label=f'{graph_type.capitalize()} (B)',
                color=colors[i], linewidth=2, linestyle='-', markersize=6)
        ax2.plot(data['n'], data['time_dijkstra'] * 1000,
                marker='s', label=f'{graph_type.capitalize()} (D)',
                color=colors[i], linewidth=2, linestyle='--', markersize=6, alpha=0.7)
    ax2.set_xlabel('Graph Size (vertices)')
    ax2.set_ylabel('Time (milliseconds)')
    ax2.set_title('Execution Time: Breakthrough (solid) vs Dijkstra (dashed)', fontweight='bold')
    ax2.legend(fontsize=8, ncol=2)
    ax2.grid(True, alpha=0.3)
    ax2.set_xscale('log')
    ax2.set_yscale('log')

    # Plot 3: Memory usage
    ax3 = plt.subplot(2, 2, 3)
    for i, graph_type in enumerate(graph_types):
        data = df[df['graph_type'] == graph_type].sort_values('n')
        ax3.plot(data['n'], data['memory_mb'],
                marker='o', label=graph_type.capitalize(),
                color=colors[i], linewidth=2, markersize=6)
    ax3.set_xlabel('Graph Size (vertices)')
    ax3.set_ylabel('Memory Usage (MB)')
    ax3.set_title('Peak Memory Usage', fontweight='bold')
    ax3.legend(fontsize=9)
    ax3.grid(True, alpha=0.3)
    ax3.set_xscale('log')
    ax3.set_yscale('log')

    # Plot 4: Edges vs time for breakthrough
    ax4 = plt.subplot(2, 2, 4)
    for i, graph_type in enumerate(graph_types):
        data = df[df['graph_type'] == graph_type].sort_values('m')
        ax4.scatter(data['m'], data['time_breakthrough'] * 1000,
                   label=graph_type.capitalize(),
                   color=colors[i], s=80, alpha=0.7)
    ax4.set_xlabel('Number of Edges')
    ax4.set_ylabel('Breakthrough Time (milliseconds)')
    ax4.set_title('Breakthrough Time vs Edge Count', fontweight='bold')
    ax4.legend(fontsize=9)
    ax4.grid(True, alpha=0.3)
    ax4.set_xscale('log')
    ax4.set_yscale('log')

    plt.suptitle('SSSP Breakthrough Algorithm - Comprehensive Performance Analysis',
                 fontsize=16, fontweight='bold', y=0.995)
    plt.tight_layout(rect=[0, 0, 1, 0.99])
    plt.savefig(output, dpi=300, bbox_inches='tight')
    print(f"Saved combined plot to {output}")

def print_statistics(df):
    """Print summary statistics"""
    print("\n" + "="*70)
    print("Performance Statistics Summary")
    print("="*70 + "\n")

    print(f"Total tests run: {len(df)}")
    print(f"Graph size range: {df['n'].min():,} to {df['n'].max():,} vertices")
    print(f"Edge count range: {df['m'].min():,} to {df['m'].max():,} edges")
    print()

    print("Speedup Statistics:")
    print(f"  Average:  {df['speedup'].mean():.3f}x")
    print(f"  Median:   {df['speedup'].median():.3f}x")
    print(f"  Min:      {df['speedup'].min():.3f}x")
    print(f"  Max:      {df['speedup'].max():.3f}x")
    print()

    breakthrough_faster = (df['speedup'] > 1.0).sum()
    dijkstra_faster = (df['speedup'] <= 1.0).sum()
    print("Winner by test count:")
    print(f"  Breakthrough faster: {breakthrough_faster} ({100*breakthrough_faster/len(df):.1f}%)")
    print(f"  Dijkstra faster:     {dijkstra_faster} ({100*dijkstra_faster/len(df):.1f}%)")
    print()

    print("By graph type:")
    for graph_type in df['graph_type'].unique():
        data = df[df['graph_type'] == graph_type]
        avg_speedup = data['speedup'].mean()
        print(f"  {graph_type.capitalize():10s}: {avg_speedup:.3f}x average speedup")
    print()

    print("Correctness:")
    all_correct = df['match'].all()
    print(f"  All results match: {all_correct}")
    if not all_correct:
        mismatches = (~df['match']).sum()
        print(f"  Mismatches found: {mismatches}")
    print()

def main():
    """Main function"""
    print("\n" + "="*70)
    print("SSSP Breakthrough Algorithm - Performance Plotting Tool")
    print("="*70 + "\n")

    # Load results
    filename = sys.argv[1] if len(sys.argv) > 1 else 'performance_results.csv'
    df = load_results(filename)

    # Print statistics
    print_statistics(df)

    # Generate plots
    print("Generating plots...")
    print("-" * 70)
    plot_speedup(df)
    plot_absolute_times(df)
    plot_memory_usage(df)
    plot_combined_comparison(df)

    print("\n" + "="*70)
    print("All plots generated successfully!")
    print("="*70 + "\n")

    print("Generated files:")
    print("  - performance_speedup.png   (speedup comparison)")
    print("  - performance_time.png      (absolute time comparison)")
    print("  - performance_memory.png    (memory usage)")
    print("  - performance_combined.png  (all-in-one view)")
    print()

if __name__ == '__main__':
    main()
