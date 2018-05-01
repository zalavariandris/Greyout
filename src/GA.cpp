//
//  Population.cpp
//  ofGO_3_1_candidatestruct
//
//  Created by András Zalavári on 2017. 12. 16..
//
#define RANDOM_NUM      ((float)rand()/(RAND_MAX))
#include "GA.hpp"
using namespace std;
using namespace GA;

/*
 Chromosome
 */
Chromosome::Chromosome(size_t length){
    for(auto i=0; i<length; i++)
        mSequence.push_back(std::rand() % 255); // use std::generate instead??
}

void Chromosome::uniform_mutate(){
    /*!
     * UNIFORM MUTATION
     * Every genes has MUTATION_RATE chance to mutate,
     * when a gene mutates replace with a random number
     */
    float MUTATION_RATE = 0.0003;
    for (int cell_idx=0; cell_idx < mSequence.size(); cell_idx++) {
        if (RANDOM_NUM<MUTATION_RATE) {
            mSequence[cell_idx] = rand()%256;
        }
    }
}

void Chromosome::tweak_mutate(){
    /*!
     * Every genes has MUTATION_RATE chance to mutate,
     * if so, then shift with a random value
     */
    float MUTATION_RATE = 0.03;
    
    for (int cell_idx=0; cell_idx < mSequence.size(); cell_idx++) {
        if (RANDOM_NUM<MUTATION_RATE) {
            int value = mSequence[cell_idx];
            
            int shift = (RANDOM_NUM-0.5) * 256;
            value = (value+shift) % 256;
            mSequence[cell_idx] = value;
        }
    }
}

vector<shared_ptr<Chromosome>> Chromosome::uniform_crossover(vector<shared_ptr<Chromosome>> parents){
    float CROSSOVER_RATE = 0.3;
    auto children = std::vector<shared_ptr<Chromosome>>({make_shared<Chromosome>(parents[0]->mSequence.size()), make_shared<Chromosome>(parents[1]->mSequence.size())});
    
    // TODO: handle different Chromosome_LENGTHs
    // Perform crossover, or simply clone the parents;
    if (RANDOM_NUM < CROSSOVER_RATE){
        for(int cell_idx=0; cell_idx < parents[0]->mSequence.size(); cell_idx++){
            if(RANDOM_NUM < 0.5){
                children[0]->mSequence[cell_idx] = parents[0]->mSequence[cell_idx];
                children[1]->mSequence[cell_idx] = parents[1]->mSequence[cell_idx];
            } else {
                children[0]->mSequence[cell_idx] = parents[1]->mSequence[cell_idx];
                children[1]->mSequence[cell_idx] = parents[0]->mSequence[cell_idx];
            }
        }
    } else {
        for(int cell_idx=0; cell_idx < parents[0]->mSequence.size(); cell_idx++)
            children[0]->mSequence[cell_idx] = parents[0]->mSequence[cell_idx];
        for(int cell_idx=0; cell_idx < parents[1]->mSequence.size(); cell_idx++)
            children[1]->mSequence[cell_idx] = parents[1]->mSequence[cell_idx];
    }
    
    return children;
}

/*
 Population
 */
void Population::init(int size){
    generation = 0;
    for(int i=0;i<size; i++){
        shared_ptr<Candidate> candidate = make_shared<Candidate>();
        candidate->genes = make_shared<Chromosome>(128*128);
        candidates.push_back(candidate);
    }
}

vector<shared_ptr<Candidate>> Population::tournament_selection(){
    size_t parent_count = 2; // number of parents to select;
    size_t k = 2; // tournament size
    vector<shared_ptr<Candidate>> parents(2) ;
    for(int i=0; i<parents.size(); i++){
        shared_ptr<Candidate> best;
        for(int j=0; j<k; j++){
            shared_ptr<Candidate> opponent = candidates[rand()%size()];
            if(!best or opponent->cost < best->cost)
                best = opponent;
        }
        parents[i] = best;
    }
    return parents;
}

void Population::breed(){
    int SIZE = size();

    
    vector<shared_ptr<Candidate>> offsprings; // allocate next generation

    while (offsprings.size() < candidates.size()) {
        // Tournament Selection
        vector<shared_ptr<Candidate>> parents;
        parents = tournament_selection();

        vector<shared_ptr<Chromosome>> parents_genes;
        parents_genes.push_back(parents[0]->genes);
        parents_genes.push_back(parents[1]->genes);
        
        // Crossover parents
        vector<shared_ptr<Chromosome>>children_genes = Chromosome::crossover(parents_genes);

        // Mutate
        children_genes[0]->mutate();
        children_genes[1]->mutate();
        
        // create children with new genes
        auto children = vector<shared_ptr<Candidate>>();
        children.push_back(make_shared<Candidate>());
        children.push_back(make_shared<Candidate>());
        
        children[0]->genes = children_genes[0];
        children[1]->genes = children_genes[1];

        //
        offsprings.push_back(children[0]);
        offsprings.push_back(children[1]);
    }

    /*
     NEXT GENERATION
     http://cstheory.stackexchange.com/questions/14758/tournament-selection-in-genetic-algorithms
     You can either throw all the parents away and just do P = C (generational replacement), you can keep a few members of P and replace the rest with members of C (elitist replacement), you can merge them together and take the best N of the 2N total (truncation replacement)
     genereational replacement!
     */

    cost_history.push_back(best()->cost);
    candidates = offsprings;
    generation++;
}
