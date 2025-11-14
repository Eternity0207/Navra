#include "../include/dsu.h"
#include <algorithm>
DSU::DSU(int n):parent(n),rankv(n,0) {
    for (int i=0; i < n; i++)
        parent[i]=i;
    }
int DSU::find(int x) {
    if (parent[x]!=x)
        parent[x]=find(parent[x]);
    return parent[x];
     }
bool DSU::unite(int x,int y) {
    x=find(x);
    y=find(y);
    if (x==y) return false;
    if (rankv[x] < rankv[y]) std::swap(x,y);//using name space std
    //deemed to be risky in this file(as used in others)
    //also only required in this file(hence left for swap function)
    parent[y]=x;
    if (rankv[x]==rankv[y]) rankv[x]++;
    return true;
}
