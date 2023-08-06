#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <string>
using namespace std;

struct Event
{
    string country;
    string city;
    string state;
    string dateTime;
    string shape;
    string duration;
    string reportLink;
    float latitude;
    float longitude;
};

struct Node
{
    map<float, Event *> keys; // keep keys and kids in maps sorted by latitude so that they stay in the right order
    map<float, Node *> kids;
    Node *parent = nullptr;
    bool isLeaf = true;
    Node *prev = nullptr;
    Node *next = nullptr;
    Node(Event *newEvent) { keys[newEvent->latitude] = newEvent; }
    Node() {}
};