#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "structures.h"
using namespace std;

class B
{
public:
	Node *root = nullptr;
	int l = 10000;
	int N = 10;
	void Insert(Event *newEvent);
	Node *findLeaf(float newLat, Node *root);
	void addtoLeaf(Event *newEvent, Node *currNode);
	void splitNode(Node *currNode);
	void Search(vector<float> coordinates, int radius);
	void print(Event *event, float distance);
	void Traverse(Node *root, float min, float max, vector<Event *> &nodes);
};

void B::Insert(Event *newEvent)
{
	if (root == nullptr)
	{
		// if the root is empty, add the new data and mark the root as a leaf
		root = new Node(newEvent);
	}
	else
	{
		Node *currNode = findLeaf(newEvent->latitude, root); // find the right leaf to add the new data to
		addtoLeaf(newEvent, currNode);
	}
}

Node *B::findLeaf(float newLat, Node *root)
{
	// if it is a leaf, we are done
	if (root->isLeaf)
		return root;

	// start iterators for keys and children to find the correct child to move to
	Node *newRoot = root;
	auto keyIter = root->keys.begin();
	auto kidIter = root->kids.begin();

	if (newLat < keyIter->first)
	{
		// if newLat is less than the first key, go to the first child
		newRoot = kidIter->second;
	}

	else
	{
		while (keyIter != root->keys.end())
		{
			// otherwise, move up to the next child, check if the newLat is higher than each key
			++kidIter;
			if (newLat >= keyIter->first)
			{
				// newRoot while keep getting reassigned to the next higher child until a) we run out of keys or b) the key is higher
				newRoot = kidIter->second;
			}
			++keyIter;
		}
	}

	// call the function again to see if the new root is a leaf, or continue down the tree
	return findLeaf(newLat, newRoot);
}

void B::addtoLeaf(Event *newEvent, Node *currNode)
{
	// add the new data to the map of keys
	currNode->keys[newEvent->latitude] = newEvent;

	if (currNode->keys.size() > l)
	{
		// if the leaf is too large now, split it
		splitNode(currNode);
	}
}

void B::splitNode(Node *currNode)
{
	// create new nodes (non-leaves) to replace currNode
	Node *leftNode = new Node;
	leftNode->isLeaf = currNode->isLeaf;
	Node *rightNode = new Node;
	rightNode->isLeaf = currNode->isLeaf;

	if (currNode == root)
	{
		// if currNode is the root, create a new root to be its parent
		Node *parent = new Node;
		parent->isLeaf = false;
		currNode->parent = parent;
		root = parent;
	}

	int counter = 0;
	for (auto iter = currNode->keys.begin(); iter != currNode->keys.end(); ++iter)
	{
		if (counter < currNode->keys.size() / 2)
		{
			// add the first half to the left node
			leftNode->keys[iter->first] = iter->second;
		}
		else if (counter == currNode->keys.size() / 2)
		{
			// send the center key up to the parent
			currNode->parent->keys[iter->first] = iter->second;
		}
		else
		{
			// add the second half to the right node
			rightNode->keys[iter->first] = iter->second;
		}
		counter++;
	}

	// add the new nodes to the parent's map of kids (the left node will override the pointer to currNode)
	currNode->parent->kids[leftNode->keys.begin()->first] = leftNode;
	currNode->parent->kids[rightNode->keys.begin()->first] = rightNode;

	counter = 0;
	for (auto iter = currNode->kids.begin(); iter != currNode->kids.end(); ++iter)
	{
		if (counter < currNode->kids.size() / 2)
		{
			// add the first half of currNode's kids to leftNode
			leftNode->kids[iter->first] = iter->second;
			iter->second->parent = leftNode;
		}
		else
		{
			// add the second half to rightNode
			rightNode->kids[iter->first] = iter->second;
			iter->second->parent = rightNode;
		}
		counter++;
	}

	leftNode->parent = currNode->parent;
	rightNode->parent = currNode->parent;

	if (currNode->parent->kids.size() > N)
	{
		// if the parent node is too large now, split it
		splitNode(currNode->parent);
	}

	delete currNode;
}

void B::Traverse(Node *root, float min, float max, vector<Event *> &nodes)
{
	for (auto iter = root->kids.begin(); iter != root->kids.end(); ++iter)
	{
		Traverse(iter->second, min, max, nodes);
	}
	for (auto iter2 = root->keys.begin(); iter2 != root->keys.end(); ++iter2)
	{
		if (iter2->first >= min && iter2->first <= max)
		{
			nodes.push_back(iter2->second);
		}
	}
}

void B::Search(vector<float> coordinates, int radius)
{
	cout << "Alien sightings within " << radius << " miles:" << endl;

	// convert the radius to approximate value in latitude and longitude
	float latRadius = radius / 69.0;
	float longRadius = radius / 54.6;
	float latLoc = coordinates.at(0);
	float longLoc = coordinates.at(1);

	// create map to output in order
	vector<Event *> results;
	map<float, Event *> sortedResults;
	Traverse(root, latLoc - latRadius, latLoc + latRadius, results);

	for (auto i : results)
	{
		if (i->longitude > longLoc - longRadius && i->longitude < longLoc + longRadius)
		{
			// calculate distance to see if its in the radius
			float dLat = i->latitude - latLoc;
			float dLong = i->longitude - longLoc;

			float distance = pow(pow(dLat * 69, 2) + pow(dLong * 54.6, 2), 0.5);
			if (distance <= radius)
				sortedResults[distance] = i;
		}
	}
	for (auto i : sortedResults)
	{
		print(i.second, i.first);
	}
}

void B::print(Event *event, float distance)
{
	cout << "Distance: " << distance << " miles" << endl;
	cout << "   Location: " << event->city << ", " << event->state << ", " << event->country << endl;
	cout << "   Date/Duration: " << event->dateTime << ", " << event->duration << endl;
	cout << "   Shape: " << event->shape << endl;
}
