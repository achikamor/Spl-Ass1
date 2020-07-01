#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "../include/Restaurant.h"
#include "../include/Table.h"
#include "../include/Dish.h"



Restaurant::Restaurant():open(false),customerCount(0),tables(),menu(),actionsLog(){}


Restaurant::Restaurant(const std::string &configFilePath) :open(false),customerCount(0),tables(),menu(),actionsLog(){
    std::ifstream sourceFile(configFilePath);
    std::string nextLn="";
    bool firstStage=true;
    int dishId=0;
    std::string dishName;
    std::string dishType;
    std::string dishPrice;

    menu.clear();

    while (std::getline(sourceFile,nextLn)){
        if(firstStage){

            while(nextLn.at(0)=='#' || nextLn=="" || nextLn=="\r")
                std::getline(sourceFile,nextLn);
            int tempInt=std::stoi(nextLn, nullptr,10);

            std::getline(sourceFile,nextLn);
            while(nextLn.at(0)=='#' || nextLn=="" || nextLn=="\r")
                std::getline(sourceFile,nextLn);
            initTable(tempInt,nextLn);

            firstStage=false;

        }
        else{
            while(nextLn.at(0)=='#' || nextLn.empty() || nextLn=="\r") {
                std::getline(sourceFile, nextLn);
            }
            std::stringstream ss(nextLn);
            std::getline(ss,dishName,',');
            std::getline(ss,dishType,',');
            std::getline(ss,dishPrice,',');
            if(dishType=="VEG")
                menu.push_back(Dish(dishId,dishName,std::stoi(dishPrice, nullptr,10),VEG));
            else if(dishType=="ALC")
                menu.push_back(Dish(dishId,dishName,std::stoi(dishPrice, nullptr,10),ALC));
            else if(dishType=="SPC")
                menu.push_back(Dish(dishId,dishName,std::stoi(dishPrice, nullptr,10),SPC));
            else if(dishType=="BVG")
                menu.push_back(Dish(dishId,dishName,std::stoi(dishPrice, nullptr,10),BVG));
            dishId++;

        }
    }
}


void Restaurant::start() {
    std:: cout<<"Restaurant is now open!"<<std::endl;
    std::string input;
    std::string word;

    while(input!="closeall"){
        getline(std::cin,input);
        std::stringstream ss(input);
        std::getline(ss,word,' ');
        if(word=="open"){
            openTable(input.substr(5));
        }
        else if(word=="order"){
            std::getline(ss,word,' ');
            int tableId=(int)std::stoi(word);
            actionsLog.push_back(new Order(tableId));
            actionsLog.back()->act(*this);
        }
        else if(word=="move"){
            std::getline(ss,word,' ');
            int strt=std::stoi(word);
            std::getline(ss,word,' ');
            int dst=std::stoi(word);
            std::getline(ss,word,' ');
            int id=std::stoi(word);
            actionsLog.push_back(new MoveCustomer(strt,dst,id));
            actionsLog.back()->act(*this);
        }
        else if(word=="close"){
            std::getline(ss,word,' ');
            int tableId=std::stoi(word);
            actionsLog.push_back(new Close(tableId));
            actionsLog.back()->act(*this);
        }
        else if(word=="closeall"){
            actionsLog.push_back(new CloseAll());
            actionsLog.back()->act(*this);
        }
        else if(word=="menu"){
            actionsLog.push_back(new PrintMenu());
            actionsLog.back()->act(*this);
        }
        else if(word=="status"){
            std::getline(ss,word,' ');
            int tbl=std::stoi(word);
            int size=(int)tables.size();
            if(tbl>=0&&tbl<size){
                actionsLog.push_back( new PrintTableStatus(tbl));
                actionsLog.back()->act(*this);
            }
        }
        else if(word=="log"){
            BaseAction* action= new PrintActionsLog();
            action->act(*this);
            actionsLog.push_back(action);
        }
        else if(word=="backup"){
            BaseAction* BacAct=new BackupRestaurant();
            BacAct->act(*this);
            actionsLog.push_back(BacAct);
        }
        else if(word=="restore"){
           BaseAction* ResAct=new RestoreResturant;
           ResAct->act(*this);
           actionsLog.push_back(ResAct);
        }
    }
}

void Restaurant::openTable(std::string str){
    std::stringstream ss(str);
    std::vector<Customer *> customers;
    std::string cstmrName;

    std::getline(ss,cstmrName,' ');
    int tableId=std::stoi(cstmrName,nullptr,10);
    std::string cstmrType;
    while (std::getline(ss, cstmrName, ',')) {
        std::getline(ss, cstmrType, ' ');
        if (cstmrType == "veg")
            customers.push_back(new VegetarianCustomer(cstmrName, customerCount));
        else if (cstmrType == "chp")
            customers.push_back(new CheapCustomer(cstmrName, customerCount));
        else if (cstmrType == "spc")
            customers.push_back(new SpicyCustomer(cstmrName, customerCount));
        else if (cstmrType == "alc")
            customers.push_back(new AlchoholicCustomer(cstmrName, customerCount));
        customerCount++;
    }
    actionsLog.push_back(new OpenTable(tableId, customers));
    actionsLog.back()->act(*this);
}



void Restaurant::initTable(int numOfTables,std::string str){
    std::string tempStr;
    std::stringstream ss(str);
    for(int i=0;i<numOfTables;i++){
        std::getline(ss,tempStr,',');
        tables.push_back(new Table(std::stoi(tempStr, nullptr,10)));
    }
}

int Restaurant::getNumOfTables() const {
    int output=(int)tables.size();
    return output;
}

Table *Restaurant::getTable(int ind) {
    int size=(int)tables.size();
    if(ind>=0 && ind<size)
        return tables[ind];
    return nullptr;

}

const std::vector<BaseAction*> &Restaurant::getActionsLog() const {
    return actionsLog ;
}

std::vector<Dish> &Restaurant::getMenu() {
    return menu;
}

//MoveAssignmentOperator
Restaurant& Restaurant::operator=(Restaurant&& other) {
    if(this==&other)
        return *this;
    this->open = other.open;
    this->customerCount = other.customerCount;
    this->menu.clear();
    for(auto dish:other.menu)
        this->menu.push_back(Dish(dish.getId(), dish.getName(), dish.getPrice(), dish.getType()));

    int size=(int)tables.size();
    for(int i=0;i<size;i++){
        tables[i]->clearfields();
        delete(tables[i]);
        tables[i]= nullptr;
    }

    for(auto table:other.tables) {
        this->tables.push_back(new Table(*table));
        delete(table);
        table= nullptr;
    }

    for(auto action:actionsLog){
        delete(action);
        action= nullptr;
    }
    actionsLog.clear();

    for(auto action:other.actionsLog) {
        this->actionsLog.push_back(action->clone());
        delete(action);
        action= nullptr;
    }
    return *this;
}

// Copy operator
Restaurant &Restaurant::operator=(const Restaurant &other) {
    if(this==&other)
        return *this;
    this->open = other.open;
    this->customerCount = other.customerCount;
    this->menu.clear();
    int size=(int)other.menu.size();
    for(int i=0;i<size;i++)
        this->menu.push_back(Dish(other.menu[i].getId(),other.menu[i].getName(),other.menu[i].getPrice(),other.menu[i].getType()));

    int size2=(int)this->tables.size();
    for(int f=0;((f < size2) & (tables[f]!= nullptr));f++){
        tables[f]->clearfields();
        delete(tables[f]);
        tables[f]= nullptr;
    }
    tables.clear();

    for(auto table:other.tables)
        this->tables.push_back(( Table(*table)).clone());

    for(auto action:actionsLog){
        delete(action);
        action= nullptr;
    }
    actionsLog.clear();

    for(auto action:other.actionsLog)
        this->actionsLog.push_back(action->clone());
    return *this;
}
//Copy constructor
Restaurant::Restaurant(Restaurant &restaurant):open(restaurant.open),customerCount(restaurant.customerCount),tables(),menu(),actionsLog() {
    this->menu.clear();
    int size=(int)restaurant.menu.size();
    for(int i=0;i<size;i++)
        this->menu.push_back(restaurant.menu[i]);
    int size1=(int)restaurant.tables.size();
    for(int j=0;j<size1;j++) {
        this->tables.push_back(new Table(*restaurant.tables[j]));
    }
    int tempsize=(int)restaurant.actionsLog.size();
    for(int k=0; k < tempsize; k++)
        this->actionsLog.push_back(restaurant.actionsLog[k]->clone());

}
//Destructor
Restaurant::~Restaurant() {
    int size=(int)tables.size();
    for(int i=0;i<size;i++){
        tables[i]->clearfields();
        delete(tables[i]);
        tables[i]= nullptr;
    }
    tables.clear();
    int size1=(int)actionsLog.size();
    for(int j=0;j<size1;j++){
        delete(actionsLog[j]);
        actionsLog[j]= nullptr;
    }
    actionsLog.clear();
    menu.clear();
}

//Move Constructor
Restaurant::Restaurant(Restaurant&& other):open(other.open),customerCount(other.customerCount),tables(),menu(),actionsLog() {
    int size = (int) other.menu.size();
    for (int i = 0; i < size; i++)
        this->menu.push_back(other.menu[i]);
    int size1 = (int) other.actionsLog.size();
    for (int i = 0; i < size1; i++) {               //initializing this.actionlog
        this->actionsLog.push_back(other.actionsLog[i]);
        delete(other.actionsLog[i]);
        other.actionsLog[i]= nullptr;
    }

    int size2 = (int) other.tables.size();
    for (int i = 0; i < size2; i++) {              //initializing this.tables and delete other.tables
        this->tables.push_back(other.tables[i]);
        delete(other.getTable(i));              //calling table destructor
        other.tables[i]= nullptr;
    }

}
