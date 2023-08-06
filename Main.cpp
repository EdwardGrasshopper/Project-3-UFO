#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include "Bplus.h"
#include "B.h"
using namespace std;

float paradise = 0.000001;
void fillNode(Event *node, int p, string content)
{
    try
    {
        switch (p)
        {
        case 0:
            node->country = content;
            break;
        case 1:
            node->city = content;
            break;
        case 2:
            node->state = content;
            break;
        case 3:
            node->dateTime = content;
            break;
        case 4:
            node->shape = content;
            break;
        case 5:
            node->duration = content;
            break;
        case 6:
            node->reportLink = content;
            break;
        case 7:

            node->latitude = stof(content) + paradise;
            paradise = paradise + 0.000001;
            if (paradise > 0.000009)
            {
                paradise / 2;
            }
            break;

        case 8:
            node->longitude = stof(content);
            break;
        }
    }
    catch (...)
    {
        cout << "here: " << content << endl;
    }
}

int main()
{

    // ask tree type (B+ or Splay)
    cout << "-------- The UAFO (Ultimate Alien Finding Operation) --------" << endl;
    cout << "Brought to you by \"Six Sea Intel\"" << endl;
    cout << "description: Find the nearest alien sighting near you!!!" << endl;
    cout << "What type of tree do you want to use:" << endl;
    cout << "\"B+\" or \"B\"" << endl;

    string treeType;
    cin >> treeType;

    fstream file("data.csv", ios::in);
    string line, word;
    int size = 0, dups = 0;

    B Btree;
    Bplus Bplustree;

    if (treeType == "B+")
    {
        if (file.is_open())
        {
            while (getline(file, line))
            {
                Event *node = new Event;
                int col = 0;
                stringstream str(line);

                while (getline(str, word, ',') && size != 0)
                {
                    fillNode(node, col, word);
                    col++;
                }

                if (size != 0)
                {
                    Bplustree.Insert(node);
                }
                size++;
            }
        }
    }

    if (treeType == "B")
    {
        if (file.is_open())
        {
            while (getline(file, line))
            {
                Event *node = new Event;
                int col = 0;
                stringstream str(line);

                while (getline(str, word, ',') && size != 0)
                {
                    if ("" == word)
                    {
                        cout << "help!!!" << endl;
                    }
                    fillNode(node, col, word);
                    col++;
                }

                if (size != 0)
                {
                    Btree.Insert(node);
                }
                size++;
            }
        }
    }

    // ask what are your coordinates
    float curLatitude;
    float curLongitude;
    float rad;

    cout << "What is your current Latitude and Longitude" << endl;
    cout << "Latitude: ";
    cin >> curLatitude;
    cout << "Longitude: ";
    cin >> curLongitude;
    cout << "What is the search radius your looking for?" << endl;
    cout << "WARNING: suggested max radius is 70 miles" << endl;
    cout << "Radius: ";
    cin >> rad;

    vector<float> coords;
    coords.push_back(curLatitude);
    coords.push_back(curLongitude);
    if (treeType == "B+")
    {
        Bplustree.Search(coords, rad);
    }
    if (treeType == "B")
    {
        Btree.Search(coords, rad);
    }
    cout << endl
         << endl;
    cout << "Thank you for using our program!!" << endl;
    cout << "If you want to use our program give us 20 bucks and reclick the compile button";
}