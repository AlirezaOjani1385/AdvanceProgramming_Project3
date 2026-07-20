#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

const int ARGUMENT_COUNT = 2;
const string STATUS[4] = {"On Way", "At Destination City", "At Original City", "Not Found"};
const string MAIN_CITY = "Tehran";

typedef struct order {
    string originCity;
    string destinationCity;
    int weight;
    int orderId;
    string status;
    int score;
    int cost;
} order;

typedef struct truck {
    int id;
    int capacity;
    string status;
    string originCity;
    string destinationCity;
    vector<order> loads;
} truck;

typedef struct city {
    string name;
    int distance;
    float score;
} city;

void commandDetect(vector<truck>& trucksInfo, vector<city>& citiesInfo, vector<order>& orders, int& numberOfOrder, string command);
void getInfoTrucks(const string& trucks, vector<truck>& trucksInfo);
void getInfoCities(const string& cities, vector<city>& citiesInfo);
void addOrder(int numberOfOrder, vector<order>& orders, string originCity, string destinationCity, int weight);
void printResultAdd(int numberOfOrder);
order getStatusOrder(vector<order>& orders, int targetId);
void printStatus(order target);
truck findTargetTruck(int targetTruckId, vector<truck>& trucksInfo, string destinationCity);
int calculateOrderScore(int biggestId, order target);
vector<order> getLoads(vector<order>& orders, truck targetTruck, int biggestId);
void undoStatusTargetTruck(vector<truck>& trucksInfo, truck targetTruck);
int sumWeight(vector<order>& orders);
void updateStatusOrdersToWay(vector<order>& orders, vector<order>& loads);
void updateTrucksInfo(vector<truck>& trucksInfo, truck targetTruck);
truck loadingTruck(vector<order>& orders, vector<truck>& trucksInfo, int biggestId, int targetTruckId, string destinationCity);
void printResultLoading(truck targetTruck);
void updateStatusOrdersToDestination(vector<order>& orders, vector<order>& loads);
truck deliverTruck(vector<truck>& trucksInfo, vector<order>& orders, int targetTruckId);
void printResultDeliver(truck target);
int sumOutgoingScores(vector<order>& orders, string city, int biggestId);
int sumIncomingScores(vector<order>& orders, string city, int biggestId);
int distance(vector<city>& citiesInfo, string city);
void citiesScore(vector<city>& citiesInfo, vector<order>& orders, int biggestId);
string recommendedCity(vector<city>& citiesInfo, vector<order>& orders, int biggestId);
int calculateSumWeightOrders(vector<order>& orders, string destinationCity);
int freeCapacity(vector<order>& orders, truck target, string destinationCity, int biggestId);
truck findFirstTruck(vector<truck>& trucksInfo, string originalCity, int sumWeight);
int recommendedTruck(vector<truck>& trucksInfo, vector<order>& orders, string destinationCity, int biggestId);
void printResultRecommend(int recommendedTruck, string recommendedCity);
int calculateCost(vector<order>& orders, vector<city> citiesInfo);
void printResultCost(vector<order>& orders, int totalCost);

void commandDetect(vector<truck>& trucksInfo, vector<city>& citiesInfo, vector<order>& orders, int& numberOfOrder, string command) {
    if (command == "add_order") {
        numberOfOrder++;
        string originCity;
        string destinationCity;
        int weight;
        cin >> originCity >> destinationCity >> weight;
        addOrder(numberOfOrder, orders, originCity, destinationCity, weight);
        printResultAdd(numberOfOrder);
    }
    else if (command == "track") {
        int targetId;
        cin >> targetId;
        order target = getStatusOrder(orders, targetId);
        printStatus(target);
    }
    else if (command == "load") {
        int targetTruckId;
        string destinationCity;
        cin >> targetTruckId >> destinationCity;
        truck targetTruck = loadingTruck(orders, trucksInfo, numberOfOrder, targetTruckId, destinationCity);
        printResultLoading(targetTruck);
    }
    else if (command == "deliver") {
        int targetTruckId;
        cin >> targetTruckId;
        truck targetTruck = deliverTruck(trucksInfo, orders, targetTruckId);
        printResultDeliver(targetTruck);
    }
    else if (command == "recommend") {
        string recommendCity = recommendedCity(citiesInfo, orders, numberOfOrder);
        int recommendTruck = recommendedTruck(trucksInfo, orders, recommendCity, numberOfOrder);
        printResultRecommend(recommendTruck, recommendCity);
    }
    else if (command == "financial_report") {
        int totalCost = calculateCost(orders, citiesInfo);
        printResultCost(orders, totalCost);
    }
}

void getInfoTrucks(const string& trucks, vector<truck>& trucksInfo) {
    ifstream fileOfTrucks(trucks);
    if (!fileOfTrucks.is_open()) {
        cerr << "We can not open the file of trucks" << endl;
        return;
    }
    string line;
    int lineNum = 0;
    while (getline(fileOfTrucks, line)) {
        lineNum++;
        if (lineNum == 1) continue;
        stringstream streamLine(line);
        string id;
        string capacity;
        getline(streamLine, id, ',');
        getline(streamLine, capacity);
        truck tr{stoi(id), stoi(capacity), STATUS[2], MAIN_CITY};
        trucksInfo.push_back(tr);
    }    
    fileOfTrucks.close();
}

void getInfoCities(const string& cities, vector<city>& citiesInfo) {
    ifstream fileOfCities(cities);
    if (!fileOfCities.is_open()) {
        cerr << "We can not open the file of cities" << endl;
        return;
    }
    string line;
    int lineNum = 0;
    while (getline(fileOfCities, line)) {
        lineNum++;
        if (lineNum == 1) continue;
        stringstream streamLine(line);
        string name;
        string distance;
        getline(streamLine, name, ',');
        getline(streamLine, distance);
        city ci{name, stoi(distance)};
        citiesInfo.push_back(ci);
    }
    fileOfCities.close();
}

void addOrder(int numberOfOrder, vector<order>& orders, string originCity, string destinationCity, int weight) {
    order oneOrder = {originCity, destinationCity, weight, numberOfOrder, STATUS[2]};
    orders.push_back(oneOrder);
}

void printResultAdd(int numberOfOrder) {
    cout << "Order " << numberOfOrder << " added" << endl;
}

order getStatusOrder(vector<order>& orders, int targetId) {
    order target;
    for (order& ord : orders) {
        if (ord.orderId == targetId) {
            target = ord;
            return target;
        }
    }
    target.status = STATUS[3];
    return target;
}

void printStatus(order target) {
    if (target.status == STATUS[0]) {
        cout << "Order " << target.orderId << " is in transit to " << target.destinationCity << endl;
    }
    else if (target.status == STATUS[1]) {
        cout << "Order " << target.orderId << " is delivered to " << target.destinationCity << endl;
    }
    else if (target.status == STATUS[2]) {
        cout << "Order " << target.orderId << " is currently in warehouse in " << target.originCity << endl;
    }
    else {
        cout << "Order not found" << endl;
    }
}

truck findTargetTruck(int targetTruckId, vector<truck>& trucksInfo, string destinationCity) {
    truck targetTruck;
    for (truck& truckInfo : trucksInfo) {
        if (truckInfo.id == targetTruckId && truckInfo.status == STATUS[2]) {
            truckInfo.destinationCity = destinationCity;
            truckInfo.status = STATUS[0];
            targetTruck = truckInfo;
            return targetTruck;
        }
    }
    targetTruck.status = STATUS[3];
    return targetTruck;
}

int calculateOrderScore(int biggestId, order target) {
    return (target.weight + ((biggestId - target.orderId) * 5));
}

vector<order> getLoads(vector<order>& orders, truck targetTruck, int biggestId) {
    vector<order> loads;
    for (order& ord : orders) {
        if (ord.originCity == targetTruck.originCity && ord.destinationCity == targetTruck.destinationCity
            && ord.status == STATUS[2]) {
            order load;
            load = ord;
            load.score = calculateOrderScore(biggestId, ord);
            load.status = STATUS[0];
            loads.push_back(load);
        }
    }
    return loads;
}

void undoStatusTargetTruck(vector<truck>& trucksInfo, truck targetTruck) {
    for (truck& truckInfo : trucksInfo) {
        if (truckInfo.id == targetTruck.id) {
            truckInfo.status = STATUS[2];
            return;
        }
    }
}

int sumWeight(vector<order>& orders) {
    int sum = 0;
    for (order ord : orders) {
        sum += ord.weight;
    }
    return sum;
}

void updateStatusOrdersToWay(vector<order>& orders, vector<order>& loads) {
    for (int i=0; i<loads.size(); i++) {
        for (int j=0; j<orders.size(); j++) {
            if (loads[i].orderId == orders[j].orderId) {
                orders[j].status = STATUS[0];
            }
        }
    }
}

void updateTrucksInfo(vector<truck>& trucksInfo, truck targetTruck) {
    for (truck& truckInfo : trucksInfo) {
        if (truckInfo.id == targetTruck.id) {
            truckInfo = targetTruck;
        }
    }
}

void fillLoads(vector<order>& loads, truck& targetTruck) {
    int sumWeightInTruck = 0;
    for (order& load : loads) {
        if ((load.weight + sumWeightInTruck) > targetTruck.capacity) {
            continue;
        }
        sumWeightInTruck += load.weight;
        targetTruck.loads.push_back(load);
    }
}

truck loadingTruck(vector<order>& orders, vector<truck>& trucksInfo, int biggestId, int targetTruckId, string destinationCity) {
    truck targetTruck = findTargetTruck(targetTruckId, trucksInfo, destinationCity);
    if (targetTruck.status == STATUS[3]) {
        return targetTruck;
    }
    vector<order> loads = getLoads(orders, targetTruck, biggestId);
    sort(loads.begin(), loads.end(), [](order a, order b){ 
        if (a.score != b.score) return a.score > b.score;
        else return a.orderId < b.orderId;
    });
    fillLoads(loads, targetTruck);
    if (targetTruck.loads.empty()) {
        undoStatusTargetTruck(trucksInfo, targetTruck);
        targetTruck.status = STATUS[3];
        return targetTruck;
    }
    sort(targetTruck.loads.begin(), targetTruck.loads.end(), [](order a, order b){ return a.orderId < b.orderId; });
    updateStatusOrdersToWay(orders, targetTruck.loads);
    updateTrucksInfo(trucksInfo, targetTruck);
    return targetTruck;
}

void printResultLoading(truck targetTruck) {
    if (targetTruck.status == STATUS[3]) {
        cout << "No order could be loaded" << endl;
    }
    else {
        cout << "Truck " << targetTruck.id << " loaded with orders:";
        for (order load : targetTruck.loads) {
            cout << ' ' << load.orderId;
        }
        cout << endl;
    }
}

void updateStatusOrdersToDestination(vector<order>& orders, vector<order>& loads) {
    for (int i=0; i<orders.size(); i++) {
        for (int j=0; j<loads.size(); j++) {
            if (loads[j].orderId == orders[i].orderId) {
                orders[i].status = STATUS[1];
            }
        }
    }
}

truck deliverTruck(vector<truck>& trucksInfo, vector<order>& orders, int targetTruckId) {
    truck targetTruck;
    for (truck& truckInfo : trucksInfo) {
        if (truckInfo.id == targetTruckId && truckInfo.status == STATUS[0]) {
            targetTruck = truckInfo;
            updateStatusOrdersToDestination(orders, truckInfo.loads);
            truckInfo.originCity = truckInfo.destinationCity;
            truckInfo.status = STATUS[2];
            truckInfo.loads.clear();
            return targetTruck;
        }
    }
    targetTruck.id = targetTruckId;
    targetTruck.status = STATUS[3];
    return targetTruck;
}

void printResultDeliver(truck target) {
    if (target.status == STATUS[3]) {
        cout << "No orders to deliver in truck " << target.id << endl;
    }
    else {
        cout << "Truck " << target.id << " delivered orders:";
        for (order load : target.loads) {
            cout << ' ' << load.orderId;
        }
        cout << endl;
    }
}

int sumOutgoingScores(vector<order>& orders, string city, int biggestId) {
    int sum = 0;
    for (order ord : orders) {
        if (ord.originCity == city && ord.destinationCity == MAIN_CITY 
            && ord.status == STATUS[2]) {
            sum += calculateOrderScore(biggestId, ord);
        }
    }
    return sum;
}

int sumIncomingScores(vector<order>& orders, string city, int biggestId) {
    int sum = 0;
    for (order ord : orders) {
        if (ord.destinationCity == city && ord.originCity == MAIN_CITY 
            && ord.status == STATUS[2]) {
            sum += calculateOrderScore(biggestId, ord);
        }
    }
    return sum;
}

int distance(vector<city>& citiesInfo, string targetCity) {
    for (city cityInfo : citiesInfo) {
        if (cityInfo.name == targetCity) {
            return cityInfo.distance;
        }
    }
    return 1;
}

void citiesScore(vector<city>& citiesInfo, vector<order>& orders, int biggestId) {
    for (city& cityInfo : citiesInfo) {
        cityInfo.score = ((float)sumOutgoingScores(orders, cityInfo.name, biggestId) + 
        (float)sumIncomingScores(orders, cityInfo.name, biggestId)) / distance(citiesInfo, cityInfo.name);
    }
}

string recommendedCity(vector<city>& citiesInfo, vector<order>& orders, int biggestId) {
    citiesScore(citiesInfo, orders, biggestId);
    city recommend = citiesInfo[0];
    for (city cityInfo : citiesInfo) {
        if (recommend.score < cityInfo.score) {
            recommend = cityInfo;
        }
        else if (recommend.score == cityInfo.score) {
            recommend = (cityInfo.name < recommend.name) ? cityInfo : recommend;
        }
    }
    return recommend.name;
}

int freeCapacity(vector<order>& orders, truck target, string destinationCity, int biggestId) {
    target.destinationCity = destinationCity;
    vector<order> loads = getLoads(orders, target, biggestId);
    sort(loads.begin(), loads.end(), [](order a, order b){ 
        if (a.score != b.score) return a.score > b.score;
        else return a.orderId < b.orderId;
    });
    fillLoads(loads, target);
    int sumWeightLoads = sumWeight(target.loads);
    return (target.capacity - sumWeightLoads);
}

int calculateSumWeightOrders(vector<order>& orders, string destinationCity) {
    int sum = 0;
    for (order ord : orders) {
        if (ord.destinationCity == destinationCity && ord.status == STATUS[2]) {
            sum += ord.weight;
        }
    }
    return sum;
}

truck findFirstTruck(vector<truck>& trucksInfo, string originalCity, int sumWeight) {
    for (truck truckInfo : trucksInfo) {
        if (truckInfo.originCity == originalCity && truckInfo.status == STATUS[2] && truckInfo.capacity >= sumWeight) 
            return truckInfo;
    }
    return trucksInfo[0];
}

int recommendedTruck(vector<truck>& trucksInfo, vector<order>& orders, string destinationCity, int biggestId) {
    int sumWeight = calculateSumWeightOrders(orders, destinationCity);
    truck recommend = findFirstTruck(trucksInfo, MAIN_CITY, sumWeight);
    for (truck truckInfo : trucksInfo) {
        if (truckInfo.originCity == MAIN_CITY && truckInfo.status == STATUS[2] && truckInfo.capacity >= sumWeight) {
            int freeCapacityOfRecommend = freeCapacity(orders, recommend, destinationCity, biggestId);
            int freeCapacityOfTruck = freeCapacity(orders, truckInfo, destinationCity, biggestId);
            if (freeCapacityOfTruck < freeCapacityOfRecommend) {
                recommend = truckInfo;
            }
            else if (freeCapacityOfTruck == freeCapacityOfRecommend) {
                recommend = (truckInfo.id < recommend.id) ? truckInfo : recommend;
            }
        }
    }
    return recommend.id;
}

void printResultRecommend(int recommendedTruck, string recommendedCity) {
    cout << "Recommended city: " << recommendedCity << endl;
    cout << "Recommended truck: " << recommendedTruck << endl;
}

int calculateCost(vector<order>& orders, vector<city> citiesInfo) {
    int totalCost = 0;
    for (order& ord : orders) {
        if (ord.status == STATUS[1]) {
            string city = (ord.originCity == MAIN_CITY) ? ord.destinationCity : ord.originCity;
            int dis = distance(citiesInfo, city);
            ord.cost = ((ord.weight) * dis);
            totalCost += ord.cost;
        }
    }
    return totalCost;
}

void printResultCost(vector<order>& orders, int totalCost) {
    cout << "Total income: " << totalCost << endl;
    cout << "Delivered orders:" << endl;
    for (order ord : orders) {
        if (ord.status == STATUS[1]) {
            cout << ord.orderId << ' ' << ord.cost << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < ARGUMENT_COUNT + 1) {
        cerr << "You have to get two arguments" << endl;
        return 1;
    }
    vector<truck> trucksInfo;
    vector<city> citiesInfo;
    vector<order> orders;
    getInfoTrucks(argv[1], trucksInfo);
    getInfoCities(argv[2], citiesInfo);
    string command;
    int numberOfOrder = 0;
    while (cin >> command) {
        commandDetect(trucksInfo, citiesInfo, orders, numberOfOrder, command);
    }
}
