#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include "structures.h"
using namespace std;

class Bplus
{
public:
	Node *root = nullptr;
	int l = 10000;
	int N = 10;
	void Insert(Event *newEvent);
	Node *findLeaf(float newLat, Node *root);
	void addtoLeaf(Event *newEvent, Node *currNode);
	void splitLeaf(Node *currNode);
	void splitInternal(Node *currNode);
	void Search(vector<float> coordinates, int radius);
	void print(Event *event, float distance);
};

void Bplus::Insert(Event *newEvent)
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

Node *Bplus::findLeaf(float newLat, Node *root)
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

void Bplus::addtoLeaf(Event *newEvent, Node *currNode)
{
	// add the new data to the map of keys
	currNode->keys[newEvent->latitude] = newEvent;

	if (currNode->keys.size() > l)
	{
		// if the leaf is too large now, split it
		splitLeaf(currNode);
	}
}

void Bplus::splitLeaf(Node *currNode)
{
	Node *leftNode = new Node;
	Node *rightNode = new Node;

	// reassign the prev and next pointers for the linked list of leaves
	leftNode->prev = currNode->prev;
	leftNode->next = rightNode;
	rightNode->prev = leftNode;
	rightNode->next = currNode->next;

	int counter = 0;
	for (auto iter = currNode->keys.begin(); iter != currNode->keys.end(); ++iter)
	{
		// iterate through all the old node's keys
		if (counter < currNode->keys.size() / 2)
			// add the first half to the  new left node
			addtoLeaf(iter->second, leftNode);
		else
			// add the second half to the new right node
			addtoLeaf(iter->second, rightNode);

		// keep count of how many keys have been entered
		++counter;
	}

	if (currNode == root)
	{
		// if the old node was the root, create a parent for the new nodes, which will be the new root
		Node *newParent = new Node;							// create the new parent
		Event *parentKey = rightNode->keys.begin()->second; // get the key for the parent (the first key from the second child)

		newParent->keys[parentKey->latitude] = parentKey;		   // add the key to the parent
		newParent->kids[leftNode->keys.begin()->first] = leftNode; // add the left and right nodes to the parent's children
		newParent->kids[rightNode->keys.begin()->first] = rightNode;

		root = newParent;		   // make the new parent the root
		newParent->isLeaf = false; // mark the new parent as an internal node

		leftNode->parent = newParent; // assign the new parent to the left and right nodes as well
		rightNode->parent = newParent;
	}

	else
	{
		// if the old node did have a parent, add the middle key to it
		Node *parent = currNode->parent;
		Event *parentKey = rightNode->keys.begin()->second;

		parent->keys[parentKey->latitude] = parentKey;			// add the middle key to the parent
		parent->kids[leftNode->keys.begin()->first] = leftNode; // add the new nodes to the parent's kids
		parent->kids[rightNode->keys.begin()->first] = rightNode;

		leftNode->parent = parent; // give the new nodes their parent
		rightNode->parent = parent;

		if (parent->kids.size() > N)
		{
			// if the parent node is too large now, split it
			splitInternal(parent);
		}
	}

	delete currNode; // discard the old node when finished
}

void Bplus::splitInternal(Node *currNode)
{
	// create new nodes (non-leaves) to replace currNode
	Node *leftNode = new Node;
	leftNode->isLeaf = false;
	Node *rightNode = new Node;
	rightNode->isLeaf = false;

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
		splitInternal(currNode->parent);
	}

	delete currNode;
}

void Bplus::Search(vector<float> coordinates, int radius)
{
	cout << "Alien sightings within " << radius << " miles:" << endl;

	// convert the radius to approximate value in latitude and longitude
	float latRadius = radius / 69.0;
	float longRadius = radius / 54.6;
	float latLoc = coordinates.at(0);
	float longLoc = coordinates.at(1);

	// search in tree for lowest node in range latLoc +/- latRadius
	Node *currNode = findLeaf(latLoc - latRadius, root);
	Node *end = findLeaf(latLoc + latRadius, root);
	// create map to output in order
	map<float, Event *> results;

	while (currNode != end->next)
	{
		// until you reach the end of the range...
		for (auto iter = currNode->keys.begin(); iter != currNode->keys.end(); ++iter)
		{
			// check every key in the range of nodes
			Event *currEvent = iter->second;

			if (currEvent->latitude > latLoc - latRadius && currEvent->latitude < latLoc + latRadius)
			{
				if (currEvent->longitude > longLoc - longRadius && currEvent->longitude < longLoc + longRadius)
				{
					// calculate distance to see if its in the radius
					float dLat = currEvent->latitude - latLoc;
					float dLong = currEvent->longitude - longLoc;

					float distance = pow(pow(dLat * 69, 2) + pow(dLong * 54.6, 2), 0.5);
					if (distance <= radius)
						results[distance] = currEvent;
				}
			}
		}
		currNode = currNode->next;
	}
	for (auto i : results)
	{
		print(i.second, i.first);
	}
}

void Bplus::print(Event *event, float distance)
{
	cout << "Distance: " << distance << " miles" << endl;
	cout << "   Location: " << event->city << ", " << event->state << ", " << event->country << endl;
	cout << "   Date/Duration: " << event->dateTime << ", " << event->duration << endl;
	cout << "   Shape: " << event->shape << endl;
}
