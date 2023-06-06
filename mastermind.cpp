#include <iostream>
#include <cmath>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <ctime>

using namespace std;

const int N=3; //numero di individui nella popolazione
const int G=2; //coefficiente fitness giusto al posto giusto
const int S=1; //coefficiente fitness giusto al posto sbagliato
const int PARENTSTYPE=3; //Tipo di scelta dei genitori: 0 casuale, 1 migliori, 2 peggiori, 3 a roulette
const int CROSSTYPE=0; //Tipo di crossover: 0 uniforme, 1 un punto, 2 due punti, 3 biased
const int CROSSBIAS=70; //bias biased crossover
const int PROBMUT=5; //probabilità di mutazione
const int SUBTYPE=1; //Tipo di sostituzione: 0 generazionale, 1 elitaria



bool guessed=false;


string code[4];

struct Chromosome{
    string guess[4];
    int fitness;
};

int fitness(string guess[4])
{
     int totalRight = 0;
     int rightColor = 0;
     int grab;
     bool exclude[4];
     bool excludeColor[4];
     bool inList = false;
     
     for (int i = 0; i < 4; i++)
     {
         exclude[i] = false;
         excludeColor[i] = false;
     }
     
     // loops to determine which have both the correct color and position
     for (int i = 0; i < 4; i++)
     {
         if (guess[i] == code[i])
         {
            totalRight++;
            exclude[i] = true;
         }
     }
     
     // loops to determine if any colors are correct, just in the wrong position
     for (int i = 0; i < 4; i++)
     {
         if (!exclude[i])
         {
            for (int j = 0; j < 4; j++)
            {
                if (!exclude[j] && i != j)
                {
                   if ((guess[i] == code[j]) && !excludeColor[j]) 
                   {
                      inList = true;
                      grab = j;
                   }
                }
            }
            
            if (inList)
            {
               rightColor++;
               inList = false;
               excludeColor[grab] = true;
            }
         }       
     }
    int value=totalRight*G+rightColor*S;
    return value;
} 

/*int fitness (string guess[4]){

    int i, s,et=0,ft=0,gt=0,ht=0,t, gs=0, gg=0;
    for (i=0;i<4;i++){
        if (code[i]==guess[i]){
            gg++;
            switch(i){
                case 0:
                et++;
                break;
                case 1:
                ft++;
                break;
                case 2:
                gt++;
                break;
                case 3:
                ht++;
                break;
            }
        }
    }
    for (t=0;t<4;t++){
        for (s=0;s<4 && s!=i;s++){
            switch(t){
                case 0: 
                    if(guess[s]==code[t] && et==0)
                        gs++;
                    break;
                case 1: 
                    if(guess[s]==code[t] && ft==0)
                        gs++;
                    break;
                case 2: 
                    if(guess[s]==code[t] && gt==0)
                        gs++;
                    break;
                case 3: 
                    if(guess[s]==code[t] && ht==0)
                        gs++;
                    break;
            }     
        }
    }

    cout<<gg<<" giuste al posto giusto"<<endl<<gs<<" giuste al posto sbagliato"<<endl;
    int value=gs+2*gg;
    return value;
}*/

string generateGene(){
    string S="";
    for(int i=0;i<2;i++)
        S += to_string(((int)rand()%2));
    return S;
}

void generateChromosome(string chromosome[4]){

    for(int i=0;i<4;i++)
        chromosome[i]=generateGene();
}

vector<int> roulette(vector<Chromosome> population, int num){

    vector<int> prob, c;
    int totalFitness=0, totalProb=0;

    for(int i=0;i<population.size();i++)
        totalFitness+=population[i].fitness;
    for(int i=0;i<population.size();i++){
        int tempProb=(population[i].fitness*100)/totalFitness;
        prob.push_back(tempProb);
        totalProb+=prob[i];
    }

    for(int i=0;i<num;i++){
        
        int r=rand() % totalProb;
        int temp=0;

        for(int j=0;j<prob.size();j++){
            temp+=prob[j];
            if(r<temp){
                c.push_back(j);
                break;
            }
        }
    }
    return c;
}

vector<Chromosome> selection(vector<Chromosome> population){

    vector<Chromosome> newPopulation;
    vector<int> c=roulette(population, N);
    for(int i=0;i<N;i++){
        newPopulation.push_back(population[c[i]]);
    }
    return newPopulation;
}

void crossover1(vector<Chromosome> &population, int c1, int c2){
    
    int point=rand()%3+1;

    for(int i=0;i<point;i++)
        swap(population[c1].guess[i], population[c2].guess[i]);

}

void crossover2(vector<Chromosome> &population, int c1, int c2){

    int point1=0, point2=0;

    while(point1==point2){
        point1=rand()%5;
        point2=rand()%5;
    }
    if(point2<point1)
        swap(point1, point2);

    for(int i=point1;i<point2;i++)
        swap(population[c1].guess[i], population[c2].guess[i]);

}

void crossoverU(vector<Chromosome> &population, int c1, int c2){

    int r;
    for(int i=0;i<4; i++){
        r=rand()%100;
        if(r<50)
            swap(population[c1].guess[i], population[c2].guess[i]);
    }
}

void biasedCrossover(vector<Chromosome> &population, int bias, int c1, int c2){

    int r;

    if(population[c1].fitness<population[c2].fitness)
        swap(c1, c2);
    
    vector<Chromosome> children;
    Chromosome child;

    for(int i=0;i<2;i++){
        for(int j=0;j<4;j++){
            r=rand()%100;
            if(r<bias)
                child.guess[j]=population[c1].guess[j];
            else    
                child.guess[j]=population[c2].guess[j];
        }
        children.push_back(child);
    }

    population[c1]=children[0];
    population[c2]=children[1];

}

vector<int> findBest(vector<Chromosome> pop){
    int b1=0, b2=1;
    if(pop[b1].fitness<pop[b2].fitness)
        swap(b1, b2);
    for(int i=2;i<pop.size();i++){
        if(pop[i].fitness>pop[b1].fitness){
            swap(b1, b2);
            b1=i;
        }
        else if(pop[i].fitness>pop[b2].fitness)
            b2=i;
    }
    vector<int> foundBest;
    foundBest.push_back(b1); foundBest.push_back(b2);
    return foundBest;
}

vector<int> findWorst(vector<Chromosome> pop){
    int w1=0, w2=1;
    if(pop[w1].fitness>pop[w2].fitness)
        swap(w1, w2);
    for(int i=2;i<pop.size();i++){
        if(pop[i].fitness<pop[w1].fitness){
            swap(w1, w2);
            w1=i;
        }
        else if(pop[i].fitness<pop[w2].fitness)
            w2=i;
    }
    vector<int> foundWorst;
    foundWorst.push_back(w1); foundWorst.push_back(w2);
    return foundWorst;
}

vector<int> findRandom(vector<Chromosome> pop){
    int c1=0, c2=0;
    while(c1==c2){
        c1=rand()%N;
        c2=rand()%N;
    }
    vector<int> foundRandoms;
    foundRandoms.push_back(c1); foundRandoms.push_back(c2);
    return foundRandoms;
}

void printPopulation(vector<Chromosome> pop){
    for(int i=0; i<pop.size();i++){
        for(int j=0; j<4; j++)
            cout<<pop[i].guess[j]<<" ";
        cout<<endl;
    }   
}

void invert(string &s, int p){
    if(s[p]=='1')
        s.replace(p, 1, "0");
    else if(s[p]=='0')
        s.replace(p, 1, "1");
}

void mutate(int prob, vector<Chromosome> &population){

    for(int i=0; i<population.size();i++){
        for(int j=0;j<4;j++){
            for(int k=0;k<2;k++){
                int e=rand()%100;
                if(e<prob){
                    invert(population[i].guess[j], k);
                }
            }
        }
    }
}

void calculateFitness(vector<Chromosome> &population){
    for(int i=0; i<population.size();i++){
        population[i].fitness=fitness(population[i].guess);
        if(population[i].fitness==(4*G))
            guessed=true;
    }
}

vector<Chromosome> elitistSub(vector<Chromosome> a, vector <Chromosome> b){
    vector<Chromosome> newPop;
    a.insert(a.end(), b.begin(), b.end());
    sort(a.begin(), a.end(), [](Chromosome x, Chromosome y){return x.fitness < y.fitness;});
    for(int i=0;i<N;i++){
        newPop.push_back(a.back());
        a.pop_back();
    }    
    return newPop;
}

int main(){

    int gen=0;
    srand (time(NULL));
    generateChromosome(code);
    vector<Chromosome> population;
    for(int i=0;i<N;i++){
        Chromosome c;
        generateChromosome(c.guess);
        c.fitness=fitness(c.guess);
        if(c.fitness==(4*G))
            guessed=true;

        population.push_back(c);
    }   

    while(!guessed){

        gen++;
        cout<<gen<<" generazione"<<endl;

        vector<Chromosome> newPopulation=selection(population);
        
        vector<int> parents;
        switch(PARENTSTYPE){
            case 0:
                parents=findRandom(newPopulation);
                break;
            case 1:
                parents=findBest(newPopulation);
                break;
            case 2:
                parents=findWorst(newPopulation);
                break;
            case 3:
                parents=roulette(newPopulation, 2);
                break;
        }

        switch(CROSSTYPE){
            case 0:
                crossoverU(newPopulation, parents[0], parents[1]);
                break;
            case 1:
                crossover1(newPopulation, parents[0], parents[1]);
                break;
            case 2:
                crossover2(newPopulation, parents[0], parents[1]);
                break;
            case 3:
                biasedCrossover(newPopulation, CROSSBIAS, parents[0], parents[1]);
                break;
        }   

        mutate(PROBMUT, newPopulation);

        calculateFitness(newPopulation);

        switch(SUBTYPE){
            case 0:
                population=newPopulation;
                break;
            case 1:
                population=elitistSub(population, newPopulation);
                break;
        }
    }
    cout<<"Combinazione trovata alla "<<gen<<" generazione";
}
