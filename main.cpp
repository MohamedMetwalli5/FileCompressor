#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <unordered_map>
#include <queue>
#include <string>
using namespace std;

std::string the_encoded_text;
std::string the_decoded_text;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
};

// Function to allocate a new tree node
Node *makeNode(char ch, int freq, Node *left, Node *right){
    Node *node = new Node();

    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;

    return node;
}

// Comparison object to be used to order the heap
struct comp{
    bool operator()(const Node *l, const Node *r) const {
        // the highest priority item has the lowest frequency
        return l->freq > r->freq;
    }
};

// Traverse the Huffman Tree and store Huffman Codes in a map.
void encode(Node *root, string str, unordered_map<char, string> &huffmanCode) {
    if (root == nullptr) {
        return;
    }

    if (root->left == nullptr && root->right == nullptr) {
        huffmanCode[root->ch] = (str != "") ? str : "1";
    }
    encode(root->left, str + "0", huffmanCode);  // all the left branches have a coding of "0"
    encode(root->right, str + "1", huffmanCode); // all the right branches have a coding of "1"
}

// Traverse the Huffman Tree and decode the encoded string
void decode(Node *root, int &index, string str) {
    if (root == nullptr)
    {
        return;
    }

    if (root->left == nullptr && root->right == nullptr) { // it is a leaf node
        the_decoded_text += root->ch;
        return;
    }

    index++;

    if (str[index] == '0') {                                   // we must move left
        decode(root->left, index, str); // we call it again on the left node
    } else {                                    // we must move right
        decode(root->right, index, str); // we call it again on the right node
    }
}

// Builds Huffman Tree and decodes the given input text
void buildHuffmanTree(string text) {
    if (text == "") { // there is no string to be decoded
        return;
    }

    unordered_map<char, int> frequency_table;
    for (char ch : text) {
        frequency_table[ch]++;
    }

    // Create a priority queue to store the nodes in ascending order of their frequency
    priority_queue<Node *, vector<Node *>, comp> pq;

    // creating a leaf node for each character and add it to the priority queue.
    for (auto pair : frequency_table) {
        pq.push(makeNode(pair.first, pair.second, nullptr, nullptr)); //pair.first is the character to be encoded, and pair.second is the frequency of this character in the file
    }

    while (pq.size() != 1) {                          // while we can choose the two nodes with the lowest frequencies from the queue
        Node *left = pq.top(); //creating the smallest frequency node and calling it left
        pq.pop();
        Node *right = pq.top(); //creating the second smallest frequency node and calling it right
        pq.pop();

        pq.push(makeNode('\0', left->freq + right->freq, left, right)); // '\0' is the NULL character
    }

    // 'root' stores a pointer to the root of Huffman Tree
    Node *root = pq.top();

    unordered_map<char, string> huffmanCode; // storing the Huffman Codes as pairs <char,code>
    encode(root, "", huffmanCode);

    cout << "Huffman Codes are:\n"
         << endl;
    for (auto pair : huffmanCode) {
        cout << pair.first << " | " << pair.second << endl;
    }

    // Print encoded string
    string str;
    for (char ch : text) {
        str += huffmanCode[ch];
    }
    the_encoded_text += str;

    if (root->left == nullptr && root->right == nullptr) { // if it is only one node and it is a leaf node in the tree
        while (root->freq > 0) {
            the_decoded_text += root->ch;
        }
    } else { // there is more than one node in the tree
        int index = -1;
        while (index < (int)str.size() - 1) { // we didn't decode the whole encoded string
            decode(root, index, str);
        }
    }
}

int main(int argc, char *argv[]) {
    if (!strcmp(argv[1], "") || !strcmp(argv[2], "") || !strcmp(argv[3], "")) {
        cout << "Please, enter a valid file path!" << endl;
        return 0;
    }else {
        std::string line_;
        ifstream file_(argv[1]);
        if (file_.is_open()) {
            while (getline(file_, line_)) {
                buildHuffmanTree(line_);
            }

            file_.close();
        }
        std::ofstream EncodedFile;
        EncodedFile.open(argv[2], std::ios_base::app); // append instead of overwrite
        EncodedFile << the_encoded_text;
        EncodedFile.close();

        std::ofstream DecodedFile;
        DecodedFile.open(argv[3], std::ios_base::app); // append instead of overwrite
        DecodedFile << the_decoded_text;
        DecodedFile.close();

        return 0;
    }
}
