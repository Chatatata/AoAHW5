//  Student ID: 150120016
//  Buğra EKUKLU

#include <iostream>
#include <cctype>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <memory>

using String = std::string;
using UInt64 = uint64_t;
using Int64 = int64_t;
using InputFileStream = std::ifstream;

class Tree {
public:
    //  An enum class holding the color of the node
    enum class Color {
        Red,
        Black
    };
    
    //  An enum class holding the gender of the person
    enum class Gender {
        Male,
        Female
    };
    
private:
    //  Node struct which holds the tree nodes and connections
    struct Node {
        String name;
        Gender gender;
        UInt64 age;
        
        Color color;
        
        UInt64 kidsCount;
        UInt64 adultCount;
        
        Node *parent;
        Node *left;
        Node *right;
    };
    
    //  BST: Pushes node to the tree with `subroot` root
    static void enqueue(Node *incoming, Node *subroot) noexcept {
        //  Compare keys of the incoming node versus subroot
        if (incoming->name < subroot->name) {
            //  Check if left side of the subroot is nil
            if (!subroot->left) {
                subroot->left = incoming;
                incoming->parent = subroot;
            } else {
                enqueue(incoming, subroot->left);
            }
        } else {
            //  Check if right side of the subroot is nil
            if (!subroot->right) {
                subroot->right = incoming;
                incoming->parent = subroot;
            } else {
                enqueue(incoming, subroot->right);
            }
        }
    }
    
    //  Counts adults of each node in subtree with `subroot` root
    static inline UInt64 countAdults(Node *subroot) {
        UInt64 sum = 0LL;
        
        if (subroot->left) sum += countAdults(subroot->left);
        if (subroot->right) sum += countAdults(subroot->right);
        
        if (subroot->age >= 18) {
            subroot->adultCount = sum + 1;
        } else {
            subroot->adultCount = sum;
        }
        
        return subroot->adultCount;
    }
    
    //  Counts kids of each node in subtree with `subroot` root
    static inline UInt64 countKids(Node *subroot) {
        UInt64 sum = 0LL;
        
        if (subroot->left) sum += countKids(subroot->left);
        if (subroot->right) sum += countKids(subroot->right);
        
        if (subroot->age < 18) {
            subroot->kidsCount = sum + 1;
        } else {
            subroot->kidsCount = sum;
        }
        
        return subroot->kidsCount;
    }
    
    //  Finds the adult with given index using OSSelect algorithm
    static Node *findAdultWithIndex(Node *subroot, const UInt64 index) noexcept {
        UInt64 relevance = subroot->left ? subroot->left->adultCount : 0;
        
        if (subroot->age >= 18) relevance += 1;
        
        if (index == relevance) {
            return subroot->age >= 18 ? subroot : subroot->left;
        } else if (index < relevance) {
            return findAdultWithIndex(subroot->left, index);
        } else {
            return findAdultWithIndex(subroot->right, index - relevance);
        }
    }
    
    //  Finds the kid with given index using OSSelect algorithm
    static Node *findKidWithIndex(Node *subroot, const UInt64 index) noexcept {
        UInt64 relevance = subroot->left ? subroot->left->kidsCount : 0;
        
        if (subroot->age < 18) relevance += 1;
        
        if (index == relevance) {
            return subroot->age < 18 ? subroot : subroot->left;
        } else if (index < relevance) {
            return findKidWithIndex(subroot->left, index);
        } else {
            return findKidWithIndex(subroot->right, index - relevance);
        }
    }
    
    //  Prints the node with given depth using preorder typing
    inline void printNode(Node *subroot, UInt64 level = 0) const noexcept {
        for (UInt64 i = 0; i < level; ++i) {
            std::printf("-");
        }
        
        std::printf("(%c) %s %c %llu\n", subroot->color == Color::Red ? 'R':'B', subroot->name.c_str(), subroot->gender == Gender::Male ? 'M':'F', subroot->age);
        
        if (subroot->left) printNode(subroot->left, level + 1);
            if (subroot->right) printNode(subroot->right, level + 1);
                }
    
    //  Rotates the pivot to the left (slightly changed version of the one written on textbook).
    inline void rotateLeft(Node *pivot) noexcept {
        Node *temporary = pivot->right;
        
        if (temporary) {
            if (temporary->left) {
                pivot->right = temporary->left;
                temporary->left->parent = pivot;
            } else {
                pivot->right = nullptr;
            }
            
            if (pivot->parent) {
                temporary->parent = pivot->parent;
            }
            
            if (pivot->parent) {
                if (pivot == pivot->parent->left) {
                    pivot->parent->left = temporary;
                } else {
                    pivot->parent->right = temporary;
                }
            } else {
                root = temporary;
            }
            
            temporary->left = pivot;
            pivot->parent = temporary;
        }
    }
    
    //  Rotates the pivot to the right (slightly changed version of the one written on textbook).
    inline void rotateRight(Node *pivot) noexcept {
        Node *temporary = pivot->left;
        
        if (temporary) {
            if (temporary->right) {
                pivot->left = temporary->right;
                temporary->right->parent = pivot;
            } else {
                pivot->left = nullptr;
            }
            
            if (pivot->parent) {
                temporary->parent = pivot->parent;
            }
            
            if (pivot->parent) {
                if (pivot == pivot->parent->left) {
                    pivot->parent->left = temporary;
                } else {
                    pivot->parent->right = temporary;
                }
            } else {
                root = temporary;
            }
            
            temporary->right = pivot;
            pivot->parent = temporary;
        }
    }
    
    //  Triggers the counter function to count adults from root of the tree.
    inline void countAdults() noexcept {
        countAdults(root);
    }
    
    //  Triggers the counter function to count kids from root of the tree.
    inline void countKids() noexcept {
        countKids(root);
    }
    
public:
    Tree() : root(nullptr) {
        //  Blank implementation
    }
    
    //  Inserts element with given attributes
    inline void insert(String name, Gender gender, UInt64 age) noexcept {
        auto incoming = new Node();
        
        incoming->name = name;
        incoming->gender = gender;
        incoming->age = age;
        incoming->color = Color::Red;
        
        //  Make it root if there is no root yet
        if (!root) {
            incoming->color = Color::Black;
            root = incoming;
        } else {
            enqueue(incoming, root);
            
            while (incoming != root && incoming->parent->color == Color::Red) {
                if (incoming->parent == incoming->parent->parent->left) {
                    auto uncle = incoming->parent->parent->right;
                    
                    //  Case I
                    if (uncle && uncle->color == Color::Red) {
                        //  Recolor from the uncle node
                        uncle->color = Color::Black;
                        incoming->parent->color = Color::Black;
                        uncle->parent->color = Color::Red;
                        
                        incoming = uncle->parent;
                    } else {
                        //  Case II
                        if (incoming == incoming->parent->right) {
                            incoming = incoming->parent;
                            rotateLeft(incoming);
                        }
                        
                        //  Case III
                        incoming->parent->color = Color::Black;
                        incoming->parent->parent->color = Color::Red;
                        rotateRight(incoming->parent->parent);
                    }
                } else {
                    auto uncle = incoming->parent->parent->left;
                    
                    if (uncle && uncle->color == Color::Red) {
                        //  Recolor from the uncle node
                        uncle->color = Color::Black;
                        incoming->parent->color = Color::Black;
                        uncle->parent->color = Color::Red;
                        
                        incoming = uncle->parent;
                    } else {
                        if (incoming == incoming->parent->left) {
                            incoming = incoming->parent;
                            rotateRight(incoming);
                        }
                        
                        incoming->parent->color = Color::Black;
                        incoming->parent->parent->color = Color::Red;
                        rotateLeft(incoming->parent->parent);
                    }
                }
                
                root->color = Color::Black;
            }
        }
        
        countAdults();
        countKids();
    }
    
    inline void print() const noexcept {
        if (root) printNode(root);
            }
    
    inline Node *findAdultWithIndex(UInt64 index) {
        return findAdultWithIndex(root, index);
    }
    
    inline Node *findKidWithIndex(UInt64 index) {
        return findKidWithIndex(root, index);
    }
    
private:
    Node *root = nullptr;
};



int main(int argc, const char * argv[]) {
    if (argc != 2) return 9;
    
    InputFileStream inputFile(argv[1]);
    
    if (!inputFile.is_open()) return 8;
    
    String buffer;
    Tree tree;
    
    while (std::getline(inputFile, buffer)) {
        String name = buffer.substr(0, buffer.find('\t'));
        
        buffer.erase(0, buffer.find('\t') + 1);
        
        Tree::Gender gender = buffer.substr(buffer.find('\t'), 1) == "M" ? Tree::Gender::Male : Tree::Gender::Female;
        
        buffer.erase(0, buffer.find('\t') + 1);
        
        UInt64 age = std::stoll(buffer);
        
        tree.insert(name, gender, age);
    }
    
    tree.print();
    std::cout << "5th adult: " << tree.findAdultWithIndex(5)->name << std::endl;
    std::cout << "3rd non-adult: " << tree.findKidWithIndex(3)->name << std::endl;
    
    return 0;
}
















