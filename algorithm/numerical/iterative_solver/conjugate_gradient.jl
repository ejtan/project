# conjugate_gradient.jl
#
# Basic implementation of the conjugate gradient algorithm.

using PyPlot, Distributions


function make_spd_matrix(n::Int64)
    """
    make_spd_matrix(n)

    @ Inputs
    n::Int64 - dimension of the matrix.

    @ Output
    A::Array{Float64, 2} - an n x n SPD matrix.

    Generates an n x n dense positive definite matrix with random values.
    """
    while true
        A = rand(Normal(1.0, 1.0), n, n);
        A = (A + A');
        A = A + (n * eye(n));

        if isposdef(A) && issymmetric(A)
            return A
        end
    end
end


function conjugate_gradient(A::Array{Float64, 2}, b::Array{Float64, 1}, x::Array{Float64, 1})
    """
    conjugate_gradient(A, b, x)

    @ Inputs
    A::Array{Float64, 2} - SPD matrix.
    b::Array{Float64, 1} - solution vector.
    x::Array{Float64, 2} - guess vector.

    @ Output
    x::Array{Float64, 1}   - computed solution to Ax = b.
    iter::Array{Int64, 1}  - Array containing the iteration number.
    err::Array{Float64, 1} - Array containg the 2-norm of the residual per iteration.

    Performs the unpreconditioned conjugate gradient algorithm and tracks the error per iteration.
    """
    # Check if A is a SPD matrix. If not, we return a vector of 0's
    if !isposdef(A) || !issymmetric(A)
        println("Error: Matrix A is not symmertic positive definite.")
        return zeros(length(x));
    end

    n = length(x);

    # Create 2 arrays to store data for a plot.
    iter = Array{Int64, 1}(n);
    err  = Array{Float64, 1}(n);

    r = b - (A * x);
    p = r;
    r_old_prod = r' * r;

    # Insert the 0-th iteration
    iter[1] = 0;
    err[1]  = sqrt(r_old_prod);

    for i = 1:n
        Ap = A * p;

        # Update Solution and Resudual
        α = r_old_prod / (p' * Ap);
        x = x + α * p;
        r = r - α * Ap;

        r_new_prod = r' * r;

        # Update arrays
        iter[i+1] = i;
        err[i+1]  = sqrt(r_new_prod);

        # If the residual is small enough, exit early.
        if sqrt(r_new_prod) < 1e-16
            # Resize arrays so that it only contains data in the iteration
            resize!(iter, i);
            resize!(err, i);
            break;
        end

        # Update orthogonal vector
        β = r_new_prod / r_old_prod;
        p = r + β * p;
        r_old_prod = r_new_prod;
    end

    x, iter, err;
end


n = 1000;
A = make_spd_matrix(n);
b = rand(Uniform(-100.0, 100.0), n);
x = zeros(n);

# Generate solution from CG and built in LU
(x_cg, iter, err) = conjugate_gradient(A, b, x)
x_lu = A \ b;

# Compute 2-norm error
x_err = sqrt( sum( (x_cg - x_lu).^2 ) );
println("2-norm Error = ", x_err);

# Produce a plot of the errors
(fig, ax) = subplots();
ax[:semilogy](iter, err, "r-", label="CG");
ax[:set_yscale]("log");
ax[:grid]("on");
xlabel("Number of Iterations");
ylabel("2-Norm of Resudual");
title("Convergence of CG Algorithm");
show();
