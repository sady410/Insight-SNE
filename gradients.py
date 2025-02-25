import numpy as np
import pandas as pd
from scipy.stats import hmean
from sklearn.metrics import pairwise_distances


def compute_all_gradients(X, Y, P, Q, sigma):
    gradients = []
    for i in range(X.shape[0]):
        gradients.append(compute_gradients(X, Y, P, Q, sigma, i))
    
    gradients = np.array(gradients)
    
    return gradients

def compute_gradients(X, Y, P, Q, sigma, i):
    """
    Function that compute the saliency for an image X and output y.

    Parameters:
    -----------
    i: indice of the input to consider
    
    Return:
    -------
    derivative: t-sne "saliency"
    """
    y2_derivative = _compute_y2_derivative(i, Y, P, Q)
    
    yx_derivative = _compute_xy_derivative(i, X, Y, sigma)
    
    derivative = (-np.linalg.inv(y2_derivative)) @ (yx_derivative.T)

    return derivative

def _compute_y2_derivative(i, y, P, Q):
    """
    Function that compute the second derivative of t-sne regarding y_i
    
    Parameters:
    -----------
    i: indice of the input to consider
    y: low-dimensional space embedding
    P: p-values of t-sne
    Q: q-values of t-sne
    
    Return:
    -------
    4*res: derivative regarding y_i
    """
    n = y.shape[0]
    m = y.shape[1]

    d_ij = y[i] - y
    d_ij_d = np.identity(m, np.float64)
    d_ij_d = np.tile(d_ij_d, (n, 1, 1))

    e_ij = 1 + (np.linalg.norm(d_ij, axis=1))**2
    e_ij_d = 2 * d_ij
    E_ij = 1/e_ij

    distances = 1 + pairwise_distances(y, squared=True) # refactor e_ij with this maybe
    S_q = np.sum( 1/distances ) - np.trace(1/distances)
    S_q_d = -4 * np.sum((e_ij**(-2)).reshape(1, n) @ d_ij, axis=0)
    
    v_ij = (P - Q)[i]     
    v_ij_d = ( ( e_ij_d.T * e_ij**(-2) * S_q ) + ( S_q_d.reshape(m, 1) / e_ij.reshape(1, n) ) ) / ( S_q**2 )
    
    term1 = (v_ij_d * E_ij.reshape(1, n)) @ d_ij
    term2 = (e_ij_d.T * (v_ij * E_ij**2).reshape(1, n)) @ d_ij
    term3 = np.delete( ((v_ij * E_ij).reshape(n, 1, 1) * d_ij_d), i, axis=0).sum(axis=0)
    
    return 4 * ( term1 - term2 + term3 ) 
    
def _compute_xy_derivative(i, X, y, sigma):
    """
    Function that compute the second derivative of t-sne regarding x_i

    Parameters:
    -----------
    i: indice of the input to consider
    X: instances in high-dimensional space
    y: embedding in low-dimensional space 
    sigma: sigma values found by t-sne with the chosen perplexity 
    
    Return:
    -------
    4*res: derivative regarding x_i
    """
    n = y.shape[0]
    m = y.shape[1]
    
    sigma = sigma.reshape((X.shape[0],))
    
    y_ij = y[i] - y
    x_ij = X[i] - X
    x_ji = X - X[i]
    e_ij = 1 + (np.linalg.norm(y_ij, axis=1))**2
    E_ij = 1/e_ij

    exp_ij = np.exp( -( (np.linalg.norm(x_ij, axis=1)**2) / (2*(sigma[i]**2)) ) )
    exp_ji = np.exp( -( (np.linalg.norm(x_ji, axis=1)**2) / (2*(sigma**2) ) ))

    S_pi = np.delete( exp_ij, i ).sum()

    S_pi_d = (- (x_ij) * (sigma[i]**(-2)) * exp_ij.reshape(n, 1)).sum(axis=0)
    S_pi_d = np.tile(S_pi_d , (n, 1))
    
    S_pj = ((np.exp( - pairwise_distances(X, squared=True) / ( 2*(sigma**2) ) )).sum(axis=0)) - 1 # on enlève la diagonale (quand j = l)
    S_pj_d = (sigma**(-2) * exp_ji).reshape(n,1) * x_ji

    P_ji_d = ( ( -S_pi * x_ij * sigma[i]**(-2) * exp_ij.reshape(n, 1)) - ( exp_ij.reshape(n, 1) * S_pi_d ) ) / S_pi**2
    P_ij_d = ( ( (S_pj * sigma**(-2) * exp_ji).reshape(n,1) * x_ji ) - ( exp_ji.reshape(n,1) * S_pj_d ) ) / (S_pj**2).reshape(n,1)
    
    v_ij_d = (1 / (2*n)) * (P_ji_d + P_ij_d)

    # return 4 * np.delete( ((v_ij_d * E_ij).reshape(n, 1, 1) * y_ij), i, axis=0).sum(axis=0)
    return 4 * ( v_ij_d.T @ ( y_ij * E_ij.reshape(n, 1) ) )

def compute_mean_magnitudes(gradients):
    magnitudes =  np.linalg.norm(gradients, axis=1)
    mean_magnitudes = np.mean(magnitudes, axis=0)
    return mean_magnitudes

def compute_magnitudes(gradients):
    return np.linalg.norm(gradients, axis=1)



