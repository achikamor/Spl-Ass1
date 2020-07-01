#include "../include/Action.h"
#include "../include/Table.h"
#include "../include/Restaurant.h"
#include <iostream>
extern Restaurant* backup;

BaseAction::BaseAction():errorMsg(),status(PENDING){

}

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status=COMPLETED;

}

void BaseAction::error(std::string errorMsg) {
    status=ERROR;
    this->errorMsg="Error: "+errorMsg;
}

std::string BaseAction::getErrorMsg() const {
    return this->errorMsg;
}

//Destructor (default)
BaseAction::~BaseAction() {
    errorMsg.clear();
}

OpenTable::OpenTable(int id, std::vector<Customer*> &customersList): BaseAction(),tableId(id),customers() { //,customers(customersList)
    int size=(int)customersList.size();
    for(int i=0;i<size;i++)
        customers.push_back(customersList[i]);
}


void OpenTable::act(Restaurant &restaurant) {
    Table* tbl=restaurant.getTable(tableId);
    if(tbl == nullptr || tbl->isOpen()) {
        error("Table does not exist or is already open");
        std::cout<< getErrorMsg() << std::endl;
    }
    else{
        tbl->openTable();
        for(auto customer :customers){
            tbl->addCustomer(customer->clone());
        }
        complete();
    }
}

BaseAction* OpenTable::clone(){
    BaseAction* output=new OpenTable(*this);
    return output;
}

std::string OpenTable::toString() const {
    std::string output="open "+ std::to_string(tableId);
    for(auto customer :customers) {
        output +=" "+customer->toString();
    }
    if(getStatus()==ERROR)
        output+=" "+getErrorMsg();
    else
        output+=" Completed";
    return output;
}
OpenTable::~OpenTable() {
    int size=(int)this->customers.size();
    for(int i=0;i<size;i++){
        delete (customers[i]);
        customers[i] = nullptr;
    }
   customers.clear();
}
//copy constructor
OpenTable::OpenTable(OpenTable &other):tableId(other.tableId),customers() {
    int size=other.customers.size();
    for(int i=0;i<size;i++)
        this->customers.push_back(other.customers[i]->clone());
}

Order::Order(int id):BaseAction(),tableId(id) {}
void Order::act(Restaurant &restaurant) {
    Table* tbl=restaurant.getTable(tableId);
    if(tbl == nullptr || !tbl->isOpen()){
        error("Table does not exist or is not open");
        std::cout << getErrorMsg() << std::endl;
    }
    else{
        tbl->order(restaurant.getMenu());
        complete();
    }
}

BaseAction* Order::clone(){
    return new Order(*this);
}

std::string Order::toString() const {
    std::string output ="order "+std::to_string(tableId);
    if(getStatus()==ERROR)
        output+=" "+getErrorMsg();
    else
        output+=" Completed";
    return output;
}

Order::~Order() {}


MoveCustomer::MoveCustomer(int src,int dst,int customerId):BaseAction() ,srcTable(src),dstTable(dst),id(customerId){}
void MoveCustomer::act(Restaurant &restaurant){
    Table* srcT=restaurant.getTable(srcTable);
    Table* dstT=restaurant.getTable(dstTable);
    int dstSize=(int)dstT->getCustomers().size();
    if((srcT== nullptr||dstT== nullptr) || (!srcT->isOpen()|| !dstT->isOpen() ) ||
       dstT->getCapacity()==dstSize|| srcT->getCustomer(id)== nullptr){
        error("Cannot move customer");
        std:: cout << getErrorMsg()<< std::endl;
    }
    else{
        dstT->addCustomer(srcT->getCustomer(id));
        for(auto order:srcT->getOrders()){
            if(order.first==id){
                dstT->getOrders().push_back(OrderPair(id,order.second));
            }
        }
        srcT->removeCustomer(id);
        if(srcT->getCustomers().empty()) {
            srcT->closeTable();
        }
        complete();

    }
}

BaseAction* MoveCustomer::clone(){
    return new MoveCustomer(*this);
}


std::string MoveCustomer::toString() const {
    std::string output ="move "+std::to_string(srcTable)+" "+std::to_string(dstTable)+" "+std::to_string(id);
    if(getStatus()==ERROR)
        output+=" "+getErrorMsg();
    else
        output+=" Completed";
    return output;
}

MoveCustomer::~MoveCustomer() {}

PrintMenu::PrintMenu():BaseAction(){}

void PrintMenu::act(Restaurant &restaurant){
    int size=(int)restaurant.getMenu().size();
    for(int i=0;i<size;i++)
        std::cout << restaurant.getMenu()[i].tostring() <<std::endl;
    complete();
}

BaseAction *PrintMenu::clone() {
    return new PrintMenu(*this);
}

std::string PrintMenu::toString() const {
    std::string output="menu ";
    if(this->getStatus()==COMPLETED)
        output=output+"Completed";
    else
        output=output+"Pending";
    return output;
}

PrintMenu::~PrintMenu() {}


PrintTableStatus::PrintTableStatus(int id):BaseAction(),tableId(id){}

void PrintTableStatus::act(Restaurant &restaurant) {
    Table* tbl=restaurant.getTable(tableId);
    if(tbl->isOpen()){
        std::cout << "Table "<< std::to_string(tableId) <<" status: open" << std::endl;

        std::cout << "Customers:" << std::endl;
        for(auto customer:tbl->getCustomers())
            std::cout <<std:: to_string(customer->getId())<< " " << customer->getName() << std::endl;

        std::cout << "Orders:" << std::endl;
        for(auto order:tbl->getOrders())
            std::cout << order.second.getName() << " " << order.second.getPrice() <<"NIS " << order.first << std::endl;

        std::cout << "Current Bill: " << tbl->getBill() << "NIS" << std::endl;
    }
    else{
        std::cout << "Table "<< std::to_string(tableId) <<" status: closed" << std::endl;
    }

    complete();
}

BaseAction* PrintTableStatus::clone(){
    return new PrintTableStatus(*this);
}


std::string PrintTableStatus::toString() const {
    std::string output ="status "+std::to_string(tableId)+" ";
    if(getStatus()==COMPLETED)
        output+="Completed";
    return output;
}

PrintTableStatus::~PrintTableStatus() {}

PrintActionsLog::PrintActionsLog():BaseAction() {}

void PrintActionsLog::act(Restaurant &restaurant) {

    for(auto action:restaurant.getActionsLog())
        std::cout << action->toString() << std::endl;
    complete();

}

BaseAction *PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}

std::string PrintActionsLog::toString() const {
    std::string output="log ";
    if(this->getStatus()==COMPLETED)
        output=output+"Completed";
    else
        output=output+"Pending";
    return output;
}

PrintActionsLog::~PrintActionsLog() {}

Close::Close(int id): BaseAction(), tableId(id){}

void Close::act(Restaurant &restaurant) {
    Table* table=restaurant.getTable(tableId);
    if(table== nullptr ||!table->isOpen()){
        error("Table does not exist or is not open");
        std::cout<< getErrorMsg()<< std::endl;
    }
    else{
        int Bill=table->getBill();
        std::cout<<"Table "<<tableId<<" was closed. Bill "<<std::to_string(Bill)<<"NIS"<<std::endl;
        table->closeTable();
        table->clearfields();
        complete();
    }
}

BaseAction* Close::clone(){
    return new Close(*this);
}


std::string Close::toString() const {
    std::string output ="close "+std::to_string(tableId);
    if(getStatus()==ERROR)
        output+=" "+getErrorMsg();
    else
        output+=" Completed";
    return output;
}

Close::~Close() {}

CloseAll::CloseAll(): BaseAction() {}

void CloseAll::act(Restaurant &restaurant) {
    int tabId;
    int tabBill;
    for(int i=0;i<restaurant.getNumOfTables();i++){
        if(restaurant.getTable(i)->isOpen()) {
            tabId=i;
            tabBill=restaurant.getTable(i)->getBill();
            restaurant.getTable(i)->closeTable();
            std:: cout <<"Table "<<tabId<<" was closed. Bill "<<tabBill<<"NIS"<<std::endl;
        }
    }
        complete();
    }


BaseAction* CloseAll::clone(){
    return new CloseAll(*this);
}


std::string CloseAll::toString() const {
    std::string output ="closeall";
    if(getStatus()==COMPLETED)
        output+=" Completed";
    else
        output+=" Pending";
    return output;
}

CloseAll:: ~CloseAll() {}

BackupRestaurant::BackupRestaurant():BaseAction(){}

void BackupRestaurant::act(Restaurant &restaurant){
    if(backup== nullptr)
        backup=new Restaurant(restaurant);
    else{
        backup->operator=(restaurant);
    }
    complete();
}

BaseAction* BackupRestaurant::clone(){
    return new BackupRestaurant(*this);
}


std::string BackupRestaurant::toString() const {
    std::string output ="backup Completed";
    return output;
}

BackupRestaurant::~BackupRestaurant(){}

RestoreResturant::RestoreResturant():BaseAction() {}

void RestoreResturant::act(Restaurant &restaurant) {
    if(backup== nullptr){
        error("No backup available");
        std::cout<< getErrorMsg()<< std::endl;
    }
    else{
        complete();
        restaurant=*backup;

    }
}

std::string RestoreResturant::toString() const {
    std::string output ="restore";
    if(getStatus()==ERROR)
        output+=" "+getErrorMsg();
    else
        output+=" Completed";
    return output;
}

BaseAction *RestoreResturant::clone() {
    return new RestoreResturant(*this);
}

RestoreResturant::~RestoreResturant(){}