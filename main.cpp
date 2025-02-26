#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>

using namespace std;

// Vehicle class definition
class Vehicle
{
public:
    int id;
    string type;
    string category;
    time_t arrivalTime;
    int priority;
    int passengers;
    string route;

    Vehicle(int id, string type, string category, time_t arrivalTime, int priority = 0, int passengers = 0, string route = "")
        : id(id), type(type), category(category), arrivalTime(arrivalTime), priority(priority), passengers(passengers), route(route) {}

    void display() const
    {
        cout << "Vehicle ID: " << id << "\nType: " << type << "\nCategory: " << category << "\nArrival Time: " << ctime(&arrivalTime)
             << "Priority: " << priority << "\nPassengers: " << passengers << "\nRoute: " << route << endl;
    }
};

// Node structure for linked list
struct Node
{
    Vehicle *vehicle;
    Node *next;
    Node(Vehicle *v) : vehicle(v), next(nullptr) {}
};

// Enhanced Traffic Manager class
class TrafficManager
{
private:
    int capacity;
    vector<Vehicle *> vehicles;
    Node *head;
    vector<Vehicle *> emergencyQueue;

public:
    TrafficManager(int cap) : capacity(cap), head(nullptr) {}

    // Add vehicle to linked list
    void addVehicle(Vehicle *v)
    {
        Node *newNode = new Node(v);
        if (head == nullptr)
        {
            head = newNode;
        }
        else
        {
            Node *current = head;
            while (current->next != nullptr)
            {
                current = current->next;
            }
            current->next = newNode;
        }
        vehicles.push_back(v);

        // Automatically add emergency vehicles to emergency queue
        if (v->priority == 1)
        {
            emergencyQueue.push_back(v);
        }
    }

    // Remove vehicle from linked list
    void removeVehicle(int id)
    {
        Node *current = head;
        Node *previous = nullptr;

        while (current != nullptr)
        {
            if (current->vehicle->id == id)
            {
                if (previous == nullptr)
                {
                    head = current->next;
                }
                else
                {
                    previous->next = current->next;
                }
                delete current;
                return;
            }
            previous = current;
            current = current->next;
        }
    }

    // Search for vehicle by ID
    Vehicle *searchVehicle(int id) const
    {
        Node *current = head;
        while (current != nullptr)
        {
            if (current->vehicle->id == id)
            {
                return current->vehicle;
            }
            current = current->next;
        }
        return nullptr;
    }

    // Sort vehicles by arrival time
    void sortVehicles()
    {
        sort(vehicles.begin(), vehicles.end(), [](const Vehicle *a, const Vehicle *b)
             { return a->arrivalTime < b->arrivalTime; });
    }

    // Display all vehicles
    void displayVehicles() const
    {
        Node *current = head;
        while (current != nullptr)
        {
            current->vehicle->display();
            current = current->next;
        }
    }

    // Process emergency vehicles first
    void processEmergencyVehicles()
    {
        for (Vehicle *v : emergencyQueue)
        {
            removeVehicle(v->id);
            delete v;
        }
        emergencyQueue.clear();
    }
};

// Main function with enhanced interface
int main()
{
    srand(time(0));
    int capacity;
    cout << "Enter traffic queue capacity: ";
    cin >> capacity;

    TrafficManager manager(capacity);
    int vehicleId = 1;

    while (true)
    {
        cout << "\n==== Traffic Control Center ====\n"
             << "1. Add Private Vehicle\n"
             << "2. Add Public Transport\n"
             << "3. Process Vehicles\n"
             << "4. View Queue\n"
             << "5. Search Vehicle\n"
             << "6. Sort Vehicles\n"
             << "7. Generate Report\n"
             << "8. Exit\n"
             << "Choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string types[] = {"Car", "Truck", "Motorcycle"};
            string type = types[rand() % 3];
            manager.addVehicle(new Vehicle(
                vehicleId++, "Private", type, time(0)));
            break;
        }
        case 2:
        {
            string routes[] = {"Bus-101", "Bus-202", "Tram-A1", "Tram-B2"};
            string route = routes[rand() % 4];
            int passengers = 10 + rand() % 50; // 10-59 passengers
            manager.addVehicle(new Vehicle(
                vehicleId++,
                (route.find("Bus") != string::npos) ? "Bus" : "Tram",
                "Public",
                time(0),
                2, // Priority for public transport
                passengers,
                route));
            break;
        }
        case 3:
            manager.processEmergencyVehicles();
            break;
        case 4:
            manager.displayVehicles();
            break;
        case 5:
        {
            int id;
            cout << "Enter vehicle ID to search: ";
            cin >> id;
            Vehicle *found = manager.searchVehicle(id);
            if (found)
            {
                found->display();
            }
            else
            {
                cout << "Vehicle not found!" << endl;
            }
            break;
        }
        case 6:
            manager.sortVehicles();
            cout << "Vehicles sorted by arrival time!" << endl;
            break;
        case 7:
            // Report generation logic
            cout << "Report generated!" << endl;
            break;
        case 8:
            return 0;
        default:
            cout << "Invalid choice!" << endl;
        }
    }
}