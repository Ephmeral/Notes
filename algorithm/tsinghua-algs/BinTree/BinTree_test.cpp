#include "BinTree/BinTree.h"
#include "BinTree/BinNode.h"
#include "util/double_elem.h"
#include "util/rand.h"
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

void test_of_BinNode() {
    BinNode<int> root1(10);
    root1.insertAsLC(5);
    root1.insertAsRC(6);
    
    BinNode<int> root2(2);
    root2.insertAsLC(4);
    root2.insertAsRC(7);
    
    BinNode<int> root(1, nullptr, &root1, &root2);
    // std::vector<int> res;
    auto visit = [](int x) {
        std::cout << "x val = " << x << std::endl;
    };
    std::cout << "pre travel begin..." << std::endl;
    root.travPre(visit);
    std::cout << "pre travel begin..." << std::endl;

    std::cout << "post travel begin..." << std::endl;
    root.travPost(visit);
    std::cout << "post travel end..." << std::endl;

    std::cout << "in travel begin..." << std::endl;
    root.travIn(visit);
    std::cout << "in travel end..." << std::endl;
}

void test_of_BinTree() {
    BinTree<int>* root1 = new BinTree<int>();
    root1->insertAsRoot(10);
    root1->insertAsLC(root1->root(), 5);
    root1->insertAsRC(root1->root(), 6);
    
    BinTree<int>* root2 = new BinTree<int>();
    root2->insertAsRoot(2);
    root2->insertAsLC(root2->root(), 4);
    root2->insertAsRC(root2->root(), 7);
    
    BinTree<int> root;
    root.insertAsRoot(1);
    root.attachAsLC(root.root(), root1);
    root.attachAsRC(root.root(), root2);
    // std::vector<int> res;
    auto visit = [](int x) {
        std::cout << "x val = " << x << std::endl;
    };
    std::cout << "pre travel begin..." << std::endl;
    root.travPre(visit);
    std::cout << "pre travel begin..." << std::endl;

    std::cout << "post travel begin..." << std::endl;
    root.travPost(visit);
    std::cout << "post travel end..." << std::endl;

    std::cout << "in travel begin..." << std::endl;
    root.travIn(visit);
    std::cout << "in travel end..." << std::endl;
}

int main(int argc, char* argv[]) {  // 测试二叉树
    test_of_BinTree();
    return 0;
}