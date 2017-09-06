###################################################################################################
### Written by : Eric Tan
###
### Basic implementation of LU factorization with partial pivoting to solve a linear system Ax = b.
###################################################################################################

import numpy as np


def lu(A):
    """ lu(A)

    Purpose: Takes in a square matrix A and performs lu factorization with partial pivoting.

    Inputs:
        --A :: n x n matrix

    Output:
        --L :: n x n matrix. Lower part of the factorized matrix. Contains 1's on the diagonal.
        --U :: n x n matrix. Upper part of the factorized matrix.
        --p :: array of size n. Contains an array from 0 to n. Indices corresponds to swaps made
               while factorizing A.
    """

    (m, n) = A.shape

    if m != n:
        print "Error: Matrix should be square."
        print "Exiting LU routine."
        return

    p = np.arange(n)

    for k in np.arange(n-1):
        m = np.argmax(np.abs(A[k:][k])) + k

        if A[m, k] != 0:
            if m != k:
                A[[k, m]] = A[[m, k]]
                p[[k, m]] = p[[m, k]]

            A[k+1:, k] = A[k+1:, k] / A[k, k]
            A[k+1:n, k+1:n] = A[k+1:n, k+1:n] - (A[k+1:n, k] * A[k, k+1:n])

    # Split matrices to L and U
    L = np.tril(A, -1) + np.eye(n)
    U = np.triu(A)

    return (L, U, p)


def forwardsub(L, b):
    """ forwardsub(L, b)

    Purpose: Takes an lower triangular matrix and a vector and computes the solution vector.

    Inputs:
        -- L :: a n x n lower triangular matrix.
        -- b :: an array of n elements which represents a solution to the linear system.

    Outputs:
        -- x :: an array of n elements which represents the solution to Lx = b.
    """

    (m, n) = L.shape

    if m != n:
        print "Error: Matrix should be square."
        print "Exiting forward substitution routine."
        return

    x = np.zeros(n, dtype=np.float)
    for i in np.arange(n):
        x[i] = b[i]
        for j in np.arange(i):
            x[i] = x[i] - L[i, j] * x[j]

    return x


def backsub(U, b):
    """ backsub(U, b):

    Purpose: Takes an upper triangular matrix and a vector and computes the solution vector.

    Inputs:
        -- U :: an n x n upper triangular matrix.
        -- b :: an array of n elements which represents a solution to the linear system

    Outputs:
        -- x :: an array of n elements which represents the solution to Ux = b
    """

    (m, n) = U.shape

    if m != n:
        print "Error: Matrix should be square."
        print "Exiting forward substitution routine."
        return

    x = np.zeros(n, dtype=np.float)
    for i in np.arange(n-1, -1, -1):
        x[i] = b[i]
        for j in np.arange(i+1, n):
            x[i] = x[i] - U[i, j] * x[j]
        x[i] = x[i] / U[i, i]

    return x


def main():
    A = np.matrix([[8, 1, 6], [3, 5, 7], [4, 9, 2]], dtype=np.float)
    b = np.array([1, 2, 3], dtype=np.float)

    (L, U, p) = lu(A)
    y = forwardsub(L, b[p])
    x = backsub(U, y)

    print "Solution to Ax = b is ", x


if __name__ == '__main__':
    main()
