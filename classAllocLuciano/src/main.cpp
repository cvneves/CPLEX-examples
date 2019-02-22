#include <iostream>
#include <stdio.h>
#include <iostream>
#include <ilcplex/ilocplex.h>

#include <vector>
#include <random>

#include <time.h> 
#include <cmath>

#include "Data.hpp"



void solve(Data data);

int main()
{
    Data data;
    
    /* data.H = {{0,1}, {2,3}, {4,5,6}};
    data.TChH = {{}, {}, {0}, {1}, {},{},{}};

    data.nSalas = 2;
    data.nAulas = 7;
    data.nTurmas = data.H.size();

    data.Q = {60, 30};
    data.D = {30,60,30}; */

    srand(time(NULL));

    std::vector<std::vector<int>> turmaAula;
    int nAulas = 1000;
    int nTurmas = 100;

    for(int i = 0; i < nAulas; i++)
    {   
        std::vector<int> v;
        v.push_back(i);
        v.push_back(rand() % nTurmas);
        turmaAula.push_back(v);

        std::cout << v[0] << " " << v[1] << "\n";
    }

    




    // solve(data);


}

void solve(Data data)
{
    std::vector<std::vector<int>> H = data.H;
    std::vector<std::vector<int>> TChH = data.TChH;

    int nSalas = data.nSalas;
    int nAulas = data.nAulas;
    int nTurmas = H.size();

    std::vector<int> Q = data.Q;
    std::vector<int> D = data.D;

    IloEnv env;
    IloModel model(env);

    //Variaveis

    IloArray<IloBoolVarArray> x(env, nAulas);
    for (int i = 0; i < nAulas; i++)
    {
        IloBoolVarArray v(env, nSalas);
        x[i] = v;
        for (int j = 0; j < nSalas; j++)
        {
            model.add(x[i][j]);
        }
    }

    IloBoolVarArray y(env, nTurmas);
    for (int t = 0; t < nTurmas; t++)
    {
        model.add(y[t]);
    }

    //FO

    {
        IloExpr sum(env);
        for (int t = 0; t < nTurmas; t++)
        {
            sum += y[t];
        }
        model.add(IloMinimize(env, sum));
    }

    //Restriçoes

    //7.2
    for (int t = 0; t < nTurmas; t++)
    {
        for (int i : H[t])
        {
            IloExpr sum(env);
            for (int j = 0; j < nSalas; j++)
            {
                sum += x[i][j];
            }
            model.add(sum == 1);
        }
    }

    //7.3
    for (int j = 0; j < nSalas; j++)
    {
        for (int t = 0; t < nTurmas; t++)
        {
            for (int i : H[t])
            {
                for (int k : TChH[i])
                {
                    model.add(x[i][j] + x[k][j] <= 1);
                }
            }
        }
    }

    //7.4
    for (int t = 0; t < nTurmas; t++)
    {
        for (int i : H[t])
        {
            for (int j = 0; j < nSalas; j++)
            {
                model.add(x[i][j] * D[t] <= Q[j]);
            }
        }
    }

    //7.5
    for (int j = 0; j < nSalas; j++)
    {
        for (int t = 0; t < nTurmas; t++)
        {
            for (int i : H[t])
            {
                for (int k : H[t])
                {
                    model.add(y[t] >= x[i][j] - x[k][j]);
                    model.add(y[t] >= x[k][j] - x[i][j]);
                }
            }
        }
    }

    //Soluçao

    IloCplex cla(model);

    cla.solve();
    std::cout << cla.getObjValue() << "\n";

    for (int i = 0; i < nAulas; i++)
    {
        for (int j = 0; j < nSalas; j++)
        {
            std::cout << (cla.getValue(x[i][j]) > 0.9 ? 1 : 0) << " ";
        }
        std::cout << "\n";
    }
}