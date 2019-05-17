#include "edge.hpp"
#include <cmath>
#include <vector>
#define EPSILON  0.001
#define c 0.5

double* pagerank(outEdge* A, double* b, int n, int m, int version)
{
    if(version == 1)
    {
        for(int i = 0; i < n; i++)
            b[i] = c/n;
    }
    else
    {
        for(int i = 0; i < n; i++)
            b[i] = c*A[i].total_w/m;
    }

    double* nq = new double[n];
    double* nq_prev = new double[n];
    for(int i = 0; i < n; i++)
    {
        nq[i] = 0;
        nq_prev[i] = b[i];
    }

    double* new_nq;
    double* old_nq;
    int out_iter = 0;
    double delta_score = 100;

    while(delta_score > EPSILON)
    {
        if(out_iter % 2 == 0)
        {
            new_nq = nq;
            old_nq = nq_prev;
        }
        else
        {
            new_nq = nq_prev;
            old_nq = nq;
        }

        for(int i = 0; i < n; i++)
            new_nq[i] = 0;

        for(int i = 0; i < n; i++)
        {
	        if(old_nq[i] == 0)
                continue;
            if(A[i].total_w == 0)
                continue;

            if(version == 1)
            {
                double delta = (1-c)*old_nq[i]/A[i].out.size();

                std::vector<int>::iterator j = A[i].out.begin();
                for(; j != A[i].out.end(); j++)
                {
                    new_nq[*j] += delta;
                }
            }
            else
            {
                double delta = (1-c)*old_nq[i]/A[i].total_w;

                std::vector<int>::iterator j = A[i].out.begin();
                std::vector<int>::iterator k = A[i].weight.begin();
                for(; j != A[i].out.end(); j++, k++)
                {
                    new_nq[*j] += delta*(*k);
                }
            }
        }

        delta_score = 0;
        for(int i = 0; i < n; i++)
        {
            delta_score += std::abs(new_nq[i]);
            b[i] += new_nq[i];
        }
        out_iter++;
    }

    double sum = 0;
    for(int i = 0; i < n; i++)
        sum += b[i];
    for(int i = 0; i < n; i++)
        b[i] /= sum;

    delete [] nq;
    delete [] nq_prev;

    return b;
}
