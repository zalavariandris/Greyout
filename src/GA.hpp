//
//  Population.hpp
//  ofGO_3_1_candidatestruct
//
//  Created by András Zalavári on 2017. 12. 16..
//

#pragma once


#include "ofMain.h"
#include <vector>
#include <memory>

namespace GA{
    class Chromosome{
    private:
        std::vector<int> mSequence;
        void uniform_mutate();
        void tweak_mutate();
        static std::vector<std::shared_ptr<Chromosome>> uniform_crossover(std::vector<std::shared_ptr<Chromosome>> parents);
    public:
        Chromosome(size_t length);
        int * sequence(){
            return mSequence.data();
        }
        size_t length(){
            return mSequence.size();
        }
        void mutate(){
            //        tweak_mutate();
            uniform_mutate();
        }
        static std::vector<std::shared_ptr<Chromosome>> crossover(std::vector<std::shared_ptr<Chromosome>> parents){
            return uniform_crossover(parents);
        }
    };
    
    struct Candidate{
        std::shared_ptr<Chromosome> genes;
        shared_ptr<ofImage> image;
        shared_ptr<ofImage> capture;
        float cost {std::numeric_limits<float>::infinity()};
    };

    class Population{
    private:
        vector<shared_ptr<Candidate>> tournament_selection();
        
    public:
        vector<float> cost_history;
        int generation {0};
        vector<shared_ptr<Candidate>> candidates;
        
        void init(int size);
        void breed();
        
        // vector wrapper
        size_t size(){
            return candidates.size();
        }
        
        shared_ptr<Candidate> candidate(int i){
            if(i >= candidates.size())
                throw std::out_of_range("candidate index out of range");
            
            return candidates[i];
        }
        
        vector<shared_ptr<Candidate>>::iterator begin(){
            return candidates.begin();
        }
        
        vector<shared_ptr<Candidate>>::iterator end(){
            return candidates.end();
        }
        
        shared_ptr<Candidate> best(){
            auto b =  min_element(candidates.begin(), candidates.end(), [](const shared_ptr<Candidate> & A, const shared_ptr<Candidate> & B){
                return A->cost < B->cost;
            });
            return *b;
        }
    };
}
