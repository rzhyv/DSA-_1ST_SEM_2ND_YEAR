#include <iostream>
using namespace std;

struct Node{
  int data;
  Node *left;
  Node *right;
  Node(int x):data(x), left(NULL), right(NULL){}
};

struct QueueNode{
  Node *data;
  QueueNode *next;
  QueueNode(Node *n): data(n), next(NULL){}
};

struct Queue{
  private:
    QueueNode *front, *rear;
  public:
    Queue(){
      front=rear=NULL;
    }

    void enqueue(Node *data);
    Node *dequeue();
    Node *getFront();
    bool isEmpty();
};

void Queue::enqueue(Node *data) {
  QueueNode *newNode = new QueueNode(data);
  if (rear == NULL) {
    front = rear = newNode;
    return;
  }
  rear->next = newNode;
  rear = newNode;
}

Node *Queue::dequeue() {
  if (front == NULL) {
    return NULL;
  }
  QueueNode *p = front;
  Node *node = p->data;
  front = front->next;
  if (front == NULL) {
    rear = NULL;
  }
  delete(p);
  return node;
}

Node *Queue::getFront(){
  if (front == NULL){
    return NULL;
  }
  return front->data;
}

bool Queue::isEmpty(){
  return front==NULL;
}

class Btree{
private:
  Queue insertQueue;
  public:
  Node *root;
  Btree(){
    root=NULL;
  }
  void insert(int x);
  void inorder(Node *root);
  void preorder(Node *root);
  void postorder(Node *root);
  void levelOrder();
};

void Btree::insert(int x) {
  Node *newNode = new Node(x);
  
  if (root == NULL) {
    root = newNode;
    insertQueue.enqueue(newNode);
    return;
  }

  Node *parent = insertQueue.getFront();
  
  if (parent->left == NULL) {
    parent->left = newNode;
  } else {
    parent->right = newNode;
    insertQueue.dequeue(); // Remove parent from queue once both child slots are filled
  }
  
  insertQueue.enqueue(newNode);
}

void Btree::inorder(Node *root) {
  if (root == NULL) return;
  inorder(root->left);
  cout << root->data << " ";
  inorder(root->right);
}

void Btree::preorder(Node *root) {
  if (root == NULL) return;
  cout << root->data << " ";
  preorder(root->left);
  preorder(root->right);
}

void Btree::postorder(Node *root) {
  if (root == NULL) return;
  postorder(root->left);
  postorder(root->right);
  cout << root->data << " ";
}

void Btree::levelOrder() {
  if (root == NULL) return;

  Queue traversalQueue; // Separate queue so levelOrder doesn't disturb insertion state
  Node *p;
  
  traversalQueue.enqueue(root);

  while ((p = traversalQueue.dequeue()) != NULL) {
    cout << p->data << " ";
    if (p->left) {
      traversalQueue.enqueue(p->left);
    }
    if (p->right) {
      traversalQueue.enqueue(p->right);
    }
  }
  cout << endl;
}

int menu() {
  int ch;
  cout << "\n======================" << endl;
  cout << "MENU" << endl;
  cout << "======================" << endl;
  cout << "[1] Inorder" << endl;
  cout << "[2] Preorder" << endl;
  cout << "[3] Postorder" << endl;
  cout << "[4] Level Order" << endl;
  cout << "[5] Building Tree" << endl;
  cout << "[6] Exit" << endl;
  cout << "Select(1-6): ";
  cin >> ch;
  return ch;
}

int main() {
  Btree bt;
  int num;

  while (1) {
    switch (menu()) {
      case 1:
        cout << "Inorder: ";
        bt.inorder(bt.root);
        cout << endl;
        break;
      case 2:
        cout << "Preorder: ";
        bt.preorder(bt.root);
        cout << endl;
        break;
      case 3:
        cout << "Postorder: ";
        bt.postorder(bt.root);
        cout << endl;
        break;
      case 4:
        cout << "Level Order: ";
        bt.levelOrder();
        break;
      case 5:
        cout << "Enter a number: ";
        cin >> num;
        bt.insert(num);
        break;
      case 6:
        exit(0);
      default:
        cout << "Invalid Input." << endl;
    }
  }

  return 0;
}