#ifndef DSU_H
#define DSU_H

#include <vector>

class DSU {
private:
    std::vector<int> parent;
    std::vector<int> rankv;

public:
    DSU(int n = 0);
    int find(int x);
    bool unite(int x, int y);
    int size() const { return (int)parent.size(); }
};

#endif // DSU_H
