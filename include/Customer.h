#ifndef SPL_ASS1_CUSTOMER_H
#define SPL_ASS1_CUSTOMER_H

#include <vector>
#include <string>
#include "../include/Dish.h"

class Customer{
public:
    Customer(std::string c_name, int c_id);
    virtual std::vector<int> order(const std::vector<Dish> &menu)=0;
    virtual std::string toString() const = 0;
    std::string getName() const;
    int getId() const;
    virtual Customer* clone()=0;
    virtual ~Customer()=0;

private:
    const std::string name;
    const int id;
};


class VegetarianCustomer : public Customer {
public:
    VegetarianCustomer(std::string name, int id);
    std::vector<int> order(const std::vector<Dish> &menu);
    std::string toString() const;
    Customer* clone();
    ~VegetarianCustomer();
private:
};


class CheapCustomer : public Customer {
public:
    CheapCustomer(std::string name, int id);
    std::vector<int> order(const std::vector<Dish> &menu);
    std::string toString() const;
    Customer* clone();
    ~CheapCustomer();
private:
    bool ordered;
};


class SpicyCustomer : public Customer {
public:
    SpicyCustomer(std::string name, int id);
    std::vector<int> order(const std::vector<Dish> &menu);
    std::string toString() const;
    Customer* clone();
    ~SpicyCustomer();
private:
    int orderCount;
    int bvgId;
};


class AlchoholicCustomer : public Customer {
public:
    AlchoholicCustomer(std::string name, int id);
    std::vector<int> order(const std::vector<Dish> &menu);
    inline void sortALC(const std::vector<Dish> &menu);
    std::string toString() const;
    Customer* clone();
    ~AlchoholicCustomer();
    AlchoholicCustomer(AlchoholicCustomer& other);
private:
    std::vector<int> sortedALC;
    int orderCount;
};


#endif