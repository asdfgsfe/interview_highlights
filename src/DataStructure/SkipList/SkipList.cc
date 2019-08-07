// C++ code for searching and deleting element in skip list

#include <bits/stdc++.h>
using namespace std;

struct Node
{
    int key;
    Node **forward;
    Node(int, int);
};

Node::Node(int key, int level)
{
    this->key = key;
    forward = new Node*[level + 1];
    memset(forward, 0, sizeof(Node*)*(level + 1));
}

class SkipList
{
private:
    int maxLevel_;
    float P;
    int level;
    Node *header;

public:
    SkipList(int, float);
    int randomLevel();
    Node* createNode(int, int);
    void insert(int);
    void remove(int);
    void search(int);
    void display();
};

SkipList::SkipList(int maxLevel, float P)
{
    this->maxLevel_ = maxLevel;
    this->P = P;
    level = 0;
    header = new Node(-1, maxLevel);
}

int SkipList::randomLevel()
{
    float r = (float)rand() / RAND_MAX; // r = [0, 1]
    int level = 0;
    while(r < P && level < maxLevel_) {
        level++;
        r = (float)rand() / RAND_MAX;
    }
    return level;
}

Node* SkipList::createNode(int key, int level)
{
    return new Node(key, level);
}

// No duplicate key in the skip list
// TODO: key-value
// Firstly, find the right position
void SkipList::insert(int key)
{
    Node *curNode = header;

    // record the node
    Node *update[maxLevel_+1];
    memset(update, 0, sizeof(Node*)*(maxLevel_+1));

    for(int i = level; i >= 0; i--) {
        while(curNode->forward[i] != NULL && curNode->forward[i]->key < key) {
            curNode = curNode->forward[i]; // move forward
        }
        update[i] = curNode;
    }

    curNode = curNode->forward[0];

    // Level 0, list: update[0] -> key ->curNode
    // TODO: update the duplicate key's value
    if (!curNode || curNode->key != key) {
        // Generate a random level for node
        int nodeLevel = randomLevel();

        if(nodeLevel > level) {
            for(int i = level + 1; i < nodeLevel + 1; i++)
                update[i] = header;

            level = nodeLevel;
        }

        // create new node with random level generated
        Node* newNode = createNode(key, nodeLevel);

        // insert node by rearranging pointers
        for(int i = 0; i <= nodeLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
        //std::cout<<"Successfully Inserted key "<<key<<"\n";
    }
}

void SkipList::remove(int key)
{
    Node *curNode = header;

    Node *update[maxLevel_+1];
    memset(update, 0, sizeof(Node*)*(maxLevel_+1));

    for(int i = level; i >= 0; i--) {
        while(curNode->forward[i] != NULL && curNode->forward[i]->key < key)
            curNode = curNode->forward[i];
        update[i] = curNode;
    }
    curNode = curNode->forward[0];

    // key was found
    if (curNode != NULL && curNode->key == key) {
        for(int i = 0; i <= level; i++) {
            if(update[i]->forward[i] != curNode)
                break;
            update[i]->forward[i] = curNode->forward[i];
        }

        // Remove levels having no elements
        while(level > 0 && header->forward[level] == nullptr)
            level--;
        std::cout<<"Successfully deleted key "<<key<<"\n";
    }
}

void SkipList::search(int key)
{
    Node *curNode = header;

    // loop invariant: x->key < searchKey
    for(int i = level; i >= 0; i--) {
        while(curNode->forward[i] && curNode->forward[i]->key < key)
            curNode = curNode->forward[i];
    }
    curNode = curNode->forward[0];

    if(curNode != nullptr && curNode->key == key) {
        std::cout<<"Found key: "<<key<<"\n";
    }
}

void SkipList::display()
{
    std::cout << "\n*****Skip List*****\n";
    for(int i=0;i<=level;i++) {
        Node *node = header->forward[i];
        std::cout<<"Level "<<i<<": ";
        while(node != NULL) {
            std::cout<<node->key<<" ";
            node = node->forward[i];
        }
        std::cout<<"\n";
    }
}

void test()
{
    // Seed random number generator
    srand((unsigned)time(0));

    SkipList lst(3, 0.5);

    lst.insert(3);
    lst.insert(6);
    lst.insert(7);
    lst.insert(9);
    lst.insert(12);
    lst.insert(19);
    lst.insert(17);
    lst.insert(26);
    lst.insert(21);
    lst.insert(25);
    lst.display();

    //Search for node 19
    lst.search(19);

    //Delete node 19
    lst.remove(19);
    lst.display();
}
