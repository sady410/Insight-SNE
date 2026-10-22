# Insight-SNE

**Explore the features behind your t-SNE embedding.**

Insight-SNE is a desktop research tool for exploring t-SNE embeddings through gradient-based explanations. It connects points in a two-dimensional embedding to their original features, helping you inspect local feature influence, compare samples and selections, and explore where explanations behave similarly.

**[Visit the Insight-SNE website](https://insight-sne.netlify.app/)** · **[Download the current version](https://insight-sne.netlify.app/#download)** · **[Read the research paper](https://link.springer.com/article/10.1007/s10618-024-01055-6)**

## A new desktop version

This repository contains the original Qt implementation of Insight-SNE, which used the Python explanation implementation. The current desktop version builds on that work, integrates the C++ explanation implementation, and brings the analysis into a more complete exploration workspace. No Python installation is required to run the new application.

For the current product, downloads, screenshots, and getting-started instructions, head to **[insight-sne.netlify.app](https://insight-sne.netlify.app/)**. This repository remains a reference for the original implementation; its source does not represent the current desktop release.

## What you can do in the current version

- **Import and configure your data.** Load a CSV, select rows and numeric features, choose labels, and configure perplexity, iterations, standardization, and the random seed.
- **Explore an embedding interactively.** Focus individual samples, pin points, create lasso selections, and inspect their original values in a linked data table.
- **Inspect feature explanations.** Rank feature importance, examine signed X/Y directions, and inspect direction consistency across selected samples.
- **Compare targets.** Compare two to four points, classes, or selections on shared scales, with an explicit reference target and the embedding alongside the comparison.
- **Explore spatial behavior.** Display gradient vectors, feature and gradient-magnitude contours, and exact point-scope contours. Rank samples by scope similarity to a reference point and turn filtered results into a selection.
- **Save and export your work.** Save a portable `.insightsne` session and reopen it without rerunning t-SNE. Export embeddings, memberships, explanations, and point-scope results as CSV, or save the embedding as PNG, SVG, or PDF.

The current application uses C++17, Qt Widgets, and QCustomPlot, with exact t-SNE and the C++ gradient-based explanation method integrated into the desktop workflow.

## Get started

Download a package for your platform from the [website's download section](https://insight-sne.netlify.app/#download). The website provides Windows and Linux downloads, matching source archives, checksums, and a first-session guide.

For a first exploration, use the bundled Iris dataset:

1. Open `iris.csv` and select the semicolon (`;`) separator.
2. Keep the four flower measurements as features. Set `Targets` as the label column and choose **Classes**; exclude it from the numeric features.
3. Set **perplexity to 15**, **400 iterations**, **seed 42**, and **standardization disabled**.
4. Run the analysis, then click a point or make a lasso selection to inspect its explanations.
5. Compare targets, explore vectors and scope, and save your session or export a figure.

See the [first-session instructions](https://insight-sne.netlify.app/#first-session) for more guidance.

## Research and interpretation

Insight-SNE implements the gradient-based explanation method described in the [research paper published in Data Mining and Knowledge Discovery](https://link.springer.com/article/10.1007/s10618-024-01055-6). The [standalone C++ numerical implementation](https://github.com/sady410/tsne-explanations-cpp) is available separately for inspecting the method and running numerical workflows.

The explanations describe local feature influence on embedding positions. They should be interpreted alongside the original data and the settings used to produce the embedding.

In the current release, scope is defined for individual point targets. Importance and direction summaries for selections and classes are subset aggregation extensions. The application uses exact t-SNE, whose quadratic cost limits practical dataset size. A fixed seed supports repeatability within a fixed build; results can differ across compilers and platforms.

## Project development

The original Qt version was developed by the author using the Python explanation implementation. Coding agents were used during the later C++ integration and application refinement, particularly for the interface and user experience. Project direction, scientific choices, review, and release decisions remain the author's responsibility.

## License

Insight-SNE is distributed under the GNU General Public License v3. See the license and third-party notices supplied with the application. The standalone C++ numerical implementation has its own license in its repository.
