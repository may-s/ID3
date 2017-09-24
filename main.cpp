/*
 * main.cpp
 *
 *  Created on: 05-Nov-2016
 *      Author: Mayank
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <ctime>
#include "datautil.h"
#include "id3.h"

using namespace std;
void printTree(TreeNode *head, map<int, int> distinct_attr_values) {
	if(head->leaf == false) {
		cout << head->attr_id << endl;
		for(int t = 0; t < distinct_attr_values[head->attr_id]; ++t) {
			printTree(head->next[t], distinct_attr_values);
		}
	}
	else {
		cout << "value = " << head->value << endl;
	}
}
int get_distinct_attr_values(vector <struct Data> &v, int attr) {
	set<int> points;
	for (unsigned i = 0; i < v.size(); ++i) {
		points.insert(get_attr_value(attr, v[i]));
	}
	return points.size();
}

bool predicted_value(struct Data &d, TreeNode *head){
	if(head->leaf == true)
		return head->value;
	return predicted_value(d, head->next[get_attr_value(head->attr_id, d)]);
}

double accuracy(vector<struct Data> v, TreeNode *head, int target_attr) {
	int correct = 0;
	for(unsigned i = 0; i < v.size(); ++i) {
		if(get_attr_value(target_attr, v[i]) == predicted_value(v[i], head)){
			correct++;
		}
	}
	return (double)correct/v.size();
}

int main(int argc, char** argv) {
	auto training_data = get_dataset("data/adult.data");

	map<int, int> distinct_attr_values;

	for(int i = 0; i < 14; ++i) {
		distinct_attr_values[i] = get_distinct_attr_values(training_data, i);

	}
	distinct_attr_values[13] = 41;

	TreeNode *head = new TreeNode();

	auto testing_data = get_dataset("data/adult.test");;

	//~ unsigned cut = 2.0/3.0 * v.size();

	//~ for(unsigned i = 0; i < cut; ++i) {
		//~ training_data.push_back(v[i]);
	//~ }
	//~ for(unsigned i = cut; i < v.size(); ++i) {
			//~ testing_data.push_back(v[i]);
	//~ }

	std::clock_t start;
	start = std::clock();
	id3(training_data, 14, distinct_attr_values, head);
	std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

	cout << "id3 successful" << endl;

	cout << "accuracy on testing data = "<< accuracy(testing_data, head, 14) << endl;
	cout << "accuracy on training data = "<< accuracy(training_data, head, 14) << endl;

	cout << "end";
}
