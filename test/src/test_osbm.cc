#include <RandBLAS.hh>
#include <gtest/gtest.h>
#include <math.h>
#include <iostream>

using namespace RandBLAS::osbm;
using namespace RandBLAS::dense_op;

//TODO: ORGQR write a funciton that returns the explicit column orthonormal random sampling matrix
TEST(TestOSBMConstruction, SimpleExample) {
    int64_t n_cols = 3;
    int64_t n_rows = 6;
    double V[18] = {0,0,0, 0,0,0, 0,0,0, 1,0,0, 0,1,0, 0,0,1};
    double lev[6] = {0.2, 0.3, 0.4, 0.6, 0.7, 0.8};
    OSBM<double>(n_rows,n_cols,V,lev);

    EXPECT_LT(orthogonality_test<double>(n_rows, n_cols, V, n_cols), (std::numeric_limits<double>::epsilon()*n_cols*5));
    EXPECT_LT(levscore_test<double>(n_rows, n_cols, V, lev), (std::numeric_limits<double>::epsilon()*n_cols*5));
}

TEST(TestOSBMConstruction, VaryRows_Double) {
    int64_t n_cols = 10;
    int64_t n_rows;
    int i,j;
    double sum, testsum;
    for (n_rows = 50; n_rows < 101; n_rows += 10) {
        double *V = new double[n_cols*n_rows];
        double *lev = new double[n_rows];
        std::fill(V, V+n_rows*n_cols, 0);
        for (i=0; i<n_cols; i++) {
            V[(n_rows-n_cols)*n_cols + i + i*n_cols] = 1;
        }
        RandBLAS::dense_op::gen_rmat_unif<double>(1, n_rows, lev, 0);
        blas::scal(n_rows, 0.5, lev, 1); 
        for (i=0; i<n_rows; i++) {
            lev[i] += 0.5;
            sum += lev[i];
        }
        blas::scal(n_rows, n_cols/sum, lev, 1);
        std::sort(lev, lev+n_rows);

        RandBLAS::osbm::OSBM<double>(n_rows, n_cols, V, lev);
        EXPECT_LT(orthogonality_test<double>(n_rows, n_cols, V, n_cols), (std::numeric_limits<double>::epsilon()*n_cols*5));
        EXPECT_LT(levscore_test<double>(n_rows, n_cols, V, lev), (std::numeric_limits<double>::epsilon()*n_cols*5));

        RandBLAS::dense_op::apply_haar<double>(n_rows, n_cols, V, n_cols, 1);
        EXPECT_LT(orthogonality_test<double>(n_rows, n_cols, V, n_cols), (std::numeric_limits<double>::epsilon()*n_cols*5));
        EXPECT_LT(levscore_test<double>(n_rows, n_cols, V, lev), (std::numeric_limits<double>::epsilon()*n_cols*5));

        sum = 0;

        delete[] V;
        delete[] lev;
    }
}

TEST(TestOSBMConstruction, VaryRows_Float) {
    int64_t n_cols = 10;
    int64_t n_rows;
    int i,j;
    float sum, testsum;
    for (n_rows = 50; n_rows < 101; n_rows += 10) {
        float *V = new float[n_cols*n_rows];
        float *lev = new float[n_rows];
        std::fill(V, V+n_rows*n_cols, 0);
        for (i=0; i<n_cols; i++) {
            V[(n_rows-n_cols)*n_cols + i + i*n_cols] = 1;
        }
        RandBLAS::dense_op::gen_rmat_unif<float>(1, n_rows, lev, 0);
        blas::scal(n_rows, 0.5, lev, 1); 
        for (i=0; i<n_rows; i++) {
            lev[i] += 0.5;
            sum += lev[i];
        }
        blas::scal(n_rows, n_cols/sum, lev, 1);
        std::sort(lev, lev+n_rows);
        RandBLAS::osbm::OSBM<float>(n_rows, n_cols, V, lev);

        EXPECT_LT(orthogonality_test<float>(n_rows, n_cols, V, n_cols), (std::numeric_limits<float>::epsilon()*n_cols*5));
        EXPECT_LT(levscore_test<float>(n_rows, n_cols, V, lev), (std::numeric_limits<float>::epsilon()*n_cols*5));

        sum = 0;

        delete[] V;
        delete[] lev;
    }

}

TEST(TestOSBMConstruction, LargeExample) {
    int64_t n_cols = 75;
    int64_t n_rows = 1000;
    int i,j;
    double sum = 0;
    double *V = new double[n_cols*n_rows];
    double *lev = new double[n_rows];
    std::fill(V, V+n_rows*n_cols, 0);
    for (i=0; i<n_cols; i++) {
        V[(n_rows-n_cols)*n_cols + i + i*n_cols] = 1;
    }
    RandBLAS::dense_op::gen_rmat_unif<double>(1, n_rows, lev, 0);
    blas::scal(n_rows, 0.5, lev, 1); 
    for (i=0; i<n_rows; i++) {
        lev[i] += 0.5;
        sum += lev[i];
    }
    blas::scal(n_rows, n_cols/sum, lev, 1);
    std::sort(lev, lev+n_rows);
    RandBLAS::osbm::OSBM<double>(n_rows, n_cols, V, lev);

    EXPECT_LT(orthogonality_test<double>(n_rows, n_cols, V, n_cols), (std::numeric_limits<double>::epsilon()*n_cols*5));
    EXPECT_LT(levscore_test<double>(n_rows, n_cols, V, lev), (std::numeric_limits<double>::epsilon()*n_cols*5));

    delete[] V;
    delete[] lev;
}

TEST(TestOSBMConstruction, BoundaryExample) {
    int64_t n_cols = 3;
    int64_t n_rows = 6;
    double V[18] = {0,0,0, 0,0,0, 0,0,0, 1,0,0, 0,1,0, 0,0,1};
    double lev[6] = {1e-16, 0.2, 0.5, 0.6, 0.7, 1-1e-16};
    OSBM(6,3,V,lev);

    EXPECT_LT(orthogonality_test<double>(n_rows, n_cols, V, n_cols), (std::numeric_limits<double>::epsilon()*n_cols*5));
    EXPECT_LT(levscore_test<double>(n_rows, n_cols, V, lev), (std::numeric_limits<double>::epsilon()*n_cols*5));
}

TEST(TestOSBMConstruction, RandomSample) {
    int64_t n_cols = 3;
    int64_t n_rows = 6;
    double V[18] = {0,0,0, 0,0,0, 0,0,0, 1,0,0, 0,1,0, 0,0,1};
    double lev[6] = {0.2, 0.3, 0.5, 0.6, 0.6, 0.8};
    OSBM(6,3,V,lev);
    EXPECT_LT(orthogonality_test<double>(n_rows, n_cols, V, n_cols), (std::numeric_limits<double>::epsilon()*n_cols*5));
    EXPECT_LT(levscore_test<double>(n_rows, n_cols, V, lev), (std::numeric_limits<double>::epsilon()*n_cols*5));

    RandBLAS::dense_op::apply_haar<double>(n_rows, n_cols, V, n_cols, 1);
    EXPECT_LT(orthogonality_test<double>(n_rows, n_cols, V, n_cols), (std::numeric_limits<double>::epsilon()*n_cols*5));
    EXPECT_LT(levscore_test<double>(n_rows, n_cols, V, lev), (std::numeric_limits<double>::epsilon()*n_cols*5));
    
    double C[25];
    RandBLAS::dense_op::gen_rmat_haar(5, 4, C, 1);
    EXPECT_LT(orthogonality_test<double>(5, 4, C, 5), (std::numeric_limits<double>::epsilon()*n_cols*5));
}


