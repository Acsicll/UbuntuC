#include <iostream>
#include <typeinfo>
#include <memory>

namespace Singleton_pattern{

template<typename T>
class Singleton {
public:
    static T& GetInstance(){
        static T _instance;
        return _instance;
    } 
protected:
    virtual ~Singleton() {
        std::cout<<"template Singleton destruct\n";
    }
    Singleton() {
        std::cout<<"template Singleton construct\n";
    }
private:
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};

class designPattern : public Singleton<designPattern>{
    friend class Singleton<designPattern>;
private:
    designPattern(){
        std::cout<<"designPattern construct...\n";
    }
    ~designPattern(){
        std::cout<<"designPattern destruct...\n";
    }
};
};

namespace Factory_pattern{

class Product {
public:
    ~Product(){}
    virtual void Create(std::string content) = 0;
};

class ProductA : public Product {
    void Create(std::string content) override {
        std::cout<<"ProductA "<<content<<std::endl;
    }
};

class ProductB : public Product {
    void Create(std::string content) override {
        std::cout<<"ProductB "<<content<<std::endl;
    }
};

class ProductC : public Product {
    void Create(std::string content) override {
        std::cout<<"Productc "<<content<<std::endl;
    }
};

class Factory {
public:
    Product* CreateProduct(const std::type_info& ty_info){
        if(ty_info == typeid(ProductA)){
            return m_pProductA = new ProductA();
        }else if(ty_info == typeid(ProductB)){
            return m_pProductB = new ProductB();
        }else if(ty_info == typeid(ProductC)){
            return m_pProductC = new ProductC();
        }else{
            return nullptr;
        }
    }
    ~Factory(){
        if(m_pProductA){
            delete m_pProductA;
        }else if(m_pProductB){
            delete m_pProductB;
        }else if(m_pProductC){
            delete m_pProductC;
        }
    }
private:
    ProductA* m_pProductA;
    ProductB* m_pProductB;
    ProductC* m_pProductC;
};

class Factory1{
public:
    virtual Product* CreateProduct() = 0;
    Product* m_pProduct;
    virtual ~Factory1(){
        if(m_pProduct){
            std::cout<<"factory destruct\n";
            delete m_pProduct;
        }
    }
};

class Factory1A :public Factory1{
public:
    virtual Product* CreateProduct() override{
        return m_pProduct = new ProductA();
    }    
};

class Factory1B :public Factory1{
public:
    virtual Product* CreateProduct() override{
        return m_pProduct = new ProductB();
    }    
};

class Factory1C :public Factory1{
public:
    virtual Product* CreateProduct() override{
        return m_pProduct = new ProductC();
    }    
};
};

namespace ChainOfResposebility{
class Context{};


class IHandler{
public:
    IHandler() : _next(nullptr) {}
    virtual ~IHandler() {}
    void SetNextHandler(IHandler *next){
        _next = next;
    }
    bool Handle(const Context &ctx){
        if(CanHandle(ctx)){
            return HandleRequest(ctx);
        }else if(GetNextHandle()){
            return GetNextHandle()->Handle(ctx);
        }else{
            // err
        }
        return false;
    }
    
protected:
    virtual bool HandleRequest(const Context &ctx){return true;}
    virtual bool CanHandle(const Context &ctx){return true;}
    IHandler *GetNextHandle(){
        return _next;
    }
private:
    IHandler *_next;
};

class HandlerByBeauty: public IHandler{
protected:
    virtual bool HandleRequest(const Context &ctx){
        return true;
    }
    virtual bool CanHandle(const Context &ctx){
        return false;
    }
};


class HandlerByMainProgram: public IHandler{
protected:
    virtual bool HandleRequest(const Context &ctx){
        return true;
    }
    virtual bool CanHandle(const Context &ctx){
        return false;
    }
};

class HandlerByProjMgr: public IHandler{
protected:
    virtual bool HandleRequest(const Context &ctx){
        return true;
    }
    virtual bool CanHandle(const Context &ctx){
        return false;
    }
};

class HandlerByBoss: public IHandler{
protected:
    virtual bool HandleRequest(const Context &ctx){
        return true;
    }
    virtual bool CanHandle(const Context &ctx){
        return false;
    }
};

    static bool handler_leavereq(Context &ctx){
        IHandler* h0 = new HandlerByBeauty();
        IHandler* h1 = new HandlerByMainProgram();
        IHandler * h2 = new HandlerByProjMgr();
        IHandler * h3 = new HandlerByBoss();
        h0->SetNextHandler(h1);
        h1->SetNextHandler(h2);
        h2->SetNextHandler(h3);
        return h0->Handle(ctx);
    }
};


int main(){
    using namespace Singleton_pattern;
    using namespace Factory_pattern;
    using namespace ChainOfResposebility;

    auto& _instance = designPattern::GetInstance();
    printf("%d\n",&_instance);
    auto& _instance2 = designPattern::GetInstance();
    printf("%d\n",&_instance2);

    Factory factory;
    factory.CreateProduct(typeid(ProductA))->Create("A");
    factory.CreateProduct(typeid(ProductA))->Create("B");
    factory.CreateProduct(typeid(ProductA))->Create("C");

    std::shared_ptr<Factory1A>factoryA_ptr = std::make_shared<Factory1A>();
    factoryA_ptr->CreateProduct()->Create("A1");

    std::shared_ptr<Factory1B>factoryB_ptr = std::make_shared<Factory1B>();
    factoryB_ptr->CreateProduct()->Create("B1");
    
    std::shared_ptr<Factory1C>factoryC_ptr = std::make_shared<Factory1C>();
    factoryC_ptr->CreateProduct()->Create("C1");
    
    Context ctx;
    if(handler_leavereq(ctx)){
        std::cout<<"sucess\n";
    }else{
        std::cout<<"faild\n";
    }
}   