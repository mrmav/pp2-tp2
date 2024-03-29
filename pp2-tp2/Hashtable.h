/*
	<license>
	MIT License

	Copyright(c) 2018 Jorge Noro

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files(the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions :

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
	</license>
	<author> Jorge Noro </author>
	<version>1.0.0</version>
	<summary>Hashstbale functions declarations</summary>
*/

#ifndef HASHTABLE
#define HASHTABLE

#include "Types.h"

#define HASH_MAX_SIZE 367

// Represents a Node.
typedef struct Node {

	int key;
	
	Flight* flightData;

	struct Node* next;

} Node;

// Hashtable
typedef struct HashTable {

	Node* table[HASH_MAX_SIZE];

} HashTable;

HashTable* CreateHashTable();

Node* CreateNode(int key, Flight* data);

Node* AddNodeAfter(Node* node, Node* toAdd);

Node* ListSortAscending(Node* head);

void SwapNodes(Node* a, Node* b);

int AddNodeToHashTable(HashTable* ht, Node* n);

Node* RemoveNodeFromHashTable(HashTable* ht, int key);

Node* GetNodeByKeyInList(Node* head, int key);

Node* GetNodeByKeyInHashTable(HashTable* ht, int key);

Flight* CheckHashTableForFlightID(HashTable* ht, long int id);

int HashFunction(int key);

void PrintCompleteHashTable(HashTable* ht);

void PrintHashTable(HashTable* ht);

void DeleteNode(Node** node);

#endif // !HASHTABLE
