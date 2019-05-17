#include <cmath>

using namespace std;

double score_absum(double* s, int n)
{
    double sum = 0;
    for(int i = 0; i < n; i++)
    {
        sum += std::abs(s[i]);
    }
    return sum;
}

void normalize(double** pr, int n, int timeNum)
{
    double* mean = new double[n];
    double* var = new double[n];

    for(int i = 0; i < n; i++)
        mean[i] = var[i] = 0;

    for(int i = 0; i < timeNum; i++)
    {
        for(int j = 0; j < n; j++)
            mean[j] += pr[i][j];
    }

    for(int i = 0; i < n; i++)
    {
        mean[i] /= timeNum;
        for(int j = 0; j < timeNum; j++)
        {
            pr[j][i] -= mean[i];
            var[i] += std::pow(pr[j][i], 2);
        }
    }

    for(int i = 0; i < n; i++)
    {
        var[i] /= timeNum;
        for(int j = 0; j < timeNum; j++)
            pr[j][i] /= std::sqrt(var[i]);
    }

    delete [] mean;
    delete [] var;
}

void normalize_online(double* pr, double* mean, double* var, int n, double t)
{
    for(int i = 0; i < n; i++)
    {
        double new_m = t/(t+1)*mean[i] + 1/(t+1)*pr[i];
        double new_v = t/(t+1)*var[i] + 1/(t+1)*std::pow(pr[i], 2);

        mean[i] = new_m;
        var[i] = new_v;
        double sd = std::sqrt(var[i] - std::pow(mean[i], 2));

        if(sd == 0) return;
        pr[i] -= mean[i];
        pr[i] /= sd;
    }
}

double compute_anomaly_score(int t, double** pagerank1, double** pagerank2, double** mean, double** var, int n)
{
    int start_i = 0;
    int end_i = 4;

    // 1st & 2nd order derivatives
    double** d = new double*[4];
    for(int i = 0; i < 4; i++)
    {
        d[i] = new double[n];
        for(int j = 0; j < n; j++)
            d[i][j] = 0;
    }

    for(int i = 0; i < n; i ++)
    {
        if(t > 0)
        {
            d[0][i] = pagerank1[t%3][i] - pagerank1[(t-1)%3][i];
            d[2][i] = pagerank2[t%3][i] - pagerank2[(t-1)%3][i];
        }

        if(t > 1)
        {
            d[1][i] = pagerank1[t%3][i] - 2*pagerank1[(t-1)%3][i] + pagerank1[(t-2)%3][i];
            d[3][i] = pagerank2[t%3][i] - 2*pagerank2[(t-1)%3][i] + pagerank2[(t-2)%3][i];
        }
    }

    double* max = new double[4];
    double total_max = 1;
    for(int i = start_i; i < end_i; i ++)
    {
        max[i] = -1000;
        normalize_online(d[i], mean[i], var[i], n, t);

        for(int j = 0; j < n; j++)
        {
            if(max[i] < std::abs(d[i][j])) max[i] = std::abs(d[i][j]);
        }
        total_max *= max[i];
    }

    double score = -2000;

    for(int i = start_i; i < end_i; i ++)
    {
        double subscore = score_absum(d[i], n) * (total_max/max[i]);
        // max
        score = std::max(score, subscore);
    }

    for(int i = 0; i < 4; i++)
        delete [] d[i];
    delete [] d;

    return score;
}

