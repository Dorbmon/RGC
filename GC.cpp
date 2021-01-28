#include "GC.h"
#include <thread>
REngine defaultEngine;
template<class T,class ...args>
T* RObject::make (args... data) {
    if (this->engine == nullptr) {
        // 使用默认引擎
        this->engine = &defaultEngine;
    }
    RObject* obj = this->engine->Malloc<T>(data...);
    obj->setEngine(this->engine);
    obj->father = this;
    this->sons.push_back(obj->id);
    return obj;
}
template<class T,class ...args>
RObject* REngine::Malloc(args... arg) {
    while (std::atomic_flag_test_and_set(&(this->mallocing))) {
        std::this_thread::yield();
    }
    // 开始申请内存
    // 目前先不实现内存池，使用操作系统的函数
    RObject* data = new (T);
    data->id = objectIdCounter ++;
    this->objects.push_back(data);
    this->objectUse.push_back(true);
    std::atomic_flag_clear(&(this->mallocing));
    return data;
}
RObject::~RObject() {
    // 从父亲那里删除自己的存在
    if (this->father != nullptr) {
        for (auto ptr = this->sons.begin();ptr != this->sons.end();++ ptr) {
            if (*ptr == this->id) {
                this->sons.erase(ptr);
                break;
            }
        }
    }
}
void REngine::GC() {
    // Stop the world,停止内存的申请，释放
    while (std::atomic_flag_test_and_set(&(this->mallocing))) {
        std::this_thread::yield();
    }
    size_t num = this->objectUse.size();
    for (size_t index = 0;index < num;++ index) {
        this->objectUse [index] = false;
    }
    // 开始检查所有的OBJECT
    // 默认不会爆栈 dfs 来遍历
    for (auto &v:this->objects) {
        if (v->father == nullptr && !this->objectUse[v->id]) {
            this->objectUse[v->id] = true;
            this->dfs(v);
        }
    }
    // 开始释放内存
    for (size_t index = 0;index < num;++ index) {
        if (!this->objectUse[index]) {  //应该释放了
            delete(this->objects[index]);
        } else {

        }
    }
}
void REngine::dfs (RObject* now) {
    // 继续延伸下去now-
    size_t num = now->sons.size();
    for (size_t index = 0;index < num;++ index) {
        size_t sonId = now->sons[index];
        if (!this->objectUse[sonId]) {   //还没有访问
            this->objectUse[sonId] = true;
            dfs (this->objects[sonId]);
        }
    }
}