#ifndef CANDIDATE_GENERATOR_H
#define CANDIDATE_GENERATOR_H

#include <vector>
#include <iostream>
#include "Tuple.h"
#include "Candidate.h"

using namespace std;

/*
 * Executes the porcess of candidates generation and collects contrast patterns;
 */
class CandidateGenerator {
	private:
		vector<Tuple*>& reducedTable;
		vector<vector<Candidate>> candidates;
		vector<Candidate> contrastPatterns;				//exists only in one class and not outside that class
		vector<vector<int>> supportsOfCandidates;		//vector to store supports of candidates of length 1
		const int numberOfClasses;

		void findOneLengthCandidates() {
			if(reducedTable.size() == 0) {
				cout << "Empty reduced table";
				exit(0);
			}

			//we will have as many candidates(reducedTable[0].getAttributes().size()) as we have attributes in the tuple
			//so eg 4 attributes in tuple mean 4 candidates

			const int numberOfCandidates = reducedTable[0]->getAttributes()->size(); 
			
			//initializes support matrix (candidate x class)
			for(int i = 0; i < numberOfCandidates; i++) {
				vector<int> internalSupports;
				for(int i = 0; i < numberOfClasses; i++) {
					internalSupports.push_back(0);
				}
				supportsOfCandidates.push_back(internalSupports);
			}

			//full scan of reduced table
			for(unsigned long i = 0; i < reducedTable.size(); i++) {
				for(int j = 0; j < numberOfCandidates; j++) {
					if((*reducedTable[i]->getAttributes())[j] > 0.0f) {
						int tClass = reducedTable[i]->getTupleClass();
						supportsOfCandidates[j][tClass-1]++;	//increment support of atribute in appropriate class
					}
				}
			}

			//create and insert candidate
			vector<Candidate> candidatesOfLengthOne;
			for(int i = 0; i < numberOfCandidates; i++) {
				int candidateId = i + 1;
				vector<int> attributes;
				attributes.push_back(candidateId);
				Candidate candidate = Candidate(&attributes, &supportsOfCandidates[i]);
				cout << "Candidate " << candidateId << " is contast pattern: " << candidate.isContrastPattern() << endl;  
				if(candidate.isContrastPattern()) {
					contrastPatterns.push_back(candidate);
				} else {
					candidatesOfLengthOne.push_back(candidate);
				}
			}
			candidates.push_back(candidatesOfLengthOne);
			cout << "Candidate One is joinable with candidate Two " << candidatesOfLengthOne[0].isJoinable(candidatesOfLengthOne[1]) << endl;
		}

		/*
		* Generates candidates of length k+1 based on candidates of length k.
		* Return true if there are at least two candidates generated (but not contast patterns).
		*/
		bool generateCandidatesLengthKPlusOne(vector<Candidate>& candidatesLenkthK) {
			vector<int> candidatesLengthKPlusOne;
			for(int i = 0; i < candidatesLenkthK.size(); i++) {
				for(int j = i + 1; j < candidatesLenkthK.size(); j++) {
					if(candidatesLenkthK[i].isJoinable(candidatesLenkthK[j])) {
						vector<int> attributes;
						joinCandidates(attributes, candidatesLenkthK[i], candidatesLenkthK[j]);
						cout << "Joining " << i+1 << " with " << j+1 << endl;
						//@TODO calculate supports of generated candidates
						//Candidate Candidate = Candidate(...);
						//check if it is contrast patterns
						/*
							if(candidate.isContrastPattern()) {
								contrastPatterns.push_back(candidate);
							} else {
								candidatesOfLengthOne.push_back(candidate);
							}
						*/
					}
				}
			}
			if(candidatesLengthKPlusOne.size() <= 1) {
				return false;
			}
			return true;
		}

		void joinCandidates(vector<int>& attributes, Candidate& first, Candidate& second) {
			for(int i = 0; i < first.getAttributes().size(); i++) {
				attributes.push_back(first.getAttributes()[i]);
			}
			attributes.push_back(second.getAttributes().back());
		}

	public:
		CandidateGenerator(vector<Tuple*>& tReducedTable, const int tNumberOfClasses): reducedTable(tReducedTable), numberOfClasses(tNumberOfClasses) {}

		void printSupportsOfCandiadtesLengthOne() {
			for(int i = 0; i < supportsOfCandidates.size(); i++) {
				cout << "Candidate " << i + 1 << " supports: "; 
				for (int j = 0; j < supportsOfCandidates[i].size(); j++) {
					cout << supportsOfCandidates[i][j] << " ";
				}
				cout << endl;
			}
		}

		void execute() {
			findOneLengthCandidates();
			if(candidates[0].size() >= 2) {
				int k = 1;		//initial length of candidates that will be incremented
				while(1) {
					if(!generateCandidatesLengthKPlusOne(candidates[k-1])) {
						break;	//the process of generating patterns has finished
					}
				}
			}
		}

		vector<vector<Candidate>>& getCandidates() { return candidates; }
		
};
 
#endif