#include "../include/dsu.h"

DSU::DSU(int n) : parent(n), rank(n, 0) {
    for (int i = 0; i < n; i++) {
        parent[i] = i;
    }
}

int DSU::find(int x) {
    if (parent[x] != x) {
        parent[x] = find(parent[x]);//path conmpression to increase speed
    }
    return parent[x];
}

bool DSU::unite(int x, int y) {
    int px =find(x);//essentialy a way to determine which 'set' it belongs to/"FOREST OF GRAPHS"
    int py =find(y);
    if (px == py) return false;
    if (rank[px] < rank[py]) {
        parent[px] = py;
    } else if (rank[px] > rank[py]) {
        parent[py] = px;
    } else {
        parent[py] = px;
        rank[px]++;
    }
    return true;
}
