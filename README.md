# Insight-SNE

**Insight-SNE** is an interactive visual analytics tool that helps users explore **t-SNE embeddings** through **gradient-based explanations**. Rather than only displaying a low-dimensional projection, Insight-SNE provides insights into *why* data points are positioned where they are by highlighting the influence of the original features on the embedding.

The explanation method is based on the paper:

> **Gradient-based explanation for non-linear non-parametric dimensionality reduction**
> https://link.springer.com/article/10.1007/s10618-024-01055-6

## Motivation

Dimensionality reduction techniques such as **t-SNE** are widely used to visualize high-dimensional datasets. While they reveal meaningful structures and clusters, interpreting these visualizations remains challenging because the relationship between the original features and the embedding is often opaque.

Insight-SNE addresses this limitation by combining interactive visualization with gradient-based explanations, allowing users to better understand how individual features contribute to the placement of samples in the embedding.

This project originated during my PhD research and builds upon the interactive prototype presented in:

> **Insight-SNE: Understanding t-SNE Embeddings through Interactive Explanation**
> https://www.esann.org/sites/default/files/proceedings/2024/ES2024-190.pdf

## Features

* 📊 Interactive exploration of t-SNE embeddings
* 🎯 Gradient-based explanations for individual samples
* 🖱️ Interactive selection of groups of points directly in the embedding
* 📈 Aggregation of explanations across selected samples to identify common patterns and feature importance
* 🔍 Exploration of both local (individual) and group-level explanations
* 🧪 Research-oriented platform for experimenting with explainable dimensionality reduction

## Current Status

Insight-SNE is currently a **work in progress**. While the core functionality is operational, several aspects are still under active development, including:

* Improving overall stability and fixing bugs
* Better support for datasets with a large number of features
* More scalable and informative visualizations
* Performance optimizations for larger datasets
* Additional explanation and exploration capabilities
* User interface improvements

## Research Background

This repository implements and extends the methodology proposed in:

* **Gradient-based explanation for non-linear non-parametric dimensionality reduction** (2024): introduces the gradient-based explanation framework used by Insight-SNE.
* **Insight-SNE: Understanding t-SNE Embeddings through Interactive Explanation** (ESANN 2024): presents the first interactive prototype and its user evaluation.

The long-term objective is to develop a robust visual analytics tool that enables data scientists, machine learning practitioners, and researchers to better understand and interpret t-SNE embeddings through interactive, explainable exploration.
