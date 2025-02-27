import sys
import ast

import json
import numpy as np

from tsne import compute_tsne
from gradients import compute_all_gradients, compute_mean_magnitudes, compute_magnitudes, compute_mean_gradients

if __name__ == "__main__":

    input_data, perplexity, iterations = sys.argv[1], float(sys.argv[2]), int(sys.argv[3])

    X = ast.literal_eval(input_data)
    X = np.array(X, dtype=np.float64)

    # Perform t-SNE
    Y, P, Q, sigma = compute_tsne(X, no_dims=2, perplexity=perplexity, max_iter=iterations)
    gradients = compute_all_gradients(X, Y, P, Q, sigma)
    mean_magnitudes = compute_mean_magnitudes(gradients)
    mean_gradients = compute_mean_gradients(gradients)
    magnitudes = compute_magnitudes(gradients)

    # Output the result as a JSON-like string
    result = {
        "Y": Y.tolist(),
        "P": P.tolist(),
        "Q": Q.tolist(),
        "sigma": sigma.tolist(),
        "gradients": gradients.tolist(),
        "mean_gradients": mean_gradients.tolist(),
        "mean_magnitudes": mean_magnitudes.tolist(),
        "magnitudes": magnitudes.tolist()
    }

    print(json.dumps(result), flush=True) # flush is usefull ?
