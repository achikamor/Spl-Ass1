#ifndef SPL_ASS1_RESTAURANT_H
#define SPL_ASS1_RESTAURANT_H

#include <vector>
#include <string>

#include "../include/Dish.h"
#include "../include/Table.h"
#include "../include/Action.h"

class Restaurant{
public:
    Restaurant();
    Restaurant(const std::string &configFilePath);
    void start();
    int getNumOfTables() const;
    Table* getTable(int ind);
    const std::vector<BaseAction*>& getActionsLog() const; // Return a reference to the history of actions
    std::vector<Dish>& getMenu();
    virtual ~Restaurant();
    Restaurant& operator=(Restaurant&& other);
    Restaurant& operator=(const Restaurant& other);
    Restaurant(Restaurant& restaurant);
    Restaurant(Restaurant&& other);

private:
    void openTable(std::string str);
    void initTable(int numOfTables,std::string str);    //avishai XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    bool open;
    int customerCount;
    std::vector<Table*> tables;
    std::vector<Dish> menu;
    std::vector<BaseAction*> actionsLog;

};

#endif