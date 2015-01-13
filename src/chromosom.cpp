#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <limits.h>
#include <functional>

#include "chromosom.hpp"

bool compareChrom(const Chromosom& lhs, const Chromosom& rhs) {
    return (lhs.cost < rhs.cost);
}

Chromosom::Chromosom(int openStoragesLen, int userLen) {

	openStorages = openStoragesLen;

	representation.reserve(userLen);

	for (int i = 0; i < userLen; i++) {
		representation.push_back(rand()%openStorages);
	}

	capacity.reserve(openStorages);
	for (int i = openStorages - 1; i >= 0; i--) {
		capacity.emplace_back(0);
	}

}

Chromosom::Chromosom(Chromosom parent1, Chromosom parent2, 
	int openStoragesLen, int userLen, int chooseType, int randNumber) {

	openStorages = openStoragesLen;

	representation.reserve(userLen);

	if (chooseType == 0) {
		int choose;
		
		for (int i = 0; i < userLen; i++) {
			choose = rand() % 2;

			if (choose == 0) {
				representation.push_back(parent1.representation[i]);
			} else {
				representation.push_back(parent2.representation[i]);
			}
		}
	} else if (chooseType == 1) {
		
		int position = randNumber % userLen;

		for (int i = 0; i < userLen; i++) {
			if (i < position)
				representation.push_back(parent1.representation[i]);
			else
				representation.push_back(parent2.representation[i]);
		}

	} 

	capacity.reserve(openStorages);
	for (int i = openStorages - 1; i >= 0; i--) {
		capacity.emplace_back(0);
	}

}

void Chromosom::calculateCost(std::vector<User> users, std::vector<Storage> storages) {
	cost = 0;
	falseFlag = false;

	for (int i = openStorages - 1; i >= 0; i--) {
		capacity[i] = 0;
	}
	
	for (int i = users.size() - 1; i >= 0; i--) {
		User user = users[i];
		Storage storage = storages[representation[i]];

		cost += (storage.x - user.x) * (storage.x - user.x)/10.0 + (storage.y - user.y) * (storage.y - user.y)/10.0;

		capacity[representation[i]] += user.demand;

	}

	
	for (int i = openStorages - 1; i >= 0; i--) {
		if (capacity[i] > storages[i].capacity)	{
			cost = INT_MAX;
			falseFlag = true;
			break;
		}
		
	}
	
}

void Chromosom::mutation() {
	int max = rand() % ((int)(1 + representation.size() / 10));
	
	int index;
	for (int i = 0; i < max; i++) {
		index = rand() % representation.size();
		representation[index] = rand() % openStorages;
	}
}

std::vector<Chromosom> createChilds(Chromosom parent1, 
			Chromosom parent2, int chooseType) {

	int userLen = parent1.representation.size();

	std::vector<Chromosom> childs = {Chromosom(parent1.openStorages, userLen), 
		Chromosom(parent1.openStorages, userLen)};

	if (chooseType == 0) {
		int choose;
		
		for (int i = 0; i < userLen; i++) {
			choose = rand() % 2;

			if (choose == 0) {
				childs[0].representation.push_back(parent1.representation[i]);
				childs[1].representation.push_back(parent2.representation[i]);
			} else {
				childs[1].representation.push_back(parent1.representation[i]);
				childs[0].representation.push_back(parent2.representation[i]);
			}
		}

	} else if (chooseType == 1) {
		
		int position = rand() % userLen;

		for (int i = 0; i < userLen; i++) {
			if (i < position) {
				childs[0].representation.push_back(parent1.representation[i]);
				childs[1].representation.push_back(parent2.representation[i]);
			} else {
				childs[1].representation.push_back(parent1.representation[i]);
				childs[0].representation.push_back(parent2.representation[i]);
			}
				
		}

	}

	return childs; 
}
