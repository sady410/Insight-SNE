import sys
import ast
import json
import numpy as np
from tsne import compute_tsne
from gradients import compute_all_gradients, compute_mean_magnitudes, compute_magnitudes, compute_mean_gradients
from sklearn.preprocessing import StandardScaler

if __name__ == "__main__":
    # Parse command line arguments
    input_data = sys.argv[1]
    perplexity = float(sys.argv[2])
    iterations = int(sys.argv[3])
    standardize = bool(int(sys.argv[4]))  # Convert "0"/"1" to False/True

    # Parse input data
    X = ast.literal_eval(input_data)
    X = np.array(X, dtype=np.float64)

    # Apply standardization if requested using StandardScaler
    if standardize:
        scaler = StandardScaler()
        X = scaler.fit_transform(X)

    # Perform t-SNE
    Y, P, Q, sigma = compute_tsne(X, no_dims=2, perplexity=perplexity, max_iter=iterations)

    # Compute gradients and related metrics
    gradients = compute_all_gradients(X, Y, P, Q, sigma)
    mean_magnitudes = compute_mean_magnitudes(gradients)
    mean_gradients = compute_mean_gradients(gradients)
    magnitudes = compute_magnitudes(gradients)

    # Output the result as JSON
    result = {
        "Y": Y.tolist(),
        "P": P.tolist(),
        "Q": Q.tolist(),
        "sigma": sigma.tolist(),
        "gradients": gradients.tolist(),
        "mean_gradients": mean_gradients.tolist(),
        "mean_magnitudes": mean_magnitudes.tolist(),
        "magnitudes": magnitudes.tolist(),
    }

    print(json.dumps(result), flush=True)
