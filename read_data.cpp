#include <iostream>
#include <fstream>
#include "edge.hpp"

using namespace std;

struct compare_edge
{
    inline bool operator() (const timeEdge& struct1, const timeEdge& struct2)
    {
        return (struct1.t < struct2.t);
    }
};

void read_data(string path, string delimiter, int stepSize, vector<timeEdge>& edges, vector<int>& snapshots, int& n, int& m, int& timeNum)
{
    vector<int> nodes;
    vector<int> times;

    ifstream graphFile(path.c_str());
    string line;
    while(getline(graphFile, line))
    {
        size_t pos = 0;
        vector<int> tokens;
        while ((pos = line.find(delimiter)) != string::npos)
        {
            tokens.push_back(stoi(line.substr(0, pos)));
            line.erase(0, pos + delimiter.length());
        }
        tokens.push_back(stoi(line));

        edges.push_back(timeEdge(tokens));
        times.push_back(tokens[0]);
        nodes.push_back(tokens[1]);
        nodes.push_back(tokens[2]);
    }

    graphFile.close();

    sort(edges.begin(), edges.end(), compare_edge());
    sort(times.begin(), times.end());
    sort(nodes.begin(), nodes.end());

    int initial_time = times[0];
    int initial_node = nodes[0];
    for(int i = 0; i < edges.size(); i++)
    {
        edges[i].t -= initial_time;
        edges[i].src -= initial_node;
        edges[i].trg -= initial_node;
    }

    n = nodes[nodes.size()-1] - initial_node + 1;
    m = edges.size();
    timeNum = times[times.size()-1] - initial_time + 1;

    int step = 1;
    for(int i = 0; i < edges.size(); i++)
    {
        if(edges[i].t > step*stepSize)
        {
            snapshots.push_back(step);
            step = edges[i].t/stepSize + 1;

        }
    }
    if(step != *(snapshots.end()))
        snapshots.push_back(step);
}
