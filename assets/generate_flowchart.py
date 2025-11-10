#!/usr/bin/env python3
"""
Generate algorithm flowchart for SSSP Breakthrough Algorithm.

Creates a professional flowchart showing:
- Three main algorithms (FindPivots, BaseCase, BMSSP)
- Decision points and recursion structure
- Data flow between components
- Frontier reduction visualization

Output: algorithm_flowchart.svg
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch, Circle, Rectangle, Wedge
from matplotlib.patches import ConnectionPatch
import numpy as np

# Professional styling
plt.rcParams['font.family'] = 'sans-serif'
plt.rcParams['font.sans-serif'] = ['Arial', 'DejaVu Sans', 'sans-serif']
plt.rcParams['font.size'] = 10
plt.rcParams['figure.dpi'] = 300
plt.rcParams['savefig.dpi'] = 300

# Color scheme
COLORS = {
    'main_algo': '#0173B2',      # Blue - main algorithm
    'subroutine': '#DE8F05',     # Orange - subroutines
    'decision': '#029E73',       # Green - decisions
    'data': '#CC78BC',           # Purple - data structures
    'process': '#56B4E9',        # Light blue - processes
    'recursion': '#E69F00',      # Dark orange - recursion
    'text': '#000000',           # Black text
    'arrow': '#333333',          # Dark gray arrows
    'highlight': '#F0E442',      # Yellow highlight
}

def create_box(ax, x, y, width, height, text, color, style='round',
               text_color='white', fontsize=10, fontweight='normal'):
    """Create a styled box with text."""
    if style == 'round':
        box = FancyBboxPatch((x - width/2, y - height/2), width, height,
                            boxstyle="round,pad=0.05",
                            facecolor=color, edgecolor='black',
                            linewidth=1.5, zorder=2)
    elif style == 'diamond':
        # Diamond for decisions
        points = np.array([
            [x, y + height/2],      # top
            [x + width/2, y],       # right
            [x, y - height/2],      # bottom
            [x - width/2, y],       # left
        ])
        box = mpatches.Polygon(points, closed=True,
                              facecolor=color, edgecolor='black',
                              linewidth=1.5, zorder=2)
    elif style == 'cylinder':
        # Cylinder for data
        rect = Rectangle((x - width/2, y - height/2), width, height,
                        facecolor=color, edgecolor='black',
                        linewidth=1.5, zorder=2)
        ax.add_patch(rect)
        # Add ellipse top
        ellipse_top = mpatches.Ellipse((x, y + height/2), width, height*0.2,
                                       facecolor=color, edgecolor='black',
                                       linewidth=1.5, zorder=3)
        ax.add_patch(ellipse_top)
        box = None  # Already added
    else:  # rectangle
        box = Rectangle((x - width/2, y - height/2), width, height,
                       facecolor=color, edgecolor='black',
                       linewidth=1.5, zorder=2)

    if box is not None:
        ax.add_patch(box)

    # Add text
    ax.text(x, y, text, ha='center', va='center',
           color=text_color, fontsize=fontsize, fontweight=fontweight,
           zorder=4, wrap=True)

    return box

def create_arrow(ax, x1, y1, x2, y2, label='', style='solid', color=None):
    """Create a styled arrow with optional label."""
    if color is None:
        color = COLORS['arrow']

    if style == 'dashed':
        linestyle = '--'
    elif style == 'dotted':
        linestyle = ':'
    else:
        linestyle = '-'

    arrow = FancyArrowPatch((x1, y1), (x2, y2),
                           arrowstyle='->', mutation_scale=20,
                           linewidth=2, color=color,
                           linestyle=linestyle, zorder=1)
    ax.add_patch(arrow)

    # Add label if provided
    if label:
        mid_x, mid_y = (x1 + x2) / 2, (y1 + y2) / 2
        # Position label slightly offset from arrow
        offset_x = 0.15 if x2 - x1 != 0 else 0
        offset_y = 0.1 if y2 - y1 != 0 else 0
        ax.text(mid_x + offset_x, mid_y + offset_y, label,
               fontsize=8, style='italic', ha='center',
               bbox=dict(boxstyle='round,pad=0.3', facecolor='white',
                        edgecolor='none', alpha=0.8), zorder=5)

def create_flowchart():
    """Create the main algorithm flowchart."""
    fig, ax = plt.subplots(1, 1, figsize=(14, 18))
    ax.set_xlim(-1, 13)
    ax.set_ylim(0, 20)
    ax.axis('off')

    # Title
    ax.text(6, 19.5, 'SSSP Breakthrough Algorithm Flowchart',
           fontsize=18, fontweight='bold', ha='center')
    ax.text(6, 19.1, r'Breaking the $O(m + n \log n)$ Barrier',
           fontsize=12, ha='center', style='italic', color='gray')

    # ====================================================================
    # MAIN ALGORITHM (BMSSP) - Center column
    # ====================================================================

    # Start
    create_box(ax, 6, 18, 1.5, 0.5, 'START', COLORS['main_algo'],
              fontweight='bold')
    create_arrow(ax, 6, 17.75, 6, 17.3)

    # Input parameters
    create_box(ax, 6, 17, 2.5, 0.5, 'BMSSP(G, S, U, ℓ, B)',
              COLORS['main_algo'], fontweight='bold', fontsize=11)
    ax.text(6, 16.6, 'Graph G, Sources S, Vertices U\nLevel ℓ, Bound B',
           fontsize=8, ha='center', color='gray', style='italic')

    create_arrow(ax, 6, 16.5, 6, 16.1)

    # Decision: Base case?
    create_box(ax, 6, 15.7, 1.8, 0.8, 'ℓ = 0?', COLORS['decision'],
              style='diamond', fontsize=11)

    # Base case branch (YES - left)
    create_arrow(ax, 5.1, 15.7, 2.5, 15.7, label='Yes',
                color=COLORS['subroutine'])

    # Recursive case (NO - down)
    create_arrow(ax, 6, 15.3, 6, 14.9, label='No')

    # ====================================================================
    # BASE CASE ALGORITHM - Left side
    # ====================================================================

    ax.text(2.5, 16.5, 'BASE CASE', fontsize=12, fontweight='bold',
           ha='center', color=COLORS['subroutine'])

    create_box(ax, 2.5, 15.7, 2.2, 0.6, 'BaseCase(G, S, U, k)',
              COLORS['subroutine'], fontweight='bold')

    create_arrow(ax, 2.5, 15.4, 2.5, 14.9)

    create_box(ax, 2.5, 14.6, 2.0, 0.5, 'Initialize\nPriority Queue Q',
              COLORS['process'])

    create_arrow(ax, 2.5, 14.35, 2.5, 13.9)

    create_box(ax, 2.5, 13.6, 2.0, 0.6, 'Insert sources S\ninto Q',
              COLORS['process'])

    create_arrow(ax, 2.5, 13.3, 2.5, 12.8)

    # Mini-Dijkstra loop
    create_box(ax, 2.5, 12.4, 2.2, 0.7, 'Extract k+1\nclosest vertices\n(Mini-Dijkstra)',
              COLORS['process'], fontsize=9)

    create_arrow(ax, 2.5, 12.05, 2.5, 11.5)

    create_box(ax, 2.5, 11.2, 2.0, 0.5, 'Set boundary B\'',
              COLORS['process'])

    create_arrow(ax, 2.5, 10.95, 2.5, 10.5)

    create_box(ax, 2.5, 10.2, 2.0, 0.5, 'Return\ndistances',
              COLORS['subroutine'])

    # Return to main
    create_arrow(ax, 2.5, 9.95, 6, 9.95, style='dashed')
    create_arrow(ax, 6, 9.95, 6, 9.6)

    # ====================================================================
    # RECURSIVE CASE - Center (continuing from decision)
    # ====================================================================

    create_box(ax, 6, 14.6, 2.5, 0.5, 'Call FindPivots',
              COLORS['main_algo'])

    # Arrow to FindPivots (right)
    create_arrow(ax, 7.25, 14.6, 9.5, 14.6, color=COLORS['subroutine'])

    # ====================================================================
    # FINDPIVOTS ALGORITHM - Right side
    # ====================================================================

    ax.text(10.5, 16.5, 'FIND PIVOTS', fontsize=12, fontweight='bold',
           ha='center', color=COLORS['subroutine'])

    create_box(ax, 10.5, 15.7, 2.5, 0.6, 'FindPivots(G, S, U, B, k)',
              COLORS['subroutine'], fontweight='bold', fontsize=10)

    create_arrow(ax, 10.5, 15.4, 10.5, 14.9)

    create_box(ax, 10.5, 14.6, 2.2, 0.5, 'Initialize W = S\nd[s] = 0 ∀s ∈ S',
              COLORS['process'], fontsize=9)

    create_arrow(ax, 10.5, 14.35, 10.5, 13.9)

    # k iterations loop
    create_box(ax, 10.5, 13.5, 2.2, 0.7, 'For i = 1 to k:\n  Relax edges\n  Add to W',
              COLORS['process'], fontsize=9)

    # Frontier reduction visualization (small boxes)
    ax.text(10.5, 12.6, 'Frontier Reduction:', fontsize=9, ha='center',
           fontweight='bold', color=COLORS['highlight'])

    # Before: many vertices
    for i in range(10):
        circle = Circle((9.5 + i*0.2, 12.2), 0.08,
                       facecolor=COLORS['data'], edgecolor='black', linewidth=0.5)
        ax.add_patch(circle)
    ax.text(10.5, 12.0, '|S| sources', fontsize=7, ha='center')

    # Arrow down
    create_arrow(ax, 10.5, 11.85, 10.5, 11.55, style='solid',
                color=COLORS['highlight'])

    # After: fewer vertices (pivots)
    for i in range(4):
        circle = Circle((9.95 + i*0.3, 11.35), 0.1,
                       facecolor=COLORS['recursion'], edgecolor='black',
                       linewidth=1.0)
        ax.add_patch(circle)
    ax.text(10.5, 11.1, '|P| ≤ |S|/k pivots', fontsize=7, ha='center',
           fontweight='bold')

    create_arrow(ax, 10.5, 10.9, 10.5, 10.5)

    create_box(ax, 10.5, 10.2, 2.2, 0.6, 'Build forest\nIdentify pivots P',
              COLORS['process'], fontsize=9)

    create_arrow(ax, 10.5, 9.9, 10.5, 9.5)

    create_box(ax, 10.5, 9.2, 2.0, 0.5, 'Return P, W',
              COLORS['subroutine'])

    # Return to main
    create_arrow(ax, 9.5, 9.2, 7.25, 14.0, style='dashed')

    # ====================================================================
    # MAIN ALGORITHM - Continue after FindPivots
    # ====================================================================

    create_arrow(ax, 6, 14.35, 6, 13.9)

    create_box(ax, 6, 13.6, 2.5, 0.5, 'Receive pivots P',
              COLORS['main_algo'])

    create_arrow(ax, 6, 13.35, 6, 12.9)

    # Data structure
    create_box(ax, 6, 12.5, 2.5, 0.6, 'Initialize\nData Structure D',
              COLORS['data'], style='cylinder', text_color='white', fontsize=10)

    create_arrow(ax, 6, 12.2, 6, 11.7)

    # Main iteration loop
    create_box(ax, 6, 11.3, 2.8, 0.9, 'For each iteration:\n  Pull batch S\' from D\n  Process sources',
              COLORS['main_algo'], fontsize=9)

    create_arrow(ax, 6, 10.85, 6, 10.4)

    # Recursion
    rect = Rectangle((5, 9.8), 2, 1.1, facecolor=COLORS['recursion'],
                    edgecolor='black', linewidth=2, linestyle='--',
                    alpha=0.3, zorder=0)
    ax.add_patch(rect)
    ax.text(6, 10.65, 'RECURSIVE CALL', fontsize=9, ha='center',
           fontweight='bold', color=COLORS['recursion'])

    create_box(ax, 6, 10.25, 2.3, 0.6, 'BMSSP(G, S\', U, ℓ-1, B)',
              COLORS['recursion'], fontweight='bold', fontsize=10)

    # Recursion arrow (self-loop)
    arc = mpatches.FancyBboxPatch((4.5, 10.0), 0.4, 0.5,
                                 boxstyle="round,pad=0.02",
                                 facecolor='none', edgecolor=COLORS['recursion'],
                                 linewidth=2, linestyle='--')
    ax.add_patch(arc)
    ax.annotate('', xy=(4.7, 10.25), xytext=(4.7, 9.95),
               arrowprops=dict(arrowstyle='->', lw=2,
                             color=COLORS['recursion']))
    ax.text(4.0, 10.25, 'Recurse', fontsize=8, rotation=90,
           va='center', color=COLORS['recursion'], fontweight='bold')

    create_arrow(ax, 6, 9.95, 6, 9.5)

    # Edge relaxation
    create_box(ax, 6, 9.2, 2.5, 0.5, 'Relax edges from\ncompleted vertices',
              COLORS['process'], fontsize=9)

    create_arrow(ax, 6, 8.95, 6, 8.5)

    # Update data structure
    create_box(ax, 6, 8.2, 2.5, 0.5, 'BatchPrepend\nnew vertices to D',
              COLORS['data'], fontsize=9)

    create_arrow(ax, 6, 7.95, 6, 7.5)

    # Termination check
    create_box(ax, 6, 7.1, 2.0, 0.7, 'More pivots\nto process?',
              COLORS['decision'], style='diamond', fontsize=9)

    # Loop back
    create_arrow(ax, 7.0, 7.1, 8.5, 7.1, label='Yes', style='dashed')
    create_arrow(ax, 8.5, 7.1, 8.5, 11.3, style='dashed',
                color=COLORS['main_algo'])
    create_arrow(ax, 8.5, 11.3, 7.4, 11.3, style='dashed',
                color=COLORS['main_algo'])

    # Continue down
    create_arrow(ax, 6, 6.75, 6, 6.3, label='No')

    # Completion
    create_box(ax, 6, 6.0, 2.5, 0.5, 'All pivots processed',
              COLORS['main_algo'])

    create_arrow(ax, 6, 5.75, 6, 5.3)

    create_box(ax, 6, 5.0, 2.5, 0.5, 'Consolidate results',
              COLORS['main_algo'])

    create_arrow(ax, 6, 4.75, 6, 4.3)

    # Return decision
    create_box(ax, 6, 3.9, 2.0, 0.7, 'ℓ > 0?', COLORS['decision'],
              style='diamond', fontsize=10)

    # Recursive return
    create_arrow(ax, 7.0, 3.9, 9.0, 3.9, label='Yes', style='dashed',
                color=COLORS['recursion'])
    ax.text(9.5, 3.9, 'Return to\ncaller', fontsize=8, ha='left',
           color=COLORS['recursion'], style='italic')

    # Final return
    create_arrow(ax, 6, 3.55, 6, 3.1, label='No')

    create_box(ax, 6, 2.8, 2.5, 0.5, 'Return distances\nd[v] = δ(s,v)',
              COLORS['main_algo'], fontweight='bold')

    create_arrow(ax, 6, 2.55, 6, 2.1)

    # End
    create_box(ax, 6, 1.8, 1.5, 0.5, 'END', COLORS['main_algo'],
              fontweight='bold')

    # ====================================================================
    # Legend and Annotations
    # ====================================================================

    # Legend box
    legend_x, legend_y = 0.5, 5.5
    legend_items = [
        (COLORS['main_algo'], 'Main Algorithm (BMSSP)'),
        (COLORS['subroutine'], 'Subroutines'),
        (COLORS['decision'], 'Decision Points'),
        (COLORS['process'], 'Processing Steps'),
        (COLORS['data'], 'Data Structures'),
        (COLORS['recursion'], 'Recursion'),
    ]

    ax.text(legend_x + 1.0, legend_y + 0.4, 'LEGEND', fontsize=11,
           fontweight='bold', ha='center')

    for i, (color, label) in enumerate(legend_items):
        y_pos = legend_y - i * 0.4
        rect = Rectangle((legend_x, y_pos - 0.15), 0.3, 0.3,
                        facecolor=color, edgecolor='black', linewidth=1)
        ax.add_patch(rect)
        ax.text(legend_x + 0.4, y_pos, label, fontsize=8, va='center')

    # Key insights box
    insights_x, insights_y = 0.3, 2.5
    insight_text = (
        'KEY INSIGHTS:\n'
        '• FindPivots reduces frontier: O(n) → O(n/k)\n'
        '• BaseCase: Mini-Dijkstra for ℓ=0\n'
        '• BMSSP: Recursive divide-and-conquer\n'
        '• Parameters: k = log^(1/3) n, t = log^(2/3) n\n'
        '• Recursion depth: O(log^(1/3) n) levels\n'
        '• Total complexity: O(m log^(2/3) n)'
    )
    ax.text(insights_x, insights_y, insight_text, fontsize=7,
           verticalalignment='top',
           bbox=dict(boxstyle='round,pad=0.5', facecolor='lightyellow',
                    edgecolor='black', linewidth=1))

    # Complexity annotations
    ax.text(2.5, 8.5, r'Time: $O(|S| \cdot k \log k)$',
           fontsize=7, ha='center', color='gray',
           bbox=dict(boxstyle='round,pad=0.3', facecolor='white',
                    edgecolor='gray', linewidth=0.5))

    ax.text(10.5, 8.5, r'Time: $O(k \cdot |E(U)|)$',
           fontsize=7, ha='center', color='gray',
           bbox=dict(boxstyle='round,pad=0.3', facecolor='white',
                    edgecolor='gray', linewidth=0.5))

    ax.text(6, 1.2, r'Overall: $O(m \log^{2/3} n)$ time, $O(n+m)$ space',
           fontsize=9, ha='center', fontweight='bold',
           bbox=dict(boxstyle='round,pad=0.5', facecolor='lightgreen',
                    edgecolor='darkgreen', linewidth=2))

    # Add watermark
    ax.text(12.5, 0.3, 'SSSP Breakthrough Algorithm',
           fontsize=7, ha='right', color='gray', alpha=0.5, style='italic')

    plt.tight_layout()
    return fig

# Generate the flowchart
print("Generating algorithm flowchart...")
fig = create_flowchart()

# Save as SVG (scalable vector graphics)
svg_path = 'algorithm_flowchart.svg'
fig.savefig(svg_path, format='svg', bbox_inches='tight',
            facecolor='white', edgecolor='none')
print(f"[OK] Generated: {svg_path}")
print(f"  Format: SVG (scalable vector graphics)")
print(f"  Size: {fig.get_size_inches()[0]:.1f}\" x {fig.get_size_inches()[1]:.1f}\"")

# Also save as PNG for quick preview
png_path = 'algorithm_flowchart.png'
fig.savefig(png_path, format='png', dpi=300, bbox_inches='tight',
            facecolor='white', edgecolor='none')
print(f"[OK] Generated: {png_path} (preview)")

# Also save as PDF
pdf_path = 'algorithm_flowchart.pdf'
fig.savefig(pdf_path, format='pdf', bbox_inches='tight',
            facecolor='white', edgecolor='none')
print(f"[OK] Generated: {pdf_path} (presentation format)")

plt.close()

print("\nFlowchart features:")
print("  - Three main algorithms clearly delineated")
print("  - Decision diamonds for branching logic")
print("  - Recursion structure with visual feedback")
print("  - Frontier reduction visualization")
print("  - Data flow arrows with labels")
print("  - Color-coded components (main/subroutines/decisions)")
print("  - Complexity annotations")
print("  - Legend and key insights")
print("\nAll components use professional styling:")
print("  - Sans-serif fonts (Arial)")
print("  - Colorblind-friendly palette")
print("  - Clear visual hierarchy")
print("  - Scalable vector format (SVG)")
