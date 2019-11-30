/*  EXTRAS ++++++++++++++++++++
 *  1. Password Protection:
 *      The user gets to choose a password of length larger than kMinPasswordLength which is then hashed using two hashing functino as follows:
 *          (i) A hashing function that returns a sequence of variable length (uncorrelated to the length of password) that is appended to
 *              the begining of the header (after the file size). The program won't proceed (while decompressing) if the hashed attempted
 *              password does not match the sequence at the begining of the file. The program needs to know the sequence before decompressing
 *              the file because it is of variable length and shifts the entire input stream if it's wrong leading to complete failure decompressig
 *              the body of the file (and the encoding tree). This shift of the sequence is another safety feature that relies on the structure
 *              of this specific file format that relies on sequence to find charectar boundries (similar to a genetic mutation that adds or removes
 *              a nucleotide to the sequence cousing significant damage.
 *          (ii)A length preserving hashing function that consequenctly doesn't have collisions. This one is used to assure that the password attempted
 *              is exactly the password used for compression. This hash is stored after the one above so that it's shifted by the process explained.
 *  2. Encryption:
 *      This code secures the contents of the file by flipping specific bits in the sequence and flipping them back while decompressing. The program
 *      uses a hashing function that hashes a character into an integer between kMinCharHash and kMaxCharHash to know after how many bits the next
 *      bit flip occures. At the beginning, the first char of the password is hashed, which means that you need to know the original password to start
 *      this chain of bit flipping. After the first flip (and each next flip), the last char decoded is hashed into the count to the next bit to flip.
 *      This means that you need to decode all the prefix of the file before decoding any additional progress, and you need to know the hashing function.
 *  3. Encoding Tree Storage:
 *      This code stores the encoding tree using 0's to represent interior nodes that have to children, and 1's to represent leaf nodes that have a
 *      character value in them and have no children. The character is stored right after the 1. Children are stored recursively with 0's (lefts) coming
 *      before 1's (rights) of the encoding binary tree.
 */

#include "encoding.h"
#include "pqueue.h"
#include "filelib.h"
#include "vector.h"
#include "strlib.h"

const int kModHash = 16;
const int kMinPasswordLength = 4;
const int kMinCharHash = 5;
const int kMaxCharHash = 10;
/* Builds the frequency table of a given input stream by iterating through it byte by byte and incrementing the value associated with its key
 * in the map to be returned. A map with 1 associated with PSEUDO_EOF is returned for an empty stream.
 */
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    while(!input.fail()) freqTable[input.get()]++;
    freqTable.remove(EOF);
    freqTable[PSEUDO_EOF] ++;
    return freqTable;
}
/* This function starts with a frequency table (Map freqTable) and uses it according to create a binary tree according to the Huffman algorithm
 */
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    PriorityQueue<HuffmanNode*> queue;
    for(int key: freqTable) { // start with making each character a tree and enqueuing it
        HuffmanNode * newNode = new HuffmanNode(key, freqTable[key]);
        queue.add(newNode, newNode->count);
    }
    while(queue.size() > 1) { // iterates through the queue until it ends up with only one tree (one root in the queue)
        HuffmanNode * interiorNode = new HuffmanNode();
        interiorNode->zero = queue.dequeue(); // left first then right (0 then 1) according to the order of the priority queue (their count)
        interiorNode->one = queue.dequeue();
        interiorNode->count = interiorNode->zero->count + interiorNode->one->count; // the mom has the sum count of its children
        queue.add(interiorNode, interiorNode->count);
    }
    return queue.dequeue();
}
/* This function starts with an encoding tree root pointer and iterates through its leaves associating them with the appropriate
 * Huffman binary code depending on their position in the tree.
 */
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    associateBinaryChildrenWithPrefix(encodingMap, "", encodingTree);
    return encodingMap;
}
/* This recursive function associates descendents of a node with their binary encoding provided its binary encoding prefix
 */
void associateBinaryChildrenWithPrefix(Map<int, string>& encodingMap, string prefix, HuffmanNode* node) {
    if(node == NULL) return;
    if(node->isLeaf()) {
        encodingMap[node->character] = prefix;
        return;
    }
    associateBinaryChildrenWithPrefix(encodingMap, prefix+"0", node->zero);
    associateBinaryChildrenWithPrefix(encodingMap, prefix+"1", node->one);
}
/* This function encodes a given istream into a given oustream according to the rules included in encodingMap by reading byte by byte
 * of input and writing bit by bit into output
 */
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output, int nextFlip) {
    char lastInserted;
    while(!input.fail()) { // keeps reading input until and including EOF
        int inpChr = input.get();
        if(inpChr == EOF) inpChr = PSEUDO_EOF;
        for(char c: encodingMap[inpChr]) {
            int bit = (c == '0') ? 0 : 1;
            nextFlip --;
            if(nextFlip == 0) {
                bit = 1-bit;
                nextFlip = hashLastChar(lastInserted);
            }
            output.writeBit(bit);
        }
        lastInserted = inpChr;
    }
}
/* These two place holders are here to allow the program to compile because the client software expects them to exist.
 */
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {};
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {};
/* This function decodes a given istream into a given outstream according to the rules included in encodingTree by reading bit by bit
 * of input and navigating the tree while outputing the results to output
 */
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output, int nextFlip) {
    char lastChar;
    while(true) {
        HuffmanNode* currNode = encodingTree; // starts at the root each iteratino of a new character (before EOF)
        while(!currNode->isLeaf()){ // navigates down the tree in directions according to input until it hits a value (leaf)
            int bit = input.readBit();
            nextFlip --;
            if(bit<0)return;
            if(nextFlip == 0) {
                bit = 1-bit;
                nextFlip = hashLastChar(lastChar);
            }
            if(bit == 0) currNode = currNode->zero;
            else currNode = currNode->one;
        }
        output.put(lastChar = currNode->character);
        if(--(encodingTree->count) == 0) return;
    }
}
/* This function uses the functions defined above to encode the data provided through input to output, including a header that houses the
 * frequency table for fututre use in 'decompress' function
 */
void compress(istream& input, obitstream& output) {
    string password;
    do {
        cout << "Enter a password to encode this file: ";
        getline(cin, password);
    } while(password.size()<kMinPasswordLength);
    Map<int, int> frequencyTable = buildFrequencyTable(input);
    HuffmanNode* encodingTree = buildEncodingTree(frequencyTable);
    Map<int, string> encodingMap = buildEncodingMap(encodingTree);
    output << encodingTree->count; // the size of the file

    for(bool b: hashString(password))
        output.writeBit(b?1:0);
    for(char c: hashPasswordIntoSameLength(password))
        output.put(c);
    exportEncodingTreeRecursive(encodingTree, output);
    rewindStream(input); // because 'buildFrequencyTable' has already exhausted the input stream, but 'encodeData' needs to begin at the front
    encodeData(input, encodingMap, output, hashLastChar(password[0]));
}
/* This function uses the functions defined above to decode the data provided through input to output, making use of the header provided at
 * the beginning of the input steam to recover frequencyTable and encodingTree used in decodeData
 */
void decompress(ibitstream& input, ostream& output) {
    if(input.eof()) return; // when the input is empty, take no action
    int n; input >> n;
    string password;
    while(true) {
        cout << "Please enter the password used to compress this file: ";
        getline(cin, password);
        if(password.size() < kMinPasswordLength) continue;
        bool matches = !input.fail();
        for(bool b: hashString(password)) {
            if(input.fail() || input.readBit() != (b?1:0)) {
                matches = false; break;
            }
        }
        for(char c: hashPasswordIntoSameLength(password)) {
            char j = input.get();
            if(c != j || input.fail()) {
                matches = false; break;
            }
        }
        if(matches) break;
        rewindStream(input);
        cout << "Wrong password!" << endl;
    }
    HuffmanNode* encodingTree = new HuffmanNode; //= buildEncodingTree(frequencyTable);
    importEncodingTreeRecursive(encodingTree, input);
    encodingTree->count = n;
    decodeData(input, encodingTree, output, hashLastChar(password[0]));
}
/* Recursively deletes the children nodes of node before deleting it. This deletes the entire tree if provided with its root node
 */
void freeTree(HuffmanNode* node) {
    if(!node->isLeaf()) {
        freeTree(node->zero);
        freeTree(node->one);
    }
    delete node;
}
/* This function hashes a provided string (password) into a sequence of bits with size less than
 */
Vector<bool> hashString(string str) {
    Vector<bool> result;
    result.add(str[0]%2==0);
    str = toLowerCase(str);
    for(int i = 1; i < str.size(); i++) {
        if((str[i-1]+int(pow(str[i-1],i/2)+str[i]))%kModHash > ((int(pow(i, i/2))%2==0)?-1:1)*kModHash/3) result.add((str[i-1]+int(pow(str[i-1],i/2)+str[i]))%2==0);
    }
    return result;
}
/* Preserves the length of the hashed string to prevent collisions.
 * Used to hash password for the second time
 */
string hashPasswordIntoSameLength(string str) {
    for(int i = 2; i < str.size(); i++)
        str[i] = (str[i] + str[i-1] + str[i-2]) % 255; // mod is to keep chars in range
    return str;
}
/* Hashes a character into an integer in the range kMinCharHash and kMaxCharHash.
 * Used to find the next bit flip in the sequence.
 */
int hashLastChar(char c) {
    return ((c * (c+2)) % kMaxCharHash) + kMinCharHash;
}
/* Stores the encoding tree into the output stream by representing empty nodes with 0's and full ones with 1's and their values.
 * Used recursively by itself to store left trees (0's) before right trees (1's). Used by the client to store the root.
 */
void exportEncodingTreeRecursive(HuffmanNode* encodingTree, obitstream& output) {
    if(encodingTree->isLeaf()) {
        output.writeBit(1);
        output.put(encodingTree->character);
        return;
    }
    output.writeBit(0);
    exportEncodingTreeRecursive(encodingTree->zero, output);
    exportEncodingTreeRecursive(encodingTree->one, output);
}
/* Retrieves the exported tree from an input stream when it's formated in the form of the output of the function exportEncodingTreeRecursive.
 */
void importEncodingTreeRecursive(HuffmanNode* thisRoot, ibitstream& input) {
    if(input.readBit() == 1) {
        thisRoot->character = input.get();
        return;
    }
    thisRoot->zero = new HuffmanNode;
    thisRoot->one = new HuffmanNode;
    importEncodingTreeRecursive(thisRoot->zero, input);
    importEncodingTreeRecursive(thisRoot->one, input);
}
