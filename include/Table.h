#ifndef SPL_ASS1_TABLE_H
#define SPL_ASS1_TABLE_H

#include <vector>
#include "../include/Customer.h"
#include "../include/Dish.h"

typedef std::pair<int, Dish> OrderPair;

class Table{
public:
    Table(int t_capacity);
    int getCapacity() const;
    void addCustomer(Customer* customer);
    void removeCustomer(int id);
    Customer* getCustomer(int id);
    std::vector<Customer*>& getCustomers();
    std::vector<OrderPair>& getOrders();
    void order(const std::vector<Dish> &menu);
    void openTable();
    void closeTable();
    int getBill();
    bool isOpen();
    virtual ~Table();
    Table(Table& table);
    Table& operator=(const Table& other);
    Table(Table&& other);
    Table& operator=(Table&& other);
    void removeOrders(int id);
    virtual Table* clone();
    void clearfields();
private:
    int capacity;
    bool open;
    std::vector<Customer*> customersList;
    std::vector<OrderPair> orderList; //A list of pairs for each order in a table - (customer_id, Dish)
};


#endif