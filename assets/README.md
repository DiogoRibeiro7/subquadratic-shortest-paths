# Assets Directory

This directory contains visualization assets for the SSSP Breakthrough Algorithm project.

## Files

### Generated Visualizations

**Complexity Comparison:**
- **`complexity_comparison.png`** - Main complexity comparison visualization (300 DPI, PNG)
- **`complexity_comparison.pdf`** - Vector format for presentations (PDF)
- **`complexity_comparison.svg`** - Editable vector format (SVG)

**Algorithm Flowchart:**
- **`algorithm_flowchart.svg`** - Scalable flowchart (primary, SVG)
- **`algorithm_flowchart.pdf`** - Presentation format (PDF)
- **`algorithm_flowchart.png`** - Preview/quick reference (PNG)

### Generation Scripts

- **`generate_complexity_plot.py`** - Python script to generate complexity comparison plots
- **`generate_flowchart.py`** - Python script to generate algorithm flowchart

## Complexity Comparison Visualization

The main visualization (`complexity_comparison.png`) is a comprehensive 6-panel figure showing:

### Panel (a): Absolute Complexity Growth
- Compares O(m log^(2/3) n) vs O(m + n log n) vs O((m+n) log n)
- Log-log scale for sparse graphs (m = 5n)
- Shows crossover point where breakthrough becomes faster
- Annotated with practical size ranges (10K, 100K vertices)

### Panel (b): Speedup Analysis
- Speedup factor (Dijkstra / Breakthrough) vs graph size
- Multiple curves for different graph densities:
  - Very Sparse (m = 2n)
  - Sparse (m = 5n)
  - Medium (m = 10n)
  - Dense (m = 50n)
- Highlights regions where each algorithm is preferred
- Break-even line at 1× speedup

### Panel (c): Crossover Points
- Crossover point (where breakthrough becomes faster) vs density
- Shows how crossover varies with sparsity ratio ρ = m/n
- Annotated regions for small/medium/large graphs
- Helps determine when to use breakthrough algorithm

### Panel (d): Normalized Time per Edge
- Time per edge growth normalized to n=100
- Shows that breakthrough scales better: log^(2/3) n vs log n
- Demonstrates asymptotic advantage visually

### Panel (e): Practical Performance Estimates
- Bar chart with empirical timing data
- Four representative graph sizes from 1K to 1M vertices
- Direct comparison of Dijkstra vs Breakthrough
- Speedup labels on top of bars
- Based on actual benchmark results

### Panel (f): Speedup Trends by Graph Type
- Speedup vs graph size for three representative densities
- Shows optimal region for breakthrough algorithm
- Fill regions indicating which algorithm is faster
- Annotated with breakthrough optimal region

## Regenerating the Plots

### Requirements

```bash
pip install numpy matplotlib
```

### Generate All Formats

```bash
cd assets
python generate_complexity_plot.py
```

This will generate:
- `complexity_comparison.png` (300 DPI raster)
- `complexity_comparison.pdf` (vector, for presentations)
- `complexity_comparison.svg` (editable vector)

### Customization

Edit `generate_complexity_plot.py` to customize:

**Colors**: Modify the `COLORS` dictionary
```python
COLORS = {
    'breakthrough': '#0173B2',  # Blue
    'dijkstra_fib': '#DE8F05',  # Orange
    # ...
}
```

**Graph Size Range**: Modify `n_values`
```python
n_values = np.logspace(2, 6.5, 200)  # 100 to ~3M vertices
```

**Sparsity Levels**: Modify `sparsities` dictionary
```python
sparsities = {
    'Very Sparse (m = 2n)': 2,
    'Sparse (m = 5n)': 5,
    # ...
}
```

**Empirical Data**: Update `time_dijkstra` and `time_breakthrough` arrays
```python
time_dijkstra = [0.5, 10, 310, 7200]  # milliseconds
time_breakthrough = [0.8, 12, 210, 3100]  # milliseconds
```

## Usage in Documents

### LaTeX

```latex
\documentclass{article}
\usepackage{graphicx}

\begin{document}

\begin{figure}
  \centering
  \includegraphics[width=\textwidth]{assets/complexity_comparison.pdf}
  \caption{Complexity comparison between breakthrough and classical SSSP algorithms}
  \label{fig:complexity}
\end{figure}

\end{document}
```

### Markdown

```markdown
![Complexity Comparison](assets/complexity_comparison.png)
```

### PowerPoint/Presentations

1. Use the **PDF version** for best quality in presentations
2. Insert as an image: Insert → Pictures → `complexity_comparison.pdf`
3. The PDF will scale perfectly at any size

### Academic Papers

1. Use the **PDF version** for submission
2. 300 DPI ensures print quality
3. Vector format ensures sharp rendering at any zoom level

## Design Specifications

### Typography
- Font Family: Serif (Times New Roman)
- Font Sizes:
  - Main title: 16pt bold
  - Subplot titles: 13pt
  - Axis labels: 12pt
  - Legend/tick labels: 10-11pt

### Color Scheme
- **Colorblind-friendly** palette from Wong (2011)
- Primary colors:
  - Breakthrough: Blue (#0173B2)
  - Dijkstra (Fib): Orange (#DE8F05)
  - Dijkstra (Bin): Purple (#CC78BC)
- All colors tested for:
  - Protanopia (red-green colorblindness)
  - Deuteranopia (red-green colorblindness)
  - Tritanopia (blue-yellow colorblindness)

### Layout
- Figure size: 16" × 10" (suitable for presentations)
- Resolution: 300 DPI (publication quality)
- Grid: 2×3 subplots with controlled spacing
- White background (prints well)

### Style Guidelines
- All plots use log scales where appropriate
- Grid lines for readability (subtle, dotted)
- Clear legends with mathematical notation
- Annotations highlight key insights
- Consistent line widths (2.0-2.5pt for main curves)

## File Sizes

Approximate file sizes:
- PNG: ~500 KB (high resolution raster)
- PDF: ~150 KB (vector, scalable)
- SVG: ~200 KB (editable vector)

## References

**Color Scheme**:
Wong, B. (2011). "Points of view: Color blindness." Nature Methods 8(6): 441.
https://www.nature.com/articles/nmeth.1618

**Design Principles**:
- Tufte, E. R. (2001). "The Visual Display of Quantitative Information"
- Cleveland, W. S. (1994). "The Elements of Graphing Data"

## Version History

- **v1.0** (2025-01-10): Initial version with 6-panel comprehensive comparison
  - Absolute complexity curves
  - Speedup analysis
  - Crossover point analysis
  - Normalized growth comparison
  - Empirical performance estimates
  - Graph type trends

## License

These visualizations are part of the SSSP Breakthrough Algorithm implementation.
See the main LICENSE file for terms.

When using these figures in publications:
1. Cite the original paper (Duan et al., 2025)
2. Acknowledge this implementation
3. See CITATION.md for proper citation format

## Algorithm Flowchart

The flowchart (`algorithm_flowchart.svg`) is a comprehensive diagram showing:

### Main Components

**Three Main Algorithms:**
1. **BMSSP** (center) - Main recursive algorithm in blue
2. **FindPivots** (right) - Frontier reduction subroutine in orange
3. **BaseCase** (left) - Mini-Dijkstra for level 0 in orange

**Visual Elements:**
- **Rounded rectangles** - Process steps
- **Diamonds** - Decision points (branching logic)
- **Cylinders** - Data structures
- **Dashed boxes** - Recursive calls
- **Arrows** - Data flow and control flow
- **Color coding** - Different components clearly distinguished

### Key Features

**1. Decision Points:**
- "ℓ = 0?" - Determines base case vs recursive case
- "More pivots?" - Loop continuation condition
- "ℓ > 0?" - Return path decision

**2. Recursion Structure:**
- Visual feedback loop showing recursive BMSSP calls
- Decreasing level parameter (ℓ → ℓ-1)
- Self-referential arrow indicating recursion

**3. Frontier Reduction Visualization:**
- Before: 10 circles representing |S| sources
- Arrow showing transformation
- After: 4 circles representing |P| pivots (≤ |S|/k)
- Clear visual demonstration of key algorithm property

**4. Data Flow:**
- Labeled arrows showing parameter passing
- Dashed lines for returns
- Solid lines for forward flow
- Different colors for different flow types

**5. Complexity Annotations:**
- BaseCase: O(|S| · k log k)
- FindPivots: O(k · |E(U)|)
- Overall: O(m log^(2/3) n)

### Legend

The flowchart includes a complete legend showing:
- Main Algorithm (blue) - BMSSP core logic
- Subroutines (orange) - FindPivots and BaseCase
- Decision Points (green) - Branching diamonds
- Processing Steps (light blue) - Individual operations
- Data Structures (purple) - Queues and lists
- Recursion (dark orange) - Recursive calls

### Key Insights Box

Includes a summary of:
- Frontier reduction formula
- BaseCase purpose
- BMSSP approach
- Parameter values
- Recursion depth
- Total complexity

### Usage Recommendations

**For Presentations:**
- Use SVG for maximum scalability
- Zoom into specific sections as needed
- High resolution suitable for projection

**For Documentation:**
- PNG for web viewing
- PDF for printed materials
- SVG for editing in Illustrator/Inkscape

**For Papers:**
- PDF format recommended
- Scalable to any page size
- Professional quality at any zoom level

## Regenerating Assets

### Requirements

```bash
pip install numpy matplotlib
```

### Generate All Assets

```bash
cd assets

# Generate complexity comparison
python generate_complexity_plot.py

# Generate algorithm flowchart
python generate_flowchart.py
```

### Customize Flowchart

Edit `generate_flowchart.py` to customize:

**Colors**: Modify the `COLORS` dictionary
```python
COLORS = {
    'main_algo': '#0173B2',    # Blue
    'subroutine': '#DE8F05',   # Orange
    'decision': '#029E73',     # Green
    # ... customize as needed
}
```

**Layout**: Adjust box positions
```python
create_box(ax, x, y, width, height, text, color)
# x, y: center position
# width, height: box dimensions
```

**Text**: Modify labels and descriptions
```python
create_box(ax, 6, 17, 2.5, 0.5, 'Your Custom Text', COLORS['main_algo'])
```

## Support

For questions or issues with the visualizations:
1. Check that you have the required dependencies (`numpy`, `matplotlib`)
2. Ensure you're using Python 3.6+
3. Try regenerating with the script
4. Open an issue on the GitHub repository with the error message
