//===-- MMPUDAG.h - MMPU Directed Acyclic Graph class --------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
//
//
//===--------------------------------------------------------------------===//

#ifndef MMPUDAG_H
#define MMPUDAG_H

#include <stack>
#include <vector>
#include <map>
#include "llvm/Support/raw_ostream.h"
using namespace llvm;
//===----------------------------------------------------------------------===//
// DAG
//===----------------------------------------------------------------------===//

namespace MMPU {
template <typename TElemType>
class Graph {
public:
  void AddVextor(TElemType E);
  void InsertVextor(TElemType E, TElemType S, int64_t W);
  void AddArc(TElemType Src, TElemType Dest, int64_t W);
  void AddArc(unsigned Src, unsigned Dest, int64_t W);
  void DelArc(TElemType Src, TElemType Dest);
  int64_t getArcWeight(TElemType Src, TElemType Dest) const;
  void DFS(uint64_t v, bool *visited);
  void DFSTraverse();
  void BFSTraverse();                       
  int64_t *TopologicalOrder(std::stack<unsigned> &sre, unsigned &e);
  bool Criticalpath(std::map<TElemType, TElemType> &Path);

private:
  struct Arcnode {
    unsigned adjvex;
    int64_t weight;
  public:
    Arcnode(unsigned a, int64_t w) : adjvex(a), weight(w) {}
  };

  struct Vexnode {
    TElemType data;
    std::vector<struct Arcnode *> arcs;
  };
  
  struct ALgraph {
    std::vector<struct Vexnode *> vexes;
  };
  
  ALgraph algraph; // Adjacent link
  
  int64_t count;
};

template <typename TElemType>
void Graph<TElemType>::AddVextor(TElemType E) {
  algraph.vexes.push_back(new struct Vexnode());
  algraph.vexes.back()->data = E;
  errs() << algraph.vexes.size() << ":" << E << "\n";
}
  
template <typename TElemType>
void Graph<TElemType>::InsertVextor(TElemType E, TElemType S, int64_t W) {
  if (!E || !S) return;
  unsigned e = algraph.vexes.size();
  AddVextor(E);
  unsigned s = 0;
  for(unsigned i = 0; i < algraph.vexes.size(); i++) {
    if (S == algraph.vexes[i]->data) {
      s = i;
      break;
    }
  }
  while(!algraph.vexes[s]->arcs.empty()) {
    AddArc(e, algraph.vexes[s]->arcs.back()->adjvex,
           algraph.vexes[s]->arcs.back()->weight);
    delete algraph.vexes[s]->arcs.back();
    algraph.vexes[s]->arcs.pop_back();
  }
  AddArc(s, e, W);
}
  
  
template <typename TElemType>
void Graph<TElemType>::AddArc(unsigned Src, unsigned Dest, int64_t W) {
  errs() << Src << "->" << Dest;
  errs() << ":" << W << "\n";
  algraph.vexes[Src]->arcs.push_back(new struct Arcnode(Dest, W));
}
  
template <typename TElemType>
void Graph<TElemType>::AddArc(TElemType Src, TElemType Dest, int64_t W) {
  unsigned m = 0, n = 0;
  errs() << Src << "->" << Dest;
  for(unsigned j = 0; j < algraph.vexes.size(); j++) {
    if (Src == algraph.vexes[j]->data) m = j;
    if (Dest == algraph.vexes[j]->data) n = j;
  }
  errs() << "|" << m << "->" << n << ":" << W << "\n";
  algraph.vexes[m]->arcs.push_back(new struct Arcnode(n, W));
}
  
template <typename TElemType>
void Graph<TElemType>::DelArc(TElemType Src, TElemType Dest) {
  unsigned m = 0, n = 0;
  unsigned j = 0;
  for(; j < algraph.vexes.size(); j++) {
    if (Src == algraph.vexes[j]->data) m = j;
    if (Dest == algraph.vexes[j]->data) n = j;
  }
  if (j == algraph.vexes.size()) return;
  typename std::vector<struct Arcnode *>::iterator iter
    = algraph.vexes[m]->arcs.begin();
  while (iter != algraph.vexes[m]->arcs.end())
    if ((*iter)->adjvex == n) {
      delete *iter;
      algraph.vexes[m]->arcs.erase(iter);
    } else iter++;
}
  
template <typename TElemType>
int64_t Graph<TElemType>::getArcWeight(TElemType Src, TElemType Dest) const {
  unsigned m, n;
  for(unsigned j = 0; j < algraph.vexes.size(); j++) {
    if (Src == algraph.vexes[j]->data) m = j;
    if (Dest == algraph.vexes[j]->data) n = j;
  }
  for (unsigned i = 0; i < algraph.vexes[m]->arcs.size(); i++)
    if (algraph.vexes[m]->arcs[i]->adjvex == n)
      return algraph.vexes[m]->arcs[i]->weight;
  return 0;
}

template <typename TElemType>
void Graph<TElemType>::DFS(uint64_t v, bool *visited) {
  // cout << algraph.vex[v].data << endl;
  visited[v] = true;
  Arcnode *p;
  uint64_t v1;
  for(unsigned i = 0; i < algraph.vexes[v].arcs.size(); i++) {
    p = algraph.vexes[v].arcs[i];
    v1 = p->adjvex;
    if (!visited[v1]) DFS(v1, visited);
  }
}

template <typename TElemType>
void Graph<TElemType>::DFSTraverse() {
  bool *visited = new bool[algraph.vexes.size()];
  for(unsigned i = 0; i < algraph.vexes.size(); i++)
    visited[i] = false;
  for(unsigned v = 0;v < algraph.vexnum;v++)
    if(!visited[v]) DFS(v,visited);
  free(visited);
}

template <typename TElemType>
void Graph<TElemType>::BFSTraverse() {
  std::vector<int> Lq;
  Arcnode *p;
  bool *visited = new bool[algraph.vexes.size()];
  uint64_t v1;
  for(unsigned i = 0;i < algraph.vexes.size(); i++)
    visited[i] = false;
  for(int v = 0; v < algraph.vexes.size(); v++) {
    if (!visited[v]) {
      // cout << algraph.vex[v].data << endl;
      visited[v] = true;
      Lq.push_back(v);
      while(!Lq.empty()) {
        v1 = Lq.front();
        Lq.erase(Lq.begin());
        for(unsigned i = 0; i < algraph.vexes[v1].arcs.size(); i++) {
          p = algraph.vexes[v1].arcs[i];
          v1 = p->adjvex;
          if (!visited[v1]) {
            // cout << algraph.vex[v1].data << endl;
            visited[v1] = true;
            Lq.push_back(v1);
          }
        }
      }
    }
  }
  free(visited);
}

template <typename TElemType>
int64_t * Graph<TElemType>::TopologicalOrder(std::stack<unsigned> &sre,
                                              unsigned &e) {
  std::stack<unsigned> topo;
  int64_t *re = new int64_t[algraph.vexes.size()];
  unsigned *indegree = new unsigned[algraph.vexes.size()];
  unsigned v, v1, count = 0;
  for(unsigned i = 0; i < algraph.vexes.size(); i++) re[i] = 0;
  
  for(unsigned i = 0; i < algraph.vexes.size(); i++) indegree[i] = 0;
  
  Arcnode *p;
  
  for(unsigned i = 0; i < algraph.vexes.size(); i++)
    for(unsigned j = 0; j < algraph.vexes[i]->arcs.size(); j++) {
      p = algraph.vexes[i]->arcs[j];
      v = p->adjvex;
      indegree[v]++;
    }
  
  for(unsigned i = 0;i < algraph.vexes.size();i++)
    if (indegree[i] == 0) topo.push(i);
  
  while(!topo.empty()) {
    v = topo.top();
    topo.pop();
    sre.push(v);
    count++;
    for(unsigned j = 0; j < algraph.vexes[v]->arcs.size(); j++) {
      p = algraph.vexes[v]->arcs[j];
      v1 = p->adjvex;
      if (0 == --indegree[v1]) {
        topo.push(v1);
      }
      if(re[v] + p->weight > re[v1]) re[v1] = re[v] + p->weight;
    }
  }
  e = 0;
  for (unsigned i = 0; i < algraph.vexes.size(); i++)
    if (re[i] > re[e]) e = i;
  if(count < algraph.vexes.size()) return NULL;
  return re;
}


template <typename TElemType>
bool Graph<TElemType>::Criticalpath(std::map<TElemType, TElemType> &Path) {
  int64_t *rl = new int64_t[algraph.vexes.size()];
  std::stack<unsigned> sre;
  unsigned e;
  int64_t *re = TopologicalOrder(sre, e);
  if (re) {
    unsigned v, v1;
    Arcnode *p;
    for(unsigned i = 0; i < algraph.vexes.size(); i++) rl[i] = re[e];
    while(!sre.empty()) {
      v = sre.top();
      sre.pop();
      for(unsigned j = 0; j < algraph.vexes[v]->arcs.size(); j++) {
        p = algraph.vexes[v]->arcs[j];
        v1 = p->adjvex;
        if(rl[v1] - p->weight < rl[v]) rl[v] = rl[v1] - p->weight;
      }
    }
    e = 0;
    while (algraph.vexes[e]->arcs.size()) {
      for(unsigned j = 0; j < algraph.vexes[e]->arcs.size(); j++) {
        p = algraph.vexes[e]->arcs[j];
        v1 = p->adjvex;
        if (rl[v1] - p->weight == re[e]) {
          Path[algraph.vexes[e]->data] = algraph.vexes[v1]->data;
          e = v1;
          break;
        }
      }
    }
  } else return false; // Error: cycle exists in DAG;
  delete re;
  delete rl;
  return true;
}
}
#endif

