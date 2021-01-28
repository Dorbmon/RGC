#pragma once
#include <vector>
#include <atomic>
class REngine;
class RObject {
private:
    REngine* engine;
public:
    void setEngine(REngine* engine) { this->engine = engine; }
    size_t id;
    RObject* father;
    std::vector<size_t> sons;
    ~RObject();
private:
    template<class T,class ...args>
    T* make (args... data);
};
// GC引擎核心
class REngine {
private:
    std::vector<RObject*> objects;
    std::atomic_flag mallocing;
    std::vector<bool>     objectUse;    //判断object是否被引用
    size_t objectIdCounter;
public:
    template<class T,class ...args>
    RObject* Malloc (args... data);
    void GC ();
    void dfs (RObject* now);
};