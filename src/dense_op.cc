#include "dense_op.hh"

#include <iostream>
#include <stdio.h>
#include <omp.h>

#include <math.h>
#include <typeinfo>

#include <Random123/philox.h>
#include <Random123/boxmuller.hpp>
#include <Random123/uniform.hpp>


namespace RandBLAS::dense_op {

/*
Note from Random123: Simply incrementing the counter (or key) is effectively indistinguishable
from a sequence of samples of a uniformly distributed random variable.
Question: does it matter in which order (row vs col) we fill in data here?

Note: currently, the implementation is pretty ugly.
Instead of just using a template type T, i'm having atypeid conditional
For a better version, will probably need two templates - one for the data type and one 
for the distribution type (Philox4x32, Philox4x64, etc), but that might get ugly in terms of what a user sees
and hard to use.
*/

template <typename T>
void gen_rmat_unif(int64_t n_rows, int64_t n_cols, T* mat, uint32_t seed)
{
        if (typeid(T) == typeid(float))
        {
                // 4 32-bit generated values
                typedef r123::Philox4x32 CBRNG;
                CBRNG::key_type key = {{seed}};
                // Same return type as counter
                CBRNG::ctr_type ctr = {{0,0,0,0}};
                // Definde the generator
                CBRNG gen;

                int64_t dim = n_rows * n_cols;

                // Need every thread to have its own version of key for the outer loop to be parallelizable
                // Need to figure out when fork/join overhead becomes less than time saved by parallelization
                for (int64_t i = 0; i < dim; i += 4)
                {
                        // Adding critical section around the increment should make outer loop parallelizable?
                        // Easier to increment the ctr
                        ++ctr[0];
                        CBRNG::ctr_type r = gen(ctr, key);

                        // Parallelizable, but too few iterations
                        mat[i] = r123::uneg11<T>(r.v[0]);
                        mat[i + 1] = r123::uneg11<T>(r.v[1]);
                        mat[i + 2] = r123::uneg11<T>(r.v[2]);
                        mat[i + 3] = r123::uneg11<T>(r.v[3]);
                }
        }
        else if (typeid(T) == typeid(double))
        {
                // 4 64-bit generated values
                typedef r123::Philox4x64 CBRNG;
                CBRNG::key_type key = {{seed}};
                // Same return type as counter
                CBRNG::ctr_type ctr = {{0,0,0,0}};
                // Definde the generator
                CBRNG gen;

                int64_t dim = n_rows * n_cols;

                // Need every thread to have its own version of key for the outer loop to be parallelizable
                // Need to figure out when fork/join overhead becomes less than time saved by parallelization
                for (int64_t i = 0; i < dim; i += 4)
                {
                        // Adding critical section around the increment should make outer loop parallelizable?
                        // Easier to increment the ctr
                        ++ctr[0];
                        CBRNG::ctr_type r = gen(ctr, key);

                        // Parallelizable, but too few iterations
                        mat[i] = r123::uneg11<T>(r.v[0]);
                        mat[i + 1] = r123::uneg11<T>(r.v[1]);
                        mat[i + 2] = r123::uneg11<T>(r.v[2]);
                        mat[i + 3] = r123::uneg11<T>(r.v[3]);
                }
        }
        else
        {
                printf("\nType error. Only float and double are supported for now.\n");
        }
}

template <typename T>
void gen_rmat_norm(int64_t n_rows, int64_t n_cols, T* mat, uint32_t seed)
{
        if (typeid(T) == typeid(float))
        {
                // 4 32-bit generated values
                typedef r123::Philox4x32 CBRNG;
                CBRNG::key_type key = {{seed}};
                // Same return type as counter
                CBRNG::ctr_type ctr = {{0,0,0,0}};
                // Definde the generator
                CBRNG gen;

                uint64_t dim = n_rows * n_cols;

                for (uint64_t i = 0; i < dim; i += 4)
                {
                        // Easier to increment the ctr
                        ++ctr[0];
                        CBRNG::ctr_type r = gen(ctr, key);
                        // Paralleleize

                        // Take 2 32-bit unsigned random vals, return 2 random floats
                        // Since generated vals are indistinguishable form uniform, feed them into box-muller right away
                        // Uses uneg11 & u01 under the hood
                        r123::float2 pair_1 = r123::boxmuller(r.v[0], r.v[1]);
                        r123::float2 pair_2 = r123::boxmuller(r.v[2], r.v[3]);

                        mat[i] = pair_1.x;
                        mat[i + 1] = pair_1.y;
                        mat[i + 2] = pair_2.x;
                        mat[i + 3] = pair_2.y;
                }
        }
        else if (typeid(T) == typeid(double))
        {
                // 4 64-bit generated values
                typedef r123::Philox4x64 CBRNG;
                CBRNG::key_type key = {{seed}};
                // Same return type as counter
                CBRNG::ctr_type ctr = {{0,0,0,0}};
                // Definde the generator
                CBRNG gen;

                uint64_t dim = n_rows * n_cols;

                for (uint64_t i = 0; i < dim; i += 4)
                {
                        // Easier to increment the ctr
                        ++ctr[0];
                        CBRNG::ctr_type r = gen(ctr, key);
                        // Paralleleize

                        // Take 2 32-bit unsigned random vals, return 2 random floats
                        // Since generated vals are indistinguishable form uniform, feed them into box-muller right away
                        // Uses uneg11 & u01 under the hood
                        r123::double2 pair_1 = r123::boxmuller(r.v[0], r.v[1]);
                        r123::double2 pair_2 = r123::boxmuller(r.v[2], r.v[3]);

                        mat[i] = pair_1.x;
                        mat[i + 1] = pair_1.y;
                        mat[i + 2] = pair_2.x;
                        mat[i + 3] = pair_2.y;
                }
        }        
        else
        {
                printf("\nType error. Only float and double are supported for now.\n");
        }
}

// Explicit instantiation of template functions - workaround to avoid header implementations
template void gen_rmat_unif<float>(int64_t n_rows, int64_t n_cols, float* mat, uint32_t seed);
template void gen_rmat_unif<double>(int64_t n_rows, int64_t n_cols, double* mat, uint32_t seed);

template void gen_rmat_norm<float>(int64_t n_rows, int64_t n_cols, float* mat, uint32_t seed);
template void gen_rmat_norm<double>(int64_t n_rows, int64_t n_cols, double* mat, uint32_t seed);

} // end namespace RandBLAS::dense_op