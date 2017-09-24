/*
 * id3.cpp
 *
 *  Created on: 09-Nov-2016
 *      Author: Mayank
 */
#include "id3.h"
#include "datautil.h"
#include <cmath>
#include <map>
#include <iostream>

using namespace std;

double calc_entropy(const vector<struct Data > &v, int target_attr){
	int ntrue = 0, nfalse = 0;
	for(unsigned i = 0; i < v.size(); ++i){
		if(get_attr_value(target_attr, v[i]))
			ntrue++;
		else
			nfalse++;
	}
	double ptrue = (double)ntrue / (ntrue + nfalse);
	double pfalse = (double)nfalse / (ntrue + nfalse);

	double entropy = 0;
	if(ptrue != 0)
		entropy = -ptrue * log2(ptrue);
	if(pfalse != 0)
		entropy = -pfalse * log2(pfalse);
	return entropy;

}

double gain(vector<struct Data> &v, int attr_id, int target_attr) {
	map<int, vector<struct Data > > m;

	for(unsigned i = 0; i < v.size(); ++i)
		m[get_attr_value(attr_id, v[i])].push_back(v[i]);

	double entr = calc_entropy(v, target_attr);
	for(auto i = m.begin(); i != m.end(); ++i){
		double entropy = calc_entropy(i->second, target_attr);
		entr -= (double)i->second.size() / v.size() * entropy;
	}
	return entr;
}

bool test_all(vector<struct Data> &v, bool test, int target_attr) {
	for(unsigned i = 0; i < v.size(); ++i) {
		if(get_attr_value(target_attr, v[i]) != test)
			return false;
	}
	return true;
}

bool most_common(vector<struct Data> &v, int target_attr) {
	int ntrue = 0, nfalse = 0;
	for(unsigned i = 0; i < v.size(); ++i) {
		if(get_attr_value(target_attr, v[i]))
			ntrue++;
		else
			nfalse++;
	}
	if(ntrue > nfalse)
		return true;
	else
		return false;
}



void id3(vector<struct Data> data, int target_attr, std::map<int, int> distinct_attr_values, struct TreeNode *node) {
//	struct TreeNode *node = new struct TreeNode();

	// if all are positive
	if(test_all(data, true, target_attr)){
		node->leaf = true;
		node->value = true;
//		cout << "all positive" << endl;
		return;
	}
	// if all are negative
	if(test_all(data, false, target_attr)){
		node->leaf = true;
		node->value = false;
//		cout << "all negative" << endl;
		return;
	}

	if(distinct_attr_values.empty()) {
		node->leaf = true;
		node->value = most_common(data, target_attr);
		return;
	}

	node->leaf = false;

	double max_gain = 0.0; int max_gain_attr = distinct_attr_values.begin()->first;
	for(auto i = distinct_attr_values.begin(); i != distinct_attr_values.end(); ++i) {
		double g = gain(data, i->first, target_attr);

//		cout << "g = " << g ;
//		cout << " i->first = " << i->first ;
//		cout << endl;

		if(g > max_gain){
			max_gain = g;
			max_gain_attr = i->first;
		}
	}

	node->attr_id = max_gain_attr;
//	cout << "max_gain_attr " << max_gain_attr << endl;
	int n = distinct_attr_values[max_gain_attr];
	node->next = new TreeNode *[n+1];
	for(int i = 0; i <= n; ++i) {
		TreeNode *new_node = new TreeNode();
		node->next[i] = new_node;
//		cout << "max gain attr,  i (v(i)) = " << max_gain_attr << " "<< i << endl;
//		cout << "distinct_attr_values[max_gain_attr] = " << n << endl;

		// new_data = subset that have the value vi (i) for A (max_gain_attr)
		vector<struct Data> new_data;
		for(unsigned j = 0; j < data.size(); ++j) {
			if(get_attr_value(max_gain_attr, data[j]) == i)
			new_data.push_back(data[j]);
		}

		if(new_data.empty()){
//			cout << "new_data_empty" << endl;
			new_node->leaf = true;
			new_node->value = most_common(data, target_attr);
		}
		else {
//			cout << "new_data not empty" <<endl;
			distinct_attr_values.erase(max_gain_attr);
			id3(new_data, target_attr, distinct_attr_values, new_node);
//			cout << "return from recursive call" << endl;
		}
	}
}

